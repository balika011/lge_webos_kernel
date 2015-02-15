/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_resume.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_resume.c
 *  This file implements for full driver resume.
 */
#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/opm_device.h>
#include <asm/cacheflush.h>
#include <linux/version.h>
//#include <linux/highmem.h> // redefine VECTOR_PDWNC
extern void *kmap(struct page *page);
extern void kunmap(struct page *page);

#include <asm/memory.h>
//#include <asm/smp_scu.h>
//#include <linux/delay.h>

//#define DEBUG_VIA_WATCHPOINT
#ifdef DEBUG_VIA_WATCHPOINT
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#endif

#if defined(CC_SUPPORT_STR)

#include "x_linux.h"
#include "x_bim.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"

#include "x_os.h"
#include "x_util.h"

//panel
#include "nptv_if.h"    
#include "drv_vdoclk.h" 
#include "drv_display.h"
#include "drv_video.h"  
#include "pdwnc_drvif.h"

#include "x_mid.h"
#include "x_suspend.h"
#include "x_assert.h"
#include "nandhw_if.h"

#include "x_hal_io.h"

#include "x_dram.h"
#include "fbm_drvif.h"

//#if CC_TRUSTZONE_SUPPORT
//#include "tz_if.h"
//#endif
#if defined(CC_SUPPORT_STR_CORE_OFF)
#include "irtx_if.h"
#endif
#include "eeprom_if.h"

extern void vDrvODSuspend(void);
extern void vDrvODResume(void);
extern void vDrvMIBSuspend(void);
extern void vDrvMIBResume(void);
extern void vDrvScposSuspend(void);
extern void vDrvScposResume(void);
extern void vDrvVideoSuspend(void);
extern void vDrvVideoResume(void);
#ifdef CC_MT5399
extern void vDrvMJCSuspend(void);
extern void vDrvMJCResume(void);
extern void vDrvTddcResume(void);
#endif
extern void AUD_pm_str_suspend(void);
extern void AUD_pm_str_resume(void);
#ifdef CC_SUPPORT_VENC
extern void VENC_pm_str_suspend(void);
extern void VENC_pm_str_resume(void);
#endif
extern void PMX_pm_suspend(void);
extern void PMX_pm_resume(void);
extern void OSD_pm_suspend(void);
extern void OSD_pm_resume(void);
extern void GCPU_pm_str_suspend(void);
extern void GCPU_pm_str_resume(void);
extern void GFX_pm_suspend(void);
extern void GFX_pm_resume(void);
extern void DMX_pm_str_suspend(void);
extern void DMX_pm_str_resume(void);
extern void CHIP_CPU_Resume(void);
extern void CHIP_CPU_Suspend(void);
extern void VDEC_pm_str_suspend(void);
extern void VDEC_pm_str_resume(void);
extern void Image_pm_str_suspend(void);
extern void Image_pm_str_resume(void);
extern void Imgrz_pm_str_suspend(void);
extern void Imgrz_pm_str_resume(void);
extern void B2R_pm_str_suspend(void);
extern void B2R_pm_str_resume(void);
extern void SIF_pm_str_suspend(void);
extern void SIF_pm_str_resume(void);
extern void GPIO_pm_str_suspend(void);
extern void GPIO_pm_str_resume(void);
extern void NOR_pm_str_suspend(void);
extern void NOR_pm_str_resume(void);
extern void MSDC_pm_str_suspend(void);
extern void MSDC_pm_str_resume(void);
extern void PWM_pm_str_suspend(void);
extern void PWM_pm_str_resume(void);
extern void vDrvPostprocSuspend(void);
#ifndef CC_MT5881
extern void vDrvLcDimPQSuspend(void);
extern void vDrvLcdimSpiSuspend(void);
extern void vDrvLcDimPQResume(void);
extern void vDrvLcdimSpiResume(void);
#endif
#ifndef CC_MT5882
extern void vDrvThermal_pm_suspend(void);
extern void vDrvThermal_pm_resume(void);
#endif
extern void vDrvPostprocResume(void);
extern void TVD_pm_resume(void);
extern void TVD_pm_suspend(void);
extern void VBI_pm_resume(void);
extern void VBI_pm_suspend(void);
extern void TVE_pm_resume(void);
extern void TVE_pm_suspend(void);
EXTERN void    PCMCIA_pm_str_suspend(void);
EXTERN void    PCMCIA_pm_str_resume(void);
extern void HDMI_pm_suspend(void);
extern void HDMI_pm_resume(void);
#ifdef CC_HDMI_2_0_SUPPORT
extern void HDMI2_pm_suspend(void);
extern void HDMI2_pm_resume(void);
#endif
extern void MHL_pm_suspend(void);
extern void MHL_pm_resume(void);
extern void CEC_pm_resume(void);
extern void CEC_pm_suspend(void);
extern void vDrvNptvResume(void);
extern void ADC_pm_resume(void);
#ifndef CC_APP_IF_MTAL
extern void Tuner_pm_str_suspend(void);
extern void Tuner_pm_str_resume(void);
#else
extern void Mtal_tuner_pm_str_suspend(void);
extern void Mtal_tuner_pm_str_resume(void);
#endif
extern void BIM_pm_suspend(void);
extern void BIM_pm_resume(void);

//LVDS
extern void vDrvLVDSSuspend(void);
extern void vDrvLVDSResume(void);
extern void vDrvVOPLLSuspend(void);
extern void vDrvVOPLLResume(void);

extern BOOL MID_EnableRegionReadProtect(UINT32 u4Region, BOOL onoff);

extern void vDrvDisplayInit_pm_resume(void);

#if defined(CC_SUPPORT_STR_CORE_OFF)
extern UINT32 PDWNC_EnterSuspend(void);
extern void IRTX_pm_suspend(void);
extern void IRTX_pm_resume(void);
extern void WDT_pm_suspend(void);
extern void WDT_pm_resume(void);
extern void IRRX_pm_resume(void);
extern void DMARB_mid_suspend(void);
extern void DMARB_mid_resume(void);

#else
extern void PDWNC_EnterSuspend(void);
#endif
extern void PDWNC_Resume(void);
#if defined(CC_SUPPORT_STR_CORE_OFF)
extern void PDWNC_SetSuspendMode(UINT32 SuspendMode);
extern void Board_pm_suspend(void);
extern void Board_pm_resume(void);
#else
extern void PDWNC_SetSuspendMode(void);
extern void PDWNC_SetResumeMode(void);
#endif
extern INT32 _i4EtherSuspend(void);
extern INT32 _i4EtherResume(void);
//EXTERN INT32 MUSB_Suspend(void);
//EXTERN INT32 MUSB_Resume(void);

//extern void UART_pm_resume(void);

extern void vDrvTTDSuspend(void);
extern void vDrvTTDResume(void);

#ifdef CC_FBM_SUPPORT_BEIC
extern UINT32 BEBoot_pm_suspend(void);
extern UINT32 BEBoot_pm_resume(void);
#endif

//#define BYPASS_MTK_DRV_PM
#ifdef BYPASS_MTK_DRV_PM
#ifndef CC_MT5881
#define vDrvLcDimPQSuspend()
#define vDrvLcdimSpiSuspend()
#define vDrvLcDimPQResume()
#define vDrvLcdimSpiResume()
#endif

#define vDrvMIBSuspend()
#define vDrvMIBResume()
#define vDrvPostprocSuspend()
#define vDrvPostprocResume()
#define vDrvVideoSuspend()
#define vDrvVideoResume()
#define AUD_pm_str_suspend()
#define AUD_pm_str_resume()
#define vDrvTTDSuspend()
#define vDrvTTDResume()

#define DMARC_TCM_suspend()
#define DMARC_TCM_resume()
#define B2R_pm_str_suspend()
#define B2R_pm_str_resume()
#define DMX_pm_str_suspend()
#define DMX_pm_str_resume()
#define VDEC_pm_str_suspend()
#define VDEC_pm_str_resume()
#define Image_pm_str_suspend()
#define Image_pm_str_resume()
#define Imgrz_pm_str_suspend()
#define Imgrz_pm_str_resume()

#define vDrvLVDSSuspend()
#define vDrvLVDSResume()
#define vDrvVOPLLSuspend()
#define vDrvVOPLLResume()
#ifndef CC_APP_IF_MTAL
#define Tuner_pm_str_suspend()
#define Tuner_pm_str_resume()
#else
#define Mtal_tuner_pm_str_suspend()
#define Mtal_tuner_pm_str_resume()
#endif
#define vDrvODSuspend()
#define vDrvODResume()
#define TVD_pm_suspend()
#define TVD_pm_resume()
#define VBI_pm_suspend()
#define VBI_pm_resume()
#define TVE_pm_suspend()
#define TVE_pm_resume()

#define SIF_pm_str_suspend()
#define SIF_pm_str_resume()
#define NOR_pm_str_suspend()
#define NOR_pm_str_resume()
#define PWM_pm_str_suspend()
#define PWM_pm_str_resume()
#define IRTX_pm_suspend()
#define IRTX_pm_resume()

#define WDT_pm_suspend()
#define WDT_pm_resume()
#define BIM_pm_suspend()
#define BIM_pm_resume()
#define GPIO_pm_str_suspend()
#define GPIO_pm_str_resume()

#define OSD_pm_suspend()
#define OSD_pm_resume()
#define GFX_pm_suspend()
#define GFX_pm_resume()

#define GCPU_pm_str_suspend()
#define GCPU_pm_str_resume()
#define PCMCIA_pm_str_suspend()
#define PCMCIA_pm_str_resume()
/*pass*/
#define MHL_pm_suspend()
#define MHL_pm_resume()
#define HDMI_pm_suspend()
#define HDMI_pm_resume()
#ifdef CC_HDMI_2_0_SUPPORT
#define HDMI2_pm_suspend()
#define HDMI2_pm_resume()
#endif

#define CEC_pm_suspend()
#define CEC_pm_resume()

#ifdef CC_NAND_BOOT
#define NFI_pm_suspend()
#define NFI_pm_resume()
#endif 


#define vDrvScposSuspend()
#define vDrvScposResume()
#define DMARB_mid_suspend()
#define DMARB_mid_resume()
#endif
extern INT32 OSD_ShowLogo(void);
extern BOOL MPV_ShowLogo(VOID);

