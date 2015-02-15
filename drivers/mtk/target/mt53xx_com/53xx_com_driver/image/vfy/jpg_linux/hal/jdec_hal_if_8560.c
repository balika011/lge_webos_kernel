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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: jdec_hal_if.c $
 *
 * Project:
 * --------
 *   MT8105
 *
 * Description:
 * ------------
 *    Jpeg Decoder HAL
 *
 * Author: C.K. Hu
 * -------
 *
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Modtime: 03/12/18 3:29p $
 *
 * $Revision: #1 $
****************************************************************************/
// *********************************************************************
// Memo
// *********************************************************************
/*

*/
// *********************************************************************
// TODO
// *********************************************************************
/*
*/
#include "drv_config.h"
#include "x_hal_5381.h"
#include "x_ckgen.h"
//#include "x_rtos.h"
#include "x_common.h"
#include "jdec_hal_if.h"
#include "jdec_hal_errcode.h"
#include "jdec_hal.h"
#include "jdec_com_jfif.h"
//#include <string.h>
//#include "x_mmap.h"
#include "x_bim.h"
#include "jdec_drv.h"
#include "jdec_debug.h"
//#include "x_hal_1176.h"
#if PERFORMANCE_TEST
#include "x_timer.h"
#endif
//#include "chip_ver.h"
#include "drv_config.h"
//#include "sys_config.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#else
#include "x_mem_phase2.h"
#endif

#if JDEC_HAL_SUPPORT_MB
#include <asm/system.h>
#endif
#if JDEC_USE_MAP_UNMAP
extern void vJdecMapPrintf(const char *format_string, ...);
extern void vJdecVfyMapBuf(UINT32 i4Address, UINT32 i4Len, UINT32 i4Option);
extern void vJdecVfyUnMapBuf(UINT32 i4Address, UINT32 i4Len, UINT32 i4Option);
#endif


extern UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa);
extern JDEC_INST_T         _rJdecInst[];

#if JDEC_HW_LOG_ENABLE
extern UINT32 _u4JdecHwCmdIdx;
#endif
BOOL fgDisableErrConceal = FALSE;
#define JDEC_SUPPORT_AGENT_ON_OFF 0

// Work around for 16M limitation
#define JDEC_HAL_IF_MAX_PROGRESSIVE_PIC_WIDTH      4096
#define JDEC_HAL_IF_MAX_PROGRESSIVE_PIC_HEIGHT     2048

BOOL _fgJdecHalAllocate = FALSE;
void *_pvJdecHalNoneZeroHistoryBuf[3];

UINT32 _u4JdecHalHwDcHuffTblSa;
UINT32 _u4JdecHalHwAcHuffTblSa;

BOOL _fgJdecHalInitIrq = FALSE;

#if PERFORMANCE_TEST
extern void HAL_GetTime(HAL_TIME_T* pTime);
extern void HAL_GetDeltaTime(HAL_TIME_T * pResult, HAL_TIME_T * pOlder, HAL_TIME_T * pNewer);
HAL_TIME_T _rJdecStratTime, _rJdecEndTime, _rJdecTotalTime,_rJdecTotalTimeOneScan;
#endif
extern INLINE UINT32 u4HwImgReszRead32(UINT32 u4HwId,UINT32 addr);
extern INLINE UINT32 u4HwJdecRead32(UINT32 u4HwId,UINT32 addr);

#if JDEC_IO_MMU_TEST
  extern void vIIOMMU_Init(void);
  extern UINT32 u4HalGetTTB1(void);
#endif

typedef struct {
    JDEC_HAL_DEC_MODE_T eDecodingMode;          ///< Decoding Mode.
    JDEC_HAL_PIC_INFO_T rPicInfo;               ///< Input Jpeg picture information.
    JDEC_JFIF_SOS_T rScanInfo;                  ///< Input Jpeg picture Scan Infomation
    JDEC_HAL_INPUT_INFO_T rInputInfo;           ///< Input fifo information
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo;         ///< Output buffer information
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo1;        ///< Output buffer for preread in first row
    BOOL fgDecoded;                             ///< If decoded after initialization.
    UINT32 u4CurrMCUNum;                        ///< Current MCU Number.
    void *pvNoneZeroHistoryBuf[3];              ///< None Zero History Buffer.  Working buffer for Progressive Scan.
	UINT32 u4NoneZeroHistoryBufSize;//
    UINT32 u4RestartIntervalCnt;                ///< Restart Internal Counter.
    UINT32 u4EOBRUN;                            ///< remaining EOBs in EOBRUN
    BOOL fgEOBRUNSkip;                          ///< Current MCU skip by EOBRUN
    UINT8 au1CompNumOfBlkInMCU[10];             ///< The component number of each block in one MCU.
    BOOL fgStartOfScan;                         ///< Reach Start of Scan
    BOOL fgReachMarker;                         ///< Reach Marker
    BOOL fgReinit;                              ///< Re-initialization flag.
    JDEC_HAL_OUTPUT_ADDR_T rOutputAddr;         ///< Internal count for output address
    JDEC_HAL_OUTPUT_ADDR_T rOutputAddr1;        ///< Color buffer 1 for 8560 preread function for first row
    UINT32 u4OutCoefYBufSa;                     ///< Jpeg output progressive coefficient buffer Y start address
    UINT32 u4OutCoefCbBufSa;                    ///< Jpeg output progressive coefficient buffer Cb start address
    UINT32 u4OutCoefCrBufSa;                    ///< Jpeg output progressive coefficient buffer Cr start address
    JDEC_HAL_HW_STATUS_T rHwStatus;             ///< Hardware status for re-initialization.
    UINT32 u4HwDcHuffTblSa;                 ///< HW format DC Huffman Table
    UINT32 u4HwAcHuffTblSa;                     ///< HW format AC Huffman Table
    UINT32 u4HwId;                              ///< Real HW id
#if JDEC_MM_MODE_ENABLE
    BOOL fgReMcuRowMMLoop;
#endif
} JDEC_HAL_INFO_T;


JDEC_HAL_INFO_T *_prJdecHalInfo[HW_JDEC_VLD_NUM];   // HAL local information for each HW.

#if 1
#define JPEG_MMU_ADDR	JPEG_MMU_BASE
/// Read 32 bits data from GFX MMU HW registers.
#define JPEG_MMU_READ32(offset)           IO_READ32(JPEG_MMU_ADDR, (offset))

/// Write 32 bits data into GFX MMU HW registers.
#define JPEG_MMU_WRITE32(offset, value)   IO_WRITE32(JPEG_MMU_ADDR, (offset), (value))

#define JPEG_REG_RW_IOMMU_CFG0		0x0
#define JPEG_REG_RW_IOMMU_CFG1		0x4
#define JPEG_REG_RW_IOMMU_CFG2		0x8
#define JPEG_REG_RW_IOMMU_CFG3		0xc
#define JPEG_REG_RW_IOMMU_CFG4		0x10
#define JPEG_REG_RW_IOMMU_CFG8         0x20
#define JPEG_REG_RW_IOMMU_CFG9		0x24
#define JPEG_REG_RW_IOMMU_CFGA		0x28
#define JPEG_REG_RW_IOMMU_CFGB		0x2c
#define JPEG_REG_RW_IOMMU_CFGC		0x30
#define JPEG_REG_RW_IOMMU_CFGD		0x34
#endif

static BOOL fgJpgIOMMUISRInited = FALSE;
void JPEG_IOMMU_IrqHandle(UINT16 u2Vector)
{
    if (fgJpgIOMMUISRInited == 0)
    {
        //return;
        UTIL_Printf("RZ IOMMU fgIOMMUISRInited=FALSE \n");
        ASSERT(0);
    }

    if (u2Vector != VECTOR_MMU_JPG)
    {
      	 UTIL_Printf("RZ IOMMU u2Vector false \n");
        ASSERT(0);
    }

    UTIL_Printf("RZ IOMMU page fault occurs\n");
    //clear IOMMU status bit
    JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4) | 0x5);    
    JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4) & ~(0x5));   
    {
           UTIL_Printf("debug jpeg iommu\n");
          UINT32 ui4_tmp0 = 0, ui4_tmp1 = 0;
            /*pa*/
          ui4_tmp0 = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4);
          LOG(0,"CFG4= 0x%08X\n", ui4_tmp0);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x8<<8) ;
          JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4,  ui4_tmp0);
          ui4_tmp1 = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG8);
          LOG(0,"CFG4= 0x%08X,pa,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          /*va*/
          ui4_tmp0 = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x9<<8) ;
          JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, ui4_tmp0);
          ui4_tmp1 = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG8);
          LOG(0,"CFG4= 0x%08X,va,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);  
     }   
     
    //clear BIM status bit
    VERIFY(BIM_ClearIrq(u2Vector));    
    ASSERT(0);
}
#if JDEC_IO_MMU_TEST
#if 0
void  vIOMMU_OverRead_Protection(UINT32 ui4_Agent,
                                  UINT32 ui4_Mode,
                                  UINT32 ui4_StartAddr0,
                                  UINT32 ui4_Buff_sz0,
                                  UINT32 ui4_StartAddr1,
                                  UINT32 ui4_Buff_sz1)
{

	//method 0
	/*
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1); 
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3); 
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFGB, 0x21); 
	*/
	//method 1
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1); 
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGC, (ui4_StartAddr0&0xfffff000)); 
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3); 
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGD, (ui4_StartAddr1&0xfffff000)); 	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGB, 0x10021); 
	
}
#endif

