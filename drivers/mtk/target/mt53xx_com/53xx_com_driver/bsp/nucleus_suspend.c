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
 
/** @file nuclues_suspend.c
 *  provides functions for nucleus suspend to ram & resume support
 *  Used in Nucleus AP only.
 *
 */

#ifndef CC_MTK_LOADER

#include "x_bim.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"

#include "x_os.h"
#include "x_util.h"
#include "nucleus.h"

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

#ifdef CC_SUPPORT_STR_CORE_OFF
#include "irtx_if.h"
#endif
extern void vDrvODSuspend(void);
extern void vDrvODResume(void);
extern void vDrvMIBSuspend(void);
extern void vDrvMIBResume(void);
extern void vDrvScposSuspend(void);
extern void vDrvScposResume(void);
extern void AUD_pm_str_suspend(void);
extern void AUD_pm_str_resume(void);
#ifdef CC_SUPPORT_VENC
extern void VENC_pm_str_suspend(void);
extern void VENC_pm_str_resume(void);
#endif
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
extern void vDrvTTDSuspend(void);
extern void vDrvTTDResume(void);

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
extern void MHL_pm_suspend(void);
extern void MHL_pm_resume(void);
extern void HDMI_pm_suspend(void);
extern void HDMI_pm_resume(void);
extern void ADC_pm_resume(void);
extern void Tuner_pm_str_suspend(void);
extern void Tuner_pm_str_resume(void);
extern void BIM_pm_suspend(void);
extern void BIM_pm_resume(void);

//LVDS
extern void vDrvLVDSSuspend(void);
extern void vDrvLVDSResume(void);
extern void vDrvVOPLLSuspend(void);
extern void vDrvVOPLLResume(void);

extern BOOL MID_EnableRegionReadProtect(UINT32 u4Region, BOOL onoff);

extern void vDrvDisplayInit_pm_resume(void);

#ifdef CC_SUPPORT_STR_CORE_OFF
extern UINT32 PDWNC_EnterSuspend(void);
extern void IRTX_pm_suspend(void);
extern void IRTX_pm_resume(void);
extern void WDT_pm_suspend(void);
extern void WDT_pm_resume(void);
extern void IRRX_pm_resume(void);
extern void DMARB_mid_suspend(void);
extern void DMARB_mid_resume(void);
extern void DMARC_TCM_suspend(void);
extern void DMARC_TCM_resume(void);


#else
extern void PDWNC_EnterSuspend(void);
#endif
extern void PDWNC_Resume(void);
#ifdef CC_SUPPORT_STR_CORE_OFF
extern void PDWNC_SetSuspendMode(UINT32 SuspendMode);
extern void Board_pm_suspend(void);
extern void Board_pm_resume(void);
extern UINT32 DRAMC_str_save_registers(UINT32 *u4DRAMCTemp,UINT32 u4Size);

#else
extern void PDWNC_SetSuspendMode(void);
extern void PDWNC_SetResumeMode(void);
#endif
extern INT32 _i4EtherSuspend(void);
extern INT32 _i4EtherResume(void);
EXTERN INT32 MUSB_Suspend(void);
EXTERN INT32 MUSB_Resume(void);

extern void UART_pm_resume(void);
extern void vDrvNptvResume(void);


#define DO_DRAM_CHECKSUM
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

#define ENABLE_JTAG_TO_DEBUG
#ifdef ENABLE_JTAG_TO_DEBUG
INT32 CLI_Parser(const CHAR *szCmdBuf);
#endif

#ifdef NO_DEBUG_FUNCTION_PRINT
#define DebugFunctionPrint(a)    x_os_drv_set_timestamp(a)
#else
#define DebugFunctionPrint(a)    Printf(a"\n")
#endif

#ifdef DISABLE_OUTBYTE
#define MaybeDisableOutbyte(a)     SetDisableOutbyte(a)
#else
#define MaybeDisableOutbyte(a)
#endif


#define DEBUG_FUNCTION_CALL(x)\
    do{\
	DebugFunctionPrint(#x);\
	x;\
    } while (0)
static int _mt53xx_STR_suspend(void)
{
    Printf("_mt53xx_STR_suspend\n");
	#ifdef CC_SUPPORT_STR_CORE_OFF
    PDWNC_SetSuspendMode(TRUE);
	#else
	PDWNC_SetSuspendMode();
	#endif

#ifdef ENABLE_JTAG_TO_DEBUG
    CLI_Parser("b.jtag 1");
#endif
    DEBUG_FUNCTION_CALL(vDrvLVDSSuspend());
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xff, 0xfe));
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xfe, 0xff));
    DEBUG_FUNCTION_CALL(vDrvVOPLLSuspend());
    DEBUG_FUNCTION_CALL(Tuner_pm_str_suspend());
    DEBUG_FUNCTION_CALL(vDrvODSuspend());
    DEBUG_FUNCTION_CALL(vDrvMIBSuspend());
    DEBUG_FUNCTION_CALL(vDrvPostprocSuspend());
    DEBUG_FUNCTION_CALL(vDrvTTDSuspend());