#define DO_DRAM_CHECKSUM
#if defined(CC_MT5399)
#define DO_DRAMB_CHECKSUM
#endif
//#define BYPASS_CORE_OFF
#ifdef BYPASS_CORE_OFF
#define BIM_VIRT                    (IO_VIRT + 0x08000)
#define REG_RW_IRQEN        0x0034      // RISC IRQ Enable Register
static inline void __bim_writel(u32 regval32, u32 regaddr32)
{
    __raw_writel(regval32, BIM_VIRT + regaddr32);
}
static inline u32 __bim_readl(u32 regaddr32)
{   
    return __raw_readl(BIM_VIRT + regaddr32);
}
#endif
#ifdef NDEBUG
#define DO_DRAM_CHECKSUM_PARTIAL
#define DISABLE_OUTBYTE
#define NO_DEBUG_FUNCTION_PRINT
#endif

#ifdef DO_DRAM_CHECKSUM_PARTIAL
#define DRAM_CHECKSUM_BLOCK_SIZE    (256*1024)
#define DRAM_CHECKSUM_SKIP          (7)
#else
#define DRAM_CHECKSUM_BLOCK_SIZE    (1024*1024)
#define DRAM_CHECKSUM_SKIP          (0)
#endif

#if defined(CC_MT5890)||defined(CC_MT5882)||defined(CC_MT5883)
#define DRAM_POST_WRITE_FLUSH_CHA    0xf000606c
#define DRAM_POST_WRITE_FLUSH_CHB 	 0xf001006c
#define DRAM_POST_WRITE_FLUSH_CHC 	 0xf001106c
#endif

#if defined(CC_MT5890) || defined(CC_MT5882)
extern void GDMA_pm_str_suspend(void);
extern void GDMA_pm_str_resume(void);
#endif

//-----------------------------------------------------------------------------
/**  Save cpu state (and the return point for resume)
 * 
 *  @param pBuffer: Address to save cpu state. 
 *                  The buffer will be pass to resume function as param.
 *  @return  1 for save stats, 0 for resume from suspend
 */
//-----------------------------------------------------------------------------
extern int cpu_SaveSuspendReturn(UINT32 *pBuffer);


extern int DRAM_Checksum(UINT32 *puDramChecksum, UINT32 *puSramTemp);
extern void free_builtin_timestamp(void);

//#define ENABLE_JTAG_TO_DEBUG
#ifdef ENABLE_JTAG_TO_DEBUG
INT32 CLI_Parser(const CHAR *szCmdBuf);
#endif

#ifdef CC_SUPPORT_STR_DEBUG
#define RS232_READ32(off)       IO_READ32(RS232_BASE, (off))
#define RS232_WRITE32(off, val) IO_WRITE32(RS232_BASE, (off), (val))

#define REG_U0_OUTPORT          (0x0000)
#define REG_CMD_STATUS          (0x0004)
    #define CMD_MODE            (0x00)
#define REG_RS232_MODE          (0x0018)
    #define RISC_ACTIVE         (0x02)
#define REG_BUFFER_SIZE         (0x001c)
    #define SINGLE_BYTE         (0x00)
#define REG_U0_STATUS           (0x004c)
    #define STATUS_TXBUF        (0x1f00)
#define REG_U0_BUFCTL           (0x0050)
    #define DEF_BUFCTL          (0xff01)

void CHIP_DisplayChar(UINT32 u4Char)
{
//#ifndef NOLOG
    while ((RS232_READ32(REG_U0_STATUS) & STATUS_TXBUF) != STATUS_TXBUF) { }
    RS232_WRITE32(REG_U0_OUTPORT, (UINT32)(u4Char & 0xff));
    if (u4Char == ((UINT32)'\n'))
    {
        RS232_WRITE32(REG_U0_OUTPORT, (UINT32)'\r');
    }
//#endif
}

void CHIP_DisplayString(const CHAR *szString)
{
//#ifndef NOLOG
    if(szString == NULL)
    {
        return;
    }

    while (*szString != 0)
    {
        // busy wait if no tx buffer slot.
        CHIP_DisplayChar(((UINT32)((UINT8)(*szString))));
        szString++;
    }
//#endif
}

void CHIP_DisplayDigit(UINT32 u4Char)
{
    if (u4Char <= 9) { u4Char += '0'; }
    if ((u4Char >= 0x0a) && (u4Char <= 0x0f)) { u4Char += ('a' - 0x0a); }
    CHIP_DisplayChar(u4Char);
}

void CHIP_DisplayHex(UINT32 u4Val)
{
    CHIP_DisplayChar('0');
    CHIP_DisplayChar('x');
    CHIP_DisplayDigit(((u4Val >> 28) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 24) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 20) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 16) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 12) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 8) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 4) & 0x0f));
    CHIP_DisplayDigit(((u4Val >> 0) & 0x0f));            
}
#else
#define CHIP_DisplayString(fmt...)
#define CHIP_DisplayHex(fmt...)
#endif

//#define TEST_AGENT2_TOUCH
#ifdef TEST_AGENT2_TOUCH
#define DRAM_CHB_VIRT		    (IO_VIRT + 0x0F000)
static inline void __dram_chb_writel(u32 regval32, u32 regaddr32)
{
    __raw_writel(regval32, DRAM_CHB_VIRT + regaddr32);
}
static inline u32 __dram_chb_readl(u32 regaddr32)
{   
    return __raw_readl(DRAM_CHB_VIRT + regaddr32);
}
static inline void __dram_chb_writel_mask(u32 regval32, u32 regaddr32, u32 mask32)
{
    __dram_chb_writel((regval32 & mask32) | (__dram_chb_readl(regaddr32) & ~mask32), regaddr32);
}
void vDramCHBTestAgent2(UINT32 start, UINT32 size)
{
    //UINT32 start = 0x40000000;
    //UINT32 size = 0x20000000;

    __dram_chb_writel_mask(0x00000000, 0x08, 0x80000000);
    __dram_chb_writel_mask(start >> 4, 0x3c, 0x0FFFFFFF);
    __dram_chb_writel_mask(size >> 4, 0x40, 0x0FFFFFFF);
    __dram_chb_writel_mask(0x80000000, 0x08, 0x80000000);
    while (__dram_chb_readl(0x3FC) & (0x1<<10) == 0);
    if (__dram_chb_readl(0x370) != 0)
    {
	CHIP_DisplayString("Test Agent2 test fail, ret ");
        CHIP_DisplayHex(__dram_chb_readl(0x370));
	CHIP_DisplayString("\n");
    }
    __dram_chb_writel_mask(0x00000000, 0x08, 0x80000000);
}
#endif
//#define TEST_AGENT0_TOUCH
#ifdef TEST_AGENT0_TOUCH
#define DMARB_CHB_VIRT		    (IO_VIRT + 0x10000)
static inline void __dmarb_chb_writel(u32 regval32, u32 regaddr32)
{
    __raw_writel(regval32, DMARB_CHB_VIRT + regaddr32);
}
static inline u32 __dmarb_chb_readl(u32 regaddr32)
{   
    return __raw_readl(DMARB_CHB_VIRT + regaddr32);
}
static inline void __dmarb_chb_writel_mask(u32 regval32, u32 regaddr32, u32 mask32)
{
    __dmarb_chb_writel((regval32 & mask32) | (__dmarb_chb_readl(regaddr32) & ~mask32), regaddr32);
}
void vDramCHBTestAgent0(UINT32 start, UINT32 size)
{
    __dmarb_chb_writel_mask(0, 0x118, (0x1<<28));
    __dmarb_chb_writel_mask(start, 0x100, 0x7FFFFFF0);
    __dmarb_chb_writel_mask(size, 0x104, 0x7FFFFFF0);
    __dmarb_chb_writel_mask((0x1<<28), 0x118, (0x1<<28));
    while ((__dmarb_chb_readl(0x140) & 0x1) == 0x0);
    if (__dmarb_chb_readl(0x140) & (0x1<<4))
    {
	CHIP_DisplayString("Test Agent0 test fail\n");
    }
    else if (__dmarb_chb_readl(0x140) & (0x1<<8))
    {
	CHIP_DisplayString("Test Agent0 test pass\n");
    }

    __dmarb_chb_writel_mask(0, 0x118, (0x1<<28));
}
#endif

UINT32 CHIP_T8032Cmd(PDWNC_T8032_CMD_T *rCmd)
{
	UINT32 u4Val = 0;        
    UINT32 i = 0, j = 0;
    UINT32 u1Ack = 0;
	
	PDWNC_T8032CMD_SET_CMD((rCmd->u1Cmd) | ((rCmd->u1SubCmd) << 8));
	while(1)
    {          
        vIO32WriteFldAlign(PDWNC_ARM_INTEN, 0x1, FLD_AUX_INTEN);
        vIO32WriteFldAlign(PDWNC_ARM_INT, 0x1, FLD_ARM_INT);

        while(i < 100000)
        {
            //CHIP_Delay_us(100);                  
            if(IO32ReadFldAlign(PDWNC_INTSTA, FLD_AUX_INT_STA))
            {
                u4Val = PDWNC_T8032CMD_GET_RESPONSE();
                if((u4Val & 0xff) != (rCmd->u1Cmd))
                {
                        continue;
                }
                u1Ack = 1;
                break;                
            }      
            i++;
        }                             
        if(i < 10000)
        {
            CHIP_DisplayString("drvset_STR_coreoff T8032 ack !\n");
            vIO32WriteFldAlign(PDWNC_INTCLR, 0x1, FLD_AUX_INTCLR);
            u1Ack = 1;
            break;
        }            
        else
        {
            vIO32WriteFldAlign(PDWNC_INTCLR, 0x1, FLD_AUX_INTCLR);
            i = 0;
        }
        if(++j > 2 /*T8032_ACK_TIMEOUT*/)
        {
            CHIP_DisplayString("drvset_STR_coreoff T8032 NO ack!\n");
            // Ack timeout
            u1Ack = 0;
              break;
        }
    }
	return u1Ack;
}

#ifdef NO_DEBUG_FUNCTION_PRINT
//#define DebugFunctionPrint(a)    x_os_drv_set_timestamp(a)
#define DebugFunctionPrint(a)
#else
#define DebugFunctionPrint(a)    Printf(a"\n")
#endif