void vJDECSetMMU(void)
{
    UINT32 u4RegVal = 0;
    UINT32 i = 0;
    x_os_isr_fct pfnOldIsr;
   
	//==================================================================
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, 0x0020010A);

	//enable mmu
	u4RegVal = 0xFE|(0x1<<0);
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG0, u4RegVal);     
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG0);   
	UTIL_Printf("[JPG_VFY] MMU CFG0 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG1, HalGetMMUTableAddress());
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG1);   
	UTIL_Printf("[JPG_VFY] MMU CFG1 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG2, 0x0b190715);
	u4RegVal =JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG2);   
	UTIL_Printf("[JPG_VFY] MMU CFG2 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG3, 0x0000001d);
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG3);   
	UTIL_Printf("[JPG_VFY] MMU CFG3 is %x . \n",u4RegVal);	 

   if (fgJpgIOMMUISRInited == 0)
    {
        fgJpgIOMMUISRInited = 1;
        //i4GCPU_HAL_ClrIntr(GCPU_INT_MASK);
        BIM_ClearIrq(VECTOR_MMU_JPG);
        
        if (x_reg_isr(VECTOR_MMU_JPG, JPEG_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
        {
        	  UTIL_Printf("JPG IOMMU x_reg_isr create error \n");	
            ASSERT(0);
        }
    }
 }
#endif
UINT32 u4JdecPow(UINT32 u4Base, UINT32 u4Pow)
{
    UINT32 u4Value = 1;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if (u4Base == 0)
        return 0;
    if (u4Pow == 0)
        return 1;
    while(u4Pow != 0)
    {
        u4Value = u4Value * u4Base;
        u4Pow --;
    }
    return u4Value;
}
UINT32 _JDEC_CLOCK_SETING=6;

void vHwClkVDecSwitchJpeg(void)
{
#if (!CONFIG_DRV_FPGA_BOARD)
#if 0
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    UINT32 u4Temp = CKGEN_READ32(REG_RW_CLK_SEL_0);

    u4Temp = u4Temp & CLK_SEL_0_VDEC_MASK;

    if (u4Temp != CLK_SEL_0_VDEC_DMPLL_1_4)
    {
        CKGEN_WRITE32(REG_RW_CLK_SEL_0,(CKGEN_READ32(REG_RW_CLK_SEL_0) & (~CLK_SEL_0_VDEC_MASK)) | (CLK_SEL_0_VDEC_DMPLL_1_4 << 12));
    }
#else
    
#if JDEC_SUPPORT_AGENT_ON_OFF
    CKGEN_AgtOnClk(e_CLK_PNG);
#endif
    

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    //UTIL_Printf("[jdec] jpeg clock %d\n",_JDEC_CLOCK_SETING);
    CKGEN_AgtSelClk(e_CLK_JPEG , _JDEC_CLOCK_SETING);
#else
    UINT32 u4Value;
    u4Value = CKGEN_AgtGetClk(e_CLK_PNG);

    if(u4Value != CLK_CFG1_PNG_SEL_SYSPLL2_1_2)
        CKGEN_AgtSelClk(e_CLK_PNG , CLK_CFG1_PNG_SEL_SYSPLL2_1_2);
#endif
#endif
#else 
   UINT32 u4SysClock=_JDEC_CLOCK_SETING;//200
   IO_WRITE32(CKGEN_BASE, 0x394, u4SysClock);
   UTIL_Printf("[jdec] Vfy clock[0x%x]\n",_JDEC_CLOCK_SETING);
#endif //#if 0
#endif //#if (!CONFIG_DRV_FPGA_BOARD)
}

BOOL _fgJdecHalResume = FALSE;


/// Initialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Init(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4HwId
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    #if JDEC_HW_LOG_ENABLE
    _u4JdecHwCmdIdx = 0;
    #endif

    // Set clock for Jpeg.
    vHwClkVDecSwitchJpeg();
    
    // Power on VLD
    vHwJdecVLDPowerOn(u4JDecID,TRUE);

    // VLD reset.
    vHwJdecHwReset(u4JDecID);

    // Clear SRAM
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
    vHwJdecSRAMClear(u4JDecID);

    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,TRUE);

    // Adjust this register to correctly decode.
    vHwJdecSetMiscReg(u4JDecID);
#endif

    // Set output to color buffer as raster scan mode.
   // vHwJdecSetReadWriteOutputBufMode(u4JDecID);

    // Set VLD wait time out value.
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
    vHwJdecSetVLDWaitTimeoutValue(u4JDecID,0x3000);
#endif


#if JDEC_IO_MMU_TEST
    vJDECSetMMU();
#endif

    if(!_fgJdecHalAllocate)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
        //_prJdecHalInfo[0] = (JDEC_HAL_INFO_T *)x_alloc_aligned_dma_mem(sizeof(JDEC_HAL_INFO_T),2048);
        _prJdecHalInfo[0] = (JDEC_HAL_INFO_T *)x_mem_alloc(sizeof(JDEC_HAL_INFO_T));
        #else
        _prJdecHalInfo[0] = (JDEC_HAL_INFO_T *)x_alloc_aligned_nc_mem(sizeof(JDEC_HAL_INFO_T),2048);
        #endif
        //_prJdecHalInfo[1] = (JDEC_HAL_INFO_T *)x_alloc_aligned_nc_mem(sizeof(JDEC_HAL_INFO_T),2048);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
#if 1
        _u4JdecHalHwDcHuffTblSa = 0;//(UINT32)x_alloc_aligned_nc_mem(4096,16);
        _u4JdecHalHwAcHuffTblSa = 0;//(UINT32)x_alloc_aligned_nc_mem(4096,16);
#else
        {
            UINT32 u4Addr = (UINT32)g_pvJpegBuffer+0x600000;

            u4Addr = ((u4Addr+15)>>4)<<4;
            _u4JdecHalHwDcHuffTblSa[0] = u4Addr;
            _u4JdecHalHwAcHuffTblSa[0] = u4Addr+4096;

            _u4JdecHalHwDcHuffTblSa[1] = u4Addr+4096*2;
            _u4JdecHalHwAcHuffTblSa[1] = u4Addr+4096*3;
        }
#endif
#endif // CONFIG_DRV_MT8520

        _fgJdecHalAllocate = TRUE;
    }

    x_memset((void*)_prJdecHalInfo[u4JDecID],0,sizeof(JDEC_HAL_INFO_T));

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    _prJdecHalInfo[u4JDecID]->u4HwId = u4HwId;
#else
    _prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa = _u4JdecHalHwDcHuffTblSa;
    _prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa = _u4JdecHalHwAcHuffTblSa;
#endif

    #if PERFORMANCE_TEST
    x_memset(&_rJdecStratTime, 0, sizeof(HAL_TIME_T));
    x_memset(&_rJdecEndTime, 0, sizeof(HAL_TIME_T));
    x_memset(&_rJdecTotalTime, 0, sizeof(HAL_TIME_T));
    x_memset(&_rJdecTotalTimeOneScan, 0, sizeof(HAL_TIME_T));
    #endif

    return S_JDEC_HAL_OK;
}


/// Uninitialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    UINT32 u4Cnt=0;
    #if JDEC_USE_MAP_UNMAP
    UINT32 u4Width,u4Height,u4Tmp;
    #endif
    if(u4JDecID >= HW_JDEC_VLD_NUM)
    {
    	UTIL_Printf("[jdec_hal] invalid u4JdecID\n");
        return E_JDEC_HAL_FAIL;
    }
	i4HwJdecSetVLDSwWritePointer(u4JDecID, TRUE, (_prJdecHalInfo[0]->rInputInfo.u4InFifoSa + _prJdecHalInfo[0]->rInputInfo.u4InFifoSz + 128));

    while (1)
    {
    	if (!(u4HwJdecRead32(u4JDecID,0xF4) & 0x8 ) && !(u4HwJdecRead32(u4JDecID, 0xE8) & 0xFF000000))
    	{
    		break;
    	}
    }   
#if JDEC_IO_MMU_TEST
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8560)
     if(fgRunBelowSegmentBefore(IC_8560_VER_A) == TRUE)
      {
         IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_JPG,  REG_IOMMU_CFG4_RST_TLB); 
         x_thread_delay(2);
         IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_JPG,  REG_IOMMU_CFG4_EN); 
       }
    else if(fgRunBelowSegmentBefore(IC_8560_VER_B) == TRUE)
      {
         IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_JPG,  ((unsigned)0x1 << 31)); 
         while (IOMMU_READ32(REG_RW_IOMMU_CFG4,IOMMU_JPG) & ((unsigned)0x1 << 28));
      }
      else
#endif      
      {
           JPEG_MMU_WRITE32(REG_RW_IOMMU_CFG4, (JPEG_MMU_READ32(REG_RW_IOMMU_CFG4)| 0x80000000));
          while(JPEG_MMU_READ32(REG_RW_IOMMU_CFG8)&((unsigned)0x1 << 29));
      }
#endif

    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,FALSE);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
    // disable interrupt
    vHwJdecSetInterrupt(u4JDecID, FALSE);
#endif

    // Software reset VLD for other module
    vHwJdecHwReset(u4JDecID);  //sun modify for test

    // Power off
    vHwJdecVLDPowerOn(u4JDecID,FALSE);

    //vPngSetClockOff();
#if JDEC_SUPPORT_AGENT_ON_OFF
    CKGEN_AgtOffClk(e_CLK_PNG);
#endif

#if JDEC_USE_MAP_UNMAP
    u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
    u4Width = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
    u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
    u4Height = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
#endif
     UTIL_Printf("_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa:%d\n",_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
    if(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa != 0)
    {
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
	     #if JDEC_USE_MAP_UNMAP
			vJdecMapPrintf("[JDEC_HAL]Unmap from 0x%x to 0x%x point1 \n",_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa, 
						(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa + u4Width * u4Height * 2));
			vJdecVfyUnMapBuf(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa, (u4Width * u4Height * 2), BIDIRECTIONAL);
	     #endif
#endif 	 
	     x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa, TRUE);
    }
    UTIL_Printf("_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa:%d\n",_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
    if(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa != 0)
    {
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
      #if JDEC_USE_MAP_UNMAP
        vJdecMapPrintf("[JDEC_HAL]Unmap from 0x%x to 0x%x point1 \n",_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa, 
                    (_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa + u4Width * u4Height * 2));
        vJdecVfyUnMapBuf(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa, (u4Width * u4Height * 2), BIDIRECTIONAL);
        
        #endif
#endif	 	
        x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa, TRUE);
    }
    UTIL_Printf("_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa:%d\n",_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
    if(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa != 0)
    {
 #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
		#if JDEC_USE_MAP_UNMAP
        vJdecMapPrintf("[JDEC_HAL]Unmap from 0x%x to 0x%x point1 \n",_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa, 
                    (_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa + u4Width * u4Height * 2));
        vJdecVfyUnMapBuf(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa, (u4Width * u4Height * 2), BIDIRECTIONAL);
        #endif
 #endif
        x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa, TRUE);

    }   
    _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = 0;
    _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = 0;
    _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = 0;
     
    {
        // free the non-zero history buffer and huffman table
        if (_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa != 0)
        {
			x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa, FALSE);
            _prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa = 0;
        }
 
        if (_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa != 0)
        {
           x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa, FALSE);           
		   _prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa = 0;
        }
	}
    for(u4Cnt=0; u4Cnt<3; u4Cnt++)
 	{	
        if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt] != NULL)
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
			#if JDEC_USE_MAP_UNMAP
            vJdecMapPrintf("[JDEC_HAL]Unmap from 0x%x to 0x%x point2 \n",(UINT32)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt], 
                    ((UINT32)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt] +  _prJdecHalInfo[u4JDecID]->u4NoneZeroHistoryBufSize));
            vJdecVfyUnMapBuf((UINT32)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt], _prJdecHalInfo[u4JDecID]->u4NoneZeroHistoryBufSize, BIDIRECTIONAL);
            #endif
            #endif
			x_hw_mem_free((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt], TRUE);
        }
      
        _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Cnt] = NULL;
      
    }
     _prJdecHalInfo[u4JDecID]->u4NoneZeroHistoryBufSize =0;
        // free hal info
    if (_fgJdecHalAllocate)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
        //x_free_aligned_dma_mem((void *)_prJdecHalInfo[0]);
        x_mem_free((void *)_prJdecHalInfo[0]);
        #else
        x_free_aligned_nc_mem((void *)_prJdecHalInfo[0]);
        #endif
        _fgJdecHalAllocate = FALSE;
   }
 
    return S_JDEC_HAL_OK;
}


