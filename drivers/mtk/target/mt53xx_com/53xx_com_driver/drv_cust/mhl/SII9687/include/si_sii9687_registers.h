//***************************************************************************
//! @file     si_sii9687_registers.h
//! @brief    SiI9687 Device Register Manifest Constants.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_9687REGS_H__
#define __SI_9687REGS_H__

#include "si_cra_cfg.h"
//------------------------------------------------------------------------------
// Registers in Page 0  (0xB0/0xB2)
//------------------------------------------------------------------------------

#define REG_DEV_IDL_RX          (PP_PAGE_0 | 0x02)
#define REG_DEV_IDH_RX          (PP_PAGE_0 | 0x03)
#define REG_DEV_REV             (PP_PAGE_0 | 0x04)

// Software Reset Register
#define REG_SRST                (PP_PAGE_0 | 0x05)
#define BIT_SWRST_AUTO                  BIT4   // Auto SW Reset
#define BIT_SWRST_PA                    BIT1   // InstaPort Reset
#define BIT_SWRST                       BIT0   // Global SW reset


// System Status Register
#define REG_STATE               (PP_PAGE_0 | 0x06)
#define BIT_SCDT                        BIT0   // DE detect
#define BIT_CKDT                        BIT1   // Active TMDS clock detect
#define BIT_PWR5V                       BIT3   // Selected port 5V detect
#define BIT_PWRON_STANDBY               BIT5   // PDN power state (set if NOT in standby)
#define BIT_MHL                         BIT7   // MHL source detect

// System Control register
#define REG_HDCP_RST            (PP_PAGE_0 | 0x07)
#define BIT_HDCP_RST                    BIT0   // Reset HDCP logic
#define BIT_NO_USE_SCDT                 BIT3   // Don't use SCDT to generate reset


// System Control Register #1 TODO:SHV - not in PR
#define REG_SYS_CTRL1           (PP_PAGE_0 | 0x08)
#define BIT_EHDMI_VOLTAGE_CTRL          BIT1   // Enable 4V power on reserved pin in non-eHDMI mode
#define VAL_PAD_COMB_DDC_MASK           0xF8
#define BIT_PAD_COMB_DDC_P0             BIT3   // eHDMI PAD-COMB-DDC controls
#define BIT_PAD_COMB_DDC_P1             BIT4
#define BIT_PAD_COMB_DDC_P2             BIT5
#define BIT_PAD_COMB_DDC_P3             BIT6

// Port Switch Register
#define REG_HDCP_DDC_CTRL       (PP_PAGE_0 | 0x09)
#define BIT_DDC0_EN                     BIT4   // Port DDC access enable
#define BIT_DDC1_EN                     BIT5
#define BIT_DDC2_EN                     BIT6
#define BIT_DDC3_EN                     BIT7
#define MSK_DDC_EN                      0xF0
#define VAL_DDC_PORT_ALL_EN             0xF0
#define VAL_DDC_PORT_ALL_DIS            0x00

// Port Switch Register 2
#define REG_RX_PORT_SEL         (PP_PAGE_0 | 0x0A)
#define MSK_PORT_ALL                    0x07   // Input port selection mask
#define BIT_DDC_DELAY_EN                BIT3   // 300 ns delay between SCL rising edge and SDA falling edge
#define BIT_20MHZ_PWRDN_EN              BIT5   // Disable 20 MHz OSC (PDN)

// MHL Reset
#define REG_SYS_RESET2          (PP_PAGE_0 | 0x0B)
#define BIT_MHL_SRST                    BIT4   // Reset MHL control bus logic

// HDCP DDC termination disable Register
#define REG_PWD_PORT_CTRL       (PP_PAGE_0 | 0x0C)
#define BIT_HDCP_AUTO_DISABLE           BIT0   // Disable HDCP DDC access in Standby
#define BIT_HDCP_TERM_DISABLE           BIT1   // Disable termination impedance in Standby
#define VAL_HDCP_DISABLED               0x03


// Hot plug Control Register
#define REG_HP_CTRL             (PP_PAGE_0 | 0x10)
#define VAL_HP_PORT0_MASK               0x03
#define VAL_HP_PORT0_HI                 0x01   // HPD follows PWR5V in a port
#define VAL_HP_PORT1_HI                 0x04
#define VAL_HP_PORT2_HI                 0x10
#define VAL_HP_PORT3_HI                 0x40
#define VAL_HP_PORT_ALL_HI              0x55
#define VAL_HP_PORT_ALL_LO              0x00   // HPD low in all ports, if manual HPD control is enabled
#define VAL_HP_PORT_MHL                 0xAA   // HPD line in high impedance state TODO:SHV - needed at all?

// Slave address registers
#define REG_SLAVE_ADDR0         (PP_PAGE_0 | 0x16)
#define REG_SLAVE_ADDR1         (PP_PAGE_0 | 0x17)
#define REG_SLAVE_ADDR2         (PP_PAGE_0 | 0x18)
#define REG_SLAVE_ADDR3         (PP_PAGE_0 | 0x19)
#define REG_SLAVE_ADDR4         (PP_PAGE_0 | 0x15)
#define REG_SLAVE_ADDR5         (PP_PAGE_0 | 0x14)
#define REG_SLAVE_ADDR6         (PP_PAGE_0 | 0x11)
#define REG_SLAVE_ADDR7         (PP_PAGE_0 | 0x12)
#define REG_SLAVE_ADDR8         (PP_PAGE_0 | 0x13)
// IPV slave address
#define REG_SLAVE_ADDR11        (PP_PAGE_0 | 0x40)

#define REG_SHD_BKSV_1_RX       (PP_PAGE_0 | 0x1A)
#define REG_SHD_BKSV_2_RX       (PP_PAGE_0 | 0x1B)
#define REG_SHD_BKSV_3_RX       (PP_PAGE_0 | 0x1C)
#define REG_SHD_BKSV_4_RX       (PP_PAGE_0 | 0x1D)
#define REG_SHD_BKSV_5_RX       (PP_PAGE_0 | 0x1E)

#define REG_HDCP_SHD_RI1        (PP_PAGE_0 | 0x1F)
#define REG_HDCP_SHD_RI2        (PP_PAGE_0 | 0x20)

#define REG_HDCP_SHD_AKSV1      (PP_PAGE_0 | 0x21)
#define REG_HDCP_SHD_AKSV2      (PP_PAGE_0 | 0x22)
#define REG_HDCP_SHD_AKSV3      (PP_PAGE_0 | 0x23)
#define REG_HDCP_SHD_AKSV4      (PP_PAGE_0 | 0x24)
#define REG_HDCP_SHD_AKSV5      (PP_PAGE_0 | 0x25)