#ifdef DISABLE_OUTBYTE
#define MaybeDisableOutbyte(a)     SetDisableOutbyte(a)
#else
#define MaybeDisableOutbyte(a)
#endif

#ifdef DEBUG_VIA_WATCHPOINT
struct perf_event * __percpu *wbp = NULL;

static void sample_hbp_handler(struct perf_event *bp,
                               struct perf_sample_data *data,
                               struct pt_regs *regs)
{
        dump_stack();
        printk(KERN_INFO "Dump stack from sample_hbp_handler\n");
}

#define WATCHPOINT_ADDRESS 0xC0013964
int vEnableWatchpoint(void)
{
    int ret;
    struct perf_event_attr attr;

    printf("vEnableWatchpoint: 0x%x\n", *((UINT32*)WATCHPOINT_ADDRESS));

    hw_breakpoint_init(&attr);
    attr.bp_addr = WATCHPOINT_ADDRESS;
    attr.bp_len = HW_BREAKPOINT_LEN_4;
    attr.bp_type = HW_BREAKPOINT_W;

    wbp = register_wide_hw_breakpoint(&attr, sample_hbp_handler, NULL);
    if (IS_ERR((void __force *)wbp)) {
        ret = PTR_ERR((void __force *)wbp);
        goto fail;
    }

    return 0;

fail:
    printk(KERN_INFO "Breakpoint registration failed\n");

    return ret;
}

int vDiableWatchpoint(void)
{
    if (wbp != NULL)
        unregister_wide_hw_breakpoint(wbp);
    return 0;
}
#endif


#define DEBUG_FUNCTION_CALL(x)\
    do{\
	DebugFunctionPrint(#x);\
	x;\
    } while (0)
static int _mt53xx_drv_suspend(void)
{
    Printf("_mt53xx_STR_suspend\n");
    #if defined(CC_SUPPORT_STR_CORE_OFF)
    PDWNC_SetSuspendMode(TRUE);
    #else
    PDWNC_SetSuspendMode();
    #endif
	#ifndef CC_MT5882
	DEBUG_FUNCTION_CALL(vDrvThermal_pm_suspend());
	#endif

#ifdef ENABLE_JTAG_TO_DEBUG
    CLI_Parser("b.jtag 1");
#endif
    DEBUG_FUNCTION_CALL(DMARC_TCM_suspend());
    DEBUG_FUNCTION_CALL(vDrvLVDSSuspend());
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xff, 0xfe)); // cyj.note: go with AP
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xfe, 0xff));
    DEBUG_FUNCTION_CALL(vDrvVOPLLSuspend());
	#ifndef CC_APP_IF_MTAL
    DEBUG_FUNCTION_CALL(Tuner_pm_str_suspend());
	#else
	Printf("[DEMOD]Mtal_tuner_pm_str_suspend start!\n");
	DEBUG_FUNCTION_CALL(Mtal_tuner_pm_str_suspend());
	Printf("[DEMOD]Mtal_tuner_pm_str_suspend end!\n");
	#endif
    DEBUG_FUNCTION_CALL(vDrvVideoSuspend());
	DEBUG_FUNCTION_CALL(PMX_pm_suspend());
    DEBUG_FUNCTION_CALL(AUD_pm_str_suspend());
    DEBUG_FUNCTION_CALL(OSD_pm_suspend());
    DEBUG_FUNCTION_CALL(GFX_pm_suspend());
    DEBUG_FUNCTION_CALL(GCPU_pm_str_suspend());
    DEBUG_FUNCTION_CALL(DMX_pm_str_suspend());
    DEBUG_FUNCTION_CALL(VDEC_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Image_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Imgrz_pm_str_suspend());
    DEBUG_FUNCTION_CALL(TVD_pm_suspend());
    DEBUG_FUNCTION_CALL(VBI_pm_suspend());
    DEBUG_FUNCTION_CALL(TVE_pm_suspend());
    DEBUG_FUNCTION_CALL(B2R_pm_str_suspend());
    DEBUG_FUNCTION_CALL(PCMCIA_pm_str_suspend());
    DEBUG_FUNCTION_CALL(MHL_pm_suspend());
    DEBUG_FUNCTION_CALL(HDMI_pm_suspend());
	#ifdef CC_HDMI_2_0_SUPPORT
	DEBUG_FUNCTION_CALL(HDMI2_pm_suspend());
	#endif
	DEBUG_FUNCTION_CALL(CEC_pm_suspend());

	#ifdef CC_NAND_BOOT
    DEBUG_FUNCTION_CALL(NFI_pm_suspend());
	#endif 
	
    DEBUG_FUNCTION_CALL(SIF_pm_str_suspend());
    //DEBUG_FUNCTION_CALL(NOR_pm_str_suspend()); // nor flash
    DEBUG_FUNCTION_CALL(PWM_pm_str_suspend());
    //DEBUG_FUNCTION_CALL(_i4EtherSuspend()); // native driver
    //DEBUG_FUNCTION_CALL(MUSB_Suspend()); // native driver
#if defined(CC_MT5890) || defined(CC_MT5882)
	DEBUG_FUNCTION_CALL(GDMA_pm_str_suspend());
#endif

#ifdef CC_SUPPORT_VENC
    DEBUG_FUNCTION_CALL(VENC_pm_str_suspend());
#endif
#if defined(CC_SUPPORT_STR_CORE_OFF)
    DEBUG_FUNCTION_CALL(IRTX_pm_suspend());
    DEBUG_FUNCTION_CALL(WDT_pm_suspend());
    DEBUG_FUNCTION_CALL(BIM_pm_suspend());
    DEBUG_FUNCTION_CALL(GPIO_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Board_pm_suspend());
    DEBUG_FUNCTION_CALL(DMARB_mid_suspend());
    #ifdef CC_FBM_SUPPORT_BEIC
	DEBUG_FUNCTION_CALL(BEBoot_pm_suspend());
	#endif

//#define CC_PM_UNIT_TEST
#ifndef CC_PM_UNIT_TEST
    UNUSED(PDWNC_EnterSuspend());
#endif
#else
    PDWNC_EnterSuspend();  
#endif
#if defined(CC_MT5399) || defined(CC_MT5890)||defined(CC_MT5882)
    x_os_drv_reset_timestamp();
    free_builtin_timestamp();
#endif

    return 0;
}

static HANDLE_T _hResumeSema=(HANDLE_T)(NULL);
static volatile int resumeThreadDone;
static void _ParallelResumeThread(void *pvArgs)
{
    DTVCFG_T rDtvCfg;
	
    DEBUG_FUNCTION_CALL(vDrvDisplayInit_pm_resume());
	x_sema_unlock(_hResumeSema);

	EEPDTV_GetCfg(&rDtvCfg);
    #ifdef CC_SUPPORT_ANDROID_L_RTC_WAKEUP
    if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) || (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
    #else
    if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) && (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
    #endif
    {
        Printf("_ParallelResumeThread: Quiet Boot\n");
    }
	else if (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_DISABLE_LOGO)
	{
	    Printf("_ParallelResumeThread: Disable Logo\n");
	}	
    #ifdef CC_QUIET_HOT_BOOT_SUPPORT
    else if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_QUIET_HOT_BOOT)
    {
        Printf("_ParallelResumeThread: Quiet Hot Boot\n");
    }
    #endif		
    else
    {       
    	DEBUG_FUNCTION_CALL(vDrvLVDSResume());
    }
	DEBUG_FUNCTION_CALL(ADC_pm_resume());// please put it after the one who call VOPLL_pm_resume();

    resumeThreadDone|=1<<0;
    DebugFunctionPrint("ParallelResume complete");
}

static void _ParallelResumeVideoThread(void *pvArgs)
{
	VERIFY (x_sema_lock(_hResumeSema, X_SEMA_OPTION_WAIT) == OSR_OK);	
    DEBUG_FUNCTION_CALL(vDrvVideoResume());
    resumeThreadDone|=1<<1;
}

static void _ParallelResumeAudThread(void *pvArgs)
{
    DEBUG_FUNCTION_CALL(AUD_pm_str_resume());
    resumeThreadDone|=1<<2;
}

static void _ParallelResumeTunerThread(void *pvArgs)
{
	#ifndef CC_APP_IF_MTAL
    DEBUG_FUNCTION_CALL(Tuner_pm_str_resume());
	#else
	Printf("[DEMOD]Mtal_tuner_pm_str_resume start\n");
	DEBUG_FUNCTION_CALL(Mtal_tuner_pm_str_resume());
	Printf("[DEMOD]Mtal_tuner_pm_str_resume end\n");
	#endif
    resumeThreadDone|=1<<3;
}

#ifdef CC_FBM_SUPPORT_BEIC
static void _ParallelResumeBEBootThread(void *pvArgs)
{
    while (!(resumeThreadDone&0x0003)) x_thread_delay(20);
    DEBUG_FUNCTION_CALL(BEBoot_pm_resume());
	resumeThreadDone|=1<<4;
}
#endif
void mt53xx_SuspendLeave(void)
{
    DEBUG_FUNCTION_CALL(Board_pm_resume()); // timer
    DEBUG_FUNCTION_CALL(BIM_pm_resume()); // interrupt mask
}