/// Set Jpeg decoding mode
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Decoding_Mode(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_DEC_MODE_T eDecodingMode                   ///< [IN] Jpeg decoding mode
)
{
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    IC_VERSION_T eIcVer;
#endif
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    eIcVer = BSP_GetIcVersion();
    if (eIcVer< IC_VER_C)
    {
        if (JDEC_HAL_DEC_MODE_BASELINE_PIC == eDecodingMode)
            return E_JDEC_HAL_FAIL;
    }

    _prJdecHalInfo[u4JDecID]->eDecodingMode = eDecodingMode;

#else
    _prJdecHalInfo[u4JDecID]->eDecodingMode = eDecodingMode;

    if ((eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
        (eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
        (eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC))
    {
        vHwJdecSetNewBaseline(u4JDecID, TRUE);
    }
    else
    {
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
        vHwJdecSetNewBaseline(u4JDecID, FALSE);
#else
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT ||
           eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT ||
           eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
        {
            vHwJDecSetNewProgressive(u4JDecID, TRUE, FALSE);
        }
 #if JDEC_MM_MODE_ENABLE
	else if( eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS )
	{
	   vHwJDecSetMMpassMode(u4JDecID);
	}
#endif		
        else
        {
            if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
               eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE ||
               eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
                vHwJDecSetNewProgressive(u4JDecID, FALSE, TRUE);
            else
                vHwJDecSetNewProgressive(u4JDecID, FALSE, FALSE);
        }
#endif
#endif
    }
#endif
    return S_JDEC_HAL_OK;
}

/// Set Jpeg input picture information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Pic_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_PIC_INFO_T *prPicInfo                      ///< [IN] Jpeg input picture information
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rPicInfo = *prPicInfo;

// 8550
    if ((_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
    #if JDEC_MM_MODE_ENABLE
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) ||
    #endif 
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
    {
        UINT32 u4Comp;
        UINT32 u4Width, u4Height,u4Tmp;



        u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
        u4Width = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
        u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
        u4Height = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
		_prJdecHalInfo[u4JDecID]->u4NoneZeroHistoryBufSize =(u4Width * u4Height+7)>> 3;
        for(u4Comp=0; u4Comp<3; u4Comp++)
        {
           if(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp]!=0)
           {
             VERIFY(0);
           }
  	       _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp]= (UINT32)x_hw_mem_alloc(_prJdecHalInfo[u4JDecID]->u4NoneZeroHistoryBufSize,2048,TRUE);
  
           if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] == 0)
           {
                VERIFY(FALSE);
                return E_JDEC_HAL_FAIL;
           }
        }
    }
#if JDEC_IO_MMU_TEST
      JDEC_VERY_HAL_LOG("[JDEC_HAL] NoneZero History buffer 0 = %x\n",_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0]);
      JDEC_VERY_HAL_LOG("[JDEC_HAL] NoneZero History buffer 1 = %x\n",_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1]);
      JDEC_VERY_HAL_LOG("[JDEC_HAL] NoneZero History buffer 2 = %x\n",_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2]);
#endif

    {
        UINT32 u4CompId;
        JDEC_HAL_OUTPUT_ADDR_T *prOutputAddr = &(_prJdecHalInfo[u4JDecID]->rOutputAddr);
        JDEC_JFIF_SOF_T *prSofInfo = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo;

        for(u4CompId = 0; u4CompId<prSofInfo->u1NumComponents; u4CompId++)
        {
            UINT32 u4WidthPerMCU = (prSofInfo->arSofComp[0].u1HSampFactor /
                                    prSofInfo->arSofComp[u4CompId].u1HSampFactor) * 8;
            UINT32 u4HeightPerMCU = (prSofInfo->arSofComp[0].u1VSampFactor /
                                     prSofInfo->arSofComp[u4CompId].u1VSampFactor) * 8;

            prOutputAddr->au4MCUWidth[u4CompId] = (prSofInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU;
            prOutputAddr->au4MCUHeight[u4CompId] = (prSofInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU;
        }

        prOutputAddr->u4CombMCUWidth = (prSofInfo->u2ImageWidth + prSofInfo->arSofComp[0].u1HSampFactor * 8 - 1) /
                         (prSofInfo->arSofComp[0].u1HSampFactor * 8);
        prOutputAddr->u4CombMCUHeight = (prSofInfo->u2ImageHeight + prSofInfo->arSofComp[0].u1VSampFactor * 8 - 1) /
                          (prSofInfo->arSofComp[0].u1VSampFactor * 8);
    }

    return S_JDEC_HAL_OK;
}

/// Set Huffman Table information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_HuffmanTable_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_DHT_T *prHuffTblInfo                      ///< [IN] Huffman Table information
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    //x_memcpy(_prJdecHalInfo[u4JDecID]->rPicInfo.prHuffTblInfo,prHuffTblInfo,sizeof(JDEC_JFIF_DHT_T));

    return S_JDEC_HAL_OK;
}


/// Set Jpeg scan information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Scan_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_SOS_T *prScanInfo                         ///< [IN] Input Jpeg scan information
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rScanInfo = *prScanInfo;
    _prJdecHalInfo[u4JDecID]->u4EOBRUN = 0;
    _prJdecHalInfo[u4JDecID]->u4RestartIntervalCnt = 0;
    _prJdecHalInfo[u4JDecID]->fgStartOfScan = TRUE;
    _prJdecHalInfo[u4JDecID]->fgReachMarker = FALSE;

    return S_JDEC_HAL_OK;
}


/// Set Jpeg input information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Input_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_INPUT_INFO_T *prInputInfo                  ///< [IN] Input Jpeg input information
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rInputInfo = *prInputInfo;

    return S_JDEC_HAL_OK;
}


/// Set Jpeg output information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Output_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,          ///< [IN] Input Jpeg output information
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo1 
)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rOutputInfo = *prOutputInfo;
    _prJdecHalInfo[u4JDecID]->rOutputAddr.fgAddrModified = TRUE;
    _prJdecHalInfo[u4JDecID]->rOutputInfo1 = *prOutputInfo1;

    return S_JDEC_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
/// Set Jpeg output information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Burst_Mode(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_SOF_T  *prSofInfo,                ///< [IN] Input Jpeg sof information
    BOOL fgBurstEn                                     ///< [IN] Input burst enable
)
{
    UINT32 u4TmpVal = 0;
	    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    if(fgBurstEn)
     {
        // hsamplfactor(cb)*vsamplfactor(cb) = hsamplfactor(cr)*vsamplfactor(cr)
       u4TmpVal = prSofInfo->arSofComp[1].u1HSampFactor * prSofInfo->arSofComp[1].u1VSampFactor;
       u4TmpVal = prSofInfo->arSofComp[0].u1HSampFactor * prSofInfo->arSofComp[0].u1VSampFactor / u4TmpVal;
   switch(u4TmpVal)
   {
       case 2: //format 422
           vHwJdecSetBurstMode(u4JDecID,TRUE,TRUE);
           break;
        case 4: //format 420
           vHwJdecSetBurstMode(u4JDecID,TRUE,FALSE);
           break;
        default:
           vHwJdecSetBurstMode(u4JDecID,FALSE,FALSE);
           break;
    }
      }
     else
     {
        vHwJdecSetBurstMode(u4JDecID,FALSE,FALSE);
      }
    return S_JDEC_HAL_OK;
}
#endif

