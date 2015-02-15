#ifndef _HW_CKGEN_H_
#define _HW_CKGEN_H_

#include "x_hal_5381.h" /* for IO_VIRT */
#include "hw_pll_group.h"

#define IO_CKGEN_BASE (IO_VIRT + 0xD000)

//Page CKGEN_1
#define CKGEN_STRAP_OP (IO_CKGEN_BASE + 0x000)
    #define FLD_RS232_FORCE_JTAG_PIN Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_STRAP_3_0 Fld(4,10,AC_MSKB1)//[13:10]
#define CKGEN_CHIPID (IO_CKGEN_BASE + 0x060)
    #define FLD_CHIP_ID Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_CKG_SWID (IO_CKGEN_BASE + 0x064)
    #define FLD_SOFTWARE_ID Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_APLLCFG (IO_CKGEN_BASE + 0x080)
    #define FLD_APLL_DIV_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_APLL_BP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_APLL_K2 Fld(6,8,AC_MSKB1)//[13:8]
    #define FLD_APLL_K1 Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_APLL2CFG (IO_CKGEN_BASE + 0x084)
    #define FLD_APLL2_DIV_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_APLL2_BP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_APLL2_K2 Fld(6,8,AC_MSKB1)//[13:8]
    #define FLD_APLL2_K1 Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_MISCCFG (IO_CKGEN_BASE + 0x088)
    #define FLD_SYSPLL_DIV_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_SYSPLL_BP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_VOPLL_DIV_RST Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_VOPLL_BP Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_SAWLESSPLL_DIV_RST Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_SAWLESSPLL_BP Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_USBPLL_DIV_RST Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_USBPLL_BP Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_HADDS1_VSP_DIV_RST Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_HADDS1_VSP_BP Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_CPUPLL_DIV_RST Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_CPUPLL_BP Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_XPLL_DIV_RST Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_XPLL_BP Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_XPLL25M_DIV_RST Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_XPLL25M_BP Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_HAPLL294M_DIV_RST Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_HAPLL294M_BP Fld(1,14,AC_MSKB1)//[14:14]
#define CKGEN_APLL3CFG (IO_CKGEN_BASE + 0x08C)
    #define FLD_APLL3_DIV_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_APLL3_BP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_APLL3_K2 Fld(6,8,AC_MSKB1)//[13:8]
    #define FLD_APLL3_K1 Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_DMSSCFG0 (IO_CKGEN_BASE + 0x090)
    #define FLD_RG_PCW_NCPO_CHG Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_RG_PCW_NCPO Fld(31,0,AC_MSKDW)//[30:0]
#define CKGEN_DMSSCFG1 (IO_CKGEN_BASE + 0x094)
    #define FLD_RG_SSC_PRD Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_RG_SSC_TRI_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_RG_SSC_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_RG_FIFO_START_MAN Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_RG_SSC_PHASE_INI Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_RG_CTS_CTL Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_RG_CLK_PH_INV Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_RG_DMSS_RESETB_NOSCAN Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_RG_NCPO_EN Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_DMSSCFG2 (IO_CKGEN_BASE + 0x098)
    #define FLD_RG_SSC_DELTA1 Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_RG_SSC_DELTA Fld(16,0,AC_FULLW10)//[15:0]
#define CKGEN_PLLCALIB (IO_CKGEN_BASE + 0x0C0)
    #define FLD_CALI_FAIL Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_SOFT_RST_CAL Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_CAL_SEL Fld(3,16,AC_MSKB2)//[18:16]
    #define FLD_CAL_TRI Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_DBGCKSEL Fld(7,8,AC_MSKB1)//[14:8]
    #define FLD_CAL_MODE Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_PLLCALIBRANGEMIN (IO_CKGEN_BASE + 0x0C4)
    #define FLD_CALI_MIN Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PLLCALIBRANGEMAX (IO_CKGEN_BASE + 0x0C8)
    #define FLD_CALI_MAX Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PLLCALIBCNT (IO_CKGEN_BASE + 0x0CC)
    #define FLD_CALI_CNT Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_VPCLK_STOP (IO_CKGEN_BASE + 0x100)
    #define FLD_VDOIN_MPCLK_STOP Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_LLC_DLY_CLK_STOP Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PIP_CLK_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_PIP_CLK_STOP Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_MAIN_CLK_STOP Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_VBI2_CLK_STOP Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_VBI_CLK_STOP Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CCH_CLK_STOP Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_DVI_CLK_STOP Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_VGA_CLK_STOP Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_HDTV_CLK_STOP Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_TVD_CK54_STOP Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TVD3D_STOP Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_VPCLK_CFG (IO_CKGEN_BASE + 0x104)
    #define FLD_RST_OCLK_DIV45 Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_TCLK_DIV_SEL Fld(2,26,AC_MSKB3)//[27:26]
    #define FLD_TVD_CLK_SEL Fld(2,24,AC_MSKB3)//[25:24]
    #define FLD_RST_TCLK_DIV Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_ADC_C54M_SEL Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_TVD_CK54_SEL Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_CCD_REV Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_PIP_CLK_INV Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_MAIN_CLK_INV Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_TVE_TVD_CLK_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_OCLK_CNT_RST Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_VGA_SEL Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_PIX_CLK_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_HDTV_CLK_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_POCLK_DPLL_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_POCLK_DPLL_CKEN Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_RGBCLK_CFG (IO_CKGEN_BASE + 0x108)
    #define FLD_VGA_CKO_SEL Fld(4,28,AC_MSKB3)//[31:28]
		#define FLD_C_PIX_CLK_INV Fld(1, 29, AC_MSKB3) //29
    #define FLD_HDMI_CLK_REG_DPCLK_U3 Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_HDMI_CLK_REG_PCLK_U3 Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_HDMI_CLK_REG_DPCLK_U2 Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_HDMI_CLK_REG_PCLK_U2 Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_HDMI_CLK_REG_DPCLK_U1 Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_HDMI_CLK_REG_PCLK_U1 Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_HDMI_CLK_REG_DPCLK_U0 Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_HDMI_CLK_REG_PCLK_U0 Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_RGB_DUTY_SET Fld(4,16,AC_MSKB2)//[19:16]
    #define FLD_HADDS3_REFERENCE_SEL Fld(2,13,AC_MSKB1)//[14:13]
    #define FLD_RGB_INV_SEL Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_HADDS2_REFERENCE_SEL Fld(2,10,AC_MSKB1)//[11:10]
    #define FLD_RGB_CLK_SEL Fld(2,8,AC_MSKB1)//[9:8]
    #define FLD_HDMI_ABIST_CLK_EN Fld(4,4,AC_MSKB0)//[7:4]
    #define FLD_RGB_DELAY_SET Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_PSWCLK_CFG (IO_CKGEN_BASE + 0x10C)
    #define FLD_VDAC2_SIF_SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_TVECK_TST Fld(3,28,AC_MSKB3)//[30:28]
    #define FLD_VDAC2_PD Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_VDAC2_SEL Fld(3,24,AC_MSKB3)//[26:24]
    #define FLD_VDAC1_PD Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_VDAC1_SEL Fld(3,20,AC_MSKB2)//[22:20]
    #define FLD_TVE2FS_SEL Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_TVE54_PD Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_TVE27_SEL Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_TVE54_NORM_CK_SEL Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_OCLK_TVE_27M_PD Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_DAC2_DTDCK Fld(2,13,AC_MSKB1)//[14:13]
    #define FLD_DAC1_DTDCK Fld(2,11,AC_MSKB1)//[12:11]
    	#define FLD_DEMOD_TVE_CK_SEL Fld(2,11,AC_MSKB1)//[12:11]
    #define FLD_ABIST_CK_SEL Fld(2,9,AC_MSKB1)//[10:9]
    #define FLD_ABIST_CK1_D2_TOG Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_LVDS_CLK_CTS_SEL Fld(2,6,AC_MSKB0)//[7:6]
    #define FLD_LVDS_CLK_CTS_TOG Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_VDAC1_SIF_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_DAC1_TST_CK_SEL Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_DAC2_TST_CK_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_LVDS_CLK_CTS_PD Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TVE_ABIST_CK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_OCLK_TEST (IO_CKGEN_BASE + 0x110)
    #define FLD_OCLK_DIV_SEL Fld(2,30,AC_MSKB3)//[31:30]
    #define FLD_OCLK_DIV2_SEL Fld(2,28,AC_MSKB3)//[29:28]
    #define FLD_HDMI_TST_SEL_U3 Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_HDMI_TST_SEL_U2 Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_HDMI_TST_SEL_U1 Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_VB1_CK_INV Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_VB1_CK_SEL Fld(2,20,AC_MSKB2)//[21:20]
    #define FLD_OCLK_TVE_SEL Fld(3,16,AC_MSKB2)//[18:16]
    #define FLD_HDMI_TSTSEL Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_HADDS3_FCLK_SEL Fld(2,2,AC_MSKB0)//[3:2]
    #define FLD_HADDS2_FCLK_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_TCLK_TEST (IO_CKGEN_BASE + 0x114)
    #define FLD_TCLK_INV Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_TCLK_PD Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_TCLK_TSTSEL Fld(2,20,AC_MSKB2)//[21:20]
    #define FLD_TCLK_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define FLD_OCLK_INV Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_OCLK_PD Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_OCLK_TSTSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define FLD_OCLK_SEL Fld(2,0,AC_MSKB0)//[1:0]
        #define FLD_OCLK_SEL_XTAL 0
        #define FLD_OCLK_SEL_LVDS 1
        #define FLD_OCLK_SEL_PPLL 2