// Session random number (8 bytes)
#define REG_HDCP_SHD_AN1        (PP_PAGE_0 | 0x26)

// TODO:SHV - repeater reg
#define REG_HDCP_BCAPS_SET      (PP_PAGE_0 | 0x2E)
#define BIT_HDMI_CAPABLE                0x80
#define BIT_I2C_FAST                    0x10
#define BIT_P0_FIFO_RDY                 0x01
#define BIT_P0_REPEATER                 0x02
#define BIT_P1_FIFO_RDY                 0x04
#define BIT_P1_REPEATER                 0x08
#define BIT_P2_FIFO_RDY                 0x10
#define BIT_P2_REPEATER                 0x20
#define BIT_P3_FIFO_RDY                 0x40
#define BIT_P3_REPEATER                 0x80
#define VAL_P0_FIFO_RDY                 0x01
#define VAL_P0_REPEATER                 0x02
#define VAL_P1_FIFO_RDY                 0x04
#define VAL_P1_REPEATER                 0x08
#define VAL_P2_FIFO_RDY                 0x10
#define VAL_P2_REPEATER                 0x20
#define VAL_P3_FIFO_RDY                 0x40
#define VAL_P3_REPEATER                 0x80
#define VAL_P1_3_FIFO_RDY               0x55


// HDCP Debug Register
#define REG_HDCP_SHD_BSTAT2     (PP_PAGE_0 | 0x30)
#define BIT_HDMI_MODE                   BIT4

#define REG_HDCPCTRL            (PP_PAGE_0 | 0x31)
#define BIT_CLEAR_RI                    0x80

// HDCP Status
#define REG_HDCP_STAT           (PP_PAGE_0 | 0x32)
#define BIT_AUTH_DONE                   BIT4
#define BIT_DECRYPTING                  BIT5

// Interrupt Control and Status
#define REG_INTR_STATE          (PP_PAGE_0 | 0x70)
#define BIT_INTR                        BIT0    // Global interrupt status
#define BIT_INTR_GRP0                   BIT1    // INTR1, INTR2, INTR4
#define BIT_INTR_GRP1                   BIT2    // INTR5, INTR6, INTR8, INTR9, INTR10
#define BIT_INTR_GRP_PA                 BIT3    // PA_INTR1-PA_INTR10
#define BIT_INTR_GRP5                   BIT6    // CBUS interrupts

#define REG_INT_STATUS1         (PP_PAGE_0 | 0x71)
#define REG_INT_MASK1           (PP_PAGE_0 | 0x75)
#define BIT_PDN_PWR_CHG                 BIT3    // Power Down domain power status change

#define REG_INT_STATUS2         (PP_PAGE_0 | 0x72)
#define REG_INT_MASK2           (PP_PAGE_0 | 0x76)
#define BIT_SCDT_CHG                    BIT3
#define BIT_CKDT_CHG                    BIT4
#define BIT_SW_INT                      BIT5
#define BIT_HDMI_DETECT_INT             BIT7

#define REG_INT_CTRL            (PP_PAGE_0 | 0x79)
#define BIT_INT_POLARITY                BIT1    // Active LOW (1), active HIGH (0)
#define BIT_INT_OD                      BIT2    // Open drain (1), push-pull (0)
#define BIT_DO_SW_INT                   BIT3    // Triggers interrupt by software (1)

#define REG_INT_STATUS5         (PP_PAGE_0 | 0x7B)
#define REG_INT_MASK5           (PP_PAGE_0 | 0x7D)
#define VAL_P5V_P0_P3_CHG               0x0F
#define VAL_CKDT_P0_P3_CHG              0xF0

#define REG_INT_STATUS6         (PP_PAGE_0 | 0x7C)
#define REG_INT_MASK6           (PP_PAGE_0 | 0x7E)
#define BIT_5VPWR_CHANGE                BIT0    // Main pipe (selected port)

#define REG_TMDS_TERMCTRL3      (PP_PAGE_0 | 0x80)
#define MSK_TERM_P2                     0xE0
#define VAL_TERM_P2_HDMI                0x20
#define VAL_TERM_P2_MHL                 0xC0
#define MSK_TERM_P3                     0x07
#define VAL_TERM_P3_HDMI                0x01
#define VAL_TERM_P3_MHL                 0x06

#define REG_TMDS_TERMCTRL0      (PP_PAGE_0 | 0x82)
#define VAL_TERM_ON                     0x00
#define VAL_TERM_MHL                    0x01
#define VAL_TERM_SURGE                  0x02
#define VAL_TERM_OFF                    0x03
#define MSK_TERM                        0x03

#define REG_TMDS_TERMCTRL1      (PP_PAGE_0 | 0x83)
#define MSK_TERM_P0                     0xE0
#define VAL_TERM_P0_HDMI                0x20
#define VAL_TERM_P0_MHL                 0xC0
#define MSK_TERM_P1                     0x07
#define VAL_TERM_P1_HDMI                0x01
#define VAL_TERM_P1_MHL                 0x06

#define REG_CH0_PD_SYS3         (PP_PAGE_0 | 0x88)
#define MSK_CH0_PD_TMDS_CLK_EN          0x0F

// ESD protection control
#define REG_ESD_PROT_SEL_HWSW   (PP_PAGE_0 | 0x8D)  // TODO:SHV - not in PR
#define BIT_ESD_MASK                    0x1F    // 1 - SW controlled, 0 - HW controlled (default)

#define REG_INT_STATUS7         (PP_PAGE_0 | 0x90)
#define REG_INT_MASK7           (PP_PAGE_0 | 0x92)
#define BIT_INT_BOOT_DONE               BIT6    // NVRAM -> SRAM boot complete IRQ

#define REG_INTR9               (PP_PAGE_0 | 0x94)
#define REG_INTR9_MASK          (PP_PAGE_0 | 0xA0)

//------------------------------------------------------------------------------
// Registers in Page 2      (0x66)
//------------------------------------------------------------------------------

#define REG_TMDS3_CCTRL1            (PP_PAGE_2 | 0x00)

#define REG_TMDS3_CTRL2             (PP_PAGE_2 | 0x02)
#define REG_TMDS3_CTRL3             (PP_PAGE_2 | 0x03)
#define REG_TMDS3_CLKDETECT_CTL     (PP_PAGE_2 | 0x04)
#define REG_PLL3_CALREFSEL          (PP_PAGE_2 | 0x07)
#define REG_PLL3_VCOCAL             (PP_PAGE_2 | 0x0A)