/// Trigger Jpeg HAL decoding
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
#include "x_hal_arm.h"
INT32 i4JDEC_HAL_Decode(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    INT32 i4Ret;
    //BOOL fgLastScan = FALSE;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    prJdecHalInfo = _prJdecHalInfo[u4JDecID];
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
       (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
#endif
    {
    // EOBRUN skip
    if((prJdecHalInfo->rScanInfo.u1Ah == 0) && (prJdecHalInfo->rScanInfo.u1Ss != 0) && // AC first
       (prJdecHalInfo->u4EOBRUN != 0) && // EOBRUN
       !((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Not Last Scan
    {
        // 8. Status Update
        i4HwJdecCalculateOutputBufAddr(u4JDecID,prJdecHalInfo->u4CurrMCUNum,
                                                &(prJdecHalInfo->rOutputAddr),
                                                prJdecHalInfo->rPicInfo.prSofInfo,
                                                &(prJdecHalInfo->rScanInfo),
                                                &(prJdecHalInfo->rOutputInfo),
                                                prJdecHalInfo->u4OutCoefYBufSa,
                                                prJdecHalInfo->u4OutCoefCbBufSa,
                                                prJdecHalInfo->u4OutCoefCrBufSa,
                                                prJdecHalInfo->fgReinit,
                                                (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT),
                                                (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE),
                                                prJdecHalInfo->eDecodingMode);
        prJdecHalInfo->u4CurrMCUNum++;
        prJdecHalInfo->u4EOBRUN--;
        prJdecHalInfo->fgEOBRUNSkip = TRUE;
        return S_JDEC_HAL_OK;
    } 
    else
    {
        prJdecHalInfo->fgEOBRUNSkip = FALSE;
    }
    }
    // Initialize setting
    if((!prJdecHalInfo->fgDecoded) || prJdecHalInfo->fgReinit)
    {
        // 0. Buffer clean
        if(!prJdecHalInfo->fgDecoded)
        {
            if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
        #if JDEC_MM_MODE_ENABLE
                || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
	 #endif
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)

                )
            {
                UINT32 u4Comp;
                UINT32 u4Width,u4Height;
                UINT32 u4Tmp;

                u4Tmp = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
                u4Width = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
                u4Tmp = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
                u4Height = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;

                if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
              #if JDEC_MM_MODE_ENABLE
                  (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)||
	       #endif 
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
                {
                    JDEC_VERY_HAL_LOG("[JDEC_HAL] init nonzero buffer\n");
                    // Clean none-zero history buffer
	                for(u4Comp=0; u4Comp<3; u4Comp++)
	                {
	                    x_memset((void *)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],0,
	                                               prJdecHalInfo->u4NoneZeroHistoryBufSize);
	                    #if JDEC_USE_MAP_UNMAP
	                    vJdecMapPrintf("[JDEC_HAL] nonzero Map from 0x%x to 0x%x  point 1 \n",(UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],
	                                ((UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp] + prJdecHalInfo->u4NoneZeroHistoryBufSize));
	                    vJdecVfyMapBuf((UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp], prJdecHalInfo->u4NoneZeroHistoryBufSize, TO_DEVICE);
	                    #else
	                    BSP_FlushDCacheRange( (UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],
	                                           prJdecHalInfo->u4NoneZeroHistoryBufSize );
	                    #endif
	                }

                }

                if(prJdecHalInfo->u4OutCoefYBufSa != 0)
                     x_hw_mem_free((void *)prJdecHalInfo->u4OutCoefYBufSa, TRUE);
                if(prJdecHalInfo->u4OutCoefCbBufSa != 0)
                     x_hw_mem_free((void *)prJdecHalInfo->u4OutCoefCbBufSa, TRUE);
                if(prJdecHalInfo->u4OutCoefCrBufSa != 0)
                      x_hw_mem_free((void *)prJdecHalInfo->u4OutCoefCbBufSa, TRUE);

                if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
 #if JDEC_MM_MODE_ENABLE
                    || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
 #endif
                    ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
                    ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)

                   )
                {
                    if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                    {
                        UTIL_Printf("coeff allcate\n");
                        prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);
                   }
                   else
#if JDEC_MM_MODE_ENABLE
     			    if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
     	            {
     	                prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_hw_mem_alloc( u4Width*u4Tmp*2*prJdecHalInfo->rOutputInfo.ui4McuRowNumber,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_hw_mem_alloc( u4Width*u4Tmp*2*prJdecHalInfo->rOutputInfo.ui4McuRowNumber,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_hw_mem_alloc( u4Width*u4Tmp*2*prJdecHalInfo->rOutputInfo.ui4McuRowNumber,2048,TRUE);
     	           }
		           else
#endif
                   {
                        prJdecHalInfo->u4OutCoefYBufSa =  (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);
     	                prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_hw_mem_alloc(u4Width*u4Height*2,2048,TRUE);                 

                    }
        #if JDEC_IO_MMU_TEST
                    JDEC_VERY_HAL_LOG("[JDEC_HAL] Y Coff SA = %x\n",prJdecHalInfo->u4OutCoefYBufSa);
                    JDEC_VERY_HAL_LOG("[JDEC_HAL] CB Coff SA = %x\n",prJdecHalInfo->u4OutCoefCbBufSa);
                    JDEC_VERY_HAL_LOG("[JDEC_HAL] CR Coff SA = %x\n",prJdecHalInfo->u4OutCoefCrBufSa);
        #endif
                    VERIFY(prJdecHalInfo->u4OutCoefYBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCbBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCrBufSa  != 0);
#if JDEC_MM_MODE_ENABLE
                    if (prJdecHalInfo->eDecodingMode ==JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
                    {
                        // Clean coefficient buffer
                        if(!_fgJdecHalResume)
                        {
                            x_memset((void *)prJdecHalInfo->u4OutCoefYBufSa,0, u4Width * u4Tmp * 2);                            
                            
	                        #if JDEC_USE_MAP_UNMAP
	                        BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Tmp * 2,BIDIRECTIONAL);
	                        #else
	                        BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Tmp * 2);
	                        #endif
                        }
    
                        if(!_fgJdecHalResume)
                        {
                            x_memset((void *)prJdecHalInfo->u4OutCoefCbBufSa,0,u4Width * u4Tmp * 2);
                          #if JDEC_USE_MAP_UNMAP
                            BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefCbBufSa, u4Width * u4Tmp * 2,BIDIRECTIONAL);
                          #else                          
                            BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefCbBufSa,u4Width * u4Tmp * 2);
                          #endif  
                        }
    
                        if(!_fgJdecHalResume)
                        {
                            x_memset((void *)prJdecHalInfo->u4OutCoefCrBufSa,0,u4Width * u4Tmp * 2);
                          #if JDEC_USE_MAP_UNMAP
                            BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Tmp * 2,BIDIRECTIONAL);
                          #else                            
                            BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Tmp * 2);
                          #endif  
                        }
                    }
					else
#endif
					{
	                    // Clean coefficient buffer
	                    if(!_fgJdecHalResume)
	                    {
	                        x_memset((void *)prJdecHalInfo->u4OutCoefYBufSa,0,u4Width * u4Height * 2);
	                        #if JDEC_USE_MAP_UNMAP
	                        vJdecMapPrintf("[JDEC_HAL] Map from 0x%x to 0x%x y point 2 \n",prJdecHalInfo->u4OutCoefYBufSa,
	                                     (prJdecHalInfo->u4OutCoefYBufSa + u4Width * u4Height * 2));
	                        //BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Height * 2,BIDIRECTIONAL);
	                        vJdecVfyMapBuf(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Height * 2, TO_DEVICE);
                                //HalFlushDCacheMultipleLine(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Height * 2);
	                        #else
	                        BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefYBufSa, u4Width * u4Height * 2);
	                        #endif
	                    }

	                    if(!_fgJdecHalResume)
	                    {
	                        x_memset((void *)prJdecHalInfo->u4OutCoefCbBufSa,0,   u4Width * u4Height * 2);
	                        #if JDEC_USE_MAP_UNMAP
	                        vJdecMapPrintf("[JDEC_HAL] Map from 0x%x to 0x%x  Cb point 2 \n",prJdecHalInfo->u4OutCoefCbBufSa,
	                                     (prJdecHalInfo->u4OutCoefCbBufSa + u4Width * u4Height * 2));
	                      //  BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefCbBufSa, u4Width * u4Height * 2,BIDIRECTIONAL);
                                 //HalFlushDCacheMultipleLine(prJdecHalInfo->u4OutCoefCbBufSa, u4Width * u4Height * 2);                                 
                                vJdecVfyMapBuf(prJdecHalInfo->u4OutCoefCbBufSa, u4Width * u4Height * 2, TO_DEVICE);
	                        #else
	                        BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefCbBufSa,u4Width * u4Height * 2);
	                        #endif
	                    }

	                    if(!_fgJdecHalResume)
	                    {
	                        x_memset((void *)prJdecHalInfo->u4OutCoefCrBufSa,0, u4Width * u4Height * 2);
	                        #if JDEC_USE_MAP_UNMAP
	                        vJdecMapPrintf("[JDEC_HAL] Map from 0x%x to 0x%x Cr point 2 \n",prJdecHalInfo->u4OutCoefCrBufSa,
	                                     (prJdecHalInfo->u4OutCoefCrBufSa + u4Width * u4Height * 2));
	                        //BSP_dma_map_vaddr(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Height * 2,BIDIRECTIONAL);
                                 //HalFlushDCacheMultipleLine(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Height * 2);                                 
                                vJdecVfyMapBuf(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Height * 2, TO_DEVICE);
	                        #else
	                        BSP_FlushDCacheRange(prJdecHalInfo->u4OutCoefCrBufSa, u4Width * u4Height * 2);
	                        #endif
	                    }
                   }
                }
            }

            if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC) ||
               (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
#if JDEC_MM_MODE_ENABLE
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
#endif   
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
#endif
                    )
            {
                // enable interrupt
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }

        }

        // 1. Set Decodeing Mode to HW
        vHwJdecSetDecodeMode(u4JDecID,prJdecHalInfo->eDecodingMode);

        // 2. Set Picture Information to HW
        // Set picture information
        switch(prJdecHalInfo->rPicInfo.prSofInfo->eJpegFormat)
        {
            case E_JPG_BASELINE:
            case E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN:
                vHwJdecSetDecodeModeBaseline(u4JDecID);
                break;
            case E_JPG_PROGRESSIVE_HUFFMAN:
                vHwJdecSetDecodeModeProgressive(u4JDecID);
                break;
            default:
                return E_JDEC_HAL_FAIL;
        }

        switch(prJdecHalInfo->eDecodingMode)
        {
            case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
            case JDEC_HAL_DEC_MODE_BASELINE_PIC:
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
            case JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT:
#endif

                // Set Picture Size
                vHwJdecSetPictureSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo);

                // Restart Interval Processing.
                if(prJdecHalInfo->rPicInfo.u4RestartInterval>0)
                {
                    vHwJdecSetRestartInterval(u4JDecID,prJdecHalInfo->rPicInfo.u4RestartInterval);
                }
                break;
            default:
                break;
        }


        // Set Quantization Table
        if((i4Ret = i4HwJdecSetQuantTable(u4JDecID,prJdecHalInfo->rPicInfo.prQTblInfo)) < 0)
            return i4Ret;

        // Initialize Restart Interval Counter
        if(prJdecHalInfo->rPicInfo.u4RestartInterval>0)
        {
            prJdecHalInfo->u4RestartIntervalCnt = prJdecHalInfo->rPicInfo.u4RestartInterval;
        }

        //JDEC_VERY_HAL_LOG("[JDEC_DRV] HAL Input buffer SA= %x \n",prJdecHalInfo->rInputInfo.u4InFifoSa);
        // 4. Set Input Buffer Information to HW
        vHwJdecSetInputBuffer(u4JDecID,prJdecHalInfo->rInputInfo.u4InFifoSa,prJdecHalInfo->rInputInfo.u4InFifoSz);

        //JDEC_VERY_HAL_LOG("[JDEC_DRV] HAL rd point %x \n",prJdecHalInfo->rInputInfo.u4RdPoint);
        if(!prJdecHalInfo->fgReinit)
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rInputInfo.u4RdPoint,0); // Set VLD read pointer to bitstream start address.
        else
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rHwStatus.u4ReadAddrBytes,
                                                  prJdecHalInfo->rHwStatus.u4ReadAddrBits);
        if (i4Ret < 0)
        {
        	UTIL_Printf("[jdec_hal] set 4HwJdecSetBarrelShifterAddr failed\n");
            return i4Ret;
        }