#define CKGEN_DDDSVSP_CFG (IO_CKGEN_BASE + 0x118)
    #define FLD_FS_OCLK_INV Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_DDDS2_VSP_TSTCK_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_DDDS1_VSP_TSTCK_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_FS_OC_PRE2_SEL Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_FS_OC_PRE1_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_FS_OC_TSTCK_SEL Fld(2,0,AC_MSKB0)//[1:0]
        #define FS_OC_TSTCK_SEL_DDDS2_NOSS 0
        #define FS_OC_TSTCK_SEL_DDDS1_SS 1
        #define FS_OC_TSTCK_SEL_DDDS1_SRC_CK 2
#define CKGEN_DBGCFG (IO_CKGEN_BASE + 0x140)
    #define FLD_DBGCK_DIV_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_DBGCK_BP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_MONCK_DIV_RST Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_MONCK_BP Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_MON_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_ABIST_CKCFG (IO_CKGEN_BASE + 0x144)
    #define FLD_DUTY_HIGH_CNT Fld(8,24,AC_FULLB3)//[31:24]
    #define FLD_DUTY_ALL_CNT Fld(8,16,AC_FULLB2)//[23:16]
    #define FLD_DUTY_METER_TARGET_SEL Fld(4,12,AC_MSKB1)//[15:12]
    #define FLD_DUTY_METER_DONE Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_DUTY_METER_ON Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_ABIST_VGA_DIV Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_ABIST_VGA_TOG Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_ABIST_SRAM_CKCFG (IO_CKGEN_BASE + 0x148)
    #define FLD_ABIST_SRAM_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_ABIST_SRAM_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_ABIST_SRAM_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_ABIST_PULSE_CFG (IO_CKGEN_BASE + 0x154)
    #define FLD_ABIST_PULSE_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_ABIST_PULSE_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_CI_TSIF_CFG (IO_CKGEN_BASE + 0x158)
    #define FLD_CI_TSO2_GCK_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_CI_TSO2_CK_INV Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_CI_TSO1_GCK_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_CI_TSO1_CK_INV Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_CI_TSO2_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_CI_TSO2_OE Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CI_TSO1_OE Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CI_SCK_INV Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_SYNC_MODE Fld(4,8,AC_MSKB1)//[11:8]
    #define FLD_CI_ADR_OE Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_S2P_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_DATA_FLIP_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CK_INV Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_SYNC_INV Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_VLD_INV Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_SOFT_RST Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_EXTTS_CKCFG (IO_CKGEN_BASE + 0x160)
    #define FLD_EXT2_TS_CK_SEL Fld(3,20,AC_MSKB2)//[22:20]
    #define FLD_EXT1_TS_CK_SEL Fld(3,16,AC_MSKB2)//[18:16]
    #define FLD_CI_TSO2_CK_SEL Fld(3,12,AC_MSKB1)//[14:12]
    #define FLD_CI_TSO_CK_SEL Fld(3,8,AC_MSKB1)//[10:8]
#define CKGEN_ETH_MISC_CTRL (IO_CKGEN_BASE + 0x164)
    #define FLD_ETH_DACG_TCLK_INV Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_ETH_TXT_TCLK_INV Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_ETH_AFE_PWD Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_ETH_ANA_XPLL_CK_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_MBIST_CK_CTRL (IO_CKGEN_BASE + 0x16C)
    #define FLD_BIST_OD_CK_SEL Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_BIST_VDEC_SLWCK_SEL Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_BIST_AUD_CK_SEL Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_BIST_DISP_CLK_SEL Fld(2,18,AC_MSKB2)//[19:18]
    #define FLD_BIST_CK54_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_BIST_PCLK_SEL Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_BIST_RGB_SEL Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_BIST_PIP_SEL Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_BIST_MAIN_SEL Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_BIST_TCLK_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_BIST_OCLK_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_BIST_VPCLK_SEL Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_BIST_SS_OCLK_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_BIST_VDOIN_OCLK_SEL Fld(1,1,AC_MSKB0)//[1:1]
#define CKGEN_BUS_DCM_CTRL (IO_CKGEN_BASE + 0x170)
    #define FLD_BUS_DCM_CK Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_BUS_DCM_DEMOD Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_BUS_DCM_VIPROC Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_BUS_DCM_DISP Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_BUS_DCM_VDEC Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_BUS_DCM_IP Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_BLOCK_RST_CFG0 (IO_CKGEN_BASE + 0x1C0)
    #define FLD_PASER_TOP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_USB_RST Fld(2,27,AC_MSKB3)//[28:27]
    #define FLD_ETHER_RST Fld(2,25,AC_MSKB3)//[26:25]
    #define FLD_NIC_DRAMMSTR_RST Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_IMGRZ_RST Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_GCPU_RST Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_GDMA_RST Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PERI_RST Fld(9,10,AC_MSKW21)//[18:10]
    #define FLD_RFI_RST Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_JPEG_RST Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_B2R_RST Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_GRAPH_RST Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_DDRPHY_RST Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_DMARB_RST Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_SIM_HOST_RST Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_MSDC_RST Fld(1,0,AC_MSKB0)//[0:0]