#ifndef CC_MT5881
    DEBUG_FUNCTION_CALL(vDrvLcDimPQSuspend());
    DEBUG_FUNCTION_CALL(vDrvLcdimSpiSuspend()); 
#endif
#ifndef CC_MT5882
    DEBUG_FUNCTION_CALL(vDrvThermal_pm_suspend());
#endif
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
    DEBUG_FUNCTION_CALL(NFI_pm_suspend());
    DEBUG_FUNCTION_CALL(SIF_pm_str_suspend());
    DEBUG_FUNCTION_CALL(NOR_pm_str_suspend());
    DEBUG_FUNCTION_CALL(PWM_pm_str_suspend());
    DEBUG_FUNCTION_CALL(vDrvScposSuspend());
    DEBUG_FUNCTION_CALL(_i4EtherSuspend());
    DEBUG_FUNCTION_CALL(MUSB_Suspend());
#ifdef CC_SUPPORT_STR_CORE_OFF
    DEBUG_FUNCTION_CALL(IRTX_pm_suspend());
    DEBUG_FUNCTION_CALL(WDT_pm_suspend());
    DEBUG_FUNCTION_CALL(BIM_pm_suspend());
    DEBUG_FUNCTION_CALL(GPIO_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Board_pm_suspend());
	DEBUG_FUNCTION_CALL(DMARB_mid_suspend());
	DEBUG_FUNCTION_CALL(DMARC_TCM_suspend());
    UNUSED(PDWNC_EnterSuspend());
#else
    PDWNC_EnterSuspend();  
#endif
    return 0;
}

static volatile int resumeThreadDone;
static void _ParallelResumeThread(void *pvArgs)
{
    DEBUG_FUNCTION_CALL(vDrvDisplayInit_pm_resume());
    DEBUG_FUNCTION_CALL(vDrvLVDSResume());
	DEBUG_FUNCTION_CALL(ADC_pm_resume());// please put it after the one who call VOPLL_pm_resume();

    resumeThreadDone=1;
    DebugFunctionPrint("ParallelResume complete");
}

static int _mt53xx_STR_resume()
{
    HANDLE_T hResumeThread;

    Printf("_mt53xx_STR_resume\n");
#ifdef CC_SUPPORT_STR_CORE_OFF
    PDWNC_SetSuspendMode(FALSE);

    DEBUG_FUNCTION_CALL(Board_pm_resume());
    DEBUG_FUNCTION_CALL(GPIO_pm_str_resume());
    DEBUG_FUNCTION_CALL(BIM_pm_resume());
	DEBUG_FUNCTION_CALL(DMARB_mid_resume());
	DEBUG_FUNCTION_CALL(DMARC_TCM_resume());
#else
    PDWNC_SetResumeMode();
#endif
    DEBUG_FUNCTION_CALL(SIF_pm_str_resume());
    DEBUG_FUNCTION_CALL(vDrvNptvResume());
    DEBUG_FUNCTION_CALL(vDrvScposResume());
    DEBUG_FUNCTION_CALL(vDrvVOPLLResume());
    DEBUG_FUNCTION_CALL(vDrvODResume());
    DEBUG_FUNCTION_CALL(vDrvMIBResume());
#ifndef CC_MT5881
    DEBUG_FUNCTION_CALL(vDrvLcDimPQResume());
    DEBUG_FUNCTION_CALL(vDrvLcdimSpiResume());        
#endif
#ifndef CC_MT5882
    DEBUG_FUNCTION_CALL(vDrvThermal_pm_resume());
#endif
    resumeThreadDone = 0;
    x_thread_create(&hResumeThread, "ResumeThread",
                    2048, 100,
                    _ParallelResumeThread, 0, NULL);

    DEBUG_FUNCTION_CALL(vDrvPostprocResume());
    DEBUG_FUNCTION_CALL(vDrvTTDResume());
    DEBUG_FUNCTION_CALL(AUD_pm_str_resume());
    DEBUG_FUNCTION_CALL(OSD_pm_resume());
    DEBUG_FUNCTION_CALL(GFX_pm_resume());
    DEBUG_FUNCTION_CALL(GCPU_pm_str_resume());
    DEBUG_FUNCTION_CALL(DMX_pm_str_resume());
    DEBUG_FUNCTION_CALL(VDEC_pm_str_resume());
    DEBUG_FUNCTION_CALL(Image_pm_str_resume());
    DEBUG_FUNCTION_CALL(Imgrz_pm_str_resume());
    DEBUG_FUNCTION_CALL(TVD_pm_resume());
    DEBUG_FUNCTION_CALL(NOR_pm_str_resume());
    DEBUG_FUNCTION_CALL(Tuner_pm_str_resume());
    DEBUG_FUNCTION_CALL(VBI_pm_resume());
    DEBUG_FUNCTION_CALL(TVE_pm_resume());
    DEBUG_FUNCTION_CALL(B2R_pm_str_resume());
    DEBUG_FUNCTION_CALL(PCMCIA_pm_str_resume());
    DEBUG_FUNCTION_CALL(HDMI_pm_resume());
    DEBUG_FUNCTION_CALL(MHL_pm_resume());
    DEBUG_FUNCTION_CALL(NFI_pm_resume());
    DEBUG_FUNCTION_CALL(PWM_pm_str_resume());

    //DEBUG_FUNCTION_CALL(ADC_pm_resume());// please put it after the one who call VOPLL_pm_resume();
    DEBUG_FUNCTION_CALL(MUSB_Resume());


#ifdef CC_SUPPORT_STR_CORE_OFF
    DEBUG_FUNCTION_CALL(IRTX_pm_resume());
    DEBUG_FUNCTION_CALL(WDT_pm_resume());
	DEBUG_FUNCTION_CALL(IRRX_pm_resume());
#endif
    DEBUG_FUNCTION_CALL(_i4EtherResume());
    
    DEBUG_FUNCTION_CALL(UART_pm_resume());
    
    PDWNC_Resume();

    // Wait until resume thread complete.
    while (!resumeThreadDone) x_thread_delay(20);

    return 0;
}