#if 0
        i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,prJdecHalInfo->rInputInfo.u4WrPoint);
#else 
		// Add by pingzhao ,temp solution,maybe other mode should set write pointer also
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
    #if JDEC_MM_MODE_ENABLE
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
	#endif   
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
#endif
            )
        {
            i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,prJdecHalInfo->rInputInfo.u4WrPoint);
            //JDEC_VERY_HAL_LOG("[JDEC_DRV]HAL set write pointer %x \n",prJdecHalInfo->rInputInfo.u4WrPoint);
        }
#endif
        // 5. Set Output Buffer Information to HW

        /*The last parameter is for last scan, if enhance mode, each scan is last scan,
          and the cofficient buffer pitch should equal with color buffer pitch, for multi-collect
          mode, the previous scan's cofficient buffer pitch should equal with color buffer picth * 2 ,and
          the last scan's cofficient buffer pitch shoule equal with color buffer pitch*/
#if JDEC_MM_MODE_ENABLE
		if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
        {
          i4HwJdecSetMMPassPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                        &(prJdecHalInfo->rScanInfo),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE));
        }
		else
#endif		
		{
        i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                        &(prJdecHalInfo->rScanInfo),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
                                         prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE));
		}

        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
         #if JDEC_MM_MODE_ENABLE
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
         #endif
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
           )
            i4HwJdecSetCoefPitchPerRow(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo);

    }
#if JDEC_MM_MODE_ENABLE
   if(prJdecHalInfo->fgReMcuRowMMLoop && prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
   	{

        UINT32 u4Comp=0;
       // Clean none-zero history buffer
#if 1
	    vHwJdecSetInterrupt(u4JDecID, TRUE);
        vHwJdecSetDecodeMode(u4JDecID,prJdecHalInfo->eDecodingMode);

        // 2. Set Picture Information to HW
        // Set picture information
        switch(prJdecHalInfo->rPicInfo.prSofInfo->eJpegFormat)
        {
            case E_JPG_BASELINE:
            case E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN:
                vHwJdecSetDecodeModeBaseline(u4JDecID);
                break;
            case E_JPG_PROGRESSIVE_HUFFMAN:
                vHwJdecSetDecodeModeProgressive(u4JDecID);
                break;
            default:
                return E_JDEC_HAL_FAIL;
        }
		    if((i4Ret = i4HwJdecSetQuantTable(u4JDecID,prJdecHalInfo->rPicInfo.prQTblInfo)) < 0)
            return i4Ret;

        // Initialize Restart Interval Counter
        if(prJdecHalInfo->rPicInfo.u4RestartInterval>0)
        {
            prJdecHalInfo->u4RestartIntervalCnt = prJdecHalInfo->rPicInfo.u4RestartInterval;
        }

        //JDEC_VERY_HAL_LOG("[JDEC_DRV] HAL Input buffer SA= %x \n",prJdecHalInfo->rInputInfo.u4InFifoSa);
        // 4. Set Input Buffer Information to HW
        vHwJdecSetInputBuffer(u4JDecID,prJdecHalInfo->rInputInfo.u4InFifoSa,prJdecHalInfo->rInputInfo.u4InFifoSz);

        //JDEC_VERY_HAL_LOG("[JDEC_DRV] HAL rd point %x \n",prJdecHalInfo->rInputInfo.u4RdPoint);
        if(!prJdecHalInfo->fgReinit)
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rInputInfo.u4RdPoint,0); // Set VLD read pointer to bitstream start address.
        else
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rHwStatus.u4ReadAddrBytes,
                                                  prJdecHalInfo->rHwStatus.u4ReadAddrBits);
        if (i4Ret < 0)
        {
        	UTIL_Printf("[jdec_hal] set 4HwJdecSetBarrelShifterAddr failed\n");
            return i4Ret;
        }
		    i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,prJdecHalInfo->rInputInfo.u4WrPoint);
#endif  
        JDEC_VERY_HAL_LOG("[JDEC_HAL] Clean nonzero buffer\n");
       for(u4Comp=0; u4Comp<3; u4Comp++)
       {
              x_memset((void *)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],0,
                               prJdecHalInfo->u4NoneZeroHistoryBufSize);
#if JDEC_USE_MAP_UNMAP
             BSP_dma_map_vaddr((UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp], 
                               prJdecHalInfo->u4NoneZeroHistoryBufSize,
                               BIDIRECTIONAL);
#else                  
              BSP_FlushDCacheRange( (UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],
                          prJdecHalInfo->u4NoneZeroHistoryBufSize);
#endif                         
       }       

      i4HwJdecSetMMPassPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                   prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                   prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                   &(prJdecHalInfo->rScanInfo),
                                   (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
                                   prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
                                   prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE));

	  i4HwJdecSetCoefPitchPerRow(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo);
	  prJdecHalInfo->fgReMcuRowMMLoop =FALSE;

   	}
#endif
    if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
        vHwJdecForNewEnhance(u4JDecID, TRUE, TRUE);

    // Scan info update
    if(prJdecHalInfo->fgStartOfScan || prJdecHalInfo->fgReinit)
    {
        // Initialize VLD marker_found and encounter_marker register
        vHwJdecInitScan(u4JDecID);


        // 2. Set Picture Information to HW
        if((i4Ret = i4HwJdecSetHuffmanTable(u4JDecID,prJdecHalInfo->rPicInfo.prHuffTblInfo,
                                           (UINT8 *)(prJdecHalInfo->u4HwDcHuffTblSa),
                                           (UINT8 *)(prJdecHalInfo->u4HwAcHuffTblSa))) < 0)
            return i4Ret;

        i4HwJdecSetCurrUsedQuantTable(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo,&(prJdecHalInfo->rScanInfo));


        // 3. Set Scan Information to HW
        // Set Ah, Al, Ss, Se information
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
#endif            
            )
        {
            vHwJdecSetAhAl(u4JDecID,0,0);
        }
        else
        {
            vHwJdecSetAhAl(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Al);
        }

        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
         #if JDEC_MM_MODE_ENABLE
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
	#endif    
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)

           )

        {
            vHwJdecSetDcAc(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Ss);
        }
        
        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
         #if JDEC_MM_MODE_ENABLE
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
         #endif    
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
           )

        {
            if ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))
                vHwJdecSetLastScan(u4JDecID,TRUE);
            else
                vHwJdecSetLastScan(u4JDecID,FALSE);
        } else
        {
            vHwJdecSetLastScan(u4JDecID,TRUE);
        }

        // Set DC Huffman List of Each block in MCU
        // Set AC Huffman List of Each block in MCU
        i4HwJdecSetBlkAndDcAcTblList(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,
                                              &(prJdecHalInfo->rScanInfo));
        // Set Component number of Each block in MCU
        i4HwJdecSetBlkAndCompMembership(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,
                                                 &(prJdecHalInfo->rScanInfo));
        i4HwJdecSetDcAcNeededList(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));

        // Set Block Position in MCU
        i4HwJdecSetBlockPostition(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo,&(prJdecHalInfo->rScanInfo));

        i4HwJdecSetBlockCount(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));

        // 5. Set Output Buffer Information to HW
        if (((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))// Last scan
        || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) //for enhance every scan is last scan
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)

        )
        {
     #if JDEC_MM_MODE_ENABLE  
    		if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
            {           
                 i4HwJdecSetMMPassPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                            prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                            prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                            &(prJdecHalInfo->rScanInfo),
                                            TRUE);
			}
    		else
		#endif		
            {
                 i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                                 prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                                 prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                                 &(prJdecHalInfo->rScanInfo),
                                                 TRUE, TRUE);
            }
                
        }

        // 6. HW related setting
        if(
#if JDEC_MM_MODE_ENABLE
           ( prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) ||
#endif
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           //|| (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
           //|| (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
           )

        {
            vHwJdecSetNoneZeroHistoryBuf(u4JDecID,(UINT32)(prJdecHalInfo->pvNoneZeroHistoryBuf[prJdecHalInfo->rScanInfo.au1CompIdx[0]]));
        }

        if(!prJdecHalInfo->fgReinit)
            prJdecHalInfo->u4CurrMCUNum = 0;
        vHwJdecCleanInterBuf(u4JDecID,prJdecHalInfo->fgStartOfScan);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

  #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
 #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
#endif   
           )
        {
           i4HwJdecSetCoefMCUSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
        }
   #endif

        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
#if JDEC_MM_MODE_ENABLE
           ( prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) ||
#endif         
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            i4HwJdecSetCoefMCUSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
            i4HwJdecSetCoefPitch(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
            vHwJdecSetPictureSizeForProg(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, &(prJdecHalInfo->rScanInfo));
        }

        //7. Set Coefficient buffer
        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT ||
#if JDEC_MM_MODE_ENABLE
           ( prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) ||
#endif		
           //prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
           prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
           //|| prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE
          )
        {
            vHwJdecSetOutputCoefBufAddr(u4JDecID,prJdecHalInfo->u4OutCoefYBufSa,
                                                 prJdecHalInfo->u4OutCoefCbBufSa,
                                                 prJdecHalInfo->u4OutCoefCrBufSa,
                                                 &(prJdecHalInfo->rScanInfo));
        }
        //8.Set Bank0/1 For mcu-row mode in prog

        if(prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT &&
           prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
        prJdecHalInfo->fgStartOfScan = FALSE;
#else
       // if((prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) &&
       //    (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
        prJdecHalInfo->fgStartOfScan = FALSE;
#endif
    }
    else
    {
        vHwJdecCleanInterBuf(u4JDecID,prJdecHalInfo->fgStartOfScan);
    }

    // 2. Picture Information
    // Restart Interval Processing.
    if(prJdecHalInfo->fgDecoded && (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW))
    {
        if(prJdecHalInfo->rPicInfo.u4RestartInterval>0)
        {
            prJdecHalInfo->u4RestartIntervalCnt--;
            if(prJdecHalInfo->u4RestartIntervalCnt == 0)
            {
                i4HwJdecProcRestart(u4JDecID);
                prJdecHalInfo->u4EOBRUN = 0;
                prJdecHalInfo->u4RestartIntervalCnt = prJdecHalInfo->rPicInfo.u4RestartInterval;
            }
        }
    }

    // 3. Set Scan Information to HW
    // Set Ah, Al, Ss, Se information
    if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)||
        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU)
   #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
        ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
   #endif           
        )
    {
        vHwJdecSetSsSe(u4JDecID,0,0);
    }
    else
    {
        if((prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) &&
           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) &&
           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) &&
 #if JDEC_MM_MODE_ENABLE
           ( prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) &&