void vDoChBChecksum(UINT32 init);
static int _mt53xx_drv_resume(void)
{
    HANDLE_T hResumeThread;
	HANDLE_T hResumePostProThread;
	HANDLE_T hResumeAudThread;
	HANDLE_T hResumeTunerThread;
    #ifdef CC_FBM_SUPPORT_BEIC
	HANDLE_T hResumeBeBootThread;
    #endif

    Printf("_mt53xx_STR_resume\n");
    x_os_drv_set_timestamp("_mt53xx_STR_resume start");
#if defined(CC_SUPPORT_STR_CORE_OFF)
    PDWNC_SetSuspendMode(FALSE);

    //DEBUG_FUNCTION_CALL(Board_pm_resume()); // timer, core driver, move to mt53xx_SuspendLeave
    DEBUG_FUNCTION_CALL(GPIO_pm_str_resume());
    //DEBUG_FUNCTION_CALL(BIM_pm_resume()); // interrupt mask, core driver, move to mt53xx_SuspendLeave
    DEBUG_FUNCTION_CALL(DMARB_mid_resume());
#else
    PDWNC_SetResumeMode();
#endif
#ifndef CC_MT5882
	DEBUG_FUNCTION_CALL(vDrvThermal_pm_resume());
#endif
    DEBUG_FUNCTION_CALL(DMARC_TCM_resume());
    DEBUG_FUNCTION_CALL(SIF_pm_str_resume());
    DEBUG_FUNCTION_CALL(vDrvNptvResume());
    DEBUG_FUNCTION_CALL(vDrvVOPLLResume());
    resumeThreadDone = 0;
	VERIFY(x_sema_create(&_hResumeSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);	
    x_thread_create(&hResumeThread, "ResumeThread",
                    2048, 100,
                    _ParallelResumeThread, 0, NULL);
    x_thread_create(&hResumePostProThread, "ResumePostProThread",
                    2048, 100,
                    _ParallelResumeVideoThread, 0, NULL);

    x_thread_create(&hResumeAudThread, "ResumeAudThread",
                    2048, 100,
                    _ParallelResumeAudThread, 0, NULL);
	
    x_thread_create(&hResumeTunerThread, "ResumeTunerThread",
                    2048, 100,
                    _ParallelResumeTunerThread, 0, NULL);

	DEBUG_FUNCTION_CALL(PMX_pm_resume());
    DEBUG_FUNCTION_CALL(OSD_pm_resume());
    DEBUG_FUNCTION_CALL(GFX_pm_resume());
    DEBUG_FUNCTION_CALL(GCPU_pm_str_resume());
	#ifdef CC_FBM_SUPPORT_BEIC
	x_thread_create(&hResumeBeBootThread, "ResumeBeBootThread",
                    2048, 100,
                    _ParallelResumeBEBootThread, 0, NULL);	
	#endif

	#if defined(CC_MT5890) || defined(CC_MT5882)
		DEBUG_FUNCTION_CALL(GDMA_pm_str_resume());
	#endif
    DEBUG_FUNCTION_CALL(DMX_pm_str_resume());
    DEBUG_FUNCTION_CALL(VDEC_pm_str_resume());
    DEBUG_FUNCTION_CALL(Image_pm_str_resume());
    DEBUG_FUNCTION_CALL(Imgrz_pm_str_resume());
    DEBUG_FUNCTION_CALL(TVD_pm_resume());
    // DEBUG_FUNCTION_CALL(NOR_pm_str_resume()); // nor flash
    DEBUG_FUNCTION_CALL(VBI_pm_resume());
    DEBUG_FUNCTION_CALL(TVE_pm_resume());
    DEBUG_FUNCTION_CALL(B2R_pm_str_resume());
    DEBUG_FUNCTION_CALL(PCMCIA_pm_str_resume());
    DEBUG_FUNCTION_CALL(HDMI_pm_resume());
	#ifdef CC_HDMI_2_0_SUPPORT
	DEBUG_FUNCTION_CALL(HDMI2_pm_resume());
	#endif
    DEBUG_FUNCTION_CALL(MHL_pm_resume());
	DEBUG_FUNCTION_CALL(CEC_pm_resume());

	#ifdef CC_NAND_BOOT
    DEBUG_FUNCTION_CALL(NFI_pm_resume());
	#endif 
	
    DEBUG_FUNCTION_CALL(PWM_pm_str_resume());

    //DEBUG_FUNCTION_CALL(ADC_pm_resume());// move in _ParallelResumeThread
    //DEBUG_FUNCTION_CALL(MUSB_Resume()); // native driver

#ifdef CC_SUPPORT_VENC
    DEBUG_FUNCTION_CALL(VENC_pm_str_resume());
#endif 

    #if defined(CC_SUPPORT_STR_CORE_OFF)
    DEBUG_FUNCTION_CALL(IRTX_pm_resume());
    DEBUG_FUNCTION_CALL(WDT_pm_resume());
	DEBUG_FUNCTION_CALL(IRRX_pm_resume());	
    #endif
    //DEBUG_FUNCTION_CALL(_i4EtherResume()); //native driver
    
    //DEBUG_FUNCTION_CALL(UART_pm_resume()); //native driver
    
    PDWNC_Resume();
	#ifdef CC_FASTBOOT_DRIVER_LOGO
		#ifdef CC_BOOT_VDEC_LOGO
		MPV_ShowLogo();
		#else
		OSD_ShowLogo();
		#endif
	#endif
    // Wait until resume thread complete.
    //while (!resumeThreadDone) x_thread_delay(20);
#ifdef DO_DRAMB_CHECKSUM
    //while (resumeThreadDone!=0x7) x_thread_delay(20);
    //vDoChBChecksum(0);
#endif
    #ifdef CC_FBM_SUPPORT_BEIC
    while (!(resumeThreadDone&0x0010)) x_thread_delay(20);
    #endif
    x_os_drv_set_timestamp("_mt53xx_STR_resume done");

    return 0;
}

#ifdef DO_DRAM_CHECKSUM
static UINT32 CheckSum(void *pStart, UINT32 size)
{
    UINT32 result = 0;
    UINT32 a, b, c, d, i;
    UINT32 *puStart;

    if ((UINT32)pStart < 0xBF000000)
    {
	puStart = (UINT32*)((UINT32)pStart | 0xC0000000);
    }
    else
    {
	puStart = (UINT32*)((UINT32)pStart);
    }

    ASSERT((size&3) == 0);
    ASSERT(((UINT32)pStart&3) == 0);

    for (i=0; i<=size-16; i+=16)
    {
        a = puStart[0];
        b = puStart[1];
        c = puStart[2];
        d = puStart[3];
        puStart += 4;
        result += a;
        result += b;
        result += c;
        result += d;
    }

    for (; i<size; i+=4)
    {
        result += puStart[0];
        puStart++;
    }

    return result;
}
#endif /* DO_DRAM_CHECKSUM */

static inline unsigned long vaddr_to_phys(void const *x)
{
   unsigned long i;
    asm volatile (
    "mcr  p15, 0, %1, c7, c8, 0\n"
    "  mrc p15, 0, %0, c7, c4, 0\n"
    : "=r" (i) : "r" (x) : "cc");
    if (i & 1)
        return 0;
    return (i & PAGE_MASK) | ((unsigned long)(x) & ~PAGE_MASK);
}
////////////////////////////////////////////////////////////////////////////////
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
 static int __devinit _mt53xx_STR_probe(struct opm_device *pdev)
#else
static int _mt53xx_STR_probe(struct opm_device *pdev)
#endif
{
    printk("_mt53xx_STR_probe\n");
    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
static int __devexit _mt53xx_STR_remove(struct opm_device *pdev)
#else
static int _mt53xx_STR_remove(struct opm_device *pdev)
#endif
{
    printk("_mt53xx_STR_remove\n");
    return 0;
}

void mt53xx_register_core_suspend(void (*func)(void));
void mt53xx_SuspendEnter(void);
#ifdef BYPASS_CORE_OFF_SIMPLE
void mt53xx_SuspendEnter_fake_simple(void);
#endif
static int _mt53xx_STR_suspend(struct opm_device *pdev, pm_message_t state)
{
#ifndef BYPASS_CORE_OFF_SIMPLE
    mt53xx_register_core_suspend(mt53xx_SuspendEnter);
#else
    mt53xx_register_core_suspend(mt53xx_SuspendEnter_fake_simple);
#endif
    _mt53xx_drv_suspend();

    return 0;
}

static int _mt53xx_STR_resume(struct opm_device *pdev)
{
    _mt53xx_drv_resume();
    return 0;
}

#if defined(CC_MT5399) && defined(DO_DRAMB_CHECKSUM)
#define ENABLE_GDMA_EXP
#endif
#ifdef ENABLE_GDMA_EXP
#undef GDMA_BASE
#define GDMA_BASE 0xf0001000
#define REG_GDMA_CONFIG         0x0004
  #define FILL_MODE_MSK   (1<<12)
  #define FILL_MODE_FILL  (1<<12)
  #define FILL_MODE_DMA  (0<<12)
  #define WPROT_EN       (1<<27)
  #define SOFT_RESET   (1<<31)
  #define INTR_MSK_FLG   (1<<0)

#define REG_GDMA_STATUS         0x0008
  #define GDMA_ON   (1<<31)
#define REG_GDMA_FIRE               0x000c
#define REG_GDMA_DATA_LEN           0x0010
#define REG_GDMA_SRC_ADDR           0x0014
#define REG_GDMA_DST_ADDR     0x0018
#define REG_GDMA_FILLVALUE    0x002c

#define _GDMA_READ32(OFFSET)			IO_READ32(GDMA_BASE,OFFSET)
#define _GDMA_WRITE32(OFFSET,VALUE)		IO_WRITE32(GDMA_BASE,OFFSET,VALUE)
#define _GDMA_WRITE32MSK(OFFSET,VALUE,MASK)     IO_WRITE32MSK(GDMA_BASE,OFFSET,VALUE,MASK)
void GDMA_Init(void)
{
    _GDMA_WRITE32MSK(REG_GDMA_CONFIG, ((UINT32)0x3)<<30 , 0xC0000000 ); //soft reset
    _GDMA_WRITE32MSK(REG_GDMA_CONFIG, (0x0)<<30 , 0xC0000000 ); //un-reset
    _GDMA_WRITE32MSK( REG_GDMA_CONFIG, 0x00000080, 0x00000080 );
    _GDMA_WRITE32MSK(REG_GDMA_CONFIG, 0, WPROT_EN);
    _GDMA_WRITE32MSK(REG_GDMA_CONFIG, FILL_MODE_DMA , FILL_MODE_MSK );  //dma memory copy
}
void GDMA_MemCpy(void* dst, const void* src, UINT32 len)
{
    if (len > 0xffffff) // size limitation
    {
	return;
    }

    //static UINT32 gdma_inited = 0;
    //if (gdma_inited == 0)
    {
        //gdma_inited = 1;
        GDMA_Init();
    }

    // flushinvalid
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)dst), len); // TODO: it should be VIRTUAL(src)
    HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)dst), len);
    _GDMA_WRITE32MSK( REG_GDMA_SRC_ADDR, (UINT32)src , 0xFFFFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_DST_ADDR, (UINT32)dst , 0xFFFFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_DATA_LEN, len, 0x00FFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_FIRE, 0x1, 0x1);

    while ((_GDMA_READ32(0x94) & 0x10000) != 0)
    {
        //Printf("GDMA busy\n");
        //HAL_Delay_us(1000);                  
    }
    while (_GDMA_READ32(REG_GDMA_STATUS) != 0);
    //HAL_Delay_us(1000);
    HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)dst), len);

}