#ifdef DO_DRAM_CHECKSUM
static UINT32 CheckSum(void *pStart, UINT32 size)
{
    UINT32 result = 0;
    UINT32 a, b, c, d, i;
    UINT32 *puStart = (UINT32*)pStart;

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

extern void SetDisableOutbyte(BOOL fgDisableOutbyte);
extern void mt53xx_dram_suspend(void);
extern int mt53xx_dram_suspend_sz;


void mt53xx_SuspendEnter()
{
    UINT32 *puCpuStats, state, size;
	#if defined(CC_MT5890)
    UINT32 uDRAMCTemp[SUSP_DRAMC_REGISTER_TOTAL+1];
	#else
    UINT32 uDRAMCTemp[SUSP_DRAMC_PER_CHANNEL*2+1];
	#endif
#ifdef DO_DRAM_CHECKSUM
    INT32 ret;
    UINT32 uNoChecksum1, uNoChecksum2, uError, result, cnum, totalSize, i, idx, checknum;
    UINT32 *puChecksum;

    // Allocate space out of critical section, to prevent x_mem_alloc enable IRQ.
    ret = DRVCUST_OptQuery(eFbmMemAddr, &totalSize);
    if (ret != 0)
    {
        totalSize = 64*1024*1024;
        Printf("FBM address unknown\n");
    }
    else
        Printf("FBM address 0x%x\n", totalSize);
    cnum = totalSize/DRAM_CHECKSUM_BLOCK_SIZE;
    checknum = cnum/(DRAM_CHECKSUM_SKIP+1) + 2;
    puChecksum = x_mem_alloc(checknum * sizeof(UINT32));
    uNoChecksum1 = (UINT32)puChecksum / DRAM_CHECKSUM_BLOCK_SIZE;
    uNoChecksum2 = (UINT32)uDRAMCTemp / DRAM_CHECKSUM_BLOCK_SIZE;
    if (!puChecksum)
        return;
#endif /* DO_DRAM_CHECKSUM */

    // Allocate space out of critical section, to prevent x_mem_alloc enable IRQ.
    puCpuStats = x_mem_alloc(SUSP_CPU_STATS_SIZE);
    if (!puCpuStats)
    {
#ifdef DO_DRAM_CHECKSUM
        x_mem_free(puChecksum);
#endif
        return;
    }

    x_os_drv_set_timestamp("Start Suspend");

    // Calling all modules' suspend functions.
    _mt53xx_STR_suspend();

    // Make sure all logs are printed.
    UTIL_LogFlush();

    x_os_drv_set_timestamp("Suspend Complete");

    // disable IRQ
    state = HalCriticalStart();
    Printf("Prepare for suspend...\n");

    // Saving current CPU's stats for resume.
    if (cpu_SaveSuspendReturn(puCpuStats))
    {
        // Get DRAM IO control registers from DRAMC, skip 4bytes for CPU stats.
        size = sizeof(uDRAMCTemp)-4;
        size = DRAMC_str_save_registers(uDRAMCTemp+1, sizeof(uDRAMCTemp)-4);
		UNUSED(size);
		
        // Save CPU stats ptr & DRAMC registers to PDWNC data SRAM
        ASSERT(size+4 < PDWNC_SUSPEND_SAVE_SIZE);
        uDRAMCTemp[0] = (UINT32)puCpuStats;

        // Debug print to help check first read error.
        Printf("DRAMC Saves 0x%08x:  0x%08x\n", uDRAMCTemp[0], *(UINT32*)uDRAMCTemp[0]);

        // Copying Suspend Data to T8032 XData
        {
            UINT32 i;
            volatile UINT8* pu1Ptr = (volatile UINT8*)uDRAMCTemp;
            
            IO_WRITE32(PDWNC_T8032_INTEN,0,0);//clear all interrupt	
            vIO32WriteFldAlign(PDWNC_CLKPDN, 0x1, FLD_T8032PD);
            vIO32WriteFldAlign(PDWNC_UP_CFG, 0x1, FLD_XDATA_ACC);//switch xdata to ARM	  
            vIO32WriteFldAlign(PDWNC_UP_ADDR,(XDATA_ADDR + PDWNC_SUSPEND_SAVE_OFFSET), FLD_UP_ADDR);
            
            for(i = 0; i < PDWNC_SUSPEND_SAVE_SIZE; i++)
            {
                    IO_WRITE32(PDWNC_UP_DATA, 0, *pu1Ptr);
                    pu1Ptr ++;
            }	
    
            vIO32WriteFldAlign(PDWNC_UP_CFG, 0x0, FLD_XDATA_ACC);//switch xdata to 8032  
            vIO32WriteFldAlign(PDWNC_CLKPDN, 0x0, FLD_T8032PD);
        }

	MID_WarmBootRegionProtect(TRUE);

        // DRAM Checksum, if necessary.
#ifdef DO_DRAM_CHECKSUM
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

        // Copy necessary code to SRAM
        x_memcpy((void *) TEMP_IRAM_AREA, (void*)&mt53xx_dram_suspend, mt53xx_dram_suspend_sz);

        Printf("Going to bed...");

        // Flush caches.
        HalFlushDCache();
        HalInvalidateICache();

        // No more DRAM access after this point
        // Jumping to SRAM suspend code, no return.
        (void) ((void (*)(void))TEMP_IRAM_AREA)();
        while (1);
    }

    // Resumed.
    Printf("Coming back, now wakeup HW\n");

    MID_WarmBootRegionProtect(FALSE);

    // DRAM Checksum, if necessary
#ifdef DO_DRAM_CHECKSUM
    uError=0;
    if (uNoChecksum1 == uNoChecksum2)
    {
        Printf("Stack == Checksum area, skip DRAM Checksum check\n");
    }
    else
    {
        i = puChecksum[checknum-1];
        puChecksum[checknum-1] = 0;
        result = CheckSum((void*)(uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE),
                          DRAM_CHECKSUM_BLOCK_SIZE);
        if (result != i)
        {
            uError++;
            Printf("Checksum check error, offset 0x%08x original 0x%08x current 0x%08x\n",
                   uNoChecksum1*DRAM_CHECKSUM_BLOCK_SIZE, i, result);
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
            Printf("DRAM checksum error, offset 0x%08x original 0x%08x current 0x%08x\n",
                   i*DRAM_CHECKSUM_BLOCK_SIZE, puChecksum[idx], result);
        }
        idx++;
        i += DRAM_CHECKSUM_SKIP;
    }
    x_mem_free(puChecksum);

    if (uError)
    {
        // DRAM checksum error, reboot
        Printf("DRAM checksum error %d, reboot!\n", uError);
        for (i=0; i<15; i++)
            Printf("!!!!!!!!!!!!!!!!\n");

        PDWNC_Reboot();
    }
#endif /* DO_DRAM_CHECKSUM */

    x_os_drv_set_timestamp("Start Resume");

    // Enable IRQ.
    HalCriticalEnd(state);

    // Calling all modules' resume function.
    MaybeDisableOutbyte(1);
    _mt53xx_STR_resume();
    MaybeDisableOutbyte(0);

    x_mem_free(puCpuStats);

    Printf("Resume complete, back to work\n");
    x_os_drv_set_timestamp("Resume Complete");
}

#endif /* CC_MTK_LOADER */