#endif     
           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            if((prJdecHalInfo->rScanInfo.u1Ah == 0) && (prJdecHalInfo->rScanInfo.u1Ss != 0) && // AC first
               (prJdecHalInfo->u4EOBRUN != 0) && // EOBRUN
               ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Last Scan
                    vHwJdecSetSsSe(u4JDecID,64,63);
            else //sun modified
             vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
        }
        else
           vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
    }

    // 5. Set Output Buffer Information to HW
    {
        BOOL fgLastScan = FALSE;

        if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
            #if JDEC_MM_MODE_ENABLE
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
            #endif
            
#endif
            )
        {
            if ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))
            {
                fgLastScan = TRUE;
            }
            else
            {
                fgLastScan = FALSE;
            }
        }
        else
        {
            fgLastScan = TRUE;
        }
        //vJdecLog_L("[JDEC_VERIFY] Last scan =0x%x\n", fgLastScan);
        i4HwJdecCalculateOutputBufAddr(u4JDecID,prJdecHalInfo->u4CurrMCUNum,
                                                &(prJdecHalInfo->rOutputAddr),
                                                prJdecHalInfo->rPicInfo.prSofInfo,
                                                &(prJdecHalInfo->rScanInfo),
                                                &(prJdecHalInfo->rOutputInfo),
                                                prJdecHalInfo->u4OutCoefYBufSa,
                                                prJdecHalInfo->u4OutCoefCbBufSa,
                                                prJdecHalInfo->u4OutCoefCrBufSa,
                                                prJdecHalInfo->fgReinit,
                                                TRUE,
                                                fgLastScan,
                                                prJdecHalInfo->eDecodingMode);

     {
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) && // Multi-collect mode
            !((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Not Last Scan
        {
 #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
            // I don't know why must set it,
            // But if not set, it will write out to this address.
            vHwJdecSetOutputBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YCoeffBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CbCoeffBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CrCoeffBufSa,
                                             &(prJdecHalInfo->rScanInfo));
 #endif
        } else
        {
            vHwJdecSetOutputBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                             &(prJdecHalInfo->rScanInfo));
        }

        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
      || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#endif
          )
        {
            vHwJdecSetOutputCoefBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YCoeffBufSa,
                                                 prJdecHalInfo->rOutputAddr.u4CbCoeffBufSa,
                                                 prJdecHalInfo->rOutputAddr.u4CrCoeffBufSa,
                                                 &(prJdecHalInfo->rScanInfo));
        }
        }
    }

    switch(prJdecHalInfo->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
         {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
           if(prJdecHalInfo->rOutputAddr.u4MCUy == 0) //first row
           {
             vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CrBufSa);
             #if CONFIG_DRV_VERIFY_SUPPORT
             if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
             {
                prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch*8;
                prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch*8;
                prJdecHalInfo->rOutputAddr.u4CrBufSa +=  prJdecHalInfo->rOutputInfo.u4OutCrBufPitch*8;
             } else
             {
                prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch
                                                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8);
                prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch
                                                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor * 8);
                prJdecHalInfo->rOutputAddr.u4CrBufSa += prJdecHalInfo->rOutputInfo.u4OutCrBufPitch
                                                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor * 8);
             }
             
             vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                      prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                      prJdecHalInfo->rOutputAddr.u4CrBufSa);
             #else         
             vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo1.u4OutYBufSa,
                                                      prJdecHalInfo->rOutputInfo1.u4OutCbBufSa,
                                                      prJdecHalInfo->rOutputInfo1.u4OutCrBufSa);
             #endif
            if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
             {
                prJdecHalInfo->rOutputAddr.u4MCUy++;
             }
         }
         else // not the first row
         {
            if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
            {
                
             vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CrBufSa);
            }
            else
            {
             vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                   prJdecHalInfo->rOutputAddr.u4CrBufSa);
        }
        }
#else
           vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                prJdecHalInfo->rOutputAddr.u4CrBufSa);
           vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                prJdecHalInfo->rOutputAddr.u4CrBufSa);
#endif
           vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                       (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                       (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
          }
        break;
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
            {
                vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
            }
        break;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
        case JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT:
            {
                vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                i4HwJdecSetVideoOutMode(u4JDecID,TRUE,TRUE,prJdecHalInfo->rPicInfo.prSofInfo);
                vHwJdecSetPicturePixelSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->u2ImageWidth);
               
            }            
        break;
#endif
        
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
  #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
        {
           if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
           {
             switch(prJdecHalInfo->rScanInfo.au1CompIdx[0])
             {
              case 0:
              if( prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
               {
                  vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                        prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                        prJdecHalInfo->rOutputAddr.u4YBufSa);

                  prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch*8;

                  vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                                  prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                                  prJdecHalInfo->rOutputAddr.u4YBufSa);
                  if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
                  {
                    prJdecHalInfo->rOutputAddr.u4MCUy++;
                  }
              }else  // not the first row
              {
                 if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
                 vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                       prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                       prJdecHalInfo->rOutputAddr.u4YBufSa);
                 else
                 vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                       prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                       prJdecHalInfo->rOutputAddr.u4YBufSa);
             }
            break;
            case 1:
            if( prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
            {
               vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CbBufSa);

               prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch*8;

               vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CbBufSa);
              if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
              {
                  prJdecHalInfo->rOutputAddr.u4MCUy++;
              }
            }else  // not the first row
            {
              if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
              vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                    prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                    prJdecHalInfo->rOutputAddr.u4CbBufSa);
              else
              vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                    prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                    prJdecHalInfo->rOutputAddr.u4CbBufSa);
             }
            break;
            case 2:
            if(prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
            {
               vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CrBufSa);

               prJdecHalInfo->rOutputAddr.u4CrBufSa +=  prJdecHalInfo->rOutputInfo.u4OutCrBufPitch*8;

               vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                     prJdecHalInfo->rOutputAddr.u4CrBufSa);
              if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
              {
                prJdecHalInfo->rOutputAddr.u4MCUy++;
              }
            }else  // not the first row
            {
                if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
                 vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                         prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                         prJdecHalInfo->rOutputAddr.u4CrBufSa);
                else
                 vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                           prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                           prJdecHalInfo->rOutputAddr.u4CrBufSa);
             }
                        break;
                    }
                    vHwJdecSetMCUWidth(u4JDecID,8,
                                                8,
                                                8);
                }
                else
                {
                   if( prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
                    {
                         vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                               prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                               prJdecHalInfo->rOutputAddr.u4CrBufSa);

                         prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch
                                                                            *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8);
                         prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch
                                                                            *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor * 8);
                         prJdecHalInfo->rOutputAddr.u4CrBufSa += prJdecHalInfo->rOutputInfo.u4OutCrBufPitch
                                                                            *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor * 8);

                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa);

                        if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
                         {
                            prJdecHalInfo->rOutputAddr.u4MCUy++;
                         }
                    }
                   else // not the first row
                   {
                      if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
                      vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                            prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                            prJdecHalInfo->rOutputAddr.u4CrBufSa);
                      else
                      vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                           prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                           prJdecHalInfo->rOutputAddr.u4CrBufSa);
                    }
                    vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                }
            }
            break;
  #else
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            {
                if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
                {
                    switch(prJdecHalInfo->rScanInfo.au1CompIdx[0])
                    {
                        case 0:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                             prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                             prJdecHalInfo->rOutputAddr.u4YBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                             prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                             prJdecHalInfo->rOutputAddr.u4YBufSa);
                        break;
                        case 1:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa);
                        break;
                        case 2:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa);
                        break;

                    }
                    vHwJdecSetMCUWidth(u4JDecID,8,
                                                8,
                                                8);
                }
                else
                {
                    vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CrBufSa);
                    vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CrBufSa);
                    vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                }
            }
            break;
#endif
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        {
                if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
                {
                    switch(prJdecHalInfo->rScanInfo.au1CompIdx[0])
                    {
                        case 0:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa);
                        break;
                        case 1:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa);
                        break;
                        case 2:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                        break;

                    }
                    vHwJdecSetMCUWidth(u4JDecID,8,
                                                8,
                                                8);
                }
                else
                {
                    vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                    vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                    vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                }
            }
            break;