#define DO_DRAMB_BACKUP
#ifdef DO_DRAMB_BACKUP
void GDMA_PHY_MemCpy(void* dst, const void* src, UINT32 len)
{
    if (len > 0xffffff) // size limitation
    {
	return;
    }

    //static UINT32 gdma_inited = 0;
    //if (gdma_inited == 0)
    {
        //gdma_inited = 1;
        GDMA_Init();
    }

    _GDMA_WRITE32MSK( REG_GDMA_SRC_ADDR, (UINT32)src , 0xFFFFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_DST_ADDR, (UINT32)dst , 0xFFFFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_DATA_LEN, len, 0x00FFFFFF );
    _GDMA_WRITE32MSK( REG_GDMA_FIRE, 0x1, 0x1);

    while ((_GDMA_READ32(0x94) & 0x10000) != 0)
    {
        //Printf("GDMA busy\n");
        //HAL_Delay_us(1000);                  
    }
    while (_GDMA_READ32(REG_GDMA_STATUS) != 0);

}

void GDMA_PHY_MemCpy_Exp(void* dst, const void* src, UINT32 len)
{
    UINT32 i = 0, bs = 0x800000;
    while (len >= bs)
    {
	GDMA_PHY_MemCpy(dst + i*bs, src + i*bs, bs);
	i++;
	len -= bs;
    }
}
#endif
#endif

#ifdef DO_DRAMB_CHECKSUM
//#define DRAM_CHB_START 0x20000000
//#define DRAM_CHB_END   (0x20000000+0xa00000)
#define DRAM_CHB_START (TCMGET_CHANNELA_SIZE()*0x100000)
#define DRAM_CHB_END   ((TCMGET_CHANNELA_SIZE()+TCMGET_CHANNELB_SIZE())*0x100000 - 0x7800000)
//#define DRAM_CHB_START 0x20000000
//#define DRAM_CHB_END   0x38800000
//#define DRAM_CHB_START 0x00000000 // chA start
//#define DRAM_CHB_END   0x10760000 // chA end -> see /proc/iomem
//#define DRAM_CHB_START 0x40000000
//#define DRAM_CHB_END   0x78800000
void vDoChBChecksum(UINT32 init)
{
    void* pv_Phys;
    void* pvVirt;
    UINT32 z_Size=0x1000;
    struct page* page;
    UINT32 checksum = 0;
    static UINT32 *puChecksum = NULL;
    UINT32 idx, val;
    UINT32 uError = 0;
#ifdef ENABLE_GDMA_EXP
    static void *temp_page = NULL;
#endif
#ifdef ENABLE_GDMA_EXP_CMP
    UINT32 err = 0, same = 0;
#endif

    if (puChecksum == NULL)
    {
        UINT32 totalSize, cnum, checknum;
        totalSize = DRAM_CHB_END-DRAM_CHB_START;
        checknum = cnum = totalSize/z_Size;
        puChecksum = x_mem_alloc(checknum * sizeof(UINT32));
        if (!puChecksum)
            return;
    }

    CHIP_DisplayString("DRAM_CHB_START: ");
    CHIP_DisplayHex(DRAM_CHB_START);
    CHIP_DisplayString(" DRAM_CHB_END: ");
    CHIP_DisplayHex(DRAM_CHB_END);
    CHIP_DisplayString("\n");

#ifdef ENABLE_GDMA_EXP
    if (temp_page == NULL)
    {
        temp_page = (void *)get_zeroed_page(GFP_ATOMIC);
        if (!temp_page)
            return;
    }
#endif

    //CHIP_DisplayString("DRAM CHB CHECKSUM Start: ");
    for (pv_Phys = (void*)DRAM_CHB_START, idx=0; pv_Phys < (void*)DRAM_CHB_END;  pv_Phys += z_Size, idx++)
    {
#ifdef ENABLE_GDMA_EXP
        if (init == 2)
        {
            GDMA_MemCpy((void*)PHYSICAL((UINT32)temp_page), pv_Phys, z_Size);
        }
#endif
        page = phys_to_page((int)pv_Phys);
        pvVirt = kmap(page);
#ifdef ENABLE_GDMA_EXP_CMP
        if (memcmp(temp_page, pvVirt, z_Size) != 0)
        {
            err++;
        }
        else
        {
            same++;
        }
#endif
        //CHIP_DisplayHex(pvVirt); CHIP_DisplayString("\n");
#ifdef ENABLE_GDMA_EXP
        if (init == 2)
        {
            val = CheckSum(temp_page, z_Size);
        }
        else
#endif
        {
            val = CheckSum(pvVirt, z_Size);
        }
        if ((init == 1) || (init == 2))
        {
            puChecksum[idx] = val;
        }
        else if (init == 0)
        {
            if (puChecksum[idx] != val)
            {
                uError++;
                CHIP_DisplayString("DRAM CHB checksum error, offset ");
                CHIP_DisplayHex((UINT32)pv_Phys);
                CHIP_DisplayString(" original ");
                CHIP_DisplayHex(puChecksum[idx]);
                CHIP_DisplayString(" current ");
                CHIP_DisplayHex(val);
                CHIP_DisplayString("\n");
            }
        }
        checksum += val;
        kunmap(page);
    }

    if (uError)
    {
        UINT32 i;
        CHIP_DisplayString("DRAM CHB checksum error ");
        CHIP_DisplayHex(uError);
        CHIP_DisplayString(", reboot!\n");
        for (i=0; i<15; i++)
            CHIP_DisplayString("!!!!!!!!!!!!!!!!\n");
        PDWNC_Reboot();
        while (1);
    }
    CHIP_DisplayString("DRAM CHB CHECKSUM: ");
    CHIP_DisplayHex(checksum);
    CHIP_DisplayString("\n");
#ifdef ENABLE_GDMA_EXP_CMP
    CHIP_DisplayHex(err);
    CHIP_DisplayString("\n");
    CHIP_DisplayHex(same);
    CHIP_DisplayString("\n");
#endif
}
#endif

#ifdef CHB_ACCESS_TEST
#define SET_CMP_TWO_MEM(x, y) \
    do {\
        idx = 0;\
        val[0] = (x);\
        val[1] = (y);\
        memset(pvVirt[idx++], val[0], z_Size);\
        memset(pvVirt[idx++], val[1], z_Size);\
        memset(pvVirt[idx++], val[0], z_Size);\
        memset(pvVirt[idx++], val[1], z_Size);\
        if ((memcmp(pvVirt[0], pvVirt[2], z_Size) != 0)\
            || (memcmp(pvVirt[1], pvVirt[3], z_Size) != 0))\
        {\
            CHIP_DisplayString("Access Test fail in pattern ");\
            CHIP_DisplayHex(val[0]);\
            CHIP_DisplayString(":");\
            CHIP_DisplayHex(val[1]);\
            CHIP_DisplayString("\n");\
            while (1);\
        }\
    } while (0);\

void vDoChBAccessTest(UINT32 count)
{
    void* pvVirt[4];
    void* pv_Phys;
    UINT32 z_Size;
    UINT32 idx = 0, loop = 0;
    UINT32 val[2];

    pv_Phys = 0x38800000; z_Size = 0x1000;
    pvVirt[idx++] = ioremap_nocache((int)pv_Phys, z_Size);
    pv_Phys += z_Size; z_Size = 0x1000;
    pvVirt[idx++] = ioremap_nocache((int)pv_Phys, z_Size);
    pv_Phys += z_Size; z_Size = 0x1000;
    pvVirt[idx++] = ioremap_nocache((int)pv_Phys, z_Size);
    pv_Phys += z_Size; z_Size = 0x1000;
    pvVirt[idx++] = ioremap_nocache((int)pv_Phys, z_Size);

    for (loop = 0; loop < count; loop++)
    {
        SET_CMP_TWO_MEM(0xffffffff, 0x00000000);
        SET_CMP_TWO_MEM(0x00000000, 0xffffffff);
        SET_CMP_TWO_MEM(0xAAAAAAAA, 0x55555555);
        SET_CMP_TWO_MEM(0x55555555, 0xAAAAAAAA);
        SET_CMP_TWO_MEM(0x5A5A5A5A, 0xA5A5A5A5);
        SET_CMP_TWO_MEM(0xA5A5A5A5, 0x5A5A5A5A);
    }

    idx = 0;
    iounmap(pvVirt[idx++]);
    iounmap(pvVirt[idx++]);
    iounmap(pvVirt[idx++]);
    iounmap(pvVirt[idx++]);

    CHIP_DisplayString("Access Test Done\n");\
}
#endif
void vFlushChABPostWriteFifo(UINT32 start, UINT32 size)
{
    void* pvVirt;
    UINT32* pvData;
    void* pv_Phys;
    UINT32 z_Size;
    UINT32 loop = 0;

    CHIP_DisplayString("vFlushChABPostWriteFifo: ");
    CHIP_DisplayHex(start);
    CHIP_DisplayString("\n");
    pv_Phys = (void*)start; z_Size = size;
    pvVirt = ioremap_nocache((int)pv_Phys, z_Size);
    if (pvVirt == NULL)
    {
        CHIP_DisplayString("fail\n");
        return;
    }
    pvData = (UINT32*) pvVirt;
    // write
    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
	pvData[loop] = 0xA5A5A5A5;
    }
    // read
    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
	pvData[loop+512] = pvData[loop];
    }

    // write
    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
	pvData[loop] = 0xA5A5A5A5;
    }
    iounmap(pvVirt);
}
void vFlushCoreL1(UINT32* pvData, UINT32 size)
{
    UINT32 loop;
    UINT32 split = (size/4)/2;

    CHIP_DisplayString("vFlushCoreL1: ");
    CHIP_DisplayHex((UINT32)pvData);
    CHIP_DisplayString("\n");
    // write
    for (loop=0; loop<split; loop++) // bus width(128bit)*6*2
    {
	pvData[loop] = 0xA5A5A5A5;
    }
    // read
    for (loop=0; loop<split; loop++) // bus width(128bit)*6*2
    {
	pvData[loop+split] = pvData[loop];
    }

    // write
    for (loop=0; loop<split; loop++) // bus width(128bit)*6*2
    {
	pvData[loop] = 0xA5A5A5A5;
    }
}