#define REG_TMDS2_CCTRL1            (PP_PAGE_2 | 0x80)

#define REG_TMDS2_CTRL2             (PP_PAGE_2 | 0x82)
#define REG_TMDS2_CTRL3             (PP_PAGE_2 | 0x83)
#define REG_TMDS2_CLKDETECT_CTL     (PP_PAGE_2 | 0x84)
#define REG_PLL2_CALREFSEL          (PP_PAGE_2 | 0x87)
#define REG_PLL2_VCOCAL             (PP_PAGE_2 | 0x8A)


//------------------------------------------------------------------------------
// Registers in Page 5     (0x50)  //TODO: sync with Sparta
//------------------------------------------------------------------------------
#define REG_PA_INTR1                (PP_PAGE_PAUTH1 | 0x01)
#define BIT_DM_VALUE_CHANGE             BIT1
#define BIT_MP_HW_HPE                   BIT2
#define BIT_MP_HPE                      BIT3
#define BIT_RP_HW_HPE                   BIT4
#define BIT_RP_HPE                      BIT5
#define BIT_MP_STABLE_SCDT              BIT6
#define BIT_MP_STABLE_HDCP              BIT7

#define REG_PA_INTR2                (PP_PAGE_PAUTH1 | 0x02)
#define BIT_MP_STABLE_NON_HDCP          BIT0
#define BIT_RP_STABLE_SCDT              BIT1
#define BIT_RP_STABLE_HDCP              BIT2
#define BIT_RP_STABLE_NON_HDCP          BIT3
#define BIT_MP_AV_MUTE                  BIT4
#define BIT_MP_AV_UNMUTE                BIT5
#define BIT_MP_GCP                      BIT6
#define BIT_RP_AV_MUTE                  BIT7

#define REG_PA_INTR3                (PP_PAGE_PAUTH1 | 0x03)
#define BIT_RP_AV_UNMUTE                BIT0
#define BIT_RP_GCP                      BIT1
#define BIT_HPD_START_MP                BIT2
#define BIT_HPD_END_MP                  BIT3
#define BIT_RXTERM_START_MP             BIT4
#define BIT_RXTERM_END_MP               BIT5
#define BIT_HDDC_START_MP               BIT6
#define BIT_HDDC_END_MP                 BIT7

#define REG_PA_INTR4                (PP_PAGE_PAUTH1 | 0x04)
#define BIT_EDDC_START_MP               BIT0
#define BIT_EDDC_END_MP                 BIT1
#define BIT_HPD_START_RP                BIT2
#define BIT_HPD_END_RP                  BIT3
#define BIT_RXTERM_START_RP             BIT4
#define BIT_RXTERM_END_RP               BIT5
#define BIT_HDDC_START_RP               BIT6
#define BIT_HDDC_END_RP                 BIT7

#define REG_PA_INTR5                (PP_PAGE_PAUTH1 | 0x05)
#define BIT_EDDC_START_RP               BIT0
#define BIT_EDDC_END_RP                 BIT1
#define BIT_HDCP_GOOD                   BIT2
#define BIT_DECRYPT                     BIT3
#define BIT_AUTH_CHG_P0                 BIT4
#define BIT_AUTH_CHG_P1                 BIT5
#define BIT_AUTH_CHG_P2                 BIT6
#define BIT_AUTH_CHG_P3                 BIT7
#define VAL_AUTH_CHG_MASK_P0_P3         ( BIT_AUTH_CHG_P0 | BIT_AUTH_CHG_P1 | BIT_AUTH_CHG_P2 | BIT_AUTH_CHG_P3 )

#define REG_PA_INTR6                (PP_PAGE_PAUTH1 | 0x06)
#define BIT_MP_NO_AIF                   BIT2
#define BIT_MP_NO_AVI                   BIT3
#define BIT_MP_NO_VSI                   BIT4
#define BIT_MP_NEW_AIF                  BIT5
#define BIT_MP_NEW_AVI                  BIT6
#define BIT_MP_NEW_VSI                  BIT7
#define VAL_MP_IF_STATUS_MASK           0xFC

#define REG_PA_INTR7                (PP_PAGE_PAUTH1 | 0x07)
#define BIT_RP_NO_AIF                   BIT0
#define BIT_RP_NO_AVI                   BIT1
#define BIT_RP_NO_VSI                   BIT2
#define BIT_RP_NEW_AIF                  BIT3
#define BIT_RP_NEW_AVI                  BIT4
#define BIT_RP_NEW_VSI                  BIT5
#define VAL_RP_IF_STATUS_MASK           0x3F
#define BIT_MP_MUTE_CONDITION           BIT6
#define BIT_MP_UNMUTE_CONDITION         BIT7

#define REG_PA_INTR8                (PP_PAGE_PAUTH1 | 0x08)
#define BIT_RP_mute                     BIT0
#define BIT_RP_unmute                   BIT1
#define BIT_P0_ECC                      BIT2
#define BIT_P1_ECC                      BIT3
#define BIT_P2_ECC                      BIT4
#define BIT_P3_ECC                      BIT5
#define BIT_P4_ECC                      BIT6
#define BIT_MP_MUTE_STATUS              BIT7

#define REG_PA_INTR9                (PP_PAGE_PAUTH1 | 0x09)
#define BIT_RP_MUTE_STATUS              BIT0
#define BIT_MP_RES_STABLE_CHG           BIT3

#define REG_PA_INTR1_MASK           (PP_PAGE_PAUTH1 | 0x15)
#define REG_PA_INTR3_MASK           (PP_PAGE_PAUTH1 | 0x17)

#define REG_PA_INTR5_MASK           (PP_PAGE_PAUTH1 | 0x19)
#define BIT_INTR5_MASK_PORT0_AUTH       BIT4
#define BIT_INTR5_MASK_PORT1_AUTH       BIT5
#define BIT_INTR5_MASK_PORT2_AUTH       BIT6
#define BIT_INTR5_MASK_PORT3_AUTH       BIT7

#define REG_PA_INTR6_MASK           (PP_PAGE_PAUTH1 | 0x1A)

#define BIT_PA6_MASK_MP_NO_AIF          BIT2
#define BIT_PA6_MASK_MP_NO_AVI          BIT3
#define BIT_PA6_MASK_MP_NO_VSI          BIT4
#define BIT_PA6_MASK_MP_NO_ALL          0x1C

#define BIT_PA6_MASK_MP_NEW_AIF         BIT5
#define BIT_PA6_MASK_MP_NEW_AVI         BIT6
#define BIT_PA6_MASK_MP_NEW_VSI         BIT7
#define BIT_PA6_MASK_MP_NEW_ALL         0xE0

