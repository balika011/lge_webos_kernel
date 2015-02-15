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

#ifndef X_CKGEN_8561_H
#define X_CKGEN_8561_H

//============================================================================
// Include files
//============================================================================
#include "x_hal_ic.h"
#include "x_typedef.h"

#define XTAL_CLOCK_27M                      (27000000)   // 27MHz
//============================================================================
// Register definitions
//============================================================================
#define REG_RW_VERSION                      0x0000          //0x8561

#define REG_RW_ARMPLL_CFG0                  0x0004          //ARMPLL Configuration 0 Register
  #define ARMPLL_FMEN 						  (1U<<31)	    //PLL Ref/FB monitor clock enable
  #define ARMPLL_VODEN                        (1U<<26)      //CHP OverDrive Enable; 1'b1: Enable
  #define ARMPLL_ACCEN                        (1U<<25)      //Fast Slew Enable; 1'b1: Enable
  #define ARMPLL_MONEN                        (1U<<24)      //Monitor for debug enable; 1'b1: Enable
  #define ARMPLL_FPEN                         (1U<<23)      //PLL four phase output enable; 1'b0: 2 phase output; 1'b1: 4 phase output
  #define ARMPLL_LF                           (1U<<22)      //Frequency Band Control; 1'b0: Band > 700MHz; 1'b1: Band < 700MHz
  #define ARMPLL_HF                           (1U<<21)      //Boost to 1.4GHz don't guarantee MP
  #define ARMPLL_BR                           (1U<<20)      //Resistance adjustment for Bandwidth
                                                            //1'b0: BW = Fref/10; 1'b1: BW = Fref/20
  #define ARMPLL_BP                           (1U<<19)      //Capacitance adjustment for Bandiwdth
  #define ARMPLL_DIV_OFFSET                   (16)          //Time domain cap multiplication ratio
  #define ARMPLL_DIV_MASK                     (0x7<<16)     //N:*2^(N)
  #define ARMPLL_FBSEL_OFFSET                 (14)          //Feedback clock select
  #define ARMPLL_FBSEL_MASK                   (0x3<<14)     //00: Fvco/1 01: Fvco/2 1X: Fvco/4 (Fvco output < 700MHz => 2'b00 Fvco output > 700MHz => 2'b01)
  #define ARMPLL_CKCTRL_OFFSET                (12)          //Fast Slew Time Control
  #define ARMPLL_CKCTRL_MASK                  (0x3<<12)     //00:2^9*Tin 01:2^8*Tin 10:2^7*Tin 11:2^6*Tin
  #define ARMPLL_POSDIV_OFFSET                (10)          //Post-divider ratio for single-phase output
  #define ARMPLL_POSDIV_MASK                  (0x3<<10)     //00:/1  01:/2  1X:/4
  #define ARMPLL_PREDIV_OFFSET                (8)           //Pre-divider ratio
  #define ARMPLL_PREDIV_MASK                  (0x3<<8)      //2'b00:/1  2'b01:/2  2'b1X:/4
  #define ARMPLL_FBDIV_OFFSET                 (1)           //Feedback divide ratio
  #define ARMPLL_FBDIV_MASK                   (0x7F<<1)     //00:/1  01:/2 ... N:/(N+1)
  #define ARMPLL_PWD                          (1U<<0)       //1'b0: Power On

#define REG_RO_APLL_STA0                    0x0008          //APLL Group Status 0
  #define APLL_STA0_VCOCAL_FAIL_MASK          (1U<<7)       //VCOCAL fail
  #define APLL_STA0_VCOCAL_CPLT_MASK          (1U<<6)       //VCOCAL complete
  #define APLL_STA0_VCO_STATE_MASK            (0x3F)        //VCO state

#define REG_RO_PLL_STA0                     0x000C          //PLL Group Status 0
  #define PLL_STA0_PORD_MASK                  (1U<<16)      //USBPHY lookback test status for ABIST

#define REG_RO_PLL_STA2                     0x0014          //PLL Group Status 2
  #define PLL_STA2_SATA_RXD2_OFFSET           (20)          //SATA2 RX data for ABIST, tie to 0
  #define PLL_STA2_SATA_RXD2_MASK             (0x3FF<<20)   //
  #define PLL_STA2_SATA_RXD_OFFSET            (10)          //SATA1 RX data for ABIST, tie to 0
  #define PLL_STA2_SATA_RXD_MASK              (0x3FF<<10)   //
  #define PLL_STA2_SACD_ADCOUT_OFFSET         (0)           //SACD ADC output data for ABIST
  #define PLL_STA2_SACD_ADCOUT_MASK           (0x3FF>>0)    //

#define REG_RW_SYSPLL1_CFG0                 0x0018          //SYSPLL1 Configuration 0 Register
  #define SYSPLL1_VODEN                       (1U<<26)      //CHP OverDrive Enable; 1'b1: Enable
  #define SYSPLL1_ACCEN                       (1U<<25)      //Fast Slew Enable; 1'b1: Enable
  #define SYSPLL1_MONEN                       (1U<<24)      //Monitor for debug enable; 1'b1: Enable
  #define SYSPLL1_FPEN                        (1U<<23)      //PLL four phase output enable; 1'b0: 2 phase output; 1'b1: 4 phase output
  #define SYSPLL1_LF                          (1U<<22)      //Frequency Band Control; 1'b0: Band > 700MHz; 1'b1: Band < 700MHz
  #define SYSPLL1_HF                          (1U<<21)      //Boost to 1.4GHz don't guarantee MP
  #define SYSPLL1_BR                          (1U<<20)      //Resistance adjustment for Bandwidth
                                                            //1'b0: BW = Fref/10; 1'b1: BW = Fref/20
  #define SYSPLL1_BP                          (1U<<19)      //Capacitance adjustment for Bandiwdth
  #define SYSPLL1_DIV_OFFSET                  (16)          //Time domain cap multiplication ratio
  #define SYSPLL1_DIV_MASK                    (0x7<<16)     //N:*2^(N)
  #define SYSPLL1_FBSEL_OFFSET                (14)          //Feedback clock select
  #define SYSPLL1_FBSEL_MASK                  (0x3<<14)     //00: Fvco/1 01: Fvco/2 1X: Fvco/4 (Fvco output < 700MHz => 2'b00 Fvco output > 700MHz => 2'b01)
  #define SYSPLL1_CKCTRL_OFFSET               (12)          //Fast Slew Time Control
  #define SYSPLL1_CKCTRL_MASK                 (0x3<<12)     //00:2^9*Tin 01:2^8*Tin 10:2^7*Tin 11:2^6*Tin
  #define SYSPLL1_POSDIV_OFFSET               (10)          //Post-divider ratio for single-phase output
  #define SYSPLL1_POSDIV_MASK                 (0x3<<10)     //00:/1  01:/2  1X:/4
  #define SYSPLL1_PREDIV_OFFSET               (8)           //Pre-divider ratio
  #define SYSPLL1_PREDIV_MASK                 (0x3<<8)      //00:/1  01:/2  1X:/4
  #define SYSPLL1_FBDIV_OFFSET                (1)           //Feedback divide ratio
  #define SYSPLL1_FBDIV_MASK                  (0x7F<<1)     //00:/1  01:/2 ... N:/(N+1)
  #define SYSPLL1_PWD                         (1U<<0)       //1'b0: Power On

#define REG_RW_SYSPLL1_CFG1                 0x001C          //SYSPLL1 Configuration 1 Register
  #define SYSPLL1_DDS_PI_C_OFFSET             (29)          //SYSPLL1_DDS PI capacitor adjustment
  #define SYSPLL1_DDS_PI_C_MASK               (0x7<<29)     //
  #define SYSPLL1_SSC_PHINI                   (1U<<28)      //DDS SSC initial phase (0:down 1:up)
  #define SYSPLL1_CLK_PH_INV                  (1U<<27)      //SYSPLL1_DDS clock inversion
  #define SYSPLL1_DDS_PREDIV2                 (1U<<26)      //SYSPLL1_DDS predivide by 2
  #define SYSPLL1_DDS_PI_PL_ENB               (1U<<25)      //SYSPLL1_DDS PI pull low function enable bar
  #define SYSPLL1_DDS_RST_SEL                 (1U<<24)      //DDS NCPO PI reset bar
  #define SYSPLL1_FMEN                        (1U<<23)      //PLL Ref/FB monitor clock enable
  #define SYSPLL1_USB_DIV_OFFSET              (20)          //To usb clock
  #define SYSPLL1_USB_DIV_MASK                (0x7<<20)     //=432M/ (RG_USB_DIV[2:0]+2)/4
  #define SYSPLL1_DDS_HF_EN                   (1U<<19)      //DDS high frequency mode enable
  #define SYSPLL1_SSC_TRI_EN                  (1U<<18)      //DDS SSC enable
  #define SYSPLL1_SSC_EN                      (1U<<17)      //DDS SSC enable
  #define SYSPLL1_FIFO_START_MAN              (1U<<16)      //DDS FIFO manual start
  #define SYSPLL1_PCW_NCPO_P180_OFFSET        (3)           //DDS NCPO PCW 180 phase
  #define SYSPLL1_PCW_NCPO_P180_MASK          (0x1FFF<<3)   //
  #define SYSPLL1_PCW_NCPO_P180_CHG           (1U<<2)       //DDS PCW asynchrounous 180 phase clock
  #define SYSPLL1_DDS_RSTB                    (1U<<1)       //DDS NCPO PI reset bar
  #define SYSPLL1_DDS_PWDB                    (1U<<0)       //DDS PI power down bar

#define REG_RW_SYSPLL1_CFG2                 0x0020          //SYSPLL1 Configuration 2 Register
  #define SYSPLL1_PCW_NCPO_CHG                (1U<<31)      //DDS PCW asynchrounous clock
  #define SYSPLL1_PCW_NCPO_OFFSET             (1)           //DDS NCPO PCW
  #define SYSPLL1_PCW_NCPO_MASK               (0x7FFFFFFF<<1) //
  #define SYSPLL1_PCW_NCPO                    (1U<<0)       //DDS PCW asynchrounous clock

#define REG_RW_SYSPLL1_CFG3                 0x0024          //SYSPLL1 Configuration 3 Register
  #define SYSPLL1_SSC_DELTA_OFFSET            (16)          //DDS SSC dither amplitude control
  #define SYSPLL1_SSC_DELTA_MASK              (0xFFFF<<16)  //
  #define SYSPLL1_SSC_PRD_OFFSET              (0)           //DDS SSC dither period control
  #define SYSPLL1_SSC_PRD_MASK                (0xFFFF<<16)  //

#define REG_RW_SYSPLL1_CFG4                 0x0028          //SYSPLL1 Configuration 4 Register
  #define SYSPLL1_DDS_DMY_OFFSET              (16)          //DDS dummy registers
  #define SYSPLL1_DDS_DMY_MASK                (0xFFFF<<16)  //
  #define SYSPLL1_SSC_DELTA1_OFFSET           (0)           //DDS SSC dither amplitude control for initial
  #define SYSPLL1_SSC_DELTA1_MASK             (0xFFFF<<16)  //