//Page CKGEN_2
#define CKGEN_BLOCK_RST_CFG1 (IO_CKGEN_BASE + 0x1C4)
    #define FLD_AUDIO_PWM2_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_AUDIO_PWM1_RST Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_AUDIO_AIN_RST Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_SCALER_RST Fld(3,24,AC_MSKB3)//[26:24]
    #define FLD_ANA_REG_RST Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_VDEC_RST Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_DEMUX_RST Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_POD_RST Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_TCON_RST Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_TVE_RST Fld(2,13,AC_MSKB1)//[14:13]
    #define FLD_DVBT_RST Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_ATSC_RST Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_ATD_RST Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_VDOIN_RST Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_AUDIO_RST Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_MIB_RST Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_OD_RST Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_OSD_RST Fld(1,1,AC_MSKB0)//[1:1]
#define CKGEN_BLOCK_CKEN_CFG0 (IO_CKGEN_BASE + 0x1C8)
    #define FLD_PARSER_TOP Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_USB_CKEN Fld(2,27,AC_MSKB3)//[28:27]
    #define FLD_ETHER_CKEN Fld(2,25,AC_MSKB3)//[26:25]
    #define FLD_NIC_DRAMMSTR_TOP_CKEN Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_IMGRZ_CKEN Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_GCPU_CKEN Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_GDMA_CKEN Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PERI_CKEN Fld(9,10,AC_MSKW21)//[18:10]
    #define FLD_RFI_CKEN Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_JPEG_CKEN Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_B2R_CKEN Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_GRAPH_CKEN Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_DDRPHY_CKEN Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_DMARB_CKEN Fld(2,3,AC_MSKB0)//[4:3]
    #define FLD_SIM_HOST_CKEN Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_MSDC_CKEN Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_BLOCK_CKEN_CFG1 (IO_CKGEN_BASE + 0x1CC)
    #define FLD_AUDIO_PWM2_CKEN Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_AUDIO_PWM1_CKEN Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_AUDIO_AIN_CKEN Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_SCALER_CKEN Fld(3,24,AC_MSKB3)//[26:24]
    #define FLD_ANA_REG_CKEN Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_VDEC_CKEN Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_DEMUX_CKEN Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_POD_CKEN Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_TCON_CKEN Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_TVE_CKEN Fld(2,13,AC_MSKB1)//[14:13]
    #define FLD_DVBT_CKEN Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_ATSC_CKEN Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_ATD_CKEN Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_VDOIN_CKEN Fld(3,7,AC_MSKW10)//[9:7]
    #define FLD_AUDIO_CKEN Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_MIB_CKEN Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_OD_CKEN Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_OSD_CKEN Fld(1,1,AC_MSKB0)//[1:1]
#define CKGEN_MISC_CKCFG (IO_CKGEN_BASE + 0x200)
    #define FLD_TOG Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_DLY_SOURCE Fld(4,12,AC_MSKB1)//[15:12]
    #define FLD_DLY_PHASE Fld(4,8,AC_MSKB1)//[11:8]
    #define FLD_SLOW_CK Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_CPU_CKCFG (IO_CKGEN_BASE + 0x208)
    #define FLD_BUS_CK_TST Fld(3,11,AC_MSKB1)//[13:11]
    #define FLD_BUS_CK_SEL Fld(3,8,AC_MSKB1)//[10:8]
    #define FLD_CPU_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_CPU_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_FLASH_CKCFG (IO_CKGEN_BASE + 0x20C)
    #define FLD_FLASH_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_FLASH_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
#define CKGEN_MEM_CKCFG (IO_CKGEN_BASE + 0x210)
    #define FLD_M_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
#define CKGEN_AUD_CKCFG (IO_CKGEN_BASE + 0x214)
    #define FLD_RST_HDMI_CNT Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_HDMI_CNT Fld(3,20,AC_MSKB2)//[22:20]
    #define FLD_TOG_HDMI_CLK Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_RST_AO_CNT Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_AO_CNT Fld(3,12,AC_MSKB1)//[14:12]
    #define FLD_TOG_AMOCLK Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_AUD_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_AUD_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_AUD_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_AUD2_CKCFG (IO_CKGEN_BASE + 0x218)
    #define FLD_RST_BO_CNT Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_BO_CNT Fld(3,12,AC_MSKB1)//[14:12]
    #define FLD_TOG_BOMCLK Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_AUD2_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_AUD2_DK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_AUD2_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_AUD3_CKCFG (IO_CKGEN_BASE + 0x21C)
    #define FLD_AUD3_CK_DIVIDE Fld(8,16,AC_FULLB2)//[23:16]
    #define FLD_AUD3_CK_RST Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_AUD3_CK_BYPASS Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_AUD3_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_AUD3_DK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_AUD3_CK_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_PWM_CKCFG (IO_CKGEN_BASE + 0x220)
    #define FLD_PWMH_CK_TST Fld(3,8,AC_MSKB1)//[10:8]
    #define FLD_PWM_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_PWM_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_PWM_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_PWM2_CKCFG (IO_CKGEN_BASE + 0x224)
    #define FLD_PWMH2_CK_TST Fld(3,8,AC_MSKB1)//[10:8]
    #define FLD_PWM2_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_PWM2_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_PWM2_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_B2R_HD_CKCFG (IO_CKGEN_BASE + 0x228)
    #define FLD_B2R_HD_CK_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_B2R_HD_CK_SRC Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_B2R_HD_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_B2R_HD_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_B2R_HD_CK_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_B2R_HD_DIVCFG (IO_CKGEN_BASE + 0x22C)
    #define FLD_B2R_HD_CK_IDEAL_PLL_N Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_B2R_HD_CK_IDEAL_PLL_M Fld(16,0,AC_FULLW10)//[15:0]
#define CKGEN_B2R_SD_CKCFG (IO_CKGEN_BASE + 0x230)
    #define FLD_B2R_SD_CK_SEL_2 Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_B2R_SD_CK_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_B2R_SD_CK_SRC Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_B2R_SD_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_B2R_SD_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_B2R_SD_CK_PRE_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_B2R_SD_DIVCFG (IO_CKGEN_BASE + 0x234)
    #define FLD_B2R_SD_CK_IDEAL_PLL_N Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_B2R_SD_CK_IDEAL_PLL_M Fld(16,0,AC_FULLW10)//[15:0]