//UINT32 volatile _yjdbg_2 = 0;
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
extern void *mt53xx_saved_core1;
#endif
#ifndef CONFIG_SMP
extern void mt53xx_resume_up_check(void);
#endif
#ifdef CONFIG_SMP
extern void arch_disable_nonboot_cpus_early_resume(void);
#endif
#define DUMMY_DATA_LEN (32)
#define DUMMY_VALUE (0x12345678)
UINT32* _dummy_data_1;
//UINT32 _last_data_1=0;
//extern void __l2x0_flush_all(void);
//extern void __l2x0_disable(void);
extern int cpu_SaveSuspendReturn(UINT32 *pBuffer);
extern void mt53xx_dram_suspend(void);
extern int mt53xx_dram_suspend_sz;
UINT32 volatile avoid_compile_opt = 0;
void mt53xx_SuspendEnter(void)
{
    UINT32 *puCpuStats, state, size;
	#if defined(CC_MT5890)
    UINT32 uDRAMCTemp[PDWNC_SUSPEND_SAVE_SIZE];
	#else
    UINT32 uDRAMCTemp[SUSP_DRAMC_PER_CHANNEL*2+1];
	#endif
    FBM_POOL_T * prPool;
    u32 value=0;
#ifdef DO_DRAM_CHECKSUM //data
    void* pvVirt = NULL;
    volatile UINT32* pvData = NULL;
    void* pv_Phys;
    UINT32 z_Size;
    UINT32 loop = 0;
#endif
#ifdef DO_DRAM_CHECKSUM
    INT32 ret;
    UINT32 uNoChecksum1, uNoChecksum2, uError, result, cnum, totalSize, i, idx, checknum;
    UINT32 *puChecksum;
    UINT32 tmp=0;

    // Allocate space out of critical section, to prevent x_mem_alloc enable IRQ.
    ret = DRVCUST_OptQuery(eFbmMemAddr, &totalSize);
    if (ret != 0)
    {
        totalSize = 64*1024*1024;
        Printf("FBM address unknown\n");
    }
    else
        Printf("FBM address 0x%x\n", totalSize);
    totalSize = __pa(high_memory-1);
    cnum = totalSize/DRAM_CHECKSUM_BLOCK_SIZE;
    checknum = cnum/(DRAM_CHECKSUM_SKIP+1) + 2;
    puChecksum = x_mem_alloc(checknum * sizeof(UINT32));
    uNoChecksum1 = (UINT32)__pa(puChecksum) / DRAM_CHECKSUM_BLOCK_SIZE;
    uNoChecksum2 = (UINT32)__pa(uDRAMCTemp) / DRAM_CHECKSUM_BLOCK_SIZE;
    if (!puChecksum)
        return;
#endif /* DO_DRAM_CHECKSUM */
    UNUSED(value);

    // Allocate space out of critical section, to prevent x_mem_alloc enable IRQ.
    puCpuStats = x_mem_alloc(SUSP_CPU_STATS_SIZE);
    if (!puCpuStats)
    {
#ifdef DO_DRAM_CHECKSUM
        x_mem_free(puChecksum);
#endif
        return;
    }

    ////x_os_drv_set_timestamp("Start Suspend");

    ////// Calling all modules' suspend functions.
    ////_mt53xx_STR_suspend();

    ////// Make sure all logs are printed.
    ////UTIL_LogFlush();

    ////x_os_drv_set_timestamp("Suspend Complete");

    _dummy_data_1 = x_mem_alloc(DUMMY_DATA_LEN);
    if (!_dummy_data_1)
    {
#ifdef DO_DRAM_CHECKSUM
        x_mem_free(puChecksum);
#endif
	x_mem_free(puCpuStats);
	return;
    }
    _dummy_data_1[0] = DUMMY_VALUE;

    // disable IRQ
    state = HalCriticalStart();
    Printf("Prepare for suspend...\n");

#if defined(CC_MT5399) // FOZ is only needed for external L2
    // enable FOZ after L2 resume, disable first
    __asm__ __volatile__("mrc p15, 0, %0, c1, c0, 1" : "=r" (value));
    value &= ~(0x8); // FOZ=0x8
    __asm__ __volatile__("mcr p15, 0, %0, c1, c0, 1" : : "r" (value));
#endif

    // Saving current CPU's stats for resume.
    if (cpu_SaveSuspendReturn(puCpuStats))
    {
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
        u32 value;
#if 0
        //ACTLR.SMP=0
        __asm__ __volatile__("mrc p15, 0, %0, c1, c0, 1\n": "=r"(value)); 
        value &= ~0x40;
        __asm__ __volatile__("mcr p15, 0, %0, c1, c0, 1\n": :"r"(value));
        //scu_power_mode
#endif

#ifndef BYPASS_CORE_OFF
        // SCTRL.Z: program flow prediction
        //u32 value;
        __asm__ __volatile__("mrc p15, 0, %0, c1, c0, 0\n": "=r"(value)); 
        value &= ~0x800;
        __asm__ __volatile__("mcr p15, 0, %0, c1, c0, 0\n": :"r"(value));
#endif
#endif

        avoid_compile_opt = 1;
        // Get DRAM IO control registers from DRAMC, skip 4bytes for CPU stats.
        size = sizeof(uDRAMCTemp)-4;
        size = DRAMC_str_save_registers(uDRAMCTemp+1, sizeof(uDRAMCTemp)-4);

        // Save CPU stats ptr & DRAMC registers to PDWNC data SRAM
        ASSERT(size+4 < PDWNC_SUSPEND_SAVE_SIZE);
        uDRAMCTemp[0] = (UINT32)puCpuStats;

        // Debug print to help check first read error.
        Printf("DRAMC Saves 0x%08x:  0x%08x\n", uDRAMCTemp[0], *(UINT32*)uDRAMCTemp[0]);
#if defined(CC_MT5890)
        //*puCpuStats = vaddr_to_phys((void*)(*puCpuStats)) + 0xa0000000; // address of cpu_Resume, from 0xbf... to physical address
        *puCpuStats = (vaddr_to_phys((void*)(*puCpuStats))) + 0xa0000000; // address of cpu_Resume, from 0xbf... to physical address
#else
        *puCpuStats = vaddr_to_phys((void*)(*puCpuStats)) | 0x80000000; // address of cpu_Resume, from 0xbf... to physical address
#endif
        Printf("DRAMC Saves 0x%08x:  0x%08x\n", uDRAMCTemp[0], *(UINT32*)uDRAMCTemp[0]);
#if defined(CC_MT5890)
        uDRAMCTemp[0] = (__pa((UINT32)puCpuStats)) + 0xa0000000; // address of CPU context-saving, to physical address
#else
        uDRAMCTemp[0] = ((UINT32)puCpuStats & 0x3fffffff) | 0x80000000; // address of CPU context-saving, to physical address
#endif
        Printf("DRAMC Saves 0x%08x\n", uDRAMCTemp[0]);

        // Copying Suspend Data to T8032 XData
        {
            UINT32 i;
            volatile UINT8* pu1Ptr = (volatile UINT8*)uDRAMCTemp;
		#if 0 // show dramc register value. // yjdbg
			Printf("store dramc register.\n");
			for(i=0;i<PDWNC_SUSPEND_SAVE_SIZE/4;i+=4)
			{
				Printf("%d:0x%08X  0x%08X  0x%08X  0x%08X .\n",i,uDRAMCTemp[i],uDRAMCTemp[i+1],uDRAMCTemp[i+2],uDRAMCTemp[i+3]);
			}
		#endif
			PDWNC_T8032_CMD_T rCmd;
			rCmd.u1Cmd = PDWNC_CMD_ARMT8032_XDATA_SWITCH;
			rCmd.u1SubCmd = 0x1;
			MSET_XDATA_TO_ARM_FLAG();
			CHIP_T8032Cmd(&rCmd);
			vIO32WriteFldAlign(PDWNC_UP_CFG, 0x1, FLD_XDATA_ACC);//switch xdata to ARM	  
			vIO32WriteFldAlign(PDWNC_UP_ADDR,(XDATA_ADDR + PDWNC_SUSPEND_SAVE_OFFSET), FLD_UP_ADDR);
			for(i = 0; i < PDWNC_SUSPEND_SAVE_SIZE; i++)
			{
					IO_WRITE32(PDWNC_UP_DATA, 0, *pu1Ptr);
					pu1Ptr ++;
			}
			vIO32WriteFldAlign(PDWNC_UP_CFG, 0x0, FLD_XDATA_ACC);//switch xdata to T8032
			MCLR_XDATA_TO_ARM_FLAG();

			//IO_WRITE32(PDWNC_T8032_INTEN,0,0);//clear all interrupt	
			IO_WRITE32(PDWNC_INTCLR,0,0xffffffff);//clear all interrupt    
        }
#ifdef DO_DRAMB_CHECKSUM
        //vDoChBChecksum(1);
#ifdef ENABLE_GDMA_EXP
        flush_cache_all();
        // Flush caches. P.S. will contain L1+L2, if it has
        HalFlushDCache();

        //vDoChBChecksum(2); // use GDMA to update checksum
#endif

#if 0 // NG at ldrex, strex
#include <asm/proc-fns.h>
	// Disable L1 cache
        flush_cache_all();
	cpu_proc_fin(); // need to export cpu_v7_proc_fin in kernel
        flush_cache_all();
	__asm__ ("isb");
#endif

        //vDoChBChecksum(0); // check the result immediately
#endif

#ifdef DO_DRAMB_BACKUP
        MID_Reset();
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x20000000, 0x20000000); // duplicate chB to debug
        MID_WarmBootRegionProtect(TRUE); // to blocking write request to DRAM PHY
        flush_cache_all();
        HalFlushDCache();
        //GDMA_PHY_MemCpy_Exp(0x00000000, 0x00000000, 0x20000000);
        //GDMA_PHY_MemCpy_Exp(0x20000000, 0x20000000, 0x20000000); // chB.F.512 to chB.F.512 => MID protect: 0/1390: AP error

	//experiments
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x00000000, 0x20000000); // chA.F.512 to chB.B.512 // MID protect, 1/31: AP error
        //GDMA_PHY_MemCpy_Exp(0x20000000, 0x00000000, 0x20000000); // chA.F.512 to chB.F.512 // MID protect, 1/21: AP error
        //GDMA_PHY_MemCpy_Exp(0x00000000, 0x00000000, 0x20000000); // chA.F.512 to chA.F.512 // MID protect, 1/12: AP error
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x00000000, 0x20000000); // chA.F.512 to chB.B.512 // wo MID protect, 0/300: AP error
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x20000000, 0x20000000); // chB.F.512 to chB.B.512 // wo MID protect, 0/2970: AP error
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x00000000, 0x20000000); // chA.F.512 to chB.B.512 // MID protect only B, 1/8, 1/64
        //GDMA_PHY_MemCpy_Exp(0x40000000, 0x00000000, 0x20000000); // chA.F.512 to chB.B.512 // MID protect only A, 1/816, 1/877

        //mdelay(10000); // delay 10s to test if the dram content is broken