#define REG_RW_SYSPLL2_CFG0                 0x0030          //SYSPLL2 Configuration 0 Register
  #define SYSPLL2_FMEN						  (1U<<31)	    //PLL Ref/FB monitor clock enable
  #define SYSPLL2_VODEN                       (1U<<26)      //CHP OverDrive Enable; 1'b1: Enable
  #define SYSPLL2_ACCEN                       (1U<<25)      //Fast Slew Enable; 1'b1: Enable
  #define SYSPLL2_MONEN                       (1U<<24)      //Monitor for debug enable; 1'b1: Enable
  #define SYSPLL2_FPEN                        (1U<<23)      //PLL four phase output enable; 1'b0: 2 phase output; 1'b1: 4 phase output
  #define SYSPLL2_LF                          (1U<<22)      //Frequency Band Control; 1'b0: Band > 700MHz; 1'b1: Band < 700MHz
  #define SYSPLL2_HF                          (1U<<21)      //Boost to 1.4GHz don't guarantee MP
  #define SYSPLL2_BR                          (1U<<20)      //Resistance adjustment for Bandwidth
                                                            //1'b0: BW = Fref/10; 1'b1: BW = Fref/20
  #define SYSPLL2_BP                          (1U<<19)      //Capacitance adjustment for Bandiwdth
  #define SYSPLL2_DIV_OFFSET                  (16)          //Time domain cap multiplication ratio
  #define SYSPLL2_DIV_MASK                    (0x7<<16)     //N:*2^(N)
  #define SYSPLL2_FBSEL_OFFSET                (14)          //Feedback clock select
  #define SYSPLL2_FBSEL_MASK                  (0x3<<14)     //00: Fvco/1 01: Fvco/2 1X: Fvco/4 (Fvco output < 700MHz => 2'b00 Fvco output > 700MHz => 2'b01)
  #define SYSPLL2_CKCTRL_OFFSET               (12)          //Fast Slew Time Control
  #define SYSPLL2_CKCTRL_MASK                 (0x3<<12)     //00:2^9*Tin 01:2^8*Tin 10:2^7*Tin 11:2^6*Tin
  #define SYSPLL2_POSDIV_OFFSET               (10)          //Post-divider ratio for single-phase output
  #define SYSPLL2_POSDIV_MASK                 (0x3<<10)     //00:/1  01:/2  1X:/4
  #define SYSPLL2_PREDIV_OFFSET               (8)           //Pre-divider ratio
  #define SYSPLL2_PREDIV_MASK                 (0x3<<8)      //00:/1  01:/2  1X:/4
  #define SYSPLL2_FBDIV_OFFSET                (1)           //Feedback divide ratio
  #define SYSPLL2_FBDIV_MASK                  (0x7F<<1)     //00:/1  01:/2 ... N:/(N+1)
  #define SYSPLL2_PWD                         (1U<<0)       //1'b0: Power On

#define REG_RW_PLL_CFG0                     0x0038          //PLL Group Configuration 0
  #define PLL_ABIST_DIV_OFFSET                (24)          //Dummy reg
  #define PLL_ABIST_DIV_MASK                  (0xFF<<24)    //

  #define PLL_PLLABIST_PD					  (1U<<23)	    //PLLGP abist power down
  #define PLL_PLLABIST_DIV					  (1U<<22)	    //Selected clock
  #define PLL_PLLGPTEST_EN                    (1U<<21)      //PLLGP test mode enable
  #define PLL_BRCKMON_PD					  (1U<<17)	    //dummy register
  #define PLL_BRCKTX_EN						  (1U<<16)	    //TX power down for MEMPLL
  #define PLL_GPTOAD_EN                       (1U<<15)      //PLLGP TO ADPLL opendrain test enable
  #define PLL_MPX_EN                          (1U<<14)      //MPX test enable signal
  #define PLL_WDTS_EN                         (1U<<13)      //Watchog enable, 0:disable, 1:enable
  #define PLL_RESERVE                         (0x1FFF<<0)

#define REG_RW_PLL_CFG1                     0x003C          //PLL Group Configuration 1

#define REG_RW_PLL_CFG2                     0x0040          //PLL Group Configuration 2
  #define PLL_PCW_NCPO_OFFSET                 (1)           //DDS NCPO PCW
  #define PLL_PCW_NCPO_MASK                   (0x7FFFFFFF<<1) //
  #define PLL_PCW_NCPO_CHG                    (1U<<0)       //DDS PCW asynchrounous clock

#define REG_RW_PLL_CFG3                     0x0044          //PLL Group Configuration 3
  #define MEMPLL_CKSEL                        (1U<<21)      //MEMPLL clock select; 0 : DDS; 1 : 27M
  #define PLL_SEL_CK                          (1U<<20)      //PLL group test mode control; 0: AIO mode; 1: CLK mode
  #define PLL_MONSEL_OFFSET                   (16)          //PLL group monitor control
  #define PLL_MONSEL_MASK                     (0xF<<16)     //

#define REG_RW_TST_CFG0                     0x005C          //Test Configuration 0
  #define TST_CFG0_MC_TSTSEL_OFFSET           (28)          //MC clock source selection
  #define TST_CFG0_MC_TSTSEL_MASK             (0x3<<28)     //00 set mc clock source as PLL
  #define TST_CFG0_VDEC_TSTSEL_OFFSET         (24)          //VDEC clock source selection
  #define TST_CFG0_VDEC_TSTSEL_MASK           (0x3<<24)     //00 set mc clock source as PLL
  #define TST_CFG0_ADSP_TSTSEL_OFFSET         (22)          //ADSP clock source selection
  #define TST_CFG0_ADSP_TSTSEL_MASK           (0x7<<22)     //00 set mc clock source as PLL
  #define TST_CFG0_AXICLK_RATIO_OFFSET        (18)          //Define the axiclk divide ratio correlated to ARM1 clock.
  #define TST_CFG0_AXICLK_RATIO_MASK          (0x7<<18)     //The real axiclk divide ratio is this value added by 1.
  #define TST_CFG0_RISC_TSTSEL_OFFSET         (16)          //RISC clock source selection
  #define TST_CFG0_RISC_TSTSEL_MASK           (0x3<<16)     //00 set mc clock source as PLL;
  #define TST_CFG0_NFLASH_TSTSEL_OFFSET       (10)          //NAND FLASH clock source selection
  #define TST_CFG0_NFLASH_TSTSEL_MASK         (0x3<<10)     //00 set mc clock source as PLL;
  #define TST_CFG0_BCLK_RATIO_OFFSET          (4)           //Define the BCLK divide ratio correlated to axiclk.
  #define TST_CFG0_BCLK_RATIO_MASK            (0x7<<4)      //The real BCLK divide ratio is this value added by 1.
  #define TST_CFG1_TEST_RIRC_INHIBIT          (1U<<0)       //reset in test_cpum mode, test_cpum mode will be released if this bit is set

#define REG_RW_TST_CFG1                     0x0060          //Test Configuration Register 1
  #define TST_CFG1_MVDO2_TSTSEL_OFFSET        (30)          //MVDO2 clock source selection
  #define TST_CFG1_MVDO2_TSTSEL_MASK          (0x3<<30)     //00 set mc clock source as PLL
  #define TST_CFG1_OSD_TSTSEL_OFFSET          (28)          //OSD clock source selection
  #define TST_CFG1_OSD_TSTSEL_MASK            (0x3<<28)     //00 set mc clock source as PLL
  #define TST_CFG1_JPEG_TSTSEL_OFFSET         (26)          //JPEG clock source selection
  #define TST_CFG1_JPEG_TSTSEL_MASK           (0x3<<26)     //00 set mc clock source as PLL
  #define TST_CFG1_GRAPH_TSTSEL_OFFSET        (24)          //GRAPH clock source selection
  #define TST_CFG1_GRAPH_TSTSEL_MASK          (0x3<<24)     //00 set mc clock source as PLL
  #define TST_CFG1_RESZ_TSTSEL_OFFSET         (22)          //resizer clock source selection
  #define TST_CFG1_RESZ_TSTSEL_MASK           (0x3<<22)     //00 set mc clock source as PLL
  #define TST_CFG1_PNG_TSTSEL_OFFSET          (20)          //PNG clock source selection
  #define TST_CFG1_PNG_TSTSEL_MASK            (0x3<<20)     //00 set mc clock source as PLL
  #define TST_CFG1_RISCCLK_RATIO_OFFSET       (10)          //Define the ARM1 clock divide ratio correlated to risc_clk.
  #define TST_CFG1_RISCCLK_RATIO_MASK         (0x7<<10)     //The real ARM1 clock divide ratio is this value added by 1.
  #define TST_CFG1_TEST_TSTSEL_OFFSET         (8)           //test clcok option selection
  #define TST_CFG1_TEST_TSTSEL_MASK           (0x3<<8)      //00 default test clk pair: pad ettxclk / pad exrxclk
  #define TST_CFG1_DRAM_TSTSEL_OFFSET         (0)           //DRAM clock source selection
  #define TST_CFG1_DRAM_TSTSEL_MASK           (0x1F<<0)     //5'b00100 for boot-up; 5'b00000 set mc clock source as PLL for normal use;

#define REG_RO_RST_CFG                      0x0064          //Reset Configuration
  #define RST_CFG_TRAP_OFFSET 				  (16)		    //TRAP pin = {nfwen, nfren, nfcle, nfale, nfcen, aosdata[3:0],amute}
  #define RST_CFG_TRAP_MASK					  (0x3FF<<16)    //TRAP[0]: 0: normal mode, 1: ICE mode
														    //TRAP[5:1] = [1,x,x,0,1]: test scan mode, [1,x,x,1,0] test cpum mode, [1,0,1,x,x] OLT test mode
														    //[1,1,0,x,x] ABIST mode
														    //TRAP[9:6] = [0,x,x,x]: nand from ND pin, [1,0,0,0]: eMMC boot from GPIO pin(mode1)
														    //[1,0,0,1]: eMMC boot from GPIO pin(mode2), [1,0,1,0]: eMMC boot from ND pin(mode1)
														    //[1,0,1,1]: eMMC boot from ND pin(mode2)

#define REG_RW_TST_CFG2                     0x0068          //Test Configuration Register 1
  #define TST_CFG2_DISP_1244M_TSTSEL_OFFSET         (19)
  #define TST_CFG2_DISP_1244M_TSTSEL_MASK           (0x3<<19)
  #define TST_CFG2_GCPU_TSTSEL_OFFSET               (17)
  #define TST_CFG2_GCPU_TSTSEL_MASK                 (0x3<<17)
  #define TST_CFG2_SVO_STDBY_TSTSEL_OFFSET          (13)
  #define TST_CFG2_SVO_STDBY_TSTSEL_MASK            (0x3<<13)
  #define TST_CFG2_SPDIFIN_TSTSEL_OFFSET            (4)
  #define TST_CFG2_SPDIFIN_TSTSEL_MASK              (0x3<<4)

#define REG_RW_RST_RISC_PD                  0x006C          //RISC Reset Pull-Down Register
                                                            //pull down the reset signal for RISC, which is write-protected.
                                                            //If wants to pull down this signal, write in data should be 16¡¦h8105.

#define REG_RW_CLK_CFG0                     0x0070          //Clock Selection Configuration 0
  #define CLK_PDN_VFD                         (1U<<31)      //turn off VFD
  #define CLK_SLOW_RISC_SEL_OFFSET            (28)          //Select divisor used to divide 27MHz clock, for RISC clock slow option
  #define CLK_SLOW_RISC_SEL_MASK              (0x7<<28)     //
  #define CLK_SLOW_RISC_SEL_27M_1_2048        (7)
  #define CLK_SLOW_RISC_SEL_27M_1_1024        (6)
  #define CLK_SLOW_RISC_SEL_27M_1_512         (5)
  #define CLK_SLOW_RISC_SEL_27M_1_256         (4)
  #define CLK_SLOW_RISC_SEL_27M_1_128         (3)
  #define CLK_SLOW_RISC_SEL_27M_1_64          (2)
  #define CLK_SLOW_RISC_SEL_27M_1_32          (1)
  #define CLK_SLOW_RISC_SEL_27M_1_16          (0)
  #define CLK_PDN_NFLASH                      (1U<<19)      //turn off Nand FLASH
  #define CLK_CLK_NFLASH_SEL_OFFSET           (16)          //Selection of NAND FLASH clock frequency
  #define CLK_CLK_NFLASH_SEL_MASK             (0x7<<16)     //
  #define CLK_CLK_NFLASH_SEL_27M              (0)
  #define CLK_CLK_NFLASH_SEL_ARMPLL_1_4       (1)
  #define CLK_CLK_NFLASH_SEL_SYSPLL1_1_2      (2)
  #define CLK_CLK_NFLASH_SEL_SYSPLL1_1_3      (3)
  #define CLK_CLK_NFLASH_SEL_SYSPLL1_1_4      (4)
  #define CLK_CLK_NFLASH_SEL_SYSPLL2_1_3      (5)
  #define CLK_CLK_NFLASH_SEL_MCK_1_2          (6)
  #define CLK_CLK_NFLASH_SEL_SVOIPLL_1_4      (7)
  #define CLK_PDN_ADSP2                       (1U<<11)      //turn off ADSP
  #define CLK_CLK_ADSP2_SEL_OFFSET            (8)           //Selection of ADSP clock frequency
  #define CLK_CLK_ADSP2_SEL_MASK              (0x7<<8)      //
  #define CLK_CLK_ADSP2_SEL_27M               (0)
  #define CLK_CLK_ADSP2_SEL_ADSP              (1)
  #define CLK_CLK_ADSP2_SEL_MCK               (2)
  #define CLK_CLK_ADSP2_SEL_SYSPLL1           (3)
  #define CLK_CLK_ADSP2_SEL_SVOIPLL_1_2       (4)
  #define CLK_PDN_ADSP                        (1U<<7)       //turn off ADSP(if clk_ADSP!=clk_DRAM)
  #define CLK_CLK_ADSP_SEL_OFFSET             (4)           //Selection of ADSP clock frequency
  #define CLK_CLK_ADSP_SEL_MASK               (0x7<<4)      //
  #define CLK_CLK_ADSP_SEL_27M                (0)
  #define CLK_CLK_ADSP_SEL_SYSPLL2_2_3        (1)
  #define CLK_CLK_ADSP_SEL_ARMPLL_1_2         (2)
  #define CLK_CLK_ADSP_SEL_SYSPLL1_1_2        (3)
  #define CLK_CLK_ADSP_SEL_SYSPLL1_1_3        (4)
  #define CLK_CLK_ADSP_SEL_SYSPLL2_1_2        (5)
  #define CLK_CLK_ADSP_SEL_SYSPLL2_1_3        (6)
  #define CLK_CLK_ADSP_SEL_USBPLL_240M        (7)
  #define CLK_PDN_RISC                        (1U<<3)       //turn off RISC
  #define CLK_CLK_RISC_SEL_OFFSET             (0)           //Selection of RISC clock frequency
  #define CLK_CLK_RISC_SEL_MASK               (0x7<<0)      //
  #define CLK_CLK_RISC_SEL_27M                (0)
  #define CLK_CLK_RISC_SEL_ARMPLL             (1)
  #define CLK_CLK_RISC_SEL_SYSPLL1            (2)
  #define CLK_CLK_RISC_SEL_SYSPLL2            (3)
  #define CLK_CLK_RISC_SEL_SLOW_RISC_CLK      (4)
  #define CLK_CLK_RISC_SEL_MCK                (5)