#define CKGEN_TS_CKCFG (IO_CKGEN_BASE + 0x238)
    #define FLD_TS1_CK_PD Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_TS1_CK_TOG Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_TS1_CK_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define FLD_TS1_CNT_RST Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_TS1_CNT Fld(3,12,AC_MSKB1)//[14:12]
    #define FLD_TS0_CNT_RST Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_TS0_CNT Fld(3,8,AC_MSKB1)//[10:8]
    #define FLD_TS0_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_TS0_CK_TOG Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_TS0_CK_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_PWM3_CKCFG (IO_CKGEN_BASE + 0x23C)
    #define FLD_PWMH3_CK_TST Fld(3,8,AC_MSKB1)//[10:8]
    #define FLD_PWM3_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_PWM3_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_PWM3_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_SPI_CKCFG (IO_CKGEN_BASE + 0x300)
    #define FLD_SPI_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_SPI_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_SPI_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_TSOUT_CKCFG (IO_CKGEN_BASE + 0x304)
    #define FLD_TSOUT_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_TSOUT_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_TSOUT_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_ATDAD_CKCFG (IO_CKGEN_BASE + 0x308)
    #define FLD_ATDAD_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_ATDAD_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_ATDAD_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_CLK54M_CKCFG (IO_CKGEN_BASE + 0x30C)
    #define FLD_CLK54M_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CLK54M_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_CLK54M_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_DMX_CKCFG (IO_CKGEN_BASE + 0x314)
    #define FLD_DMX_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DMX_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DMX_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_DSP_CKCFG (IO_CKGEN_BASE + 0x31C)
    #define FLD_DSP_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DSP_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DSP_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_AIN_CKCFG (IO_CKGEN_BASE + 0x320)
    #define FLD_AIN_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_AIN_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_AIN_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_USB_CLK_CKCFG (IO_CKGEN_BASE + 0x324)
    #define FLD_USBPHY_DIVIDE_RST Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_USBPHY_CK_DIV9 Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_USBPLL_SEL Fld(2,9,AC_MSKB1)//[10:9]
#define CKGEN_REG_NFI_CKCFG (IO_CKGEN_BASE + 0x328)
    #define FLD_NFI_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_NFI_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_NFI_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_SD_CKCFG (IO_CKGEN_BASE + 0x32C)
    #define FLD_SD_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_SD_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_SD_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_SC_CKCFG (IO_CKGEN_BASE + 0x340)
    #define FLD_SC_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_SC_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_SC_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_CI_CKCFG (IO_CKGEN_BASE + 0x358)
    #define FLD_CI_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CI_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_CI_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_DISP_CKCFG (IO_CKGEN_BASE + 0x368)
    #define FLD_SVPCLK_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_OSD1_PCLK_SEL Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_RESYNC_PCLK_TSTSEL Fld(2,24,AC_MSKB3)//[25:24]
    #define FLD_RESYNC_PCLK_TOG Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_TDDC_CK_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_POCLK_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_ASRC_CKCFG (IO_CKGEN_BASE + 0x36C)
    #define FLD_ASRC_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_ASRC_CK_TSTSEL Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_ASRC_CK_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_POCLK_DPLL_DIVCFG (IO_CKGEN_BASE + 0x378)
    #define FLD_POCLK_DPLL_IDEAL_PLL_N Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_POCLK_DPLL_IDEAL_PLL_M Fld(16,0,AC_FULLW10)//[15:0]
#define CKGEN_REG_CLK216M_CKCFG (IO_CKGEN_BASE + 0x388)
    #define FLD_CLK216M_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CLK216M_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_CLK216M_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_DVBTCI_CKCFG (IO_CKGEN_BASE + 0x38C)
    #define FLD_DVBTCI_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DVBTCI_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DVBTCI_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_JPEG_CKCFG (IO_CKGEN_BASE + 0x394)
    #define FLD_JPEG_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_JPEG_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_JPEG_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_GCPU_CKCFG (IO_CKGEN_BASE + 0x39C)
    #define FLD_GCPU_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_GCPU_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_GCPU_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_HCLK_CKCFG (IO_CKGEN_BASE + 0x3A0)
    #define FLD_HCLK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_HCLK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_HCLK_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_USB_CKCFG (IO_CKGEN_BASE + 0x3A4)
    #define FLD_USB_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_USB_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_USB_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_USB_FREE_CKCFG (IO_CKGEN_BASE + 0x3A8)
    #define FLD_USB_FREE_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_USB_FREE_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_USB_FREE_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_NIC_EXT_CKCFG (IO_CKGEN_BASE + 0x3AC)
    #define FLD_NIC_EXT_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_NIC_EXT_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_NIC_EXT_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_NIC_SLAVE_CKCFG (IO_CKGEN_BASE + 0x3B0)
    #define FLD_NIC_SLAVE_MISC Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_NIC_SLAVE_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_NIC_SLAVE_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_NIC_SLAVE_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_MIB_OCLK_CKCFG (IO_CKGEN_BASE + 0x3B4)
    #define FLD_MIB_OCLK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_MIB_OCLK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_MIB_OCLK_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_XDDS_CKCFG (IO_CKGEN_BASE + 0x3B8)
    #define FLD_XDDS_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_XDDS_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_XDDS_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_XDDS_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_DMSS_CKCFG (IO_CKGEN_BASE + 0x3BC)
    #define FLD_DMSS_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_DMSS_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DMSS_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DMSS_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_DDDS1_CKCFG (IO_CKGEN_BASE + 0x3C0)
    #define FLD_DDDS1_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_DDDS1_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DDDS1_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DDDS1_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_IMGRZ_CKCFG (IO_CKGEN_BASE + 0x3C4)
    #define FLD_IMGRZ_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_IMGRZ_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_IMGRZ_SEL Fld(4,0,AC_MSKB0)//[3:0]

//Page CKGEN_3
#define CKGEN_GRA2D_CKCFG (IO_CKGEN_BASE + 0x3C8)
    #define FLD_GRA2D_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_GRA2D_CK_TSTSEL Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_GRA2D_CK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_DVBTFEC_CKCFG (IO_CKGEN_BASE + 0x3DC)
    #define FLD_DVBTFEC_MISC Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_DVBTFEC_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DVBTFEC_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DVBTFEC_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_C27M_CKCFG (IO_CKGEN_BASE + 0x3E0)
    #define FLD_C27M_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_C27M_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_C27M_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_REG_BIST_CKCFG (IO_CKGEN_BASE + 0x3E4)
    #define FLD_BIST_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_BIST_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_BIST_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_VDEC_SYS_CKCFG (IO_CKGEN_BASE + 0x3F4)
    #define FLD_VDEC_SYS_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_VDEC_SYS_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_VDEC_SYS_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define CKGEN_REG_HADDS1_CKCFG (IO_CKGEN_BASE + 0x3FC)
    #define FLD_HADDS1_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_HADDS1_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_HADDS1_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_HADDS1_SEL Fld(1,0,AC_MSKB0)//[0:0]
		#define FLD_HADDS1_ALL Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_REG_PWM_PERI_CKCFG (IO_CKGEN_BASE + 0x404)
    #define FLD_PWM_PERI_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_PWM_PERI_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_PWM_PERI_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_REG_NIC_MASTER_CKCFG (IO_CKGEN_BASE + 0x41C)
    #define FLD_NIC_MASTER_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_NIC_MASTER_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_NIC_MASTER_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_SPDIFIN_CKCFG (IO_CKGEN_BASE + 0x420)
    #define FLD_SPDIFIN_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_SPDIFIN_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_SPDIFIN_CK_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_HDMI_FREQMETER_CKCFG (IO_CKGEN_BASE + 0x424)
    #define FLD_VDAC_CAL_CK_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_TVE_TVD_UP_CK_SEL Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_TVE_54_SRC_XOR Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_TVE_TVD_UPCK_XOR Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_TVE_108_CK_TST Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_HDMICK2_SEL Fld(3,3,AC_MSKB0)//[5:3]
    #define FLD_HDMICK1_SEL Fld(3,0,AC_MSKB0)//[2:0]