#define BIT_INTR5_MASK_AUTH_CHNG_MASK \
    ( BIT_INTR5_MASK_PORT0_AUTH \
    | BIT_INTR5_MASK_PORT1_AUTH \
    | BIT_INTR5_MASK_PORT2_AUTH \
    | BIT_INTR5_MASK_PORT3_AUTH \
    )

#define REG_PA_INTR7_MASK           (PP_PAGE_PAUTH1 | 0x1B)
#define REG_PA_INTR9_MASK           (PP_PAGE_PAUTH1 | 0x1D)

//Pre-authentication Control Register
#define REG_PAUTH_CTRL              (PP_PAGE_PAUTH1 | 0x3A)
#define BIT_PORT_FSM_EN                 0x01
#define BIT_SKIP_NON_HDCP               0x02
#define BIT_IGNORE_PAUTH_HPD            0x08
#define BIT_USE_AV_MUTE                 0x10
#define BIT_PAUTH_HPD_CON_EN            0x40
#define BIT_MP_SWAP                     0x80

#define REG_PAUTH_STAT0             (PP_PAGE_PAUTH1 | 0x3B)
#define MAIN_PIPE_MASK                  0x0F
#define ROVE_PIPE_MASK                  0xF0

//Pre-authentication Status Register
#define REG_PAUTH_STAT1             (PP_PAGE_PAUTH1 | 0x3C)
#define BIT_DECRYPT_MASK                0xF0
#define BIT_AUTH_MASK                   0x0F
#define BIT_P0_AUTH                     0x01
#define BIT_P1_AUTH                     0x02
#define BIT_P2_AUTH                     0x04
#define BIT_P3_AUTH                     0x08


#define REG_PAUTH_MPOVR             (PP_PAGE_PAUTH1 | 0x3E)       // Main pipe port select register (one-hot)
#define BIT_PAUTH_MAIN_P0               (BIT0)
#define BIT_PAUTH_MAIN_OVR              (BIT7)

#define REG_PAUTH_RPOVR             (PP_PAGE_PAUTH1 | 0x3F)       // Rove pipe port select register (one-hot)
#define BIT_PORT_0                      0x01
#define BIT_PORT_1                      0x02
#define BIT_PORT_2                      0x04
#define BIT_PORT_3                      0x08
#define MSK_ALL_PORTS                   0x0F
#define BIT_RP_FORCED_SELECTION         BIT7

//Pre-authentication Reset Length Register
#define REG_PAUTH_RSTLEN0           (PP_PAGE_PAUTH1 | 0x40)
#define REG_PAUTH_RSTLEN1           (PP_PAGE_PAUTH1 | 0x41)
#define REG_PAUTH_RSTLEN2           (PP_PAGE_PAUTH1 | 0x42)
#define REG_PAUTH_RSTLEN3           (PP_PAGE_PAUTH1 | 0x43)

    /* Pre-Authentication Reset On/Off Difference between Analog/Digital resets.    */
    /* This set of registers is for the leading edge of the resets.                 */

#define REG_PAUTH_RST_ON_DIFF0      (PP_PAGE_PAUTH1 | 0x44)
#define REG_PAUTH_RST_ON_DIFF1      (PP_PAGE_PAUTH1 | 0x45)
#define REG_PAUTH_RST_ON_DIFF2      (PP_PAGE_PAUTH1 | 0x46)
#define REG_PAUTH_RST_ON_DIFF3      (PP_PAGE_PAUTH1 | 0x47)

//Pre-Authentication Time Out Limit Register
#define REG_PAUTH_TMOUT0            (PP_PAGE_PAUTH1 | 0x48)

//Pre-Authentication Port Kept Authenticated Time Register
#define REG_PAUTH_KEEPLEN0          (PP_PAGE_PAUTH1 | 0x4C)

//Pre-Authentication Enough Limit Registers
#define REG_PAUTH_ENGHLIM0          (PP_PAGE_PAUTH1 | 0x50)
#define REG_PAUTH_ENGHLIM1          (PP_PAGE_PAUTH1 | 0x51)
#define REG_PAUTH_ENGHLIM2          (PP_PAGE_PAUTH1 | 0x52)
#define REG_PAUTH_ENGHLIM3          (PP_PAGE_PAUTH1 | 0x53)

#define REG_PAUTH_MHL_CTRL          (PP_PAGE_PAUTH1 | 0x54)
#define BIT_MP_MHL_MODE                 (BIT0)
#define BIT_MP_MHL_OVR                  (BIT1)

#define REG_PAUTH_BCAPS_CTRL        (PP_PAGE_PAUTH1 | 0x55)
#define BIT_P0_HDMI_CAPABLE             0x02
#define BIT_P1_HDMI_CAPABLE             0x08
#define BIT_P2_HDMI_CAPABLE             0x20
#define BIT_P3_HDMI_CAPABLE             0x80

#define PAUTH_HDMI_MODE             (PP_PAGE_PAUTH1 | 0x56)
#define REG_HDMI_DETECTED               0x01

#define REG_PAUTH_ECC_CTRL          (PP_PAGE_PAUTH1 | 0x57)
#define BIT_EN_ECC                      0x10

#define REG_PAUTH_ECC_CHKTIME       (PP_PAGE_PAUTH1 | 0x58)



#define REG_PAUTH_INV_CTRL          (PP_PAGE_PAUTH1 | 0x59)
#define TX_BIT_INV                      0x80
#define MP_CLK_INV                      0x01

#define REG_PAUTH_MP_AOVR           (PP_PAGE_PAUTH1 | 0x5A)
#define BIT_MP_OVR_P0                   (BIT0)

#define REG_PAUTH_RP_AOVR           (PP_PAGE_PAUTH1 | 0x5B)

#define REG_MP_DLY_STAT             (PP_PAGE_PAUTH1 | 0x5C)
#define REG_P0_DLY_STAT             (PP_PAGE_PAUTH1 | 0x5D)
#define REG_P1_DLY_STAT             (PP_PAGE_PAUTH1 | 0x5E)
#define REG_P2_DLY_STAT             (PP_PAGE_PAUTH1 | 0x5F)
#define REG_P3_DLY_STAT             (PP_PAGE_PAUTH1 | 0x60)

#define REG_PAUTH_ECC_THRES0        (PP_PAGE_PAUTH1 | 0x61)
#define REG_PAUTH_ECC_THRES1        (PP_PAGE_PAUTH1 | 0x62)