#define REG_RW_CLK_CFG1                     0x0074          //Clock Selection Configuration 1
  #define CLK_PDN_NR                          (1U<<27)      //turn off NR clk
  #define CLK_CLK_NR_SEL_OFFSET               (24)          //Selection of NR clock frequency
  #define CLK_CLK_NR_SEL_MASK                 (0x7<<24)     //
  #define CLK_CLK_NR_SEL_27M                  (0)
  #define CLK_CLK_NR_SEL_SYSPLL1_1_3          (1)
  #define CLK_CLK_NR_SEL_SYSPLL1_1_4          (2)
  #define CLK_CLK_NR_SEL_SYSPLL2_1_3          (3)
  #define CLK_CLK_NR_SEL_MCK_1_2              (4)
  #define CLK_PDN_GRAPH                       (1U<<23)      //turn off GRAPH clk
  #define CLK_CLK_GRAPH_SEL_OFFSET            (20)          //Selection of GRAPH clock frequency
  #define CLK_CLK_GRAPH_SEL_MASK              (0x7<<20)     //
  #define CLK_CLK_GRAPH_SEL_27M               (0)
  #define CLK_CLK_GRAPH_SEL_APLL              (1)
  #define CLK_CLK_GRAPH_SEL_ARMPLL_1_2        (2)
  #define CLK_CLK_GRAPH_SEL_ARMPLL_1_3        (3)
  #define CLK_CLK_GRAPH_SEL_SYSPLL1_1_2       (4)
  #define CLK_CLK_GRAPH_SEL_SYSPLL2_1_2       (5)
  #define CLK_CLK_GRAPH_SEL_MCK               (6)
  #define CLK_CLK_GRAPH_SEL_USBPLL_240M       (7)
  #define CLK_PDN_RESZ                        (1U<<19)      //turn off RESZ
  #define CLK_CLK_RESZ_SEL_OFFSET             (16)          //Selection of RESZ clock frequency
  #define CLK_CLK_RESZ_SEL_MASK               (0x7<<16)     //
  #define CLK_CLK_RESZ_SEL_27M                (0)
  #define CLK_CLK_RESZ_SEL_SYSPLL1_1_2        (1)
  #define CLK_CLK_RESZ_SEL_SYSPLL2_1_2        (2)
  #define CLK_CLK_RESZ_SEL_USBPLL_240M        (3)
  #define CLK_CLK_RESZ_SEL_MCD_1_2            (4)
  #define CLK_PDN_PNG                         (1U<<15)      //turn off PNG
  #define CLK_CLK_PNG_SEL_OFFSET              (12)          //Selection of PNG clock frequency
  #define CLK_CLK_PNG_SEL_MASK                (0x7<<12)     //
  #define CLK_CLK_PNG_SEL_27M                 (0)
  #define CLK_CLK_PNG_SEL_APLL                (1)
  #define CLK_CLK_PNG_SEL_ARMPLL_1_2          (2)
  #define CLK_CLK_PNG_SEL_ARMPLL_1_3          (3)
  #define CLK_CLK_PNG_SEL_SYSPLL1_1_2         (4)
  #define CLK_CLK_PNG_SEL_SYSPLL2_1_2         (5)
  #define CLK_CLK_PNG_SEL_MCK                 (6)
  #define CLK_CLK_PNG_SEL_USBLL_240M          (7)
  #define CLK_PDN_OSD                         (1U<<11)      //turn off OSD
  #define CLK_CLK_OSD_SEL_OFFSET              (8)           //Selection of OSD clock frequency
  #define CLK_CLK_OSD_SEL_MASK                (0x7<<8)      //
  #define CLK_CLK_OSD_SEL_27M                 (0)
  #define CLK_CLK_OSD_SEL_SYSPLL1_1_2         (1)
  #define CLK_CLK_OSD_SEL_SYSPLL2_1_3         (2)
  #define CLK_CLK_OSD_SEL_USBPLL_240M         (3)
  #define CLK_CLK_OSD_SEL_MCK_1_2             (4)
  #define CLK_CLK_OSD_SEL_APLL                (5)
  #define CLK_CLK_OSD_SEL_ARMPLL_1_3          (6)
  #define CLK_CLK_OSD_SEL_SYSPLL2_1_2         (7)
  #define CLK_PDN_MC                          (1U<<7)       //turn off MC
  #define CLK_CLK_MC_SEL_OFFSET               (4)           //Selection of MC clock frequency
  #define CLK_CLK_MC_SEL_MASK                 (0x7<<4)      //
  #define CLK_CLK_MC_SEL_27M                  (0)
  #define CLK_CLK_MC_SEL_APLL                 (1)
  #define CLK_CLK_MC_SEL_MCK                  (2)
  #define CLK_CLK_MC_SEL_ARMPLL_1_3           (3)
  #define CLK_CLK_MC_SEL_SYSPLL1_1_2          (4)
  #define CLK_CLK_MC_SEL_SYSPLL2_1_2          (5)
  #define CLK_CLK_MC_SEL_SYSPLL2_1_3          (6)
  #define CLK_CLK_MC_SEL_USBPLL_240M          (7)
  #define CLK_PDN_VDEC                        (1U<<3)       //turn off VDEC
  #define CLK_CLK_VDEC_SEL_OFFSET             (0)           //Selection of VDEC clock frequency
  #define CLK_CLK_VDEC_SEL_MASK               (0x7<<0)      //
  #define CLK_CLK_VDEC_SEL_27M                (0)
  #define CLK_CLK_VDEC_SEL_SYSPLL1_1_2        (1)
  #define CLK_CLK_VDEC_SEL_SYSPLL2_1_3        (2)
  #define CLK_CLK_VDEC_SEL_USBPLL_240M        (3)
  #define CLK_CFG1_VDEC_SEL_MCK_1_2           (5)
  #define CLK_CFG1_VDEC_SEL_ARMPLL_1_3        (6)
  #define CLK_CFG1_VDEC_SEL_HA_DDS            (7)


#define REG_RW_CLK_CFG2                     0x0078          //Clock Selection Configuration 2
  #define CLK_PDN_ABIST                             (1U<<31)      //turn off ABIST frequency meter clock
  #define CLK_CFG2_ABIST_SEL_OFFSET                 (28)          //Selection of ABIST clock frequency
  #define CLK_CFG2_ABIST_SEL_MASK                   (0x7<<28)     //
  #define CLK_CFG2_ABIST_SEL_27M                    (0)
  #define CLK_CFG2_ABIST_SEL_AD_XTAL27M_CK          (1)
  #define CLK_CFG2_ABIST_SEL_AD_PLL_MONCK           (2)
  #define CLK_CFG2_ABIST_SEL_AD_MEMPLL_MONCK        (3)
  #define CLK_CFG2_ABIST_SEL_HDMI_LBOUT             (4)
  #define CLK_CFG2_ABIST_SEL_USB20_CLK480M          (5)
  #define CLK_CFG2_ABIST_SEL_SYSPLL1_1_5            (6)
  #define CLK_CFG2_ABIST_SEL_CLK_PLL                (7)
  #define CLK_PDN_ABIST2                            (1U <<19)     //turn off ABIST2 frequency meter clock
  #define CLK_CFG2_ABIST2_SEL_OFFSET                (16)          //Selection of ABIST2 clock frequency
  #define CLK_CFG2_ABIST2_SEL_MASK                  (0x7<<16)     //
  #define CLK_CFG2_ABIST2_SEL_27M                   (0)
  #define CLK_CFG2_ABIST2_SEL_P0_AD_PL_RCLK250      (1)
  #define CLK_CFG2_ABIST2_SEL_AD_USB20_MONCLK       (2)
  #define CLK_CFG2_ABIST2_SEL_AD_USB20_MONCLK_1P    (3)
  #define CLK_CFG2_ABIST2_SEL_AL0_IN                (4)
  #define CLK_CFG2_ABIST2_SEL_AR0_IN                (5)
  //#define CLK_CFG2_ABIST2_SEL_AD_HDMI_LBOUT_SUB     (6)
  #define CLK_CFG2_ABIST2_SEL_PLL_CLOCK_MUX_OUTPUT  (7)
  #define CLK_PDN_SD_HCLK                           (1U<<15)      //turn off SD
  #define CLK_CFG2_SD_HCLK_SEL_OFFSET               (13)           //Selection of SD clock frequency
  #define CLK_CFG2_SD_HCLK_SEL_MASK                 (0x3<<13)     //
  #define CLK_CFG2_SD_HCLK_SEL_27M                  (0x00)
  #define CLK_CFG2_SD_HCLK_SEL_SYSPLL1_1_8          (0x01)
  #define CLK_CFG2_SD_HCLK_SEL_SYSPLL2_1_12         (0x02)
  #define CLK_CFG2_SD_HCLK_SEL_SYSPLL1_1_10         (0x03)
  #define CLK_PDN_SD                                (1U<<12)      //turn off SD
  #define CLK_CFG2_SD_SEL_OFFSET                    (5)           //Selection of SD clock frequency
  #define CLK_CFG2_SD_SEL_MASK                      (0x7F<<5)     //
  #define CLK_CFG2_SD_SEL_27M                       (0x00)
  #define CLK_CFG2_SD_SEL_SYSPLL1_1_8               (0x01)
  #define CLK_CFG2_SD_SEL_SYSPLL1_1_10              (0x02)
  #define CLK_CFG2_SD_SEL_SYSPLL1_1_12              (0x03)
  #define CLK_CFG2_SD_SEL_SYSPLL2_1_12              (0x04)
  #define CLK_CFG2_SD_SEL_MCK_1_8                   (0x05)
  #define CLK_CFG2_SD_SEL_SYSPLL2_1_18              (0x06)
  #define CLK_CFG2_SD_SEL_SYSPLL2_1_26              (0x07)
  #define CLK_CFG2_SD_SEL_SYSPLL1_1_24              (0x08)
  #define CLK_CFG2_SD_SEL_27M_1_2                   (0x09)
  #define CLK_CFG2_SD_SEL_27M_1_4                   (0x19)
  #define CLK_CFG2_SD_SEL_27M_1_8                   (0x29)
  #define CLK_CFG2_SD_SEL_27M_1_16                  (0x39)
  #define CLK_CFG2_SD_SEL_27M_1_32                  (0x49)
  #define CLK_CFG2_SD_SEL_27M_1_64                  (0x59)
  #define CLK_CFG2_SD_SEL_27M_1_128                 (0x69)
  #define CLK_CFG2_SD_SEL_27M_1_256                 (0x79)
  #define CLK_PDN_IR                                (1U<<4)       //turn off IR
  #define CLK_CFG2_IR_DIV_SEL_OFFSET                (0)           //Select divisor used to divide 27MHz clock, for IR clock
  #define CLK_CFG2_IR_DIV_SEL_MASK                  (0xF<<0)      //
  #define CLK_CFG2_IR_DIV_SEL_27M                   (0)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_2               (1)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_4               (2)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_8               (3)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_16              (4)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_32              (5)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_64              (6)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_128             (7)
  #define CLK_CFG2_IR_DIV_SEL_27M_1_256             (8)