#define CKGEN_REG_DEMOD_VDAC_CKCFG (IO_CKGEN_BASE + 0x428)
    #define FLD_DEMOD_VDAC_CK_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_DEMOD_VDAC_CK_TST Fld(3,4,AC_MSKB0)//[6:4]
    #define FLD_DEMOD_VDAC_CK_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_PADPUCFG (IO_CKGEN_BASE + 0x500)
    #define FLD_CIGPIO1_PU Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO0_PU Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_ASPDIFIN_PU Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_GPIO2_PU Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_GPIO5_PU Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_GPIO4_PU Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_GPIO3_PU Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_POWE_PU Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_PAALE_PU Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_PACLE_PU Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_POCE0_PU Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_POCE1_PU Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_POOE_PU Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PARB_PU Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_PDD7_PU Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_PDD6_PU Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_PDD5_PU Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_PDD4_PU Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_PDD3_PU Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_PDD2_PU Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PDD1_PU Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PDD0_PU Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_OSCL1_PU Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_OSDA1_PU Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_OPWM2_PU Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_OPWM1_PU Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_OPWM0_PU Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_GPIO1_PU Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_GPIO0_PU Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_TCON8_PU Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_TCON5_PU Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TCON2_PU Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADPUCFG1 (IO_CKGEN_BASE + 0x504)
    #define FLD_CIGPIO42_PU Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO41_PU Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_CIGPIO40_PU Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_CIGPIO39_PU Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_CIGPIO38_PU Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_CIGPIO37_PU Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_CIGPIO36_PU Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_CIGPIO35_PU Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_CIGPIO34_PU Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_CIGPIO33_PU Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_CIGPIO32_PU Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_CIGPIO31_PU Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_CIGPIO30_PU Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_CIGPIO29_PU Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_CIGPIO28_PU Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_CIGPIO27_PU Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_CIGPIO26_PU Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_CIGPIO19_PU Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO14_PU Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO13_PU Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO12_PU Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO11_PU Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO10_PU Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO9_PU Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO8_PU Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO7_PU Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_GPIO6_PU Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO6_PU Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO5_PU Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPIO4_PU Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO3_PU Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO2_PU Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADPUCFG2 (IO_CKGEN_BASE + 0x508)
    #define FLD_GPIO7_PU Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO56_PU Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO55_PU Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO54_PU Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO53_PU Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO52_PU Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO51_PU Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO50_PU Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO49_PU Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CIGPIO48_PU Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO47_PU Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO46_PU Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPI45_PU Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO44_PU Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO43_PU Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADPDCFG (IO_CKGEN_BASE + 0x510)
    #define FLD_CIGPIO1_PD Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO0_PD Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_ASPDIFIN_PD Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_GPIO2_PD Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_GPIO5_PD Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_GPIO4_PD Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_GPIO3_PD Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_POWE_PD Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_PAALE_PD Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_PACLE_PD Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_POCE0_PD Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_POCE1_PD Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_POOE_PD Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PARB_PD Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_PDD7_PD Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_PDD6_PD Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_PDD5_PD Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_PDD4_PD Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_PDD3_PD Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_PDD2_PD Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PDD1_PD Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PDD0_PD Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_OSCL1_PD Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_OSDA1_PD Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_OPWM2_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_OPWM1_PD Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_OPWM0_PD Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_GPIO1_PD Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_GPIO0_PD Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_TCON8_PD Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_TCON5_PD Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TCON2_PD Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADPDCFG1 (IO_CKGEN_BASE + 0x514)
    #define FLD_CIGPIO42_PD Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO41_PD Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_CIGPIO40_PD Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_CIGPIO39_PD Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_CIGPIO38_PD Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_CIGPIO37_PD Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_CIGPIO36_PD Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_CIGPIO35_PD Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_CIGPIO34_PD Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_CIGPIO33_PD Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_CIGPIO32_PD Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_CIGPIO31_PD Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_CIGPIO30_PD Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_CIGPIO29_PD Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_CIGPIO28_PD Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_CIGPIO27_PD Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_CIGPIO26_PD Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_CIGPIO19_PD Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO14_PD Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO13_PD Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO12_PD Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO11_PD Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO10_PD Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO9_PD Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO8_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO7_PD Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_GPIO6_PD Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO6_PD Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO5_PD Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPIO4_PD Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO3_PD Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO2_PD Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADPDCFG2 (IO_CKGEN_BASE + 0x518)
    #define FLD_GPIO7_PD Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO56_PD Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO55_PD Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO54_PD Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO53_PD Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO52_PD Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO51_PD Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO50_PD Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO49_PD Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CIGPIO48_PD Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO47_PD Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO46_PD Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPI45_PD Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO44_PD Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO43_PD Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADSMTCFG (IO_CKGEN_BASE + 0x520)
    #define FLD_CIGPIO1_SMT Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO0_SMT Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_ASPDIFIN_SMT Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_GPIO2_SMT Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_GPIO5_SMT Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_GPIO4_SMT Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_GPIO3_SMT Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_POWE_SMT Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_PAALE_SMT Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_PACLE_SMT Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_POCE0_SMT Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_POCE1_SMT Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_POOE_SMT Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PARB_SMT Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_PDD7_SMT Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_PDD6_SMT Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_PDD5_SMT Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_PDD4_SMT Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_PDD3_SMT Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_PDD2_SMT Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PDD1_SMT Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PDD0_SMT Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_OSCL1_SMT Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_OSDA1_SMT Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_OPWM2_SMT Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_OPWM1_SMT Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_OPWM0_SMT Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_GPIO1_SMT Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_GPIO0_SMT Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_TCON8_SMT Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_TCON5_SMT Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TCON2_SMT Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADSMTCFG1 (IO_CKGEN_BASE + 0x524)
    #define FLD_CIGPIO42_SMT Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO41_SMT Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_CIGPIO40_SMT Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_CIGPIO39_SMT Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_CIGPIO38_SMT Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_CIGPIO37_SMT Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_CIGPIO36_SMT Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_CIGPIO35_SMT Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_CIGPIO34_SMT Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_CIGPIO33_SMT Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_CIGPIO32_SMT Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_CIGPIO31_SMT Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_CIGPIO30_SMT Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_CIGPIO29_SMT Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_CIGPIO28_SMT Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_CIGPIO27_SMT Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_CIGPIO26_SMT Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_CIGPIO19_SMT Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO14_SMT Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO13_SMT Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO12_SMT Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO11_SMT Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO10_SMT Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO9_SMT Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO8_SMT Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO7_SMT Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_GPIO6_SMT Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO6_SMT Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO5_SMT Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPIO4_SMT Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO3_SMT Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO2_SMT Fld(1,0,AC_MSKB0)//[0:0]