#endif
#ifdef TEST_AGENT2_TOUCH
	vDramCHBTestAgent2(0x40000000, 0x08000000);
	vDramCHBTestAgent2(0x48000000, 0x08000000);
	vDramCHBTestAgent2(0x50000000, 0x08000000);
	vDramCHBTestAgent2(0x58000000, 0x08000000);
#endif
#ifdef TEST_AGENT0_TOUCH
	//vDramCHBTestAgent0(0x20000000, 0x20000000);
	vDramCHBTestAgent0(0x40000000, 0x20000000);
#endif
        flush_cache_all();
        // Flush caches. P.S. will contain L1+L2, if it has
        HalFlushDCache();
#ifdef CC_MT5399 // for CHB
	prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_MJC , NULL);
	if (prPool != NULL)
	{
	    vFlushChABPostWriteFifo(prPool->u4Addr, 0x1000);
	}
#endif
#if !defined(CC_MT5890) // CC_MT5890 can flush dram write fifo via request
	prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_3DC , NULL);
	if (prPool != NULL)
	{
	    vFlushCoreL1((UINT32*)VIRTUAL(prPool->u4Addr), 0x10000*2); // size: 128K = L1*2
	    vFlushChABPostWriteFifo(prPool->u4Addr, 0x1000);
	}
#endif
#if defined(CC_MT5890)
        *((volatile UINT32*)(0xf000606c))=((*((volatile UINT32*)(0xf000606c)))|0x30);
        *((volatile UINT32*)(0xf001006c))=((*((volatile UINT32*)(0xf001006c)))|0x30);
        *((volatile UINT32*)(0xf001106c))=((*((volatile UINT32*)(0xf001106c)))|0x30);
#elif defined(CC_MT5882)
        *((volatile UINT32*)(0xf000606c))=((*((volatile UINT32*)(0xf000606c)))|0x30);
#endif
        MID_WarmBootRegionProtect(TRUE);

        // DRAM Checksum, if necessary.
#if 0 // def DO_DRAM_CHECKSUM // move after Printf, because printk will pollute memory
        for (i=idx=0; i<cnum; i++)
        {
            if (i == uNoChecksum1 || i == uNoChecksum2)
                continue;
            puChecksum[idx++] = CheckSum((void*)(i*DRAM_CHECKSUM_BLOCK_SIZE),
                                         DRAM_CHECKSUM_BLOCK_SIZE);
            i += DRAM_CHECKSUM_SKIP;
        }
        puChecksum[checknum-1] = 0;
        puChecksum[checknum-1] = CheckSum((void*)(uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE),
                                    DRAM_CHECKSUM_BLOCK_SIZE);
#endif

        CHIP_DisplayString("Going to bed...\n");
        //vDoChBChecksum(0); // test the mid

#ifdef DO_DRAMB_CHECKSUM
        //vDoChBChecksum(1);
#endif
        MID_WarmBootRegionProtect(FALSE); // report mid intrusion status
        if (MID_QueryIntrusionStatus())
        {
            //while (1); // TODO: filter the GDMA overwrite
        }
        // DRAM Checksum, if necessary.
#ifdef DO_DRAM_CHECKSUM
/*
1. use the follwoing write/read/write flow to flush the data form fifo in dram controller
2. write 0 to dram so that not to change the checksum value of dram
*/
	prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_3DC , NULL);
	if (prPool != NULL)
	{
        pv_Phys = (void*)(prPool->u4Addr); z_Size = 0x1000;
        pvVirt = ioremap_nocache((int)pv_Phys, z_Size);
        pvData = (UINT32*) pvVirt;
        if(pvData == NULL)
        {
            CHIP_DisplayString("Failed to flush DRAMC wfifo !\n");
            return;
        }
        // write
        for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
        {
    	    pvData[loop] = 0;
        }
        // read
        for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
        {
    	    pvData[loop+512] = pvData[loop];
        }
    
        // write
        for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
        {
    	    pvData[loop] = 0;
        }
//        iounmap(pvVirt);
	}
        // checksum workaround for address of cpu_Resume
        tmp = *puCpuStats; // the value will be gone after resume, because the cpu context is already saved
        *puCpuStats = TEMP_IRAM_AREA+0x800+(tmp&0xfff);

        for (i=idx=0; i<cnum; i++)
        {
            if (i == uNoChecksum1 || i == uNoChecksum2)
                continue;
            puChecksum[idx++] = CheckSum((void*)(i*DRAM_CHECKSUM_BLOCK_SIZE),
                                         DRAM_CHECKSUM_BLOCK_SIZE);
            i += DRAM_CHECKSUM_SKIP;
        }
        puChecksum[checknum-1] = 0;
        if(uNoChecksum1<cnum)
        {
            puChecksum[checknum-1] = CheckSum((void*)(uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE),
                    DRAM_CHECKSUM_BLOCK_SIZE);
        }
        else
        {
            CHIP_DisplayString(" [Note] uNoChecksum1");
            CHIP_DisplayHex(uNoChecksum1);
            CHIP_DisplayString("\n");
        }
        *puCpuStats = tmp;
#endif
#ifdef BYPASS_CORE_OFF
        *puCpuStats = TEMP_IRAM_AREA+0x800+(tmp&0xfff); // to pass the checksum checking
#endif
        // Copy necessary code to SRAM
        x_memcpy((void *) TEMP_IRAM_AREA, (void*)&mt53xx_dram_suspend, mt53xx_dram_suspend_sz);

        //HalInvalidateICache();//replace it with disabling L1/L2 cache
        flush_icache_range((unsigned long)TEMP_IRAM_AREA,                 
                (unsigned long)(TEMP_IRAM_AREA) + mt53xx_dram_suspend_sz);
        flush_cache_all();
        // Flush caches. P.S. will contain L1+L2, if it has
        HalFlushDCache();
        HalFlushInvalidateDCache();

#if 0//david
	// DMARB workaround to flush fifo
        flush_cache_all();
	outer_flush_all(); // NOTE: in order to flush preempt_count in smpDoCacheOps:preempt_enable // TODO: remove kernel version of L1+L2 flush, only flushing preempt_count doesnot work

	HalFlushInvalidateDCacheMultipleLine((UINT32)current->stack, 32);  // NOTE: dummy read to ensure write fifo is empty

        CHIP_DisplayString("preempt_count: ");
        CHIP_DisplayHex(preempt_count());
        CHIP_DisplayString("\n");
        CHIP_DisplayString("va: ");
        CHIP_DisplayHex((UINT32)current->stack);
        CHIP_DisplayString(" ,value: ");
        CHIP_DisplayHex(*(UINT32*)((UINT32)(current->stack)+4));
        CHIP_DisplayString("\n");

	HalFlushWriteBuffer();

	// invalidate - dsb - write - flushinvalidate - dsb - read - dsb
	HalFlushInvalidateDCacheMultipleLine((UINT32)_dummy_data_1, DUMMY_DATA_LEN);
	HalFlushWriteBuffer();
	_dummy_data_1[0] = DUMMY_VALUE;
	HalFlushInvalidateDCacheMultipleLine((UINT32)_dummy_data_1, DUMMY_DATA_LEN);
	HalFlushWriteBuffer();
        CHIP_DisplayString("dummy write/read: ");
        CHIP_DisplayHex(_dummy_data_1[0]);
        CHIP_DisplayString("\n");
	HalFlushWriteBuffer();
#else
/*
1. use the follwoing write/read/write flow on non cacheable region to flush the data form fifo in dram controller
2. write 0 to dram so that not to change the checksum value of dram
*/
    if(pvData == NULL)
    {
        CHIP_DisplayString("Failed to flush DRAMC wfifo !!\n");
        return;
    }

    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
	    pvData[loop] = 0;
    }
    // read
    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
	    pvData[loop+512] = pvData[loop];
    }

    // write
    for (loop=0; loop<512; loop++) // bus width(128bit)*6*2
    {
    	pvData[loop] = 0;
    }
#endif	
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)// ARMv7
	__asm__ __volatile__("isb");
#else
	__asm__ ("mcr p15, 0, %0, c7, c10, 5\n" : : "r" (value)); // dmb
#endif

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#ifndef BYPASS_CORE_OFF
        value = 0;
        __asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0\n": :"r"(value)); // flush I,D TLBs
        __asm__ __volatile__("isb");
#endif
        __asm__ __volatile__("dmb");
        __asm__ __volatile__("dsb");
        __asm__ __volatile__("isb");

	__asm__ __volatile__("clrex"); // Execute a CLREX instruction

// L1 cache
#if 1
#include <asm/proc-fns.h>
	// Disable L1 cache
        flush_cache_all();
	//cpu_proc_fin(); // need to export cpu_v7_proc_fin in kernel
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	HalDisableL1Caches(); // Disable I/D L1 cache
#endif
        flush_cache_all();
	//scu_power_mode(0xf2002000, SCU_PM_POWEROFF);
	//*(volatile unsigned char*) 0xf2002008 = 0x3;