#define REG_RW_CLK_CFG3                     0x007C          //Clock Selection Configuration 3
  #define CLK_PDN_BCLK                        (1U<<31)      //turn off bclk
  #define CLK_CFG3_CLK_BCLK_SEL_OFFSET        (28)          //Selection of bclk clock frequency
  #define CLK_CFG3_CLK_BCLK_SEL_MASK          (0x7<<28)     //
  #define CLK_CFG3_CLK_BCLK_SEL_27M           (0)
  #define CLK_CFG3_CLK_BCLK_SEL_MCK           (1)
  #define CLK_CFG3_CLK_BCLK_SEL_SYSPLL1_2_3   (2)
  #define CLK_CFG3_CLK_BCLK_SEL_SYSPLL1_1_2   (3)
  #define CLK_CFG3_CLK_BCLK_SEL_SYSPLL1_1_3   (4)
  #define CLK_CFG3_CLK_BCLK_SEL_SYSPLL2_1_2   (5)
  #define CLK_CFG3_CLK_BCLK_SEL_SVOIPLL_1_2   (6)
  #define CLK_CFG3_CLK_BCLK_SEL_BCLK_ORG      (7)
  #define CLK_PDN_SYS_SUB                     (1U<<19)      //turn off clk_sys_sub
  #define CLK_CFG3_SYS_SUB_SEL_OFFSET         (16)          //Selection of sys_sub clock frequency
  #define CLK_CFG3_SYS_SUB_SEL_MASK           (0x7<<16)     //
  #define CLK_CFG3_SYS_SUB_SEL_27M            (0)
  #define CLK_CFG3_SYS_SUB_SEL_SYSPLL1_1_6    (1)
  #define CLK_CFG3_SYS_SUB_SEL_SYSPLL2_1_8    (2)
  #define CLK_PDN_JPEG                        (1U<<15)      //turn off clk_jpeg
  #define CLK_CFG3_JPEG_SEL_OFFSET            (12)          //Selection of jpeg clock frequency
  #define CLK_CFG3_JPEG_SEL_MASK              (0x7<<12)     //
  #define CLK_CFG3_JPEG_SEL_27M               (0)
  #define CLK_CFG3_JPEG_SEL_SYSPLL2_1_4       (1)
  #define CLK_CFG3_JPEG_SEL_SYSPLL2_1_3       (2)
  #define CLK_CFG3_JPEG_SEL_SYSPLL1_1_8       (3)
  #define CLK_CFG3_JPEG_SEL_SYSPLL1_1_6       (4)
  #define CLK_CFG3_JPEG_SEL_SYSPLL1_1_4       (5)
  #define CLK_CFG3_JPEG_SEL_SYSPLL1_1_2       (6)
  #define CLK_CFG3_JPEG_SEL_MCK               (7)
  #define CLK_PDN_MVDO2                       (1U<<11)      //turn off clk_mvdo2
  #define CLK_CLK_MVDO2_SEL_OFFSET            (8)           //Select divisor used to divide 27MHz clock, for mvdo2 clock
  #define CLK_CLK_MVDO2_SEL_MASK              (0x7<<8)      //
  #define CLK_CLK_MVDO2_SEL_27M               (0)
  #define CLK_CLK_MVDO2_SEL_SYSPLL1_1_3       (1)
  #define CLK_CLK_MVDO2_SEL_SYSPLL2_1_4       (2)
  #define CLK_PDN_GCPU                        (1U<<7)       //turn off clk_gcpu
  #define CLK_CLK_GCPU_SEL_OFFSET             (4)           //Select divisor used to divide 27MHz clock, for gcpu clock
  #define CLK_CLK_GCPU_SEL_MASK               (0x7<<4)      //
  #define CLK_CLK_GCPU_SEL_27M                (0)
  #define CLK_CLK_GCPU_SEL_MCK K              (1)
  #define CLK_CLK_GCPU_SEL_SYSPLL1_1_2        (2)
  #define CLK_CLK_GCPU_SEL_SYSPLL2_1_2        (3)
  #define CLK_CLK_GCPU_SEL_SVOIPLL_1_2        (4)
  #define CLK_CLK_GCPU_SEL_SYSPLL1_2_3        (5)
  #define CLK_CLK_GCPU_SEL_SYSPLL2_1_3        (6)
  #define CLK_PDN_SVO_STDBY                   (1U<<3)       //turn off clk_svo_stdby
  #define CLK_CLK_SVO_STDBY_SEL_OFFSET        (0)           //Selection of svo_stdby clock frequency
  #define CLK_CLK_SVO_STDBY_SEL_MASK          (0x7<<0)      //
  #define CLK_CLK_SVO_STDBY_SEL_27M           (0)
  #define CLK_CLK_SVO_STDBY_SEL_SVOIPLL_1_27  (1)

#define REG_RW_CLK27CALI                    0x0080          //CLK27 Calibration
  #define CLK27CALI_CAL_CNT_OFFSET            (16)          //Read only,report result of calibration
  #define CLK27CALI_CAL_CNT_MASK              (0xFFFF<<16)  //Frequency of PLL = (27Mhz x CAL_CNT) / (1024)
  #define CLK27CALI_CLK_EXC_PLL               (1U<<2)       //Exc clock; 0: 27MHz; 1: PLL clock
  #define CLK27CALI_PLL_TEST                  (1U<<1)       //PLL test clock; 0: PLL clock; 1: test clock
  #define CLK27CALI_TRI_CAL                   (1U<<0)       //start to do calibration on ARMPLL.


#define REG_RW_CKSTA_REG                    0x0088                //Clock change status
  #define CKSTA_REG_CHG_STA                         (1U<<0)       //1: clock change in pregress

#define REG_RW_CLK_CFG4                     0x008C                //Clock Selection Configuration 4 : HDMI rx

#define REG_RW_CLK_CFG5                     0x0090                //Clock Selection Configuration 5 : HDMI rx

#define REG_RW_CLK_CFG6                     0x0094                //Clock Selection Configuration 6
  #define CLK_CFG6_ROSC_CLK_SEL_OFFSET              (30)      //ROSC clk sel
  #define CLK_CFG6_ROSC_CLK_SEL_MASK                (0x3<<30)     //
  #define CLK_CFG6_ROSC_CKGEN_RING_OSC              (0)
  #define CLK_CFG6_ROSC_IP_YARING_OSC               (1)
  #define CLK_CFG6_ROSC_ARM_RING_OSC                (2)
  #define CLK_CFG6_ROSC_DISP_RING_OSC               (3)
  #define CLK_PDN_DISP_144M                         (1U<<21)      //turn off DISP_144M
  #define CLK_CFG6_DISP_144M_SEL_OFFSET             (18)          //Selection of DISP_144M clock frequency
  #define CLK_CFG6_DISP_144M_SEL_MASK               (0x7<<18)     //
  #define CLK_CFG6_DISP_144M_SEL_27M                (0)
  #define CLK_CFG6_DISP_144M_SEL_SYSPLL1_1_3        (1)
  #define CLK_CFG6_DISP_144M_SEL_SYSPLL2_1_4        (2)
  #define CLK_CFG6_PDN_PLL                          (1U<<17)      //turn off pll
  #define CLK_CFG6_PLL_SEL_OFFSET                   (13)          //Selection of pll clock frequency
  #define CLK_CFG6_PLL_SEL_MASK                     (0xf<<13)     //
  #define CLK_CFG6_PLL_SEL_27M                      (0)
  #define CLK_CFG6_PLL_SEL_ARMPLL                   (2)
  #define CLK_CFG6_PLL_SEL_SYSPLL1                  (3)
  #define CLK_CFG6_PLL_SEL_SYSPLL2                  (4)
  #define CLK_CFG6_PLL_SEL_SIG_APLL_A2              (5)
  #define CLK_CFG6_PLL_SVOIPLL                      (6)
  #define CLK_CFG6_PLL_SYSPLL1_1P5                  (7)
  #define CLK_CFG6_PLL_SYSPLL2_1P5                  (8)
  #define CLK_CFG6_PLL_USB_D2                       (9)
  #define CLK_CFG6_PLL_DMPLL                        (11)
  #define CLK_CFG6_PLL_SATAPLL                      (12)
  #define CLK_CFG6_PLL_CA9_ROSC                     (13)
  #define CLK_CFG6_PLL_MALI_ROSC                    (14)
  #define CLK_CFG6_PLL_AUD_ROSC                     (15)
  #define CLK_PDN_SPDIFIN                           (1U<<3)       //turn off spdifin frequency meter clock
  #define CLK_SPDIFIN_MASK      (0x7<<0)
  #define CLK_SPDIFIN_27M       (0x0<<0)
  #define CLK_SPDIFIN_SYSPLL1	(0x1<<0)
  #define CLK_SPDIFIN_SYSPLL2	(0x2<<0)
  #define CLK_SPDIFIN_SVOIPLL	(0x3<<0)
  #define CLK_SPDIFIN_USBCLK	(0x4<<0)
  #define CLK_SPDIFIN_ADPLL93M	(0x5<<0)

#define REG_RW_CLK_CFG7                     0x0098                //Clock Selection Configuration 7
  #define CLK_PDN_SD_HCLK2                          (1U<<10)      //turn off SD_HCLK2
  #define CLK_CFG7_SD_HCLK2_SEL_OFFSET              (8)           //Selection of SD_HCLK2 clock frequency
  #define CLK_CFG7_SD_HCLK2_SEL_MASK                (0x3<<8)      //
  #define CLK_CFG7_SD_HCLK2_SEL_27M                 (0)
  #define CLK_CFG7_SD_HCLK2_SEL_SYSPLL1_1_8         (1)
  #define CLK_CFG7_SD_HCLK2_SEL_SYSPLL2_1_12        (2)
  #define CLK_CFG7_SD_HCLK2_SEL_SYSPLL1_1_10        (3)
  #define CLK_PDN_SD2                               (1U<<7)       //turn off sd2
  #define CLK_CFG7_SD2_SEL_OFFSET                   (0)           //Selection of sd2 clock frequency
  #define CLK_CFG7_SD2_SEL_MASK                     (0x3f<<0)     //
  #define CLK_CFG7_SD2_SEL_27M                      (0x00)
  #define CLK_CFG7_SD2_SEL_SYSPLL1_1_8              (0x01)
  #define CLK_CFG7_SD2_SEL_SYSPLL1_1_10             (0x02)
  #define CLK_CFG7_SD2_SEL_SYSPLL1_1_12             (0x03)
  #define CLK_CFG7_SD2_SEL_SYSPLL2_1_12             (0x04)
  #define CLK_CFG7_SD2_SEL_MCK_1_8                  (0x05)
  #define CLK_CFG7_SD2_SEL_SYSPLL2_1_18             (0x06)
  #define CLK_CFG7_SD2_SEL_SYSPLL2_1_26             (0x07)
  #define CLK_CFG7_SD2D_SEL_SYSPLL1_1_24            (0x08)
  #define CLK_CFG7_SD2_SEL_27M_1_2                  (0x09)
  #define CLK_CFG7_SD2_SEL_27M_1_4                  (0x19)
  #define CLK_CFG7_SD2_SEL_27M_1_8                  (0x29)
  #define CLK_CFG7_SD2_SEL_27M_1_16                 (0x39)
  #define CLK_CFG7_SD2_SEL_27M_1_32                 (0x49)
  #define CLK_CFG7_SD2_SEL_27M_1_64                 (0x59)
  #define CLK_CFG7_SD2D_SEL_27M_1_128               (0x69)
  #define CLK_CFG7_SD2_SEL_27M_1_256                (0x79)