#define REG_PAUTH_MISC_CTRL0        (PP_PAGE_PAUTH1 | 0x63)
#define BIT_CLEAR_ECC                   0x01
#define BIT_MATCH_IND_SEL               0x02
#define BIT_DIS_GEN_VS_CTL3             0x04
#define BIT_FIX_DELAY                   0x08
#define BIT_VIDEO_MUTE_SYNC             0x10
#define BIT_AUDIO_MUTE_SYNC             0x20
#define BIT_RECOV_EN                    0x40
#define BIT_USE_FRAME_ECC               0x80

#define REG_FRAME_ECC_THR           (PP_PAGE_PAUTH1 | 0x65)
#define REG_PAUTH_ECC_ERR0          (PP_PAGE_PAUTH1 | 0x68)
#define REG_PAUTH_ECC_ERR1          (PP_PAGE_PAUTH1 | 0x69)
#define REG_PAUTH_HDMI_IDLE_CNT     (PP_PAGE_PAUTH1 | 0x6A)

#define REG_PAUTH_RST_DIS_CKDT      (PP_PAGE_PAUTH1 | 0x6E)
#define BIT_HDCP_RST_CKDT_EN            0x01
#define BIT_CKDT_FILTER_EN              0x02

#define REG_PAUTH_MODE              (PP_PAGE_PAUTH1 | 0x6F)
#define BIT_CLR_BOTH_RI                 0x20

#define REG_PA_CONFIG_1             (PP_PAGE_PAUTH1 | 0x75)

#define REG_PORT_INFOFRAME_SELECT   (PP_PAGE_PAUTH1 | 0x7C)
#define VAL_RD_PORT_SEL_MASK            0x07
#define BIT_VALID_CLEAR_NO_POWER        0x08
#define BIT_VALID_CLEAR_DVI_MODE        0x10
#define BIT_RD_PIPE_SEL                (BIT5)
#define BIT_CS_PORT_PIPE_SEL           (BIT6)
#define BIT_RD_CS_PIPE_SEL             (BIT7)

#define REG_PA_CONFIG_11            (PP_PAGE_PAUTH1 | 0x7F)
#define BIT_MP_EN_3D_PACKET_MOD         0x08
#define BIT_EN_3D_VS_INSERTION          0x10
#define BIT_SKIP_HDMI_3D_FORMAT_CHK     0x20
#define BIT_SKIP_IEEE_ID_CHK            0x40
#define BIT_SKIP_VSI_HEADER_CHK         0x80

#define REG_PA_CONFIG_13            (PP_PAGE_PAUTH1 | 0x81)
#define BIT_SKIP_3D_STRUCT_CHK          0x10

    // Auto (HW) Hot Plug Event control

#define REG_HPE_CONTROL             (PP_PAGE_PAUTH1 | 0x87)
#define BIT_OVR_MP                      0x01
#define BIT_TRIGGER_MP                  0x02
#define BIT_OVR_RP                      0x08
#define BIT_TRIGGER_RP                  0x10

#define REG_HPE_HEC_PORTS           (PP_PAGE_PAUTH1 | 0x88)
#define VAL_FW_HPE_MASK                 0xFC

#define REG_HPE_TIMER_GRAN          (PP_PAGE_PAUTH1 | 0x89)   // In ~ms (1.0235ms - 129.98ms)
#define REG_HPE_HPD_START_DLY       (PP_PAGE_PAUTH1 | 0x8A)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_HPD_END_DLY         (PP_PAGE_PAUTH1 | 0x8B)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_RXTERM_START_DLY    (PP_PAGE_PAUTH1 | 0x8C)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_RXTERM_END_DLY      (PP_PAGE_PAUTH1 | 0x8D)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_HDDC_START_DLY      (PP_PAGE_PAUTH1 | 0x8E)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_HDDC_END_DLY        (PP_PAGE_PAUTH1 | 0x8F)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_EDDC_START_DLY      (PP_PAGE_PAUTH1 | 0x90)   // In steps of REG_HPE_TIMER_GRAN
#define REG_HPE_EDDC_END_DLY        (PP_PAGE_PAUTH1 | 0x91)   // In steps of REG_HPE_TIMER_GRAN

#define REG_HPE_DRIVER_OVERRIDE_MP  (PP_PAGE_PAUTH1 | 0x92)
#define REG_HPE_DRIVER_OVERRIDE_RP  (PP_PAGE_PAUTH1 | 0x93)
#define BIT_HPD_DISABLE                 0x01
#define BIT_RXTERM_DISABLE              0x02
#define BIT_HDDC_DISABLE                0x04
#define BIT_EDDC_DISABLE                0x08
#define BIT_HPD_HW_OVR                  0x10
#define BIT_RXTERM_HW_OVR               0x20
#define BIT_HDDC_HW_OVR                 0x40
#define BIT_EDDC_HW_OVR                 0x80

#define REG_HDCP_STABLE_TIME0       (PP_PAGE_PAUTH1 | 0x97)
#define REG_HDCP_STABLE_TIME1       (PP_PAGE_PAUTH1 | 0x98)

#define REG_IP_CONFIG_43            (PP_PAGE_PAUTH1 | 0x9F)
#define BGND_DISABLE                    0x80

#define REG_IP_CONFIG_47            (PP_PAGE_PAUTH1 | 0xA3)

#define REG_MHL_1X2X_PORT_CONF      (PP_PAGE_PAUTH1 | 0xB0)


#define REG_MP_PORT_ALTRES_DATA_0   (PP_PAGE_PAUTH1 | 0xCC)       // First of 8 consecutive registers
#define REG_PORT_ALTRES_MISC        (PP_PAGE_PAUTH1 | 0xD3)       // Last of 8 consecutive registers
#define BIT_IS_INTERLACED               0x01
#define BIT_IS_HDMI                     0x08
#define BIT_ALT_RES_DATA_VALID          0x10

#define REG_RP_PORT_ALTRES_DATA_0   (PP_PAGE_PAUTH1 | 0xE9)       // First of 8 consecutive registers
#define REG_RP_PORT_ALTRES_MISC     (PP_PAGE_PAUTH1 | 0xEF)       // Last of 8 consecutive registers

#define REG_MP_GCP_STATUS           (PP_PAGE_PAUTH1 | 0xE8)
#define REG_RP_GCP_STATUS           (PP_PAGE_PAUTH1 | 0xF0)
#define BIT_MP_RES_STABLE               0x40                      // Only in MP GCP status register
#define BIT_GCP_MUTE_MP                 0x04
#define BIT_GCP_UNMUTE_MP               0x08

#define REG_RP_COLOR_DEPTH          (PP_PAGE_PAUTH1 | 0xF1)