#endif

// L2 cache
#if 0
	// Disable L2
	//outer_flush_all();
	__l2x0_flush_all();
	//outer_disable(); // no permission when trustzone is on -> TODO: move to TZ
	__l2x0_disable();
	__l2x0_flush_all();
#if 0
#if CC_TRUSTZONE_SUPPORT
	TZ_HAL_DisableL2();
#endif
#endif
#endif
	HalupFlushInvalidateDCache();
        __asm__ __volatile__("clrex"); // Execute a CLREX instruction

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");

        __asm__ __volatile__("mrc p15, 0, %0, c1, c0, 1\n": "=r"(value)); 
#if defined(CC_MT5890) || defined(CC_MT5882)
        //ACTLR.SMP=0, ACTLR.L1PF=0, ACTLR.L2PF=0
        value &= ~0x46; // cannot control L1PF and L2PF in non-secure world
#else
        //ACTLR.SMP=0, ACTLR.FW=0
        value &= ~0x41;
#endif
        __asm__ __volatile__("mcr p15, 0, %0, c1, c0, 1\n": :"r"(value));
#endif

	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");

#endif
        //vDoChBChecksum(0); // check the result after disabling L1/L2 cache // NOTE: need to re-implement

#if 0
	if (memcmp((void *) TEMP_IRAM_AREA, (void*)&mt53xx_dram_suspend, mt53xx_dram_suspend_sz) != 0)
	{
	    CHIP_DisplayString("code in sram is overwritten!!\n");
	}
#endif

        //MID_WarmBootRegionProtect(TRUE); // it will ruin the dram checksum

#if defined(CC_MT5890)
        *((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHA))=((*((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHA)))|0x30);
        *((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHB))=((*((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHB)))|0x30);
        *((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHC))=((*((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHC)))|0x30);
#elif defined(CC_MT5882)||defined(CC_MT5883)
		*((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHA))=((*((volatile UINT32*)(DRAM_POST_WRITE_FLUSH_CHA)))|0x30);

#endif

#ifndef BYPASS_CORE_OFF
        // No more DRAM access after this point
        // Jumping to SRAM suspend code, no return.
        (void) ((void (*)(void))TEMP_IRAM_AREA)();
        while (avoid_compile_opt); // avoid to use while (1), it will let compiler do optimization on local variable allocation
#else
        #if 1
        __bim_writel(0x1, REG_RW_IRQEN);
        // No more DRAM access after this point
        // Jumping to SRAM suspend code, no return.
        (void) ((void (*)(void))TEMP_IRAM_AREA)();
        //while (avoid_compile_opt); // avoid to use while (1), it will let compiler do optimization on local variable allocation
        #else
        __bim_writel(0x1, REG_RW_IRQEN);
        __asm__ ("wfi");
        #endif
#endif
    }

    BIM_SetTimeLog(2);

#if 0
#if defined(CC_MT5399) || defined(CC_MT5890)
#ifdef SMP
    //dec_preempt_count(); // temp solution
    while (preempt_count() != 1) // workaround, temp solution
    {
	CHIP_DisplayString("preempt_count.1: ");
	CHIP_DisplayHex(preempt_count());
	CHIP_DisplayString("\n");

	preempt_enable();
    }
#endif
#endif
#endif

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    memcpy((void*)0xFB00BF00, mt53xx_saved_core1, 256); // restore the code sram for core 1
    // may reset timer register
#endif
    //DMARC_TCM_partial_resume(); // restore dram size in sram

    //_yjdbg_2 = 2;
    //while (_yjdbg_2);

    // Resumed.
    CHIP_DisplayString("Coming back, now wakeup HW\n");

    // DRAM Checksum, if necessary
#ifdef DO_DRAM_CHECKSUM
    uError=0;
    if (uNoChecksum1 == uNoChecksum2)
    {
        CHIP_DisplayString("Stack == Checksum area, skip DRAM Checksum check\n");
    }
    else
    {
        i = puChecksum[checknum-1];
        puChecksum[checknum-1] = 0;
        if (uNoChecksum1<cnum)
        {
        result = CheckSum((void*)(uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE),
                          DRAM_CHECKSUM_BLOCK_SIZE);
        if (result != i)
        {
            uError++;
            //Printf("Checksum check error, offset 0x%08x original 0x%08x current 0x%08x\n",
                   //uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE, i, result);
            CHIP_DisplayString("Checksum check error, offset ");
            CHIP_DisplayHex(uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE);
            CHIP_DisplayString(" original ");
            CHIP_DisplayHex(i);
            CHIP_DisplayString(" current ");
            CHIP_DisplayHex(result);
            CHIP_DisplayString("\n");
        }
        }
    }
    for (i=idx=0; i<cnum; i++)
    {
        if (i == uNoChecksum1 || i == uNoChecksum2)
            continue;
        result = CheckSum((void*)(i*DRAM_CHECKSUM_BLOCK_SIZE),
                          DRAM_CHECKSUM_BLOCK_SIZE);
        if (result != puChecksum[idx])
        {
            uError++;
            //Printf("DRAM checksum error, offset 0x%08x original 0x%08x current 0x%08x\n",
                   //i*DRAM_CHECKSUM_BLOCK_SIZE, puChecksum[idx], result);
            CHIP_DisplayString("DRAM checksum error, offset ");
            CHIP_DisplayHex(i*DRAM_CHECKSUM_BLOCK_SIZE);
            CHIP_DisplayString(" original ");
            CHIP_DisplayHex(puChecksum[idx]);
            CHIP_DisplayString(" current ");
            CHIP_DisplayHex(result);
            CHIP_DisplayString("\n");
        }
        idx++;
        i += DRAM_CHECKSUM_SKIP;
    }
    x_mem_free(puChecksum);
    x_mem_free(_dummy_data_1);
    iounmap(pvVirt);
    MID_WarmBootRegionProtect(FALSE); // move after the checksum checking

    if (uError)
    {
        // DRAM checksum error, reboot
        //Printf("DRAM checksum error %d, reboot!\n", uError);
        CHIP_DisplayString("DRAM checksum error ");
        CHIP_DisplayHex(uError);
        CHIP_DisplayString(", reboot!\n");
        for (i=0; i<15; i++)
            CHIP_DisplayString("!!!!!!!!!!!!!!!!\n");

        PDWNC_Reboot();
    }
#endif /* DO_DRAM_CHECKSUM */

#ifdef BYPASS_CORE_OFF
    MID_Reset();
#endif

#ifdef CHB_ACCESS_TEST
    //vDoChBAccessTest(0x1000);
#endif

#ifdef DO_DRAMB_CHECKSUM
    //vDoChBChecksum(0); // NG: panic here => related L2 cache (FOZ)
#endif

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#ifndef CONFIG_SMP
    mt53xx_resume_up_check();
#endif
#ifdef CONFIG_SMP
    arch_disable_nonboot_cpus_early_resume();
#endif
#endif

    //_yjdbg_2 = 2;
    //while (_yjdbg_2);


    CHIP_DisplayString("Start Resume\n");

    // Enable IRQ.
    HalCriticalEnd(state);

    ////// Calling all modules' resume function.
    ////MaybeDisableOutbyte(1);
    ////_mt53xx_STR_resume();
    ////MaybeDisableOutbyte(0);

    x_mem_free(puCpuStats);

    BIM_SetTimeLog(3);
    mt53xx_SuspendLeave(); // restore system timer earlier
    x_os_drv_set_timestamp("mt53xx_SuspendEnter done");
    CHIP_DisplayString("Resume complete, back to work\n");
    //verify busy delay
    //CHIP_DisplayString("a\n");
    //x_os_drv_set_timestamp("a");
    //mdelay(1000);
    ////x_os_drv_set_timestamp("b");
    //CHIP_DisplayString("b\n");

    //restore WDT earlier than dpm resume
    WDT_pm_resume();
}

#ifdef BYPASS_CORE_OFF_SIMPLE
void mt53xx_SuspendEnter_fake_simple(void)
{
    __bim_writel(0x1, REG_RW_IRQEN);
    __asm__ ("wfi");
}
#endif

#if 0
static struct platform_driver _mt53xx_STR_driver =
{
	.probe          = _mt53xx_STR_probe,
	.remove         = _mt53xx_STR_remove,
	.suspend        = _mt53xx_STR_suspend,
	.resume         = _mt53xx_STR_resume,
	.driver		= {
	        .name	= "mt53xx_STR",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device _mt53xx_STR_device = {
	.name			= "mt53xx_STR",
	.id			= 0
};

int  mt53xx_STR_init(void)
{
	printk("mt53xx_STR_init\n");
	platform_device_register(&_mt53xx_STR_device);    
	return platform_driver_register(&_mt53xx_STR_driver);
}
#endif

#if 1 // def CONFIG_OPM
#include <linux/opm_device.h>
#include <linux/err.h>

extern void PDWNC_Reboot(void);
static int opmcls_reboot;
void opmcls_shutdown(struct opm_device *devptr)
{
    printk("opmcls_shutdown\n");
    if(opmcls_reboot)
        PDWNC_Reboot();
}

static struct opm_driver opmcls_driver_reboot = {
    .shutdown   = opmcls_shutdown,
    .driver     = {
        .name   = "opm_sysclass"
    },
};

static struct opm_device *device;

static struct opm_driver _mt53xx_STR_driver = {
    .probe              = _mt53xx_STR_probe,
    .remove             = _mt53xx_STR_remove,
    .suspend            = _mt53xx_STR_suspend,
    .resume             = _mt53xx_STR_resume,
    .driver             = {
        .name   = "mt53xx_STR",
        .owner  = THIS_MODULE,
    },
};
static struct opm_device *device;

int  mt53xx_STR_init(void)
{
    int err;

    if ((err = opm_driver_register(&_mt53xx_STR_driver)) < 0)
        return err;
    if ((err = opm_driver_register(&opmcls_driver_reboot)) < 0)
        return err;
    device = opm_device_register_simple("mt53xx_STR",0,ORDER_IGNORE);
    if (IS_ERR(device))
        return -ENODEV;
    return 0;
}
#endif

#endif