#define REG_RW_CLK_CFG8                     0x009C                //Clock Selection Configuration 8
  #define CLK_PDN_ABIST5                            (1U<<29)      //turn off abist5
  #define CLK_CFG8_ABIST5_SEL_OFFSET                (26)          //Selection of abist5 clock frequency
  #define CLK_CFG8_ABIST5_SEL_MASK                  (0x7<<26)     //
  #define CLK_CFG8_ABIST5_SEL_27M                   (0)
  #define CLK_PDN_ABIST4                            (1U<<25)      //turn off abist4
  #define CLK_CFG8_ABIST4_SEL_OFFSET                (22)          //Selection of abist4 clock frequency
  #define CLK_CFG8_ABIST4_SEL_MASK                  (0x7<<22)     //
  #define CLK_CFG8_ABIST4_SEL_27M                   (0)
  #define CLK_CFG8_ABIST4_SEL_AD_HDMI_LBOUT         (1)
  #define CLK_CFG8_ABIST4_SEL_AD_USB20_CLK480M      (2)
  #define CLK_CFG8_ABIST4_SEL_SACDIF_CBLT_CK        (3)
  #define CLK_CFG8_ABIST4_SEL_BE_MDAC_ABIST_C       (4)
  #define CLK_CFG8_ABIST4_SEL_BE_PML_ABIST_CK       (5)
  #define CLK_PDN_ABIST3                            (1U<<21)      //turn off abist3
  #define CLK_CFG8_ABIST3_SEL_OFFSET                (18)          //Selection of abist3 clock frequency
  #define CLK_CFG8_ABIST3_SEL_MASK                  (0x7<<18)     //
  #define CLK_CFG8_ABIST3_SEL_27M                   (0)
  #define CLK_CFG8_ABIST3_SEL_AD_SYSPLL1_CLKOUT1P5  (1)
  #define CLK_CFG8_ABIST3_SEL_AD_SYSPLL2_CLKOUT1P5  (2)
  #define CLK_CFG8_ABIST3_SEL_MEMREF_D16_CK         (3)
  #define CLK_CFG8_ABIST3_SEL_MEMFB_D16_CK          (4)
  #define CLK_CFG8_ABIST3_SEL_SPDIFIN_COASIAL       (5)
  #define CLK_CFG8_ABIST3_SEL_SPDIFIN_ARC           (6)
  #define CLK_PDN_VDEC_PRE                          (1U<<3)      //turn off VDEC_PRE
  #define CLK_CFG8_VDEC_PRE_SEL_OFFSET              (0)          //Selection of VDEC_PRE clock frequency
  #define CLK_CFG8_VDEC_PRE_SEL_MASK                (0x7<<0)     //
  #define CLK_CFG8_VDEC_PRE_SEL_27M                 (0)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_2_3         (1)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_1_2         (2)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_1_3         (3)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_1_4         (4)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_1_6         (5)
  #define CLK_CFG8_VDEC_PRE_SEL_SYSPLL1_1_8         (6)
  #define CLK_CFG8_VDEC_PRE_SEL_SVOIPLL_1_2         (7)

//======PAD Configuration***================================
#define REG_RW_PAD_CFG_0                0x00C0              //Pad Multifunction Configuration Register 0
    #define AUD_MCLK_SEL_MASK                           (3<<0)
        #define AUD_MCLK_SEL_AOMCLK_AOBCK_AOLRCK        (1<<0)
        #define AUD_MCLK_SEL_AOSDATA1_AOSDATA2_AOSDATA3 (2<<0)
    #define AOSDATA0_SEL        (0x01<<6)
    #define AOSDATA1_SEL        (0x01<<7)
    #define AOSDATA2_SEL        (0x01<<8)
    #define AOSDATA3_SEL        (0x01<<9)
    #define AOSDATA4_SEL        (0x01<<10)
    #define SPDIF_SEL           (0x01<<14)
    //#define AUD_MCIN_SEL_MASK                           (3<<12)
        //#define AUD_MCIN_SEL_VOUTD11                     (1<<12)
        //#define AUD_MCIN_SEL_GPIO4                       (2<<12)
        //#define AUD_MCIN_SEL_AOSDATA4                    (3<<12)
    //#define SPDATA0_SPDATA                       (1<<15)
    //#define SPDATA0_AOSDATA3                     (2<<15)
    //#define SPDATA0_AOSDATA4                     (3<<15)
    //#define SPDATA0_AOSDATA5                     (4<<15)
    //#define SPDATA0_MCIN                         (5<<15)
    // #define SPDATA0_MCIN                    (2<<15)
    //#define SPDATA0_GPIO3                     (3<<15)
    // #define SPDATA0_VOUTD7                     (4<<15)
    //#define SPDATA0_VOUTCLK1                     (5<<15)
    //#define SPDATA0_MASK                         (7<<15)
    //#define LINEIN_SPDATA_SEL_MASK      (0x03<<2)
    //#define LINEIN_SPDATA_SEL_GPIO3       (0x01<<2)
    //#define LINEIN_SPDATA_SEL_VOUTD7      (0x02<<2)
    //#define LINEIN_SPDATA_SEL_AOSDATA4      (0x03<<2)
    //#define AUD_SDATA5_SEL_MASK     (0x01<<11)
    #define AUD_MUTE2_SEL_MASK      (0x07<<3)
    #define AUD_MUTE2_AOLRCK       (0x01<<3)
    #define AUD_MUTE_SEL_MASK       (0x01<<11)

#define REG_RW_PAD_CFG_1                    0x00C4        //Pad Multifunction Configuration Register 1
   #define JTAG_SEL_OFST                      (20)
   #define JTAG_SEL_MASK                      (0x7<<20)
   #define JTAG_SEL_VOUTD                     (0)
   #define JTAG_SEL_HDMI                      (1)
   #define JTAG_SEL_USB10                     (4)
   #define JTAG_SEL_USB21                     (5)
   #define JTAG_SEL_USB02                     (6)

#define REG_RW_PAD_CFG_2                0x00C8        //Pad Multifunction Configuration Register 2
    #define EXT_INT1_PIN_MASK_                   (0xF<<12)
    #define EXT_INT1_PIN_SCL_                    (1<<12)
    #define EXT_INT1_PIN_GPIO20_                 (2<<12)
    #define EXT_INT1_PIN_VOUTCSYNC_              (3<<12)
    #define EXT_INT1_PIN_VOUTD10_                (4<<12)
    #define EXT_INT1_PIN_VOUT13_                 (5<<12)
    #define EXT_INT1_PIN_GPIO8_                  (6<<12)
    #define EXT_INT1_PIN_GPIO5_                  (7<<12)
    #define EXT_INT1_PIN_GPIO2_                  (8<<12)
    #define EXT_INT1_PIN_AORCK_                  (9<<12)
    #define EXT_INT1_PIN_AOSDATA2_               (0xA<<12)
    #define EXT_INT2_PIN_MASK_                   (0xF<<16)
    #define EXT_INT2_PIN_SDA_                    (1<<16)
    #define EXT_INT2_PIN_GPIO21_                 (2<<16)
    #define EXT_INT2_PIN_VOUTHSYNC               (3<<16)
    #define EXT_INT2_PIN_VOUTTD11_               (4<<16)
    #define EXT_INT2_PIN_VOUTTD14_               (5<<16)
    #define EXT_INT2_PIN_GPIO7_                  (6<<16)
    #define EXT_INT2_PIN_GPIO4_                  (7<<16)
    #define EXT_INT2_PIN_GPIO1_                  (8<<16)
    #define EXT_INT2_PIN_AOSDATA0_               (9<<16)
    #define EXT_INT2_PIN_AOSDATA3_               (0xA<<16)

#define REG_RW_PAD_CFG_3                0x00CC        //Pad Multifunction Configuration Register 3
   #define   AOMCLK_PWM_SEL_MASK          (0x1<<8)     // PWM FIFO selection for AOMCLK
     #define  AOMCLK_PWM_SEL_AOMCLK       (0x0<<8)
     #define  AOMCLK_PWM_SEL_PWM_CH0      (0x1<<8)
   #define   AOBCK_PWM_SEL_MASK           (0x1<<9)      // PWM FIFO selection for AOBCK
     #define  AOBCK_PWM_SEL_AOBCK         (0x0<<9)
     #define  AOBCK_PWM_SEL_PWM_CH1       (0x1<<9)

   //#define    HDMI_RX_SEL_MASK                               (0x1<<12)
   //    #define HDMI_RX_SEL_PIN                                   (0x0<<12)   //  audio input from pin
   // #define HDMI_RX_SEL_INTERNAL                    (0x1<<12)  // HDMI RX audio output select to ADSP
   //#define    HDMI_RX_SPDF2HDMITX_MASK         (0x1<<13)
   //    #define HDMI_RX_SPDF2HDMITX                       (0x1<<13)   // Internal HDMI Rx SPDIF out direct to HDMI Tx
   //#define    HDMI_RX_ACK_SEL_MASK                    (0x1<<14)
   //    #define HDMI_RX_ACK_SEL_PIN                          (0x0<<14)   // Pin in mclk as Audio mclk
   //    #define HDMI_RX_ACK_SEL_INTERNAL             (0x1<<14)   // HDMI RX inside gen mclk as Audio mclk

#define REG_RW_PAD_CFG_4                0x00D0        //Pad Multifunction Configuration Register 4
//CONFIG_DRV_HDMI_RX
#define REG_RW_PAD_CFG_5                0x00D4        //Pad Multifunction Configuration Register 5
     #define RW_PAD_CFG_5_EXT_ACLK_MASK  (7<<8)
        #define EXT_ACLK_ACLK_GEN               (0<<8)    // ACLK Gen
        #define EXT_ACLK_PAD_AOMCLK             (2<<8)     // PAD_PWM/PAD_AOMCLK  : b' x10
        #define EXT_ACLK_SPMCLK_INT_IN          (3<<8)   // spmclk_int_in : b'011
        #define EXT_ACLK_PAD_SPMCLK             (7<<8)    //PAD_PWM/PAD_SPMCLK  :  b'111
     #define MCIN_MPMCLK_SEL_MASK   (0x7<<15)
        #define MCIN_MPMCLK_SEL_VOUTD15         (1<<15)
        #define MCIN_MPMCLK_SEL_GPIO2           (2<<15)
        #define MCIN_MPMCLK_SEL_AOSDATA3        (3<<15)
     #define MCIN_MPBCK_SEL_MASK    (0x7<<18)
        #define MCIN_MPBCK_SEL_VOUTD13          (1<<18)
        #define MCIN_MPBCK_SEL_GPIO4            (2<<18)
        #define MCIN_MPBCK_SEL_AOSDATA1         (3<<18)
     #define MCIN_MPLRCK_SEL_MASK   (0x7<<21)
        #define MCIN_MPLRCK_SEL_VOUTD12         (1<<21)
        #define MCIN_MPLRCK_SEL_GPIO5           (2<<21)
        #define MCIN_MPLRCK_SEL_AOSDATA0        (3<<21)
     #define RW_PAD_CFG_5_SPDATA1_SEL_MASK (0x7<<24)
        #define MCIN_MPLRCK_SEL_VOUTD14         (1<<21)
        #define MCIN_MPLRCK_SEL_GPIO3           (2<<21)
        #define MCIN_MPLRCK_SEL_AOSDATA2        (3<<21)
        #define MCIN_MPLRCK_SEL_SPDIFIN0        (4<<21)
     #define RW_PAD_CFG_5_SPDATA3_SEL_MASK (3<<30)
        #define PAD_GPIO6                       (1<<30)
        #define PAD_VOUTD6                      (2<<30)
        #define PAD_VOUTD12                     (3<<30)

#define REG_RW_PAD_CFG_6                0x00D8        //Pad Multifunction Configuration Register 6
#define REG_RW_PAD_CFG_7                0x00DC        //Pad Multifunction Configuration Register 7
#define REG_RW_PAD_CFG_8                0x00E0        //Pad Multifunction Configuration Register 8
#define REG_RW_PAD_CFG_11                0x00EC       //Pad Multifunction Configuration Register 11
  //#define SPDIF_IN_COAXIAL_SPBCK        (0x01<<0)
  //#define SPDIF_IN_COAXIAL_VIND35       (0x01<<1)
  //#define SPDIF_IN_OPTICAL_SPMCLK       (0x01<<2)
  //#define SPDIF_IN_OPTICAL_VIND34       (0x01<<3)
  #define JTAG_HDMI_EN              (0x01<<16)
  //#define AUD2_SEL_MASK           (0x03<<4)
  //#define AUD2_SEL1     (0x01<<4) //AO2MCLK:VOUTDHSYNC, AO2BCK:VOUTDVSYNC......, AO2SDATA2:VOUTD2
  //#define AUD2_SEL2     (0x02<<4)//AO2MCLK:VIND24, AO2BCK:VIND25......, AO2SDATA2:VOUTD2
  //#define AUD2_SEL3     (0x03<<4)//AO2MCLK:VIND8, AO2BCK:VIND9......, AO2SDATA2:VIND13