//Page CKGEN_4
#define CKGEN_PADSMTCFG2 (IO_CKGEN_BASE + 0x528)
    #define FLD_GPIO7_SMT Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO56_SMT Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO55_SMT Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO54_SMT Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO53_SMT Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO52_SMT Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO51_SMT Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO50_SMT Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO49_SMT Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CIGPIO48_SMT Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO47_SMT Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO46_SMT Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPI45_SMT Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO44_SMT Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO43_SMT Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADSRCFG (IO_CKGEN_BASE + 0x530)
    #define FLD_CIGPIO1_SR Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO0_SR Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_ASPDIFIN_SR Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_GPIO2_SR Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_GPIO5_SR Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_GPIO4_SR Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_GPIO3_SR Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_POWE_SR Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_PAALE_SR Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_PACLE_SR Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_POCE0_SR Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_POCE1_SR Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_POOE_SR Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_PARB_SR Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_PDD7_SR Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_PDD6_SR Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_PDD5_SR Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_PDD4_SR Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_PDD3_SR Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_PDD2_SR Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PDD1_SR Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PDD0_SR Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_OSCL1_SR Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_OSDA1_SR Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_OPWM2_SR Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_OPWM1_SR Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_OPWM0_SR Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_GPIO1_SR Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_GPIO0_SR Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_TCON8_SR Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_TCON5_SR Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_TCON2_SR Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADSRCFG1 (IO_CKGEN_BASE + 0x534)
    #define FLD_CIGPIO42_SR Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_CIGPIO41_SR Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_CIGPIO40_SR Fld(1,29,AC_MSKB3)//[29:29]
    #define FLD_CIGPIO39_SR Fld(1,28,AC_MSKB3)//[28:28]
    #define FLD_CIGPIO38_SR Fld(1,27,AC_MSKB3)//[27:27]
    #define FLD_CIGPIO37_SR Fld(1,26,AC_MSKB3)//[26:26]
    #define FLD_CIGPIO36_SR Fld(1,25,AC_MSKB3)//[25:25]
    #define FLD_CIGPIO35_SR Fld(1,24,AC_MSKB3)//[24:24]
    #define FLD_CIGPIO34_SR Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_CIGPIO33_SR Fld(1,22,AC_MSKB2)//[22:22]
    #define FLD_CIGPIO32_SR Fld(1,21,AC_MSKB2)//[21:21]
    #define FLD_CIGPIO31_SR Fld(1,20,AC_MSKB2)//[20:20]
    #define FLD_CIGPIO30_SR Fld(1,19,AC_MSKB2)//[19:19]
    #define FLD_CIGPIO29_SR Fld(1,18,AC_MSKB2)//[18:18]
    #define FLD_CIGPIO28_SR Fld(1,17,AC_MSKB2)//[17:17]
    #define FLD_CIGPIO27_SR Fld(1,16,AC_MSKB2)//[16:16]
    #define FLD_CIGPIO26_SR Fld(1,15,AC_MSKB1)//[15:15]
    #define FLD_CIGPIO19_SR Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO14_SR Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO13_SR Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO12_SR Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO11_SR Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO10_SR Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO9_SR Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO8_SR Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO7_SR Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_GPIO6_SR Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO6_SR Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO5_SR Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPIO4_SR Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO3_SR Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO2_SR Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADSRCFG2 (IO_CKGEN_BASE + 0x538)
    #define FLD_GPIO7_SR Fld(1,14,AC_MSKB1)//[14:14]
    #define FLD_CIGPIO56_SR Fld(1,13,AC_MSKB1)//[13:13]
    #define FLD_CIGPIO55_SR Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_CIGPIO54_SR Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_CIGPIO53_SR Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_CIGPIO52_SR Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_CIGPIO51_SR Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_CIGPIO50_SR Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CIGPIO49_SR Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CIGPIO48_SR Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_CIGPIO47_SR Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_CIGPIO46_SR Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_CIGPI45_SR Fld(1,2,AC_MSKB0)//[2:2]
    #define FLD_CIGPIO44_SR Fld(1,1,AC_MSKB0)//[1:1]
    #define FLD_CIGPIO43_SR Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_PADDRVCFGE2_0 (IO_CKGEN_BASE + 0x540)
    #define FLD_PADDRV_E2_0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADDRVCFGE2_1 (IO_CKGEN_BASE + 0x544)
    #define FLD_PADDRV_E2_1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADDRVCFGE2_2 (IO_CKGEN_BASE + 0x548)
    #define FLD_PADDRV_E2_2 Fld(14,0,AC_MSKW10)//[13:0]
#define CKGEN_PADDRVCFGE4_0 (IO_CKGEN_BASE + 0x550)
    #define FLD_PADDRV_E4_0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADDRVCFGE4_1 (IO_CKGEN_BASE + 0x554)
    #define FLD_PADDRV_E4_1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADDRVCFGE4_2 (IO_CKGEN_BASE + 0x558)
    #define FLD_PADDRV_E4_2 Fld(14,0,AC_MSKW10)//[13:0]
        #define PADDRV_2MA 0
        #define PADDRV_4MA 1
        #define PADDRV_6MA 2
        #define PADDRV_8MA 3

#define CKGEN_PADRDSEL0 (IO_CKGEN_BASE + 0x560)
    #define FLD_PADRDSEL0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADRDSEL1 (IO_CKGEN_BASE + 0x564)
    #define FLD_PADRDSEL1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL0 (IO_CKGEN_BASE + 0x568)
    #define FLD_PADTDSEL0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL1 (IO_CKGEN_BASE + 0x56C)
    #define FLD_PADTDSEL1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL2 (IO_CKGEN_BASE + 0x570)
    #define FLD_PADTDSEL2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL3 (IO_CKGEN_BASE + 0x574)
    #define FLD_PADTDSEL3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MONASEL (IO_CKGEN_BASE + 0x580)
    #define FLD_MONA Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MONBSEL (IO_CKGEN_BASE + 0x584)
    #define FLD_MONB Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MONCSEL (IO_CKGEN_BASE + 0x588)
    #define FLD_MONC Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADRDSEL2 (IO_CKGEN_BASE + 0x590)
    #define FLD_PADRDSEL2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADRDSEL3 (IO_CKGEN_BASE + 0x594)
    #define FLD_PADRDSEL3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL4 (IO_CKGEN_BASE + 0x598)
    #define FLD_PADTDSEL4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL5 (IO_CKGEN_BASE + 0x59C)
    #define FLD_PADTDSEL5 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL6 (IO_CKGEN_BASE + 0x5A0)
    #define FLD_PADTDSEL6 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL7 (IO_CKGEN_BASE + 0x5A4)
    #define FLD_PADTDSEL7 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL8 (IO_CKGEN_BASE + 0x5A8)
    #define FLD_PADTDSEL8 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PADTDSEL9 (IO_CKGEN_BASE + 0x5AC)
    #define FLD_PADTDSEL9 Fld(28,0,AC_MSKDW)//[27:0]
#define CKGEN_PADRDSEL4 (IO_CKGEN_BASE + 0x5B0)
    #define FLD_PADRDSEL4 Fld(30,0,AC_MSKDW)//[29:0]