#define REG_ECC_ERRCOUNT_LO_MP      (PP_PAGE_PAUTH1 | 0xF2)
#define REG_ECC_ERRCOUNT_HI_MP      (PP_PAGE_PAUTH1 | 0xF3)

//------------------------------------------------------------------------------
// Registers in Page 6      (0x52)
//------------------------------------------------------------------------------

#define REG_DCK_FIFO                (PP_PAGE_PAUTH2 | 0x64)
#define BIT_NRST_DCK_FIFO               0x01
#define BIT_SCDT_RST_DCK_FIFO           0x40

    /* Pre-Authentication Reset On/Off Difference between Analog/Digital resets.    */
    /* This set of registers is for the trailing edge of the resets.                */

#define REG_PAUTH_RST_OFF_DIFF0     (PP_PAGE_PAUTH2 | 0x6D)
#define REG_PAUTH_RST_OFF_DIFF1     (PP_PAGE_PAUTH2 | 0x6E)
#define REG_PAUTH_RST_OFF_DIFF2     (PP_PAGE_PAUTH2 | 0x6F)
#define REG_PAUTH_RST_OFF_DIFF3     (PP_PAGE_PAUTH2 | 0x70)
#define REG_AIF_CAPTURE_HEADER      (PP_PAGE_PAUTH2 | 0x72)
#define REG_VSI_CAPTURE_HEADER      (PP_PAGE_PAUTH2 | 0x73)

#define REG_PAUTH_ALT_ZONE4_CFG     (PP_PAGE_PAUTH2 | 0x7D)
#define BIT_PAUTH_AZ4_INT_DIV20         (BIT7)

#define REG_TMDS1_EQ_DATA0          (PP_PAGE_PAUTH2 | 0x86)
#define REG_TMDS1_EQ_DATA1          (PP_PAGE_PAUTH2 | 0x87)
#define REG_TMDS1_EQ_DATA2          (PP_PAGE_PAUTH2 | 0x88)
#define REG_TMDS1_EQ_DATA3          (PP_PAGE_PAUTH2 | 0x89)
#define REG_TMDS1_EQ_DATA4          (PP_PAGE_PAUTH2 | 0x8A)
#define REG_TMDS1_EQ_DATA5          (PP_PAGE_PAUTH2 | 0x8B)
#define REG_TMDS1_EQ_DATA6          (PP_PAGE_PAUTH2 | 0x8C)
#define REG_TMDS1_EQ_DATA7          (PP_PAGE_PAUTH2 | 0x8D)

#define REG_TMDS1_BW_I2C            (PP_PAGE_PAUTH2 | 0x8E)

#define REG_TMDS0_EQ_DATA0          (PP_PAGE_PAUTH2 | 0xA4)
#define REG_TMDS0_EQ_DATA1          (PP_PAGE_PAUTH2 | 0xA5)
#define REG_TMDS0_EQ_DATA2          (PP_PAGE_PAUTH2 | 0xA6)
#define REG_TMDS0_EQ_DATA3          (PP_PAGE_PAUTH2 | 0xA7)
#define REG_TMDS0_EQ_DATA4          (PP_PAGE_PAUTH2 | 0xA8)
#define REG_TMDS0_EQ_DATA5          (PP_PAGE_PAUTH2 | 0xA9)
#define REG_TMDS0_EQ_DATA6          (PP_PAGE_PAUTH2 | 0xAA)
#define REG_TMDS0_EQ_DATA7          (PP_PAGE_PAUTH2 | 0xAB)

#define REG_TMDS0_BW_I2C            (PP_PAGE_PAUTH2 | 0xAC)
#define REG_TMDS0_BW_DATA0          (PP_PAGE_PAUTH2 | 0xAE)
#define REG_TMDS0_BW_DATA2          (PP_PAGE_PAUTH2 | 0xB0)
#define REG_TMDS0_CNTL              (PP_PAGE_PAUTH2 | 0xB3)
#define REG_TMDS1_BW_DATA0          (PP_PAGE_PAUTH2 | 0x90)
#define REG_TMDS1_BW_DATA2          (PP_PAGE_PAUTH2 | 0x92)
#define REG_TMDS1_CNTL              (PP_PAGE_PAUTH2 | 0x95)

#define REG_DPLL_MULTZONE_CTRL		(PP_PAGE_PAUTH2 | 0xC1)
#define REG_TMDS2_STAT6             (PP_PAGE_PAUTH2 | 0xC6)

#define REG_PAUTH_NUM_SAMPS         (PP_PAGE_PAUTH2 | 0xD0)
#define REG_PAUTH_SMP_INTV          (PP_PAGE_PAUTH2 | 0xD1)
#define REG_PAUTH_ADD_CONFIG1       (PP_PAGE_PAUTH2 | 0xD2)

#define REG_H_ACTIVE_L              (PP_PAGE_PAUTH2 | 0xD9)
#define REG_H_ACTIVE_H              (PP_PAGE_PAUTH2 | 0xDA)
#define MSK_H_ACTIVE_H                  0x1F

#define REG_V_ACTIVE_L              (PP_PAGE_PAUTH2 | 0xDD)
#define REG_V_ACTIVE_H              (PP_PAGE_PAUTH2 | 0xDE)
#define MSK_V_ACTIVE_H                  0x1F

#define REG_VFORMAT                 (PP_PAGE_PAUTH2 | 0xE0)
#define BIT_IS_INTERLACE                0x80

#define REG_RO_MP_CONTENT           (PP_PAGE_PAUTH2 | 0xE1)

#define REG_MP_COLOR_DEPTH          (PP_PAGE_PAUTH2 | 0xE2)
#define MSK_COLOR_DEPTH                 0x0F

#define REG_PAUTH_RP_IDLE_CNT       (PP_PAGE_PAUTH2 | 0xF2)

#define REG_BCH_CORRECTED_THSHLD    (PP_PAGE_PAUTH2 | 0xF3)

#define REG_PAUTH_MHL_TESTER_MODE   (PP_PAGE_PAUTH2 | 0xFA)



//------------------------------------------------------------------------------
// Registers in Page 7      (0x54)  // TODO:SHV - info frame registers changed in Rocky
//------------------------------------------------------------------------------

#define REG_PAUTH_MHL3D_CFG1            (PP_PAGE_PAUTH3 | 0x00)
#define REG_PAUTH_MHL3D_CFG2            (PP_PAGE_PAUTH3 | 0x01)

#define REG_PAUTH_MHL_IEEE_ID1          (PP_PAGE_PAUTH3 | 0x02)
#define REG_PAUTH_MHL_IEEE_ID2          (PP_PAGE_PAUTH3 | 0x03)
#define REG_PAUTH_MHL_IEEE_ID3          (PP_PAGE_PAUTH3 | 0x04)