#define REG_RW_PAD_CFG_9                0x00E4        //Pad Multifunction Configuration Register 9
  #define SPDIF_IN_OPTICAL_SEL_MASK       (0x7<<22)
  #define SPDIF_IN_OPTICAL_SEL_NONE       (0x0<<22)
  #define SPDIF_IN_OPTICAL_SEL_GPIO22     (0x1<<22)
  #define SPDIF_IN_OPTICAL_SEL_GPIO8      (0x2<<22)
  #define SPDIF_IN_OPTICAL_SEL_GPIO0      (0x3<<22)
  #define SPDIF_IN_OPTICAL_SEL_AOSDATA3   (0x4<<22)
  #define SPDIF_IN_OPTICAL_SEL_SPDIFIN0   (0x5<<22)

#define REG_RW_PAD_CTRL_0               0x0100        //Pad PU/PD/SMT/SR/E2/E4 Control Register 0
  #define AOMCLK_DRV_CUR_MASK       (3<<0)
  #define AOMCLK_DRV_CUR_0                           (0<<0) // 4mA
  #define AOMCLK_DRV_CUR_1                           (1<<0) // 8mA
  #define AOMCLK_DRV_CUR_2                           (2<<0) // 12mA
  #define AOMCLK_DRV_CUR_3                           (3<<0) // 16mA
  #define AOBCK_DRV_CUR_MASK        (3<<6)
  #define AOBCK_DRV_CUR_0           (0<<6) // 4mA
  #define AOBCK_DRV_CUR_1           (1<<6) // 8mA
  #define AOBCK_DRV_CUR_2           (2<<6) // 12mA
  #define AOBCK_DRV_CUR_3           (3<<6) // 16mA
  #define AOSDATA0_DRV_CUR_MASK     (3<<18)
  #define AOSDATA0_DRV_CUR_0            (0<<18) // 4mA
  #define AOSDATA0_DRV_CUR_1            (1<<18) // 8mA
  #define AOSDATA0_DRV_CUR_2            (2<<18) // 12mA
  #define AOSDATA0_DRV_CUR_3            (3<<18) // 16mA  #define AOSDATA0_DRV_CUR_MASK      (3<<18)
  #define AOSDATA1_DRV_CUR_MASK         (3<<24)
  #define AOSDATA1_DRV_CUR_0            (0<<24) // 4mA
  #define AOSDATA1_DRV_CUR_1            (1<<24) // 8mA
  #define AOSDATA1_DRV_CUR_2            (2<<24) // 12mA
  #define AOSDATA1_DRV_CUR_3            (3<<24) // 16mA  #define AOSDATA0_DRV_CUR_MASK      (3<<18)
  #define AOSDATA2_DRV_CUR_MASK         (3<<30)
  #define AOSDATA2_DRV_CUR_0            (0<<30) // 4mA
  #define AOSDATA2_DRV_CUR_1            (1<<30) // 8mA
  #define AOSDATA2_DRV_CUR_2            (2<<30) // 12mA
  #define AOSDATA2_DRV_CUR_3            (3<<30) // 16mA

#define REG_RW_PAD_CTRL_1               0x0104       //Pad PU/PD/SMT/SR/E2/E4 Control Register 1
  #define SPDIF_DRV_MASK                 (3<<22)
  #define SPDIF_DRV_CUR_0                (0<<22)
  #define SPDIF_DRV_CUR_1                (1<<22)
  #define SPDIF_DRV_CUR_2                (2<<22)
  #define SPDIF_DRV_CUR_3                (3<<22)
  #define AOSDATA3_DRV_CUR_MASK          (3<<4)
  #define AOSDATA3_DRV_CUR_0             (0<<4) // 4mA
  #define AOSDATA3_DRV_CUR_1             (1<<4) // 8mA
  #define AOSDATA3_DRV_CUR_2             (2<<4) // 12mA
  #define AOSDATA3_DRV_CUR_3             (3<<4) // 16mA
  #define SPDIF_INO_DRV_MASK             (3<<10)
  #define SPDIF_INO_DRV_CUR_0            (0<<10) // 4mA
  #define SPDIF_INO_DRV_CUR_1            (1<<10) // 8mA
  #define SPDIF_INO_DRV_CUR_2            (2<<10) // 12mA
  #define SPDIF_INO_DRV_CUR_3            (3<<10) // 16mA

#define REG_RW_PAD_CTRL_2               0x0108       //Pad PU/PD/SMT/SR/E2/E4 Control Register 2
#define REG_RW_PAD_CTRL_3               0x010C       //Pad PU/PD/SMT/SR/E2/E4 Control Register 3
#define REG_RW_PAD_CTRL_4               0x0110       //Pad PU/PD/SMT/SR/E2/E4 Control Register 4
#define REG_RW_PAD_CTRL_5               0x0114       //Pad PU/PD/SMT/SR/E2/E4 Control Register 5
#define REG_RW_PAD_CTRL_6               0x0118       //Pad PU/PD/SMT/SR/E2/E4 Control Register 6
#define REG_RW_PAD_CTRL_7               0x011C       //Pad PU/PD/SMT/SR/E2/E4 Control Register 7
#define REG_RW_PAD_CTRL_8               0x0120       //Pad PU/PD/SMT/SR/E2/E4 Control Register 8
#define REG_RW_PAD_CTRL_9               0x0124       //Pad PU/PD/SMT/SR/E2/E4 Control Register 9
#define REG_RW_PAD_CTRL_10              0x0128       //Pad PU/PD/SMT/SR/E2/E4 Control Register 10
#define REG_RW_PAD_CTRL_11              0x012C       //Pad PU/PD/SMT/SR/E2/E4 Control Register 11
#define REG_RW_PAD_CTRL_12              0x0130       //Pad PU/PD/SMT/SR/E2/E4 Control Register 12
#define REG_RW_PAD_CTRL_13              0x0134       //Pad PU/PD/SMT/SR/E2/E4 Control Register 13
#define REG_RW_PAD_CTRL_14              0x0138       //Pad PU/PD/SMT/SR/E2/E4 Control Register 14
#define REG_RW_PAD_CTRL_15              0x013C       //Pad PU/PD/SMT/SR/E2/E4 Control Register 15
  #define  REG_RW_COAXIAL_G                (1<<22)      // coaxial pad for analog mode
  #define  REG_RW_ARC_G                    (1<<29)      // arc pad for analog mode
#define REG_RW_PAD_CTRL_16              0x0140       //Pad PU/PD/SMT/SR/E2/E4 Control Register 16
#define REG_RW_PAD_CTRL_17              0x0144       //Pad PU/PD/SMT/SR/E2/E4 Control Register 17

#define REG_RW_PAD_CTRL_20              0x0150       //Pad PU/PD/SMT/SR/E2/E4 Control Register 20
  #define VIND8_DRV_CUR_MASK        (3<<2)
  #define VIND8_DRV_CUR_0           (0<<2) // 4mA
  #define VIND8_DRV_CUR_1           (1<<2) // 8mA
  #define VIND8_DRV_CUR_2           (2<<2) // 12mA
  #define VIND8_DRV_CUR_3           (3<<2) // 16mA
  #define VIND9_DRV_CUR_MASK        (3<<8)
  #define VIND9_DRV_CUR_0           (0<<8) // 4mA
  #define VIND9_DRV_CUR_1           (1<<8) // 8mA
  #define VIND9_DRV_CUR_2           (2<<8) // 12mA
  #define VIND9_DRV_CUR_3           (3<<8) // 16mA
  #define VIND11_DRV_CUR_MASK       (3<<20)
  #define VIND11_DRV_CUR_0          (0<<20) // 4mA
  #define VIND11_DRV_CUR_1          (1<<20) // 8mA
  #define VIND11_DRV_CUR_2          (2<<20) // 12mA
  #define VIND11_DRV_CUR_3          (3<<20) // 16mA


#define REG_RW_GPIO_EN0                 0x0180       //GPIO Output Enable Control Register 0
#define REG_RW_GPIO_EN1                 0x0184       //GPIO Output Enable Control Register 1
#define REG_RW_GPIO_EN2                 0x0188       //GPIO Output Enable Control Register 2
#define REG_RW_GPIO_EN3                 0x018C       //GPIO Output Enable Control Register 3

#define REG_RW_GPIO_OUT0                0x01A0       //GPIO Output Register 0
#define REG_RW_GPIO_OUT1                0x01A4       //GPIO Output Register 1
#define REG_RW_GPIO_OUT2                0x01A8       //GPIO Output Register 2
#define REG_RW_GPIO_OUT3                0x01AC       //GPIO Output Register 3

#define REG_RW_GPIO_IN0                 0x01C0       //GPIO Input Register 0
#define REG_RW_GPIO_IN1                 0x01C4       //GPIO Input Register 1
#define REG_RW_GPIO_IN2                 0x01C8       //GPIO Input Register 2
#define REG_RW_GPIO_IN3                 0x01CC       //GPIO Input Register 3

#define REG_RW_PAD_FECTL_0              0x0280       // FE Pad Control Selection 0
#define REG_RW_PAD_FECTL_1              0x0284       // FE Pad Control Selection 1
#define REG_RW_PAD_PWMCTL_0                 0x0288      // PWM Pad Control Selection 0
     #define RW_PAD_HIN_SPMCLK2_SEL_MASK (3<<16)
     #define PAD_GPIO8                          (1<<16)
     #define RW_PAD_HIN_SPBCK2_SEL_MASK (3<<18)
     #define PAD_ETMDIO                          (1<<18)
     #define RW_PAD_HIN_SPLRCK2_SEL_MASK (3<<20)
     #define PAD_ETMDC                          (1<<20)
     #define RW_PAD_HIN_SPSDATA2_SEL_MASK (3<<22)
     #define PAD_VOUTD0                              (1<<22)


#define REG_RW_PAD_DUTY_0               0x02C0       // Pad TDSEL/RDSEL Control 0
#define REG_RW_PAD_DUTY_1               0x02C4       // Pad TDSEL/RDSEL Control 1
#define REG_RW_PAD_DUTY_2               0x02C8       // Pad TDSEL/RDSEL Control 2
#define REG_RW_PAD_DUTY_3               0x02CC       // Pad TDSEL/RDSEL Control 3
#define REG_RW_PAD_DUTY_4               0x02D0       // Pad TDSEL/RDSEL Control 4
#define REG_RW_PAD_DUTY_5               0x02D4       // Pad TDSEL/RDSEL Control 5
#define REG_RW_PAD_DUTY_6               0x02D8       // Pad TDSEL/RDSEL Control 6
#define REG_RW_PAD_DUTY_7               0x02DC       // Pad TDSEL/RDSEL Control 7
#define REG_RW_PAD_DUTY_8               0x02E0       // Pad TDSEL/RDSEL Control 8
#define REG_RW_PAD_DUTY_9               0x02E4       // Pad TDSEL/RDSEL Control 9
#define REG_RW_PAD_DUTY_10              0x02E8       // Pad TDSEL/RDSEL Control 10

//======PWDC for moniter clock enable=======================
#define REG_RW_PWDC_XTAL_CFG                  0x0120      //Clock Enable Config
  #define REG_PWM_GPIO_PD_OFFSET              (29)
  #define REG_PWM_GPIO_PD_MASK                (0x3<<29)
  #define REG_PWM_GPIO_PU_OFFSET              (27)
  #define REG_PWM_GPIO_PU_MASK                (0x3<<27)
  #define REG_PWM_GPIO_G_OFFSET               (14)
  #define REG_PWM_GPIO_G_MASK                 (0x3<<14)
  #define REG_PWM_GPIO_EN_OFFSET              (12)
  #define REG_PWM_GPIO_EN_MASK                (0x3<<12)
  #define REG_PWM_PWMDAC_LDO_EN_OFFSET        (8)
  #define REG_PWM_PWMDAC_LDO_EN_MASK          (0x3<<8)
  #define REG_PWM_PWMDAC_STBY_REV_OFFSET      (0)
  #define REG_PWM_PWMDAC_STBY_REV_MASK        (0xFF<<0)

  #define REG_PWM_SYSPLL1_PWD                 (0x1<<26)
  #define REG_PWM_SYSPLL1_DDS_PWDB            (0x1<<25)
  #define REG_PWM_HDMIPLL_PWD                 (0x1<<24)
  #define REG_PWM_HDMIPLL_DDS_PWDB            (0x1<<23)
  #define REG_PWM_SYSPLL2_PWD                 (0x1<<22)
  #define REG_PWM_ARMPLL_PWD                  (0x1<<21)
  #define REG_PWM_APLL_PWD                    (0x1<<20)
  #define REG_PWM_APLL_DDS_PWDB               (0x1<<19)
  #define REG_PWM_PLLGP_BIAS_PWD              (0x1<<18)
  #define REG_PWM_HA_DDS_PWDB                 (0x1<<17)
  #define REG_PWM_BRCKTX_PD                   (0x1<<16)
  #define REG_PWM_MUTE_CTRL_PWD               (0x1<<11)
  #define REG_PWM_PUD_PWD                     (0x1<<10)