#if JDEC_MM_MODE_ENABLE
		case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS:
	
        {
			    UINT32 ui4_row_start =prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
				UINT32 ui4_row_end =prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
				BOOL fgLastMCUROW = FALSE;
				UINT32 ui4_remain_row = prJdecHalInfo->rOutputInfo.ui4McuRowNumber;
			    vJdecLog_L("Enter %s, rScanInfo.u1CompInScan     = 0x%x\n", __FUNCTION__,prJdecHalInfo->rScanInfo.u1CompInScan);                       
				if(prJdecHalInfo->rOutputInfo.u4DecodedMCURow >= prJdecHalInfo->rOutputInfo.u4ToalMCURow - prJdecHalInfo->rOutputInfo.ui4McuRowNumber)
				{
					fgLastMCUROW = TRUE;
					vJdecLog_L("Enter %s, meet last mcu ROW\n", __FUNCTION__);
					ui4_remain_row = prJdecHalInfo->rOutputInfo.u4ToalMCURow - prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
	            }

				if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
                {
                    UINT8 u1ComIndex = prJdecHalInfo->rScanInfo.au1CompIdx[0];   
                    vJdecLog_L("Enter %s, comp index[0x%x] u1VSampFactor[0x%x]\n", __FUNCTION__,
                                                                  u1ComIndex,
                                                                  prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[u1ComIndex].u1VSampFactor);
                           				       
                     ui4_row_start = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[u1ComIndex].u1VSampFactor *prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
                     if(fgLastMCUROW)
                     {                      
						
					    ui4_row_end =ui4_row_start 
							          +(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[u1ComIndex].u1VSampFactor) * (ui4_remain_row-1)
						              + prJdecHalInfo->rPicInfo.prSofInfo->au4ImgLastMCUHeight[u1ComIndex]-1;
					    
                     }
					 else
					 {
					    ui4_row_end =ui4_row_start -1 +
							 (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[u1ComIndex].u1VSampFactor) * prJdecHalInfo->rOutputInfo.ui4McuRowNumber;
						   
					 }					
					 switch(u1ComIndex)
                     {
                        case 0:
					    //UTIL_Printf("Enter %s, rScanInfo.au1CompId[0]     = 0x%x\n", __FUNCTION__,prJdecHalInfo->rScanInfo.au1CompIdx[0]);
				        //UTIL_Printf("Enter %s, arSofComp[0].u1VSampFactor = 0x%x\n", __FUNCTION__,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor);		
					    //ui4_row_start = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor *prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
                        //ui4_row_end =ui4_row_start + prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor-1;                 
						vHwJdecSeMCURowNum(u4JDecID,ui4_row_start, ui4_row_end);
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4YBufSa);
                        break;
                        case 1:
					    //UTIL_Printf("Enter %s, arSofComp[1].u1VSampFactor = 0x%x\n", __FUNCTION__,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor);
						//ui4_row_start = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor *prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
					    //ui4_row_end =ui4_row_start + prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor-1;
                 
						vHwJdecSeMCURowNum(u4JDecID,ui4_row_start, ui4_row_end);
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CbBufSa);
					    break;
                        case 2:
					    //UTIL_Printf("Enter %s, rScanInfo.au1CompId[1]     = 0x%x\n", __FUNCTION__,prJdecHalInfo->rScanInfo.au1CompIdx[2]);
				        //UTIL_Printf("Enter %s, arSofComp[1].u1VSampFactor = 0x%x\n", __FUNCTION__,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor);
					    //ui4_row_start = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor *prJdecHalInfo->rOutputInfo.u4DecodedMCURow;
					    //ui4_row_end =ui4_row_start + prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor-1;                 
						vHwJdecSeMCURowNum(u4JDecID,ui4_row_start, ui4_row_end);
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                                              prJdecHalInfo->rOutputAddr.u4CrBufSa);
                        break;

                    }
                    vHwJdecSetMCUWidth(u4JDecID,8,
                                                8,
                                                8);
                }
                else
                {

				    ui4_row_end = ui4_row_start+ui4_remain_row-1; 
                    vHwJdecSeMCURowNum(u4JDecID,ui4_row_start, ui4_row_end);
                    vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                                          prJdecHalInfo->rOutputAddr.u4CrBufSa);
				
                    vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                }
            }	
         	break;
	#endif		
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
            {
                if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
                {
                    switch(prJdecHalInfo->rScanInfo.au1CompIdx[0])
                    {
                        case 0:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa);
                        break;
                        case 1:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa);
                        break;
                        case 2:
                        vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                        vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
                                                              prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                        break;

                    }
                    vHwJdecSetMCUWidth(u4JDecID,8,
                                                8,
                                                8);
                }
                else
                {
                    vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                    vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                          prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                    vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
                                                (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
                }
            }
            break;
#endif
        default:
            break;
    }

    // 6. HW related setting
    //vHwJdecCleanInterBuf(u4JDecID,prJdecHalInfo->fgStartOfScan);
    if(prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT &&
       prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
    {
      vHwJdecSetCurrMCUNum(u4JDecID,prJdecHalInfo->u4CurrMCUNum);
      i4HwJdecSetEOBRUN(u4JDecID,prJdecHalInfo->u4EOBRUN);

    }

    vHwJdecSetOutputNoneZeroHistoryBufOnly(u4JDecID,FALSE);

    if(prJdecHalInfo->fgReinit)
        i4HwJdecSetHwStatus(u4JDecID,&(prJdecHalInfo->rHwStatus));

    if(fgDisableErrConceal)
    {
       vJDECErrorConceal(u4JDecID, FALSE, FALSE, FALSE);
      // UTIL_Printf("jdec disable error conceal\n");
    }
	else
	{
       vJDECErrorConceal(u4JDecID, TRUE, TRUE, TRUE);
      //  UTIL_Printf("jdec enable error conceal\n");
	}
   // mb();
    // 7. Trigger Decoding
    //HalFlushInvalidateDCache();   
    switch(prJdecHalInfo->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            vHwJdecDecodeMCU(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
            vHwJdecDecodeMCURow(u4JDecID,!(prJdecHalInfo->fgDecoded));
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
#if JDEC_MM_MODE_ENABLE
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS:
#endif
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
         case JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT:
#endif
            #if PERFORMANCE_TEST
            HAL_GetTime(&_rJdecStratTime);//pei temp debug!
          //  *(volatile UINT32 *)(IO_BASE + 0x00007100) = 0x9524C833;
          //  *(volatile UINT32 *)(IO_BASE + 0x00007104) = 0xFEB60A73;         
           *(volatile UINT32 *)(IO_BASE + 0x00007180) = 0x00000E00; 
           *(volatile UINT32 *)(IO_BASE + 0x0000718C) = 120000000;
            //u4Temp = *(volatile UINT32 *)(IO_BASE + 0x00040800); 
            //UTIL_Printf("[Jpeg][Verify]Img group is %x\n",u4Temp);
            //u4Temp = *(volatile UINT32 *)(IO_BASE + 0x00007180); 
            //UTIL_Printf("[Jpeg][Verify]Agent ID is %x\n",u4Temp);             
            //u4Temp |= (0x1<<15);  
            *(volatile UINT32 *)(IO_BASE + 0x00007180) = 0x00008E00;
           // *(volatile UINT32 *)(IO_BASE + 0x00007180) = u4Temp; 
            UTIL_Printf("[Jpeg][Verify]before jdec trigger imgesz clk  is %x\n",*(volatile UINT32 *)(IO_BASE + 0x00000074));
            #endif
            vHwJdecDecodePicture(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            vHwJdecDecodeMCURow(u4JDecID,prJdecHalInfo->fgStartOfScan);
            prJdecHalInfo->fgStartOfScan = FALSE;
            break;
        default:
            ASSERT(0);
    }


    // 8. Status Update
    prJdecHalInfo->fgDecoded = TRUE;
    prJdecHalInfo->fgReinit = FALSE;
    _fgJdecHalResume = FALSE;
    switch(prJdecHalInfo->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            prJdecHalInfo->u4CurrMCUNum++;
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
#endif
            prJdecHalInfo->u4CurrMCUNum += prJdecHalInfo->rOutputAddr.u4MCUWidth;
            prJdecHalInfo->rOutputAddr.u4MCUx = 0;
            //prJdecHalInfo->rOutputAddr.u4MCUy++;
            if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
            {
                prJdecHalInfo->rOutputAddr.u4MCUy++;
            }
            break;
        case JDEC_HAL_DEC_MODE_NONE:
            ASSERT(0);
        default:
            break;

    }

    return S_JDEC_HAL_OK;
}

#if JDEC_MM_MODE_ENABLE
INT32 i4JDEC_HAL_Update_Output_Status(UINT32 u4JDecID)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    prJdecHalInfo = _prJdecHalInfo[u4JDecID];
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    prJdecHalInfo->u4CurrMCUNum += prJdecHalInfo->rOutputAddr.u4MCUWidth;
    prJdecHalInfo->rOutputAddr.u4MCUx = 0;
    {
         prJdecHalInfo->rOutputAddr.u4MCUy+= prJdecHalInfo->rOutputInfo.ui4McuRowNumber;
    }
	prJdecHalInfo->rOutputInfo.u4DecodedMCURow+= prJdecHalInfo->rOutputInfo.ui4McuRowNumber;
	prJdecHalInfo->fgReMcuRowMMLoop =TRUE;
	JDEC_VERY_HAL_LOG("Enter %s, u4DecodedMCURow =0x%x, ui4McuRowNumber =0x%x\n", __FUNCTION__,
		prJdecHalInfo->rOutputInfo.u4DecodedMCURow,
		prJdecHalInfo->rOutputInfo.ui4McuRowNumber);
	return S_JDEC_HAL_OK;
}

JDEC_HAL_OUTPUT_INFO_T *prJDEC_HAL_Get_Output_Status(UINT32 u4JDecID)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    prJdecHalInfo = _prJdecHalInfo[u4JDecID];
    //JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    JDEC_VERY_HAL_LOG("Enter %s, u4DecodedMCURow =0x%d\n", __FUNCTION__,prJdecHalInfo->rOutputInfo.u4DecodedMCURow);
	return &prJdecHalInfo->rOutputInfo;
}

#endif

/// Get Jpeg HAL decode status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Decode_Status(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    BOOL fgFinish = FALSE;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    // Skip by EOBRun
    if(_prJdecHalInfo[u4JDecID]->fgEOBRUNSkip)
        return S_JDEC_HAL_OK;

    switch(_prJdecHalInfo[u4JDecID]->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            fgFinish = fgHwJdecIsDecodeFinish(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
#endif
            fgFinish = fgHwJdecIsDecodeMCURowFinish(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
#if JDEC_MM_MODE_ENABLE
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS:
#endif
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
        #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
        case JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT:
        #endif
            fgFinish = fgHwJdecIsDecodePictureFinish(u4JDecID);        
            JDEC_VERY_HAL_LOG("fgFinish %d\n", fgFinish);
            break;
        default:
            ASSERT(0);
    }

    if(fgFinish)
    {
        if(_prJdecHalInfo[u4JDecID]->u4EOBRUN != 0)
            _prJdecHalInfo[u4JDecID]->u4EOBRUN--;
        else
        {
            _prJdecHalInfo[u4JDecID]->u4EOBRUN = u4HwJdecGetEOBRUN(u4JDecID);

        }
        // Process marker except RST marker
        if(fgHwJdecIsFoundMarker(u4JDecID))
        {
            UINT32 u4JpegMarker = u4HwJdecGetFoundMarker(u4JDecID);
            if (((u4JpegMarker >= 0xD0) && (u4JpegMarker <= 0xD7)) || (u4JpegMarker == 0x00))
            {
                //do nothing
            }
            else
            {
                _prJdecHalInfo[u4JDecID]->fgReachMarker = TRUE;
            }
        }
        return S_JDEC_HAL_OK;
    }
    else
        return E_JDEC_HAL_FAIL;
}


/// Get Jpeg VLD read pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Read_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 *pu4AddrBytes,                               ///< [IN] Read pointer address (bytes)
    UINT32 *pu4AddrBits                                 ///< [IN] Read pointer address (bytes)
)
{
    UINT32 u4Bytes;
    UINT32 u4Bits;
    INT32 i4Ret = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;
    if(pu4AddrBytes == NULL)
        return E_JDEC_HAL_FAIL;
    if(pu4AddrBits == NULL)
        return E_JDEC_HAL_FAIL;

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)  //CONFIG_DRV_MT8520 // temp solution, after vdec ok ,will open this marco
    //u4Bytes = u4VDecReadVldRPtr(0,u4JDecID,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa));
        u4Bytes = u4VDecReadVldRPtr(0,_prJdecHalInfo[u4JDecID]->u4HwId,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa));