#define REG_CLR_PACKET_BUFFER           (PP_PAGE_PAUTH3 | 0x20)
#define BIT_CLR_VSI                         BIT0
#define BIT_CLR_AIF                         BIT1
#define BIT_VSI_IEEE_CHK_EN                 BIT3
#define BIT_USE_AIF_FOR_VSI                 BIT5

#define REG_SP_AIF_HEADER               (PP_PAGE_PAUTH3 | 0x90)

#define REG_SP_AVI_HEADER               (PP_PAGE_PAUTH3 | 0xB0)
#define REG_SP_AVI_PB4                  (PP_PAGE_PAUTH3 | 0xB7)       // Contains VIC data
#define REG_SP_AVI_PB5                  (PP_PAGE_PAUTH3 | 0xB8)
#define REG_SP_AVI_VALID                (PP_PAGE_PAUTH3 | 0xC1)
#define BIT_SP_AVI_DATA_VALID               BIT0



//------------------------------------------------------------------------------
// Registers in Page 9      (0xE0) //TODO: consolidate with si_regs_rxede9687.h
//------------------------------------------------------------------------------

#define REG_EN_EDID                 (PP_PAGE_9 | 0x01)
#define VAL_EN_DDC0                         0x01
#define VAL_EN_DDC1                         0x02
#define VAL_EN_DDC2                         0x04
#define VAL_EN_DDC3                         0x08
#define VAL_EN_DDC_VGA                      0x10
#define VAL_EN_DDC_NONE                     0x00
#define VAL_EN_DDC_ALL                      0x1F   //EDID0-4,FW contrl

#define REG_EDID_FIFO_ADDR          (PP_PAGE_9 | 0x02)
#define VAL_FIFO_ADDR_00                    0x00
#define VAL_FIFO_ADDR_80                    0x80

#define REG_EDID_FIFO_DATA          (PP_PAGE_9 | 0x03)
#define REG_EDID_FIFO_SEL           (PP_PAGE_9 | 0x04)
#define BIT_SEL_DEVBOOT                     0x20
#define BIT_SEL_EDID0                       0x01
#define BIT_SEL_EDID_VGA                    0x10
#define VAL_SEL_EDID_MASK                   0x7F
#define BIT_KSV_SELECT                      0x80

#define REG_NVM_COMMAND             (PP_PAGE_9 | 0x05)
#define VAL_PRG_EDID                        0x03
#define VAL_PRG_DEVBOOT                     0x04
#define VAL_PRG_VGA                         0x13

#define VAL_COPY_EDID                       0x05
#define VAL_COPY_DEVBOOT                    0x06
#define VAL_COPY_VGA                        0x07


#define REG_NVM_COPYTO              (PP_PAGE_9 | 0x06)
#define VAL_NVM_COPYTO_MASK                 0x1F
#define VAL_NVM_COPYTO_PORT0                0x01

#define REG_NVM_COMMAND_DONE        (PP_PAGE_9 | 0x07)
#define BIT_NVM_COMMAND_DONE                0x01

#define REG_BSM_INIT                (PP_PAGE_9 | 0x08)
#define BIT_BSM_INIT                        0x01

#define REG_BSM_STAT                (PP_PAGE_9 | 0x09)
#define BIT_BOOT_DONE                       0x04
#define BIT_BOOT_ERROR                      0x03
#define BIT_BOOT_INPROGRESS                      0x08

#define REG_NVM_STAT                (PP_PAGE_9 | 0x10)
#define VAL_NVM_EDID_VALID                  0x01
#define VAL_NVM_DEVBOOT_VALID               0x02
#define VAL_NVM_VALID                       ( VAL_NVM_EDID_VALID | VAL_NVM_DEVBOOT_VALID )

#define REG__NVM_BOOT_SEL           (PP_PAGE_9 | 0x12)
#define VAL__NVM_BOOT_SEL_MASK              0x1F

#define REG_HPD_HW_CTRL             (PP_PAGE_9 | 0x13)
#define MSK_INVALIDATE_P0123                0xF0

#define REG_CECPA_ADDR              (PP_PAGE_9 | 0x1A)

#define REG_CECPAD_L_CH0            (PP_PAGE_9 | 0x1C)
#define REG_CECPAD_H_CH0            (PP_PAGE_9 | 0x1D)
#define REG_CHECKSUM_CH0            (PP_PAGE_9 | 0x2C)

#define REG_NVM_BSM_REPLACE         (PP_PAGE_9 | 0x36)

#define REG_AUTO_CONFIG             (PP_PAGE_9 | 0x40)
#define BIT_AUTO_TERM_EN                    0x01

#define REG_I2C_PT_CTRL             (PP_PAGE_9 | 0x46)

#define REG_POR_CTRL                (PP_PAGE_9 | 0x75)
#define MSK_PDD_THRESHOLD                   0xF0
#define MSK_AON_THRESHOLD                   0x0F

#define REG_REGUL_PWR_ENABLE        (PP_PAGE_9 | 0x78)
#define BIT_PEN_RX12V                       0x01
#define BIT_NEN_RX12V                       0x02
#define BIT_PEN_TX12V                       0x04
#define BIT_NEN_TX12V                       0x08
#define BIT_PEN_LOGIC12V                    0x10
#define BIT_NEN_LOGIC12V                    0x20

#define REG_REGUL_VOLT_CTRL         (PP_PAGE_9 | 0x7A)
#define MSK_VOLT_CTRLB                      0x38
#define MSK_VOLT_CTRL                       0x07

#define REG_REGUL_BIAS_CTRL         (PP_PAGE_9 | 0x7B)
#define MSK_BIAS_CTRL                       0x07

#define REG_DEV_ID_TYPE             (PP_PAGE_9 | 0x7E)
#define BIT_DEV_ID_SVN_REV                  BIT0

#define REG_GPIO_HW_OUT_EN0         (PP_PAGE_9 | 0xC0)
#define BIT_MP_MUTE_ON                    0x0400
#define BIT_RP_MUTE_ON                    0x0200
#define BIT_3D_LEFT_FLAG                  0x0100
#define BIT_3D_VACTIVE_FLAG               0x0080
#define BIT_3D_RIGHT_FLAG                 0x0040