//======PAD Configuration&&&================================

#define REG_RO_MON_REG                      0x0200      //Monitor Bus

#define REG_RW_CKEN_CFG0                    0x0300      //Clock Enable Configuration 0
  #define CKEN_CFG0_VDOUT_CKEN_OFFSET         (31)      //VDOUT clock enable
  #define CKEN_CFG0_VDOUT_CKEN_MASK           (0x1<<31) //

  #define CKEN_CFG0_ENCODER_CKEN_OFFSET       (29)      //ENCODER clock enable
  #define CKEN_CFG0_ENCODER_CKEN_MASK         (0x3<<29) //

  #define CKEN_CFG0_FMT_CKEN_OFFSET           (27)      //FMT clock enable
  #define CKEN_CFG0_FMT_CKEN_MASK             (0x3<<27) //

  #define CKEN_CFG0_VDO_CKEN_OFFSET           (24)      //VDO clock enable
  #define CKEN_CFG0_VDO_CKEN_MASK             (0x7<<24) //

  #define CKEN_CFG0_DISP_CKEN_OFFSET          (20)      //DISP clock enable
  #define CKEN_CFG0_DISP_CKEN_MASK            (0xF<<20) //

  #define CKEN_CFG0_VDO2_CKEN_OFFSET          (17)      //VOD2 clock enable
  #define CKEN_CFG0_VDO2_CKEN_MASK            (0x7<<17) //

  #define CKEN_CFG0_JPEG_CKEN_OFFSET          (16)      //JPEG clock enable
  #define CKEN_CFG0_JPEG_CKEN_MASK            (0x1<<16) //

  #define CKEN_CFG0_GRAPH_CKEN_OFFSET         (15)      //GRAPH clock enable
  #define CKEN_CFG0_GRAPH_CKEN_MASK           (0x1<<15) //

  #define CKEN_CFG0_RLE_CKEN_OFFSET           (14)      //RLE clock enable
  #define CKEN_CFG0_RLE_CKEN_MASK             (0x1<<14) //

  #define CKEN_CFG0_OSD_RESZ_CKEN_OFFSET      (13)      //OSD_RESZ clock enable
  #define CKEN_CFG0_OSD_RESZ_CKEN_MASK        (0x1<<13) //

  #define CKEN_CFG0_GFX_CKEN_OFFSET           (12)      //GFX clock enable
  #define CKEN_CFG0_GFX_CKEN_MASK             (0x1<<12) //

  #define CKEN_CFG0_IMG_RESZ_CKEN_OFFSET      (11)      //IMG_RESZ clock enable
  #define CKEN_CFG0_IMG_RESZ_CKEN_MASK        (0x1<<11) //

  #define CKEN_CFG0_PNG_CKEN_OFFSET           (10)      //PNG clock enable
  #define CKEN_CFG0_PNG_CKEN_MASK             (0x1<<10) //

  #define CKEN_CFG0_GIF_CKEN_OFFSET           (9)      //GIF clock enable
  #define CKEN_CFG0_GIF_CKEN_MASK             (0x1<<9) //

  #define CKEN_CFG0_TCPIP_CHKSUM_CKEN_OFFSET  (8)      //TCPIP_CHKSUM clock enable
  #define CKEN_CFG0_TCPIP_CHKSUM_CKEN_MASK    (0x1<<8) //

  #define CKEN_CFG0_ETHER_NIC_CKEN_OFFSET     (7)      //ETHER_NIC clock enable
  #define CKEN_CFG0_ETHER_NIC_CKEN_MASK       (0x1<<7) //

  #define CKEN_CFG0_MSDC_CKEN_OFFSET          (6)      //MSDC clock enable
  #define CKEN_CFG0_MSDC_CKEN_MASK            (0x1<<6) //

  #define CKEN_CFG0_AUDIO_CKEN_OFFSET         (3)      //AUDIO clock enable
  #define CKEN_CFG0_AUDIO_CKEN_MASK           (0x7<<3) //

  #define CKEN_CFG0_NIF_CKEN_OFFSET           (2)      //NIF clock enable
  #define CKEN_CFG0_NIF_CKEN_MASK             (0x1<<2) //

  #define CKEN_CFG0_PRI_CKEN_OFFSET           (1)      //PERI clock enable
  #define CKEN_CFG0_PRI_CKEN_MASK             (0x1<<1) //

  #define CKEN_CFG0_GCPU_CKEN_OFFSET          (0)      //GCPU clock enable
  #define CKEN_CFG0_GCPU_CKEN_MASK            (0x1<<0) //

#define REG_RW_CKEN_CFG1                    0x0304      //Clock Enable Configuration 1
  #define CKEN_CFG0_COO_CKEN_OFFSET           (22)      //COO clock enable
  #define CKEN_CFG0_COO_CKEN_MASK             (0x1<<22) //

  #define CKEN_CFG0_POST_PROC_CKEN_OFFSET     (21)      //POST_PROC clock enable
  #define CKEN_CFG0_POST_PROC_CKEN_MASK       (0x1<<21) //

  #define CKEN_CFG0_ETHER_PHY_CKEN_OFFSET     (19)      //ETHER_PHY clock enable
  #define CKEN_CFG0_ETHER_PHY_CKEN_MASK       (0x1<<19) //

  #define CKEN_CFG0_RGB2HDMI_CKEN_OFFSET      (18)      //RGB2HDMI clock enable
  #define CKEN_CFG0_RGB2HDMI_CKEN_MASK        (0x1<<18) //

  #define CKEN_CFG0_HDMI_CKEN_OFFSET          (15)      //HDMI clock enable
  #define CKEN_CFG0_HDMI_CKEN_MASK            (0x7<<15) //

  #define CKEN_CFG0_WRAPPER_CKEN_OFFSET       (14)      //WRAPPER clock enable
  #define CKEN_CFG0_WRAPPER_CKEN_MASK         (0x1<<14) //

  #define CKEN_CFG0_SVOIF_CKEN_OFFSET         (13)      //SYVIF clock enable
  #define CKEN_CFG0_SVOIF_CKEN_MASK           (0x1<<13) //

  #define CKEN_CFG0_BIM_CKEN_OFFSET           (11)      //BIM clock enable
  #define CKEN_CFG0_BIM_CKEN_MASK             (0x3<<11) //

  #define CKEN_CFG0_SACD_CKEN_OFFSET          (10)      //SACD clock enable
  #define CKEN_CFG0_SACD_CKEN_MASK            (0x1<<10) //

  #define CKEN_CFG0_USB_CKEN_OFFSET           (7)       //USB clock enable
  #define CKEN_CFG0_USB_CKEN_MASK             (0x7<<7)  //

  #define CKEN_CFG0_VDEC_CKEN_OFFSET          (6)       //VDEC clock enable
  #define CKEN_CFG0_VDEC_CKEN_MASK            (0x1<<6)  //

  #define CKEN_CFG0_SCLER_CKEN_OFFSET         (5)       //SCLER clock enable
  #define CKEN_CFG0_SCLER_CKEN_MASK           (0x1<<5)  //

  #define CKEN_CFG0_P2I_CKEN_OFFSET           (3)       //P2I clock enable
  #define CKEN_CFG0_P2I_CKEN_MASK             (0x3<<3)  //

  #define CKEN_CFG0_NR_CKEN_OFFSET            (2)       //NR clock enable
  #define CKEN_CFG0_NR_CKEN_MASK              (0x1<<2)  //

  #define CKEN_CFG0_OSD_CKEN_OFFSET           (1)       //OSD clock enable
  #define CKEN_CFG0_OSD_CKEN_MASK             (0x1<<1)  //

  #define CKEN_CFG0_WT_CKEN_OFFSET            (0)       //WT clock enable
  #define CKEN_CFG0_WT_CKEN_MASK              (0x1<<0)  //

#define REG_RW_CKRST_CFG0                   0x0310      //Clock Reset Configuration 0
  #define CKRST_CFG0_VDOUT_CKRST_OFFSET       (31)      //VDOUT reset disable
  #define CKRST_CFG0_VDOUT_CKRST_MASK         (0x1<<31) //

  #define CKRST_CFG0_ENCODER_CKRST_OFFSET     (29)      //ENCODER reset disable
  #define CKRST_CFG0_ENCODER_CKRST_MASK       (0x3<<29) //

  #define CKRST_CFG0_FMT_CKRST_OFFSET         (27)      //FMT reset disable
  #define CKRST_CFG0_FMT_CKRST_MASK           (0x3<<27) //

  #define CKRST_CFG0_VDO_CKRST_OFFSET         (24)      //VDO reset disable
  #define CKRST_CFG0_VDO_CKRST_MASK           (0x7<<24) //

  #define CKRST_CFG0_DISP_CKRST_OFFSET        (20)      //DISP reset disable
  #define CKRST_CFG0_DISP_CKRST_MASK          (0xF<<20) //

  #define CKRST_CFG0_VDO2_CKRST_OFFSET        (17)      //VOD2 reset disable
  #define CKRST_CFG0_VDO2_CKRST_MASK          (0x7<<17) //

  #define CKRST_CFG0_JPEG_CKRST_OFFSET        (16)      //JPEG reset disable
  #define CKRST_CFG0_JPEG_CKRST_MASK          (0x1<<16) //

  #define CKRST_CFG0_GRAPH_CKRST_OFFSET       (15)      //GRAPH reset disable
  #define CKRST_CFG0_GRAPH_CKRST_MASK         (0x1<<15) //

  #define CKRST_CFG0_RLE_CKRST_OFFSET         (14)      //RLE reset disable
  #define CKRST_CFG0_RLE_CKRST_MASK           (0x1<<14) //

  #define CKRST_CFG0_OSD_RESZ_CKRST_OFFSET    (13)      //OSD_RESZ reset disable
  #define CKRST_CFG0_OSD_RESZ_CKRST_MASK      (0x1<<13) //

  #define CKRST_CFG0_GFX_CKRST_OFFSET         (12)      //GFX reset disable
  #define CKRST_CFG0_GFX_CKRST_MASK           (0x1<<12) //

  #define CKRST_CFG0_IMG_RESZ_CKRST_OFFSET    (11)      //IMG_RESZ reset disable
  #define CKRST_CFG0_IMG_RESZ_CKRST_MASK      (0x1<<11) //

  #define CKRST_CFG0_PNG_CKRST_OFFSET         (10)      //PNG reset disable
  #define CKRST_CFG0_PNG_CKRST_MASK           (0x1<<10) //

  #define CKRST_CFG0_GIF_CKRST_OFFSET         (9)       //GIF reset disable
  #define CKRST_CFG0_GIF_CKRST_MASK           (0x1<<9)  //

  #define CKRST_CFG0_TCPIP_CHKSUM_CKRST_OFFSET  (8)      //TCPIP_CHKSUM reset disable
  #define CKRST_CFG0_TCPIP_CHKSUM_CKRST_MASK    (0x1<<8) //

  #define CKRST_CFG0_ETHER_NIC_CKRST_OFFSET   (7)       //ETHER_NIC reset disable
  #define CKRST_CFG0_ETHER_NIC_CKRST_MASK     (0x1<<7)  //

  #define CKRST_CFG0_MSDC_CKRST_OFFSET        (6)       //MSDC reset disable
  #define CKRST_CFG0_MSDC_CKRST_MASK          (0x1<<6)  //

  #define CKRST_CFG0_AUDIO_CKRST_OFFSET       (3)       //AUDIO reset disable
  #define CKRST_CFG0_AUDIO_CKRST_MASK         (0x7<<3)  //

  #define CKRST_CFG0_NIF_CKRST_OFFSET         (2)       //NIF reset disable
  #define CKRST_CFG0_NIF_CKRST_MASK           (0x1<<2)  //

  #define CKRST_CFG0_PRI_CKRST_OFFSET         (1)       //PERI reset disable
  #define CKRST_CFG0_PRI_CKRST_MASK           (0x1<<1)  //

  #define CKRST_CFG0_GCPU_CKRST_OFFSET        (0)       //GCPU reset disable
  #define CKRST_CFG0_GCPU_CKRST_MASK          (0x1<<0)  //