#define CKGEN_MSDCPAD_E2 (IO_CKGEN_BASE + 0x5C0)
    #define FLD_MSDCPAD_E2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MSDCPAD_E4 (IO_CKGEN_BASE + 0x5C4)
    #define FLD_MSDCPAD_E4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MSDCPAD_SR (IO_CKGEN_BASE + 0x5C8)
    #define FLD_MSDCPAD_SR Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MSDCPAD_SMT (IO_CKGEN_BASE + 0x5CC)
    #define FLD_MSDCPAD_SMT Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MSDCPAD_PU (IO_CKGEN_BASE + 0x5D0)
    #define FLD_MSDCPAD_PU Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MSDCPAD_PD (IO_CKGEN_BASE + 0x5D4)
    #define FLD_MSDCPAD_PD Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX0 (IO_CKGEN_BASE + 0x600)
    #define FLD_PMUX0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX1 (IO_CKGEN_BASE + 0x604)
    #define FLD_PMUX1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX2 (IO_CKGEN_BASE + 0x608)
    #define FLD_PMUX2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX3 (IO_CKGEN_BASE + 0x60C)
    #define FLD_PMUX3 Fld(32,0,AC_FULLDW)//[31:0]
	#define FLD_PAD_AL0 Fld(1,23,AC_MSKB2)//[23]
	#define FLD_PAD_AR0 Fld(1,24,AC_MSKB3)//[24]
    #define FLD_PAD_AL1 Fld(1,25,AC_MSKB3)//[25]
    #define FLD_PAD_AR1 Fld(1,26,AC_MSKB3)//[26]
#define CKGEN_PMUX4 (IO_CKGEN_BASE + 0x610)
    #define FLD_PMUX4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX5 (IO_CKGEN_BASE + 0x614)
    #define FLD_PMUX5 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX_MISC (IO_CKGEN_BASE + 0x620)
    #define FLD_CIGPIO16_GPIO_PU Fld(1,31,AC_MSKB3)//[31:31]
    #define FLD_MSDC_CMDO_SEL Fld(1,30,AC_MSKB3)//[30:30]
    #define FLD_MSDC_DATA_SEL Fld(2,28,AC_MSKB3)//[29:28]
    #define FLD_OCLK_27M_HDMI Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PLL_XCLK_SEL Fld(1,3,AC_MSKB0)//[3:3]
#define CKGEN_PMUX6 (IO_CKGEN_BASE + 0x624)
    #define FLD_PMUX6 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX7 (IO_CKGEN_BASE + 0x628)
    #define FLD_PMUX7 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX8 (IO_CKGEN_BASE + 0x62C)
    #define FLD_PMUX8 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX9 (IO_CKGEN_BASE + 0x630)
    #define FLD_PMUX9 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_PMUX10 (IO_CKGEN_BASE + 0x634)
    #define FLD_PMUX10 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOOUT0 (IO_CKGEN_BASE + 0x700)
    #define FLD_GPIO_OUT0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOOUT1 (IO_CKGEN_BASE + 0x704)
    #define FLD_GPIO_OUT1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOOUT2 (IO_CKGEN_BASE + 0x708)
    #define FLD_GPIO_OUT2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOOUT3 (IO_CKGEN_BASE + 0x70C)
    #define FLD_GPIO_OUT3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOOUT4 (IO_CKGEN_BASE + 0x710)
    #define FLD_GPIO_OUT4 Fld(7,0,AC_MSKB0)//[6:0]
#define CKGEN_GPIOEN0 (IO_CKGEN_BASE + 0x720)
    #define FLD_GPIO_EN0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOEN1 (IO_CKGEN_BASE + 0x724)
    #define FLD_GPIO_EN1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOEN2 (IO_CKGEN_BASE + 0x728)
    #define FLD_GPIO_EN2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOEN3 (IO_CKGEN_BASE + 0x72C)
    #define FLD_GPIO_EN3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOEN4 (IO_CKGEN_BASE + 0x730)
    #define FLD_GPIO_EN4 Fld(7,0,AC_MSKB0)//[6:0]
#define CKGEN_GPIOIN0 (IO_CKGEN_BASE + 0x740)
    #define FLD_GPIO_IN0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOIN1 (IO_CKGEN_BASE + 0x744)
    #define FLD_GPIO_IN1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOIN2 (IO_CKGEN_BASE + 0x748)
    #define FLD_GPIO_IN2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOIN3 (IO_CKGEN_BASE + 0x74C)
    #define FLD_GPIO_IN3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_GPIOIN4 (IO_CKGEN_BASE + 0x750)
    #define FLD_GPIO_IN4 Fld(7,0,AC_MSKB0)//[6:0]
#define CKGEN_ED2INTEN0 (IO_CKGEN_BASE + 0x760)
    #define FLD_ED2INTEN0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ED2INTEN1 (IO_CKGEN_BASE + 0x764)
    #define FLD_ED2INTEN1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ED2INTEN2 (IO_CKGEN_BASE + 0x768)
    #define FLD_ED2INTEN2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ED2INTEN3 (IO_CKGEN_BASE + 0x76C)
    #define FLD_ED2INTEN3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ED2INTEN4 (IO_CKGEN_BASE + 0x770)
    #define FLD_ED2INTEN4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_LEVINTEN0 (IO_CKGEN_BASE + 0x774)
    #define FLD_LEVINTEN0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_LEVINTEN1 (IO_CKGEN_BASE + 0x778)
    #define FLD_LEVINTEN1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_LEVINTEN2 (IO_CKGEN_BASE + 0x77C)
    #define FLD_LEVINTEN2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_LEVINTEN3 (IO_CKGEN_BASE + 0x780)
    #define FLD_LEVINTEN3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_LEVINTEN4 (IO_CKGEN_BASE + 0x784)
    #define FLD_LEVINTEN4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ENTPOL0 (IO_CKGEN_BASE + 0x788)
    #define FLD_INTPOL0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ENTPOL1 (IO_CKGEN_BASE + 0x78C)
    #define FLD_INTPOL1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ENTPOL2 (IO_CKGEN_BASE + 0x790)
    #define FLD_INTPOL2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ENTPOL3 (IO_CKGEN_BASE + 0x794)
    #define FLD_INTPOL3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_ENTPOL4 (IO_CKGEN_BASE + 0x798)
    #define FLD_INTPOL4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINTEN0 (IO_CKGEN_BASE + 0x79C)
    #define FLD_INTEN0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINTEN1 (IO_CKGEN_BASE + 0x7A0)
    #define FLD_INTEN1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINTEN2 (IO_CKGEN_BASE + 0x7A4)
    #define FLD_INTEN2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINTEN3 (IO_CKGEN_BASE + 0x7A8)
    #define FLD_INTEN3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINTEN4 (IO_CKGEN_BASE + 0x7AC)
    #define FLD_INTEN4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINT0 (IO_CKGEN_BASE + 0x7B0)
    #define FLD_EXTINT0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINT1 (IO_CKGEN_BASE + 0x7B4)
    #define FLD_EXTINT1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINT2 (IO_CKGEN_BASE + 0x7B8)
    #define FLD_EXTINT2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINT3 (IO_CKGEN_BASE + 0x7BC)
    #define FLD_EXTINT3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_EXTINT4 (IO_CKGEN_BASE + 0x7C0)
    #define FLD_EXTINT4 Fld(32,0,AC_FULLDW)//[31:0]