#else
     #if JDEC_IO_MMU_TEST
        i4Ret = i4JDECReadVldRPtr(0,u4JDecID,&u4Bits,_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa, (_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa + _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz - 1), &u4Bytes);
     #else
        i4Ret = i4JDECReadVldRPtr(0,u4JDecID,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa), PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa + _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz - 1), &u4Bytes);
     #endif
#endif

    if (i4Ret == E_JDEC_HAL_FAIL)
        return i4Ret;

    if(_prJdecHalInfo[u4JDecID]->fgReachMarker)
    {
        if(u4Bytes >= 2)
            u4Bytes -= 2;
        else
            u4Bytes += _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz - 2;
    }

    if(u4Bytes < _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz)
    {
        *pu4AddrBytes = _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa + u4Bytes;
        *pu4AddrBits = u4Bits;
    } else
    {
        return E_JDEC_HAL_FAIL;
    }

    return S_JDEC_HAL_OK;
}


/// Set Jpeg VLD read pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Read_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4Addr                                       ///< [IN] Read pointer address (bytes)
)
{
    INT32 i4Ret;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,u4Addr,0);

    return i4Ret;
}

UINT32 u4JDEC_HAL_Get_VLD_WPtr(UINT32 u4JDecID)
{
    UINT32 i4Ret;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    i4Ret = u4HwJdecGetVLDWritePtr(u4JDecID);
    #if !JDEC_IO_MMU_TEST
    i4Ret = VIRTUAL(i4Ret);
    #endif
    JDEC_VERY_HAL_LOG("Enter %s, VLD_WPtr=0x%x\n", __FUNCTION__,i4Ret);
    return i4Ret;
}

UINT32 u4JDEC_HAL_Get_VLD_RPtr(UINT32 u4JDecID)
{
    UINT32 i4Ret;
	JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    i4Ret = u4HwJdecGetVLDReadPtr(u4JDecID);
    
    #if !JDEC_IO_MMU_TEST
    i4Ret = VIRTUAL(i4Ret);
    #endif
    JDEC_VERY_HAL_LOG("Enter %s, VLD_RPTR=0x%x\n", __FUNCTION__,i4Ret);
    return i4Ret;
}
/// Set Jpeg VLD write pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Write_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4Addr                                       ///< [IN] Write pointer address (bytes)
)
{
    INT32 i4Ret;
#if 0//!JDEC_IO_MMU_TEST
    u4Addr = u4Addr ? PHYSICAL(u4Addr):0;
#endif
     JDEC_VERY_HAL_LOG("Enter %s!u4Addr=0x%x\n", __FUNCTION__,u4Addr);
    if(u4JDecID >= HW_JDEC_VLD_NUM)\
        return E_JDEC_HAL_FAIL;

    u4Addr = (u4Addr + 15)/16 * 16; // 16byte align
    _prJdecHalInfo[u4JDecID]->rInputInfo.u4WrPoint = u4Addr;
    i4Ret = i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,_prJdecHalInfo[u4JDecID]->rInputInfo.u4WrPoint);
    if(i4Ret !=0)
    {
    	UTIL_Printf("[jdec_hal] set 4HwJdecSetBarrelShifterAddr failed\n");
    }
    return i4Ret;
}


/// Get Jpeg hardware status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_HW_Status(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_HW_STATUS_T *prHwStatus                    ///< [OUT] Hardware status
)
{
    INT32 i4Ret;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    if(prHwStatus == NULL)
        return E_JDEC_HAL_FAIL;

    i4Ret = i4HwJdecGetHwStatus(u4JDecID,prHwStatus);
    prHwStatus->u4CurrMCUNum = _prJdecHalInfo[u4JDecID]->u4CurrMCUNum;
    i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&(prHwStatus->u4ReadAddrBytes),&(prHwStatus->u4ReadAddrBits));

    return i4Ret;
}


/// Set Jpeg hardware status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_HW_Status(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    const JDEC_HAL_HW_STATUS_T *prHwStatus              ///< [IN] Hardware status
)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    if(prHwStatus == NULL)
        return E_JDEC_HAL_FAIL;

    prJdecHalInfo = _prJdecHalInfo[u4JDecID];

    prJdecHalInfo->u4CurrMCUNum = prHwStatus->u4CurrMCUNum;
    prJdecHalInfo->fgReinit = TRUE;
    prJdecHalInfo->fgDecoded = TRUE;
    x_memcpy((void*)&(prJdecHalInfo->rHwStatus),prHwStatus,sizeof(JDEC_HAL_HW_STATUS_T));

    return S_JDEC_HAL_OK;
}


INT32 i4JDEC_HAL_GET_Error_Type(
    UINT32 u4JDecID
    )
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    return i4HwJdecGetErrorStatus(u4JDecID);
}

void i4JDEC_HAL_PrintChkSum(UINT32 u4JDecID)
{
    vHwPrintChkSum(u4JDecID);
}
void vJDEC_HAL_StopPicModeDec(
    UINT32 u4JDecID)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vJDECSTOPPicMode(u4JDecID);
}

INT32 i4JDEC_HAL_GET_STOP_STATUS(
    UINT32 u4JDecID
    )
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if (fgJDECGetStopReady(u4JDecID))
    {
        return S_JDEC_HAL_OK;
    }
    else
    {
        return E_JDEC_HAL_FAIL;
    }

}

INT32 i4JDEC_HAL_Decode_Finish(
    UINT32 u4JDecID,
    BOOL fgLastScan,
    UINT32* pu4CurrentLastScanNum
    )
{
    UINT32 u4TotalLastScanNum = *pu4CurrentLastScanNum;
    JDEC_HAL_INFO_T *prJdecHalInfo;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    prJdecHalInfo = _prJdecHalInfo[u4JDecID];

    if (fgLastScan)
    {
        u4TotalLastScanNum += prJdecHalInfo->rScanInfo.u1CompInScan;
        *pu4CurrentLastScanNum = u4TotalLastScanNum;
    }
    if (u4TotalLastScanNum >= prJdecHalInfo->rPicInfo.prSofInfo->u1NumComponents)
    {
        return S_JDEC_HAL_OK;
    }
    else
    {
        return E_JDEC_HAL_FAIL;
    }
}
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)

void vJDEC_HAL_GetIrq(UINT16 u2Vector)
{  
	UTIL_Printf("JDEC_HAL IRQ coming------\n");//("Enter %s!\n", __FUNCTION__);
    switch (u2Vector)
    {
    case VECTOR_JPGDEC:
        VERIFY(BIM_ClearIrq(u2Vector));
        // check if decode finish or decode error
        if (i4HwJdecGetErrorStatus(0) != 0)
        {
            // error occurs
            UTIL_Printf("[JDEC] %s line %d Set JDEC_DRV_EV_DEC_ERR\n", __FUNCTION__, __LINE__);
            VERIFY(x_ev_group_set_event(_rJdecInst[0].hJdecEvent, JDEC_DRV_EV_DEC_ERR, X_EV_OP_OR) == OSR_OK);
        }
        else
        {
            #if PERFORMANCE_TEST

            HAL_GetTime(&_rJdecEndTime);
            if (_rJdecEndTime.u4Micros < _rJdecStratTime.u4Micros)
            {
                _rJdecTotalTime.u4Micros = 1000000 + _rJdecEndTime.u4Micros - _rJdecStratTime.u4Micros;
                _rJdecEndTime.u4Seconds -= 1;
            }
            else
            {
                _rJdecTotalTime.u4Micros = _rJdecEndTime.u4Micros - _rJdecStratTime.u4Micros;
            }
            _rJdecTotalTime.u4Seconds = _rJdecEndTime.u4Seconds - _rJdecStratTime.u4Seconds;
            #endif
            // decode finish


            if(_prJdecHalInfo[0]->u4EOBRUN != 0)
                _prJdecHalInfo[0]->u4EOBRUN--;
            else
            {
                _prJdecHalInfo[0]->u4EOBRUN = u4HwJdecGetEOBRUN(0);

            }
            // Process marker except RST marker
            if(fgHwJdecIsFoundMarker(0))
            {
                UINT32 u4JpegMarker = u4HwJdecGetFoundMarker(0);
                if (((u4JpegMarker >= 0xD0) && (u4JpegMarker <= 0xD7)) || (u4JpegMarker == 0x00))
                {
                    //do nothing
                }
                else
                {
                    _prJdecHalInfo[0]->fgReachMarker = TRUE;
                }
            }
            VERIFY(x_ev_group_set_event(_rJdecInst[0].hJdecEvent, JDEC_DRV_EV_DEC_DONE, X_EV_OP_OR) == OSR_OK);
        }
        break;
    }
}


BOOL fgJDEC_HAL_SetIrq(void)
{
    x_os_isr_fct pfnOldIsr;
    UTIL_Printf("Enter %s!\n", __FUNCTION__);
    if (_fgJdecHalInitIrq)
    {
        return TRUE;
    }
    else
    {
        _fgJdecHalInitIrq = TRUE;
    }

    if (x_reg_isr(VECTOR_JPGDEC, vJDEC_HAL_GetIrq, &pfnOldIsr) != OSR_OK)    //dec
    {
        ASSERT(0);
        return FALSE;
    }
    return TRUE;
}

BOOL fgJDEC_HAL_DisIrq(void)
{
    x_os_isr_fct pfnOldIsr;
    UTIL_Printf("Enter %s!\n", __FUNCTION__);
    if (_fgJdecHalInitIrq)
    {
        _fgJdecHalInitIrq = FALSE;
    }
    else
    {
        return TRUE;
    }

    if (x_reg_isr(VECTOR_JPGDEC, NULL, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
        return FALSE;
    }
    return TRUE;
}
#endif