#define REG_RW_CKRST_CFG1                   0x0314      //Clock Reset Configuration 1
  #define CKRST_CFG0_COO_CKRST_OFFSET         (22)      //COO reset disable
  #define CKRST_CFG0_COO_CKRST_MASK           (0x1<<22) //

  #define CKRST_CFG0_POST_PROC_CKRST_OFFSET   (21)      //POST_PROC reset disable
  #define CKRST_CFG0_POST_PROC_CKRST_MASK     (0x1<<21) //

  #define CKRST_CFG0_ETHER_PHY_CKRST_OFFSET   (19)      //ETHER_PHY reset disable
  #define CKRST_CFG0_ETHER_PHY_CKRST_MASK     (0x1<<19) //

  #define CKRST_CFG0_RGB2HDMI_CKRST_OFFSET    (18)      //RGB2HDMI reset disable
  #define CKRST_CFG0_RGB2HDMI_CKRST_MASK      (0x1<<18) //

  #define CKRST_CFG0_HDMI_CKRST_OFFSET        (15)      //HDMI reset disable
  #define CKRST_CFG0_HDMI_CKRST_MASK          (0x7<<15) //

  #define CKRST_CFG0_WRAPPER_CKRST_OFFSET     (14)      //WRAPPER reset disable
  #define CKRST_CFG0_WRAPPER_CKRST_MASK       (0x1<<14) //

  #define CKRST_CFG0_SVOIF_CKRST_OFFSET       (13)      //SYVIF reset disable
  #define CKRST_CFG0_SVOIF_CKRST_MASK         (0x1<<13) //

  #define CKRST_CFG0_BIM_CKRST_OFFSET         (11)      //BIM reset disable
  #define CKRST_CFG0_BIM_CKRST_MASK           (0x3<<11) //

  #define CKRST_CFG0_SACD_CKRST_OFFSET        (10)      //SACD reset disable
  #define CKRST_CFG0_SACD_CKRST_MASK          (0x1<<10) //

  #define CKRST_CFG0_USB_CKRST_OFFSET         (7)       //USB reset disable
  #define CKRST_CFG0_USB_CKRST_MASK           (0x7<<7)  //

  #define CKRST_CFG0_VDEC_CKRST_OFFSET        (6)       //VDEC reset disable
  #define CKRST_CFG0_VDEC_CKRST_MASK          (0x1<<6)  //

  #define CKRST_CFG0_SCLER_CKRST_OFFSET       (5)       //SCLER reset disable
  #define CKRST_CFG0_SCLER_CKRST_MASK         (0x1<<5)  //

  #define CKRST_CFG0_P2I_CKRST_OFFSET         (3)       //P2I reset disable
  #define CKRST_CFG0_P2I_CKRST_MASK           (0x3<<3)  //

  #define CKRST_CFG0_NR_CKRST_OFFSET          (2)       //NR reset disable
  #define CKRST_CFG0_NR_CKRST_MASK            (0x1<<2)  //

  #define CKRST_CFG0_OSD_CKRST_OFFSET         (1)       //OSD reset disable
  #define CKRST_CFG0_OSD_CKRST_MASK           (0x1<<1)  //

  #define CKRST_CFG0_WT_CKRST_OFFSET          (0)       //WT reset disable
  #define CKRST_CFG0_WT_CKRST_MASK            (0x1<<0)  //

#define REG_RW_DELSEL_CFG0                  0x0320      //SRAM DELSEL Configuration 0
  #define DELSEL_SPLVT_B256M1_OFFSET          (24)      //DELSEL for SPLVT_B256M1
  #define DELSEL_SPLVT_B256M1_MASK            (0x3<<24) //
  #define DELSEL_SPLVT_B256M2_OFFSET          (20)      //DELSEL for SPLVT_B256M2
  #define DELSEL_SPLVT_B256M2_MASK            (0x3<<20) //
  #define DELSEL_SPLVT_B256M8_OFFSET          (16)      //DELSEL for SPLVT_B256M8
  #define DELSEL_SPLVT_B256M8_MASK            (0x3<<16) //
  #define DELSEL_SPLVT_B256M16_OFFSET         (12)      //DELSEL for SPLVT_B256M16
  #define DELSEL_SPLVT_B256M16_MASK           (0x3<<12) //
  #define DELSEL_SPLVT_B64M4_OFFSET           (8)       //DELSEL for SPLVT_B64M4
  #define DELSEL_SPLVT_B64M4_MASK             (0x3<<8)  //
  #define DELSEL_TPLVT_B256M1_OFFSET          (4)       //DELSEL for TPLVT_B256M1
  #define DELSEL_TPLVT_B256M1_MASK            (0x3<<4)  //
  #define DELSEL_TPLVT_B256M2_OFFSET          (0)       //DELSEL for TPLVT_B256M2
  #define DELSEL_TPLVT_B256M2_MASK            (0x3<<0)  //

#define REG_RW_DELSEL_CFG1                  0x0324      //SRAM DELSEL Configuration 1
  #define DELSEL_SPRVT_B256M1_OFFSET          (24)      //DELSEL for SPRVT_B256M1
  #define DELSEL_SPRVT_B256M1_MASK            (0x3<<24) //
  #define DELSEL_SPRVT_B256M2_OFFSET          (20)      //DELSEL for SPRVT_B256M2
  #define DELSEL_SPRVT_B256M2_MASK            (0x3<<20) //
  #define DELSEL_SPRVT_B256M8_OFFSET          (16)      //DELSEL for SPRVT_B256M8
  #define DELSEL_SPRVT_B256M8_MASK            (0x3<<16) //
  #define DELSEL_SPRVT_B256M16_OFFSET         (12)      //DELSEL for SPRVT_B256M16
  #define DELSEL_SPRVT_B256M16_MASK           (0x3<<12) //
  #define DELSEL_SPRVT_B64M4_OFFSET           (8)       //DELSEL for SPRVT_B64M4
  #define DELSEL_SPRVT_B64M4_MASK             (0x3<<8)  //
  #define DELSEL_TPLVT_B256M1_OFFSET          (4)       //DELSEL for TPLVT_B256M1
  #define DELSEL_TPLVT_B256M1_MASK            (0x3<<4)  //
  #define DELSEL_TPLVT_B256M2_OFFSET          (0)       //DELSEL for TPLVT_B256M2
  #define DELSEL_TPLVT_B256M2_MASK            (0x3<<0)  //

#define REG_RW_DTMETER                      0x0084      //DUTY_METER
  #define DEMETER_HIGH_CNT_OFFSET             (24)      //Duty Meter High Coutner
  #define DEMETER_HIGH_CNT_MASK               (0xFF<<24)//
  #define DEMETER_ALL_CNT_OFFSET              (16)      //Duty Meter All Coutner
  #define DEMETER_ALL_CNT_MASK                (0xFF<<16)//
  #define DEMETER_DONE_OFFSET                 (8)       //Duty Meter Done
  #define DEMETER_DONE_MASK                   (0x1<<8)  //
  #define DEMETER_SEL_OFFSET                  (8)       //Duty Meter selection
  #define DEMETER_SEL_MASK                    (0x1<<8)  //0 AOMCLK; 1 AOBCK
  #define DEMETER_EN_OFFSET                   (8)       //Duty Meter Enable
  #define DEMETER_EN_MASK                     (0x1<<8)  //

#define REG_RW_CKSTA                        0x0088      //Clock Switch Status
  #define CK_STA_OFFSET                       (0)       //Clock Switch Status
  #define CK_STA_MASK                         (0x1<<0)  //0 OK; 1 Clock is not switched.

//============================================================================
// Constant definitions
//============================================================================

//============================================================================
// Type definitions
//============================================================================
typedef enum
{
    SRC_CK_APLL,
    SRC_CK_ARMPLL,
    SRC_CK_VDPLL,
    SRC_CK_DMPLL,
    SRC_CK_SYSPLL1,
    SRC_CK_SYSPLL2,
    SRC_CK_USBCK,
    SRC_CK_MEMPLL,
    SRC_CK_MCK
} SRC_CK_T;

typedef enum
{
    e_CLK_RISC,             //0   0x70.3
    e_CLK_ADSP,             //1   0x70.7
    e_CLK_ADSP2,            //2   0x70.11
    e_CLK_NFLASH,           //3   0x70.19
    e_CLK_SLOW_RISC,        //4
    e_CLK_VDEC,             //5   0x74.3
    e_CLK_MC,               //6   0x74.7
    e_CLK_OSD,              //7   0x74.11
    e_CLK_PNG,              //8   0x74.15
    e_CLK_RESZ,             //9   0x74.19
    e_CLK_GRAPH,            //10  0x74.23
    e_CLK_NR,               //11  0x74.27
    e_CLK_IR_DIV,           //12  0x78.4
    e_CLK_SD,               //13  0x78.12
    e_CLK_SD_HCLK,          //14  0x78.15
    e_CLK_ABIST2,           //15  0x78.19
    e_CLK_ABIST,            //16  0x78.31
    e_CLK_SVO_STDBY,        //17  0x7C.3
    e_CLK_GCPU,             //18  0x7C.7
    e_CLK_MVDO2,            //19  0x7C.11
    e_CLK_JPEG,             //20  0x7C.15
    e_CLK_SYS_SUB,          //21  0x7C.19
    e_CLK_BCLK,             //22  0x7C.31
    e_CLK_MAX               //23
} e_CLK_T;

typedef enum
{
    e_CKEN_VDOUT,            //0   0x300.31
    e_CKEN_ENCODER,          //1   0x300.29
    e_CKEN_FMT,              //2   0x300.27
    e_CKEN_VDO,              //3   0x300.24
    e_CKEN_DISP,             //4   0x300.20
    e_CKEN_VOD2,             //5   0x300.17
    e_CKEN_JPEG,             //6   0x300.16
    e_CKEN_GRAPH,            //7   0x300.15
    e_CKEN_RLE,              //8   0x300.14
    e_CKEN_OSD_RESZ,         //9   0x300.13
    e_CKEN_GFX,              //10  0x300.12
    e_CKEN_IMG_RESZ,         //11  0x300.11
    e_CKEN_PNG,              //12  0x300.10
    e_CKEN_GIF,              //13  0x300.9
    e_CKEN_TCPIP_CHKSUM,     //14  0x300.8
    e_CKEN_ETHER_NIC,        //15  0x300.7
    e_CKEN_MSDC,             //16  0x300.6
    e_CKEN_AUDIO,            //17  0x300.3
    e_CKEN_NIF,              //18  0x300.2
    e_CKEN_PERI,             //19  0x300.1
    e_CKEN_GCPU,             //20  0x300.0
    e_CKEN_COO,              //21  0x304.22
    e_CKEN_POST_PROC,        //22  0x304.21
    e_CKEN_ETHER_PHY,        //23  0x304.19
    e_CKEN_RGB2HDMI,         //24  0x304.18
    e_CKEN_HDMI,             //25  0x304.15
    e_CKEN_WRAPPER,          //26  0x304.14
    e_CKEN_SYVIF,            //27  0x304.13
    e_CKEN_BIM,              //28  0x304.11
    e_CKEN_SACD,             //29  0x304.10
    e_CKEN_USB,              //30  0x304.7
    e_CKEN_VDEC,             //31  0x304.6
    e_CKEN_SCLER,            //32  0x304.5
    e_CKEN_P2I,              //33  0x304.3
    e_CKEN_NR,               //34  0x304.2
    e_CKEN_OSD,              //35  0x304.1
    e_CKEN_WT,               //36  0x304.0
    e_CKEN_MAX               //37
} e_CKEN_T;

//============================================================================
// Interface
//============================================================================
extern BOOL BSP_Calibrate(SRC_CK_T eSource, UINT32 u4Clock);
extern UINT32 BSP_GetClock(SRC_CK_T eSource);
extern BOOL CKGEN_SetPLL(SRC_CK_T eSource, UINT32 u4Clock0, UINT32 u4Clock1);
extern BOOL CKGEN_AgtOnClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtOffClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtSelClk(e_CLK_T eAgt, UINT32 u4Sel);
extern UINT32 CKGEN_AgtGetClk(e_CLK_T eAgt);
#endif  // X_CKGEN_8561_H