#define REG_GPIO_DIR_EN0            (PP_PAGE_9 | 0xC2)
#define REG_GPIO_INT_EDGE_RISE0     (PP_PAGE_9 | 0xC4)
#define REG_GPIO_INT_EDGE_FALL0     (PP_PAGE_9 | 0xC6)
#define REG_GPIO_WRITE_0            (PP_PAGE_9 | 0xC8)
#define REG_GPIO_READ_0             (PP_PAGE_9 | 0xCA)
#define REG_GPIO_PAD_PU0            (PP_PAGE_9 | 0xCC)
#define REG_GPIO_PAD_PE0            (PP_PAGE_9 | 0xCE)


#define REG_CLKDETECT_STATUS        (PP_PAGE_9 | 0xD0)
#define BIT_CKDT_0                          0x01
#define BIT_CKDT_1                          0x02
#define BIT_CKDT_2                          0x04
#define BIT_CKDT_3                          0x08
#define VAL_CKDT_MASK                       0x0F

#define REG_PWR5V_STATUS            (PP_PAGE_9 | 0xD1)
#define BIT_PWR5V_0                         0x01
#define BIT_PWR5V_1                         0x02
#define BIT_PWR5V_2                         0x04
#define BIT_PWR5V_3                         0x08
#define VAL_PWR5V_MASK                      0x0F

#define REG_CBUS_PAD_SC				(PP_PAGE_9 | 0xD4)

#define REG_OSC_CTRL_20MHz			(PP_PAGE_9 | 0xFA)
#define REG_OSC_OTP_CALIB_20MHz		(PP_PAGE_9 | 0xFB)

#define REG_SPECIAL_PURPOSE         (PP_PAGE_9 | 0xFF)
#define BIT_HARDRESET                   0x80

//------------------------------------------------------------------------------
// Registers in Page 10         (0x64)
//------------------------------------------------------------------------------

#define REG_TMDS1_CCTRL1        (PP_PAGE_A | 0x00)

#define REG_TMDS1_CTRL2         (PP_PAGE_A | 0x02)

#define REG_TMDS1_CTRL3         (PP_PAGE_A | 0x03)

#define REG_TMDS1_CLKDETECT_CTL (PP_PAGE_A | 0x04)

#define REG_PLL1_CALREFSEL      (PP_PAGE_A | 0x07)
#define CALREFSEL_MASK                  0x0F
#define CALREFSEL_VAL                   0x03

#define REG_PLL1_ICPCNT         (PP_PAGE_A | 0x08)
#define PLL_SZONE_MASK                  0xC0
#define PLL_SZONE_VAL                   0x40

#define REG_PLL1_SPLLBIAS       (PP_PAGE_A | 0x09)
#define REG_PLL1_VCOCAL         (PP_PAGE_A | 0x0A)

#define TMDS1_BIST_TEST_SEL     (PP_PAGE_A | 0x64)

#define REG_ECC_ERRCOUNT        (PP_PAGE_A | 0x67)
#define BIT_ECC_CLR                     0x10

#define REG_TMDS0_CCTRL1        (PP_PAGE_A | 0x80)
#define VAL_CLK_MASK                    0x30
#define VAL_CLK_NORMAL                  0x00
#define VAL_CLK_EARLY                   0x10
#define VAL_CLK_DELAYED                 0x20
#define VAL_CLK_INVERTED                0x30


#define REG_TMDS0_CTRL2         (PP_PAGE_A | 0x82)
#define REG_TMDS0_CTRL3         (PP_PAGE_A | 0x83)
#define REG_TMDS0_CLKDETECT_CTL (PP_PAGE_A | 0x84)
#define REG_PLL0_CALREFSEL      (PP_PAGE_A | 0x87)

#define REG_PLL0_ICPCNT         (PP_PAGE_A | 0x88)
#define VAL_PLL0_SZONE_MASK             (BIT7 | BIT6)

#define REG_PLL0_SPLLBIAS       (PP_PAGE_A | 0x89)
#define REG_PLL0_VCOCAL         (PP_PAGE_A | 0x8A)
#define REG_TMDSRX_CTRL1		(PP_PAGE_A | 0x8B)
#define REG_TMDSRX_CTRL2        (PP_PAGE_A | 0x8C)
#define REG_TMDSRX_CTRL3        (PP_PAGE_A | 0x8D)
#define REG_TMDSRX_CTRL4        (PP_PAGE_A | 0x8E)



//------------------------------------------------------------------------------
// Registers in Page 11     (0x90)
//------------------------------------------------------------------------------

#define REG_TMDST_CTRL1         (PP_PAGE_B | 0x00)
#define MSK_TMDS_EN                     (BIT1 | BIT0)  // TMDS OE & Termination Enable
#define MSK_TMDS_OE                     (BIT4)

#define REG_TMDST_CTRL2         (PP_PAGE_B | 0x01)

#define REG_TMDST_CTRL3         (PP_PAGE_B | 0x02) //TODO: changed in spreadsheet
#define MSK_TMDS_TERM_EN_ALL            0x0F

#define REG_SYS_CTRL2           (PP_PAGE_B | 0x03)
#define BIT_CHEAP_HPD_ALT               0x80
#define BIT_PWR_AS_USEL                 0x40
#define BIT_DLY_CHG_RST_EN              0x20
#define BIT_DLY_CHG_INTR_CLRB           0x10
#define MSK_OVR_LOW_BW_EN               0x0C
#define MSK_OVR_LOW_BW_VAL              0x03

#define REG_SYS_CTRL3           (PP_PAGE_B | 0x04)
#define BIT_RST_N_DCKFIFO               0x02
#define BIT_CLR_ONLY_IN_DVI             0x01

#define REG_TX0_RSEN_STATUS     (PP_PAGE_B | 0x06)
#define REG_TX1_RSEN_STATUS     (PP_PAGE_B | 0x07)


#define REG_HDMI_TX_BW_CTRL     (PP_PAGE_B | 0x11)

#define REG_HDMI_SWING_CTRL     (PP_PAGE_B | 0x12)
#define REG_HDMI_BGAP_CTRL      (PP_PAGE_B | 0x13)
#define REG_HDMI_SWING_CTRL_TX1 (PP_PAGE_B | 0x15)

#define REG_TX_CTRL             (PP_PAGE_B | 0x16)

#define REG_PLL_LF_SEL          (PP_PAGE_B | 0x1A)

#define REG_ARCRX_TEST1         (PP_PAGE_B | 0xB0)
#define REG_ARCRX_TEST1_DEFAULT         (BIT2 | BIT3 | BIT4 | BIT5 | BIT6)
#define REG_ARCRX_TEST2         (PP_PAGE_B | 0xB1)
#define REG_ARCRX_TEST2_DEFAULT         (BIT0 | BIT6)

#endif  // __SI_9687REGS_H__