//Page CKGEN_5
#define CKGEN_MBIST_CTRL (IO_CKGEN_BASE + 0x800)
    #define FLD_MBIST_MON_SEL Fld(5,13,AC_MSKW21)//[17:13]
    #define FLD_PAD_DONE_CLEAR Fld(1,12,AC_MSKB1)//[12:12]
    #define FLD_PAD_FAIL_CLEAR Fld(1,11,AC_MSKB1)//[11:11]
    #define FLD_PAD_DONE_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define FLD_PAD_FAIL_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define FLD_L2C_MBIST_ON Fld(1,8,AC_MSKB1)//[8:8]
    #define FLD_L2C_MBIST_MODE Fld(1,7,AC_MSKB0)//[7:7]
    #define FLD_CA9_MBIST_ON Fld(1,6,AC_MSKB0)//[6:6]
    #define FLD_CA9_MBIST_MODE Fld(1,5,AC_MSKB0)//[5:5]
    #define FLD_MBIST_DEBUG_MODE Fld(1,4,AC_MSKB0)//[4:4]
    #define FLD_MBIST_HOLDB Fld(1,3,AC_MSKB0)//[3:3]
    #define FLD_MBIST_RSTB Fld(1,0,AC_MSKB0)//[0:0]
#define CKGEN_MBIST_BGSEL (IO_CKGEN_BASE + 0x804)
    #define FLD_MBIST_BACKGROUND Fld(16,0,AC_FULLW10)//[15:0]
#define CKGEN_MBIST_EN0 (IO_CKGEN_BASE + 0x810)
    #define FLD_MBIST_MODE0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN1 (IO_CKGEN_BASE + 0x814)
    #define FLD_MBIST_MODE1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN2 (IO_CKGEN_BASE + 0x818)
    #define FLD_MBIST_MODE2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN3 (IO_CKGEN_BASE + 0x81C)
    #define FLD_MBIST_MODE3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN4 (IO_CKGEN_BASE + 0x820)
    #define FLD_MBIST_MODE4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN5 (IO_CKGEN_BASE + 0x824)
    #define FLD_MBIST_MODE5 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_EN6 (IO_CKGEN_BASE + 0x828)
    #define FLD_MBIST_MODE6 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_DELSEL (IO_CKGEN_BASE + 0x8BC)
    #define FLD_SPM1R_DELSEL Fld(2,30,AC_MSKB3)//[31:30]
    #define FLD_SPM2R_DELSEL Fld(2,28,AC_MSKB3)//[29:28]
    #define FLD_SPM4R_DELSEL Fld(2,26,AC_MSKB3)//[27:26]
    #define FLD_SPM8R_DELSEL Fld(2,24,AC_MSKB3)//[25:24]
    #define FLD_SMP16R_DELSEL Fld(2,22,AC_MSKB2)//[23:22]
    #define FLD_SPM1_DELSEL Fld(2,20,AC_MSKB2)//[21:20]
    #define FLD_SPM2_DELSEL Fld(2,18,AC_MSKB2)//[19:18]
    #define FLD_SPM4_DELSEL Fld(2,16,AC_MSKB2)//[17:16]
    #define FLD_SPM8_DELSEL Fld(2,14,AC_MSKB1)//[15:14]
    #define FLD_SPM16_DELSEL Fld(2,12,AC_MSKB1)//[13:12]
    #define FLD_TPM1R_DELSEL Fld(2,10,AC_MSKB1)//[11:10]
    #define FLD_TPM2R_DELSEL Fld(2,8,AC_MSKB1)//[9:8]
    #define FLD_TPM1_DELSEL Fld(2,6,AC_MSKB0)//[7:6]
    #define FLD_TPM2_DELSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define FLD_DPM2R_DELSEL Fld(2,2,AC_MSKB0)//[3:2]
    #define FLD_DPM2_DELSEL Fld(2,0,AC_MSKB0)//[1:0]
#define CKGEN_MBIST_MON0 (IO_CKGEN_BASE + 0x8C0)
    #define FLD_MBIST_MON0 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON1 (IO_CKGEN_BASE + 0x8C4)
    #define FLD_MBIST_MON1 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON2 (IO_CKGEN_BASE + 0x8C8)
    #define FLD_MBIST_MON2 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON3 (IO_CKGEN_BASE + 0x8CC)
    #define FLD_MBIST_MON3 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON4 (IO_CKGEN_BASE + 0x8D0)
    #define FLD_MBIST_MON4 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON5 (IO_CKGEN_BASE + 0x8D4)
    #define FLD_MBIST_MON5 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON6 (IO_CKGEN_BASE + 0x8D8)
    #define FLD_MBIST_MON6 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON7 (IO_CKGEN_BASE + 0x8DC)
    #define FLD_MBIST_MON7 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON8 (IO_CKGEN_BASE + 0x8E0)
    #define FLD_MBIST_MON8 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON9 (IO_CKGEN_BASE + 0x8E4)
    #define FLD_MBIST_MON9 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON10 (IO_CKGEN_BASE + 0x8E8)
    #define FLD_MBIST_MON10 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON11 (IO_CKGEN_BASE + 0x8EC)
    #define FLD_MBIST_MON11 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON12 (IO_CKGEN_BASE + 0x8F0)
    #define FLD_MBIST_MON12 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON13 (IO_CKGEN_BASE + 0x8F4)
    #define FLD_MBIST_MON13 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON14 (IO_CKGEN_BASE + 0x8F8)
    #define FLD_MBIST_MON14 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON15 (IO_CKGEN_BASE + 0x8FC)
    #define FLD_MBIST_MON15 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON16 (IO_CKGEN_BASE + 0x900)
    #define FLD_MBIST_MON16 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON17 (IO_CKGEN_BASE + 0x904)
    #define FLD_MBIST_MON17 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON18 (IO_CKGEN_BASE + 0x908)
    #define FLD_MBIST_MON18 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON19 (IO_CKGEN_BASE + 0x90C)
    #define FLD_MBIST_MON19 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON20 (IO_CKGEN_BASE + 0x910)
    #define FLD_MBIST_MON20 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON21 (IO_CKGEN_BASE + 0x914)
    #define FLD_MBIST_MON21 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON22 (IO_CKGEN_BASE + 0x918)
    #define FLD_MBIST_MON22 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON23 (IO_CKGEN_BASE + 0x91C)
    #define FLD_MBIST_MON23 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON24 (IO_CKGEN_BASE + 0x920)
    #define FLD_MBIST_MON24 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON25 (IO_CKGEN_BASE + 0x924)
    #define FLD_MBIST_MON25 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON26 (IO_CKGEN_BASE + 0x928)
    #define FLD_MBIST_MON26 Fld(32,0,AC_FULLDW)//[31:0]
#define CKGEN_MBIST_MON27 (IO_CKGEN_BASE + 0x92C)
    #define FLD_MBIST_MON27 Fld(32,0,AC_FULLDW)//[31:0]

#endif /* _HW_CKGEN_H_ */
