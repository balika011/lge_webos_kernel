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
#include "x_hal_ic.h"
#include "x_ckgen.h"
#include "x_rtos.h"
#include "x_common.h"
#include "jdec_hal_if.h"
#include "jdec_hal_errcode.h"
#include "jdec_hal.h"
#include "jdec_com_jfif.h"
//#include <string.h>
#include "x_mmap.h"
#include "x_bim.h"
#include "jdec_drv.h"
#include "x_hal_1176.h"
#if PERFORMANCE_TEST
#include "x_timer.h"
#endif
#include "chip_ver.h"
#include "drv_config.h"

#include "sys_config.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#else
#include "x_mem_phase2.h"
#endif

extern BOOL CKGEN_SetPLL(SRC_CK_T eSource, UINT32 u4Clock0, UINT32 u4Clock1);
extern BOOL CKGEN_AgtOnClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtOffClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtSelClk(e_CLK_T eAgt, UINT32 u4Sel);



extern UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa);
extern JDEC_INST_T         _rJdecInst[];

#if JDEC_HW_LOG_ENABLE
extern UINT32 _u4JdecHwCmdIdx;
#endif

#define JDEC_SUPPORT_AGENT_ON_OFF 0

// Work around for 16M limitation
#define JDEC_HAL_IF_MAX_PROGRESSIVE_PIC_WIDTH      4096
#define JDEC_HAL_IF_MAX_PROGRESSIVE_PIC_HEIGHT     2048

BOOL _fgJdecHalAllocate = FALSE;
void *_pvJdecHalNoneZeroHistoryBuf[3];
//UINT32 _u4JdecHalOutCoefYBufSa[2];
//UINT32 _u4JdecHalOutCoefCbBufSa[2];
//UINT32 _u4JdecHalOutCoefCrBufSa[2];

UINT32 _u4JdecHalHwDcHuffTblSa;
UINT32 _u4JdecHalHwAcHuffTblSa;

BOOL _fgJdecHalInitIrq = FALSE;

#if PERFORMANCE_TEST
extern void HAL_GetTime(HAL_TIME_T* pTime);
extern void HAL_GetDeltaTime(HAL_TIME_T * pResult, HAL_TIME_T * pOlder, HAL_TIME_T * pNewer);
HAL_TIME_T _rJdecStratTime, _rJdecEndTime, _rJdecTotalTime;
#endif


typedef struct {
    JDEC_HAL_DEC_MODE_T eDecodingMode;          ///< Decoding Mode.
    JDEC_HAL_PIC_INFO_T rPicInfo;               ///< Input Jpeg picture information.
    JDEC_JFIF_SOS_T rScanInfo;                  ///< Input Jpeg picture Scan Infomation
    JDEC_HAL_INPUT_INFO_T rInputInfo;           ///< Input fifo information
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo;         ///< Output buffer information
    BOOL fgDecoded;                             ///< If decoded after initialization.
    UINT32 u4CurrMCUNum;                        ///< Current MCU Number.
    void *pvNoneZeroHistoryBuf[3];              ///< None Zero History Buffer.  Working buffer for Progressive Scan.
    UINT32 u4RestartIntervalCnt;                ///< Restart Internal Counter.
    UINT32 u4EOBRUN;                            ///< remaining EOBs in EOBRUN
    BOOL fgEOBRUNSkip;                          ///< Current MCU skip by EOBRUN
    UINT8 au1CompNumOfBlkInMCU[10];             ///< The component number of each block in one MCU.
    BOOL fgStartOfScan;                         ///< Reach Start of Scan
    BOOL fgReachMarker;                         ///< Reach Marker
    BOOL fgReinit;                              ///< Re-initialization flag.
    JDEC_HAL_OUTPUT_ADDR_T rOutputAddr;         ///< Internal count for output address
    UINT32 u4OutCoefYBufSa;                     ///< Jpeg output progressive coefficient buffer Y start address
    UINT32 u4OutCoefCbBufSa;                    ///< Jpeg output progressive coefficient buffer Cb start address
    UINT32 u4OutCoefCrBufSa;                    ///< Jpeg output progressive coefficient buffer Cr start address
    JDEC_HAL_HW_STATUS_T rHwStatus;             ///< Hardware status for re-initialization.
	UINT32 u4HwDcHuffTblSa;		             	///< HW format DC Huffman Table
	UINT32 u4HwAcHuffTblSa;                     ///< HW format AC Huffman Table
	UINT32 u4HwId;                              ///< Real HW id
#ifdef JDEC_SUPPORT_SLICED_COEF_BUFFER
    HANDLE_T hTempY;
    HANDLE_T hTempCb;
    HANDLE_T hTempCr;
    UINT32   u4BlkSize;
    BOOL fgSlicedCoeff;
#endif
} JDEC_HAL_INFO_T;


JDEC_HAL_INFO_T *_prJdecHalInfo[HW_JDEC_VLD_NUM];   // HAL local information for each HW.


//extern void vPngSetClockOn(void);
//extern void vPngSetClockOff(void);

UINT32 u4JdecPow(UINT32 u4Base, UINT32 u4Pow)
{
    UINT32 u4Value = 1;

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


void vHwClkVDecSwitchJpeg(void)
{
#if (!CONFIG_DRV_FPGA_BOARD)
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    UINT32 u4Temp = CKGEN_READ32(REG_RW_CLK_SEL_0);

    u4Temp = u4Temp & CLK_SEL_0_VDEC_MASK;

    if (u4Temp != CLK_SEL_0_VDEC_DMPLL_1_4)
    {
        CKGEN_WRITE32(REG_RW_CLK_SEL_0,(CKGEN_READ32(REG_RW_CLK_SEL_0) & (~CLK_SEL_0_VDEC_MASK)) | (CLK_SEL_0_VDEC_DMPLL_1_4 << 12));
    }
#else
    UINT32 u4Value;
#if JDEC_SUPPORT_AGENT_ON_OFF
    CKGEN_AgtOnClk(e_CLK_PNG);
#endif
    u4Value = CKGEN_AgtGetClk(e_CLK_PNG);

    if(u4Value != CLK_CLK_PNG_SEL_SYSPLL2_1_2)
        CKGEN_AgtSelClk(e_CLK_PNG , CLK_CLK_PNG_SEL_SYSPLL2_1_2); 
    
#endif
#endif //#if (!CONFIG_DRV_FPGA_BOARD)
}

BOOL _fgJdecHalResume = FALSE;


/// Initialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Init(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4HwId
)
{
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
    vHwJdecSRAMClear(u4JDecID);

    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,TRUE);

    // Adjust this register to correctly decode.
    vHwJdecSetMiscReg(u4JDecID);

    // Set output to color buffer as raster scan mode.
   // vHwJdecSetReadWriteOutputBufMode(u4JDecID);

    // Set VLD wait time out value.
    vHwJdecSetVLDWaitTimeoutValue(u4JDecID,0x3000);


#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
    // release CPU when intial fetch
    vJDECReleaseCPU(u4JDecID);
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
    #endif
    
    return S_JDEC_HAL_OK;
}


/// Uninitialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID,                                     ///< [IN] Jpeg decoder hardware ID
    BOOL   fgWait                                        ///< [IN] Wait hw stop
)
#else
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
#endif

{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    /*For digest page up/down problem, use software reset
    to stop picture mode has som problem, need to change 
    the stop flow:
    if fgWait == TRUE, should wait for hw stop ,then so software rest 
    The fgWait flag was controled by driver code*/
    if (fgWait)
    {
        UINT32 u4Temp = 0,u4Temp1;
        UINT32 u4RdPoint, u4RdPointBitShift;
        INT32 i4Ret;

        i4Ret = i4JDEC_HAL_Get_Read_Pointer(u4JDecID, &u4RdPoint, &u4RdPointBitShift);
        if (i4Ret == S_JDEC_HAL_OK)
        {
            while (1)
            {
                x_thread_delay(5);
                i4Ret = i4JDEC_HAL_Get_Read_Pointer(u4JDecID, &u4Temp, &u4Temp1);
                if ((u4Temp == u4RdPoint)&& (i4Ret == S_JDEC_HAL_OK))
                {
                    break;
                }
                else if (i4Ret == S_JDEC_HAL_OK)
                {
                    u4RdPoint = u4Temp;
                }
                else
                {
                    //ASSERT(0);
                    x_thread_delay(100);
                }
            }
        }
        else
        {
            //ASSERT(0);
            x_thread_delay(100);
        }
    }
#endif
    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,FALSE);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
    // disable interrupt
    vHwJdecSetInterrupt(u4JDecID, FALSE);
#endif

    // Software reset VLD for other module
   // vHwJdecHwReset(u4JDecID);  //sun modify for test

    // Power off
    vHwJdecVLDPowerOn(u4JDecID,FALSE);

    //vPngSetClockOff();
#if JDEC_SUPPORT_AGENT_ON_OFF    
    CKGEN_AgtOffClk(e_CLK_PNG);
#endif
#if !JDEC_SUPPORT_SLICED_COEF_BUFFER

    if(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa != 0)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
#else
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
#endif
        #else
        x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
        #endif
    if (_prJdecHalInfo[u4JDecID]->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        &&_prJdecHalInfo[u4JDecID]->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE
        &&_prJdecHalInfo[u4JDecID]->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#else
        )
#endif
    {
    if(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa != 0)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
#else
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
#endif
        #else
        x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
        #endif
    if(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa != 0)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
#else
        x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
#endif
        #else
        x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
        #endif
    }
    
    _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = 0;
    _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = 0;
    _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = 0;
#else // default for linux, memory phase III



    if ( 0
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)        
         || (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#endif        
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)        
         || (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
#endif         
        )
    {
        if (_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa != 0)
        {
            x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
            _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = 0;
        }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)          
        if (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        {
            if (_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa != 0)
            {
                x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
                _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = 0;
            }
            if (_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa != 0)
            {
                x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
                _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = 0;
            }
        }
#endif        
    }

    if ((_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530) 
        || (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
#endif        
        )
        {
            if (_prJdecHalInfo[u4JDecID]->fgSlicedCoeff)
            {
                x_kmem_block_free(_prJdecHalInfo[u4JDecID]->hTempY);
                x_kmem_block_free(_prJdecHalInfo[u4JDecID]->hTempCb);
                x_kmem_block_free(_prJdecHalInfo[u4JDecID]->hTempCr);
            }
            else
            {
                //
                x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
                x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
                x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
                _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = 0;
                _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = 0;
                _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = 0;
            }
            _prJdecHalInfo[u4JDecID]->hTempY = 0;
            _prJdecHalInfo[u4JDecID]->hTempCb = 0;
            _prJdecHalInfo[u4JDecID]->hTempCr = 0;
            _prJdecHalInfo[u4JDecID]->u4BlkSize = 0;
        }

#endif //!JDEC_SUPPORT_SLICED_COEF_BUFFER

    {
        // free the non-zero history buffer and huffman table
        if (_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa != 0)
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            x_free_aligned_dma_mem((void*)_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa);
#elif CONFIG_SYS_MEM_PHASE3
            x_mem_aligned_free((void*)_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa);
#else
            x_free_aligned_dma_mem((void*)_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa);
#endif
            #else
            x_free_aligned_nc_mem((void*)_prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa);
            #endif
        _prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa = 0;

        #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
        if (_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa != 0)
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa);
#elif CONFIG_SYS_MEM_PHASE3
            x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa);
#else
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa);
#endif
            #else
            x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa);
            #endif
        }
        if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0] != NULL)
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0]);
#elif CONFIG_SYS_MEM_PHASE3
            x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0]);
#else
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0]);
#endif
            #else
            x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0]);
            #endif
        }
        if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1] != NULL)
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1]);
#elif CONFIG_SYS_MEM_PHASE3
           x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1]);
#else
           x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1]);
#endif
            #else
            x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1]);
            #endif
        }
        if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2] != NULL)
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2]);
#elif CONFIG_SYS_MEM_PHASE3
            x_mem_aligned_free((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2]);
#else
            x_free_aligned_dma_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2]);
#endif
            #else
            x_free_aligned_nc_mem((void *)_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2]);
            #endif
        }
        #endif
        
        _prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa = 0;
        _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0] = NULL;
        _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1] = NULL;
        _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2] = NULL;  

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
            vHwJDecSetNewProgressive(u4JDecID, TRUE, FALSE);
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

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
/// Set Jpeg input picture information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Pic_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_PIC_INFO_T *prPicInfo                      ///< [IN] Jpeg input picture information
)
{
    UINT32 u4JdecBufBak[10] = {0};

    
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rPicInfo = *prPicInfo;

    {
        // malloc memory for non zero history and huffman table
        if ((_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
            (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
        {
            // malloc non-zero history buffer and huffman table
            UINT32 u4Addr = 0;
            UINT32 u4Temp = 0;
            UINT32 u4Size = 0;
            UINT32 u4Width, u4Height;
            INT32 i4 = 0;
        
            u4Temp= _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
            u4Width = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageWidth + (u4Temp - 1)) / u4Temp) * u4Temp;
            u4Temp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
            u4Height = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageHeight + (u4Temp - 1)) / u4Temp) * u4Temp;
           
            //8 *1024 is for huffman DC/AC table
            //(u4Width * u4Height)>> 3 * 3 is for 3 components' non-zero history buffer 
            u4Size = 8*1024 + ((u4Width * u4Height)>> 3) * 3;
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            u4Addr = (UINT32)x_alloc_aligned_dma_mem(u4Size, 32);
#elif CONFIG_SYS_MEM_PHASE3
            u4Addr = (UINT32)x_mem_aligned_alloc(u4Size, 32);
#else
            u4Addr = (UINT32)x_alloc_aligned_dma_mem(u4Size, 32);
#endif
            #else
            u4Addr = (UINT32)x_alloc_aligned_nc_mem(u4Size, 16);
            #endif
            if (u4Addr == 0)
            {
                //ASSERT(0);
                return E_JDEC_HAL_FAIL;
            }
            while (1)
            {
                if (((0x1000000 - u4Addr % 0x1000000) >= u4Size)/* &&
                    ((u4Addr & 0x3FFFFFFF) < (128*1024*1024)) */)
                {
                    break;
                }
                
                u4JdecBufBak[i4] = u4Addr;
                i4++;
                if (i4 == 10)
                {
                    //ASSERT(0);
                    while (i4 > 0)
                    {
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)u4JdecBufBak[i4 - 1]);
#else
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)u4JdecBufBak[i4 - 1]);
                        #endif
                        u4JdecBufBak[i4 -1] = 0;
                        i4 --;
                    }
                    return E_JDEC_HAL_FAIL;
                }
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                u4Addr = (UINT32)x_alloc_aligned_dma_mem(u4Size, 32);
#elif CONFIG_SYS_MEM_PHASE3
                u4Addr = (UINT32)x_mem_aligned_alloc(u4Size, 32);
#else
               u4Addr = (UINT32)x_alloc_aligned_dma_mem(u4Size, 32);
#endif
                #else
                u4Addr = (UINT32)x_alloc_aligned_nc_mem(u4Size, 16);
                #endif
                if (u4Addr == 0)
                {
                    //ASSERT(0);
                    while (i4 > 0)
                    {
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)u4JdecBufBak[i4 - 1]);
#else
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)u4JdecBufBak[i4 - 1]);
                        #endif
                        u4JdecBufBak[i4 -1] = 0;
                        i4 --;
                    }
                    return E_JDEC_HAL_FAIL;
                }
            }
            while (i4 > 0)
            {
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#elif CONFIG_SYS_MEM_PHASE3
                x_mem_aligned_free((void *)u4JdecBufBak[i4 - 1]);
#else
                x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#endif
                #else
                x_free_aligned_nc_mem((void *)u4JdecBufBak[i4 - 1]);
                #endif
                u4JdecBufBak[i4 - 1] = 0;
                i4--;
            }
            // For huffman table
            _u4JdecHalHwDcHuffTblSa = u4Addr;
            _u4JdecHalHwAcHuffTblSa = u4Addr+4096;

            // for non-zero history buffer
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[0] = (void *)(u4Addr + 4096 * 2);
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[1] = (void *)(u4Addr + 4096 * 2 + ((u4Width * u4Height)>> 3));
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[2] = (void *)(u4Addr + 4096 * 2 + ((u4Width * u4Height)>> 3) * 2);
        }
        else
        {
            // only malloc huffman table
            UINT32 u4Addr = 0;
            //UINT32 u4Temp = 0;
            INT32 i4 = 0;

            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            u4Addr = (UINT32)x_alloc_aligned_dma_mem(8*1024,32);
#elif CONFIG_SYS_MEM_PHASE3
            u4Addr = (UINT32)x_mem_aligned_alloc(8*1024,32);
#else
            u4Addr = (UINT32)x_alloc_aligned_dma_mem(8*1024,32);
#endif
            #else
            u4Addr = (UINT32)x_alloc_aligned_nc_mem(8*1024,16);
            #endif
            if (u4Addr == 0)
            {
                //ASSERT(0);
                return E_JDEC_HAL_FAIL;
            }
            while (1)
            {
                if (((0x1000000 - u4Addr % 0x1000000) >= (8*1024))/* &&
                    ((u4Addr & 0x3FFFFFFF) < (128*1024*1024)) */)
                {
                    break;
                }
                
                u4JdecBufBak[i4] = u4Addr;
                i4++;
                if (i4 == 10)
                {
                    //ASSERT(0);
                    while (i4 > 0)
                    {
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)u4JdecBufBak[i4 - 1]);
#else
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)u4JdecBufBak[i4 - 1]);
                        #endif
                        u4JdecBufBak[i4 -1] = 0;
                        i4 --;
                    }
                    return E_JDEC_HAL_FAIL;
                }
                //u4Temp = u4Addr;
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                u4Addr = (UINT32)x_alloc_aligned_dma_mem(8*1024,32);
#elif CONFIG_SYS_MEM_PHASE3
                u4Addr = (UINT32)x_mem_aligned_alloc(8*1024,32);
#else
                u4Addr = (UINT32)x_alloc_aligned_dma_mem(8*1024,32);
#endif
                #else
                u4Addr = (UINT32)x_alloc_aligned_nc_mem(8*1024,16);
                #endif
                if (u4Addr == 0)
                {
                    //ASSERT(0);
                    while(i4 > 0)
                    {
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)u4JdecBufBak[i4 - 1]);
#else
                        x_free_aligned_dma_mem((void *)u4JdecBufBak[i4 - 1]);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)u4JdecBufBak[i4 - 1]);
                        #endif
                        u4JdecBufBak[i4 - 1] = 0;
                        i4 --;
                    }
                    return E_JDEC_HAL_FAIL;
                }
            }
            while (i4 > 0)
            {
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                x_free_aligned_dma_mem((void *)u4JdecBufBak[i4-1]);
#elif CONFIG_SYS_MEM_PHASE3
                x_mem_aligned_free((void *)u4JdecBufBak[i4-1]);
#else
                x_free_aligned_dma_mem((void *)u4JdecBufBak[i4-1]);
#endif
                #else
                x_free_aligned_nc_mem((void *)u4JdecBufBak[i4-1]);
                #endif
                u4JdecBufBak[i4 - 1] = 0;
                i4--;
            }
            _u4JdecHalHwDcHuffTblSa = u4Addr;
            _u4JdecHalHwAcHuffTblSa = u4Addr+4096;
        }

        _prJdecHalInfo[u4JDecID]->u4HwDcHuffTblSa = _u4JdecHalHwDcHuffTblSa;
        _prJdecHalInfo[u4JDecID]->u4HwAcHuffTblSa = _u4JdecHalHwAcHuffTblSa;
    }
    
    //_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = _u4JdecHalOutCoefYBufSa[u4JDecID];
    //_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = _u4JdecHalOutCoefCbBufSa[u4JDecID];
    //_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = _u4JdecHalOutCoefCrBufSa[u4JDecID];
  
    {
        UINT32 u4CompId;
        JDEC_HAL_OUTPUT_ADDR_T *prOutputAddr = &(_prJdecHalInfo[u4JDecID]->rOutputAddr);
        JDEC_JFIF_SOF_T *prSofInfo = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo;

        if (prSofInfo->u1NumComponents > MAX_JPEG_COMPONENT)
        {
            ASSERT(0);
        }
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
#else   // >MT8520
/// Set Jpeg input picture information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Pic_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_PIC_INFO_T *prPicInfo                      ///< [IN] Jpeg input picture information
)
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rPicInfo = *prPicInfo;

// 8550 
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)     
    if ((_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
#else 
    if ((_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
        (_prJdecHalInfo[u4JDecID]->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT))
#endif        
    {
        UINT32 u4Comp;
        UINT32 u4Width, u4Height,u4Tmp;


        
        u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
        u4Width = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
        u4Tmp = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
        u4Height = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
        for(u4Comp=0; u4Comp<3; u4Comp++)
        {
            //_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = _pvJdecHalNoneZeroHistoryBuf[u4Comp];
            //_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = (void *)((UINT32)g_pvJpegBuffer + 1024 * u4Comp);

#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = (void *)(UINT32)x_alloc_aligned_dma_mem((u4Width * u4Height)>> 3, 32);
#elif CONFIG_SYS_MEM_PHASE3
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = (void *)(UINT32)x_mem_aligned_alloc((u4Width * u4Height)>> 3, 32);
#else
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = (void *)(UINT32)x_alloc_aligned_dma_mem((u4Width * u4Height)>> 3, 32);
#endif
#else
            _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = (void *)(UINT32)x_alloc_aligned_nc_mem((u4Width * u4Height)>> 3, 8);
#endif
            if (_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] == 0)
            {
                // allocate fail
                return E_JDEC_HAL_FAIL;
            }
        }
    }
    //_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = _u4JdecHalOutCoefYBufSa[u4JDecID];
    //_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = _u4JdecHalOutCoefCbBufSa[u4JDecID];
    //_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = _u4JdecHalOutCoefCrBufSa[u4JDecID];
  
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
#endif

/// Set Huffman Table information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_HuffmanTable_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_DHT_T *prHuffTblInfo                      ///< [IN] Huffman Table information
)
{
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
{
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
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rInputInfo = *prInputInfo;

    return S_JDEC_HAL_OK;
}


/// Set Jpeg output information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Output_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo                ///< [IN] Input Jpeg output information
)
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    _prJdecHalInfo[u4JDecID]->rOutputInfo = *prOutputInfo;
    _prJdecHalInfo[u4JDecID]->rOutputAddr.fgAddrModified = TRUE;

    return S_JDEC_HAL_OK;
}


/// Trigger Jpeg HAL decoding
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Decode(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    INT32 i4Ret;

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    prJdecHalInfo = _prJdecHalInfo[u4JDecID];

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    // flush cache memory
    //HalFlushInvalidateDCache();
    //BSP_FlushDCacheRange();
    #endif
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
  #if JDEC_SUPPORT_SLICED_COEF_BUFFER
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
                                                  prJdecHalInfo->eDecodingMode,
                                                  prJdecHalInfo->hTempY,
                                                  prJdecHalInfo->hTempCb,
                                                  prJdecHalInfo->hTempCr,
                                                  prJdecHalInfo->fgSlicedCoeff);
  #else
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
  #endif
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
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
               || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
                )
            {
                UINT32 u4Comp;
                UINT32 u4Width,u4Height;
                UINT32 u4Tmp;

                u4Tmp = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8;
                u4Width = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
                u4Tmp = prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8;
                u4Height = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
                {
#endif
                // Clean none-zero history buffer
                for(u4Comp=0; u4Comp<3; u4Comp++)
                    x_memset((void *)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp],0,
                                               (u4Width * u4Height + 7)/8);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                }
#endif

                if(prJdecHalInfo->u4OutCoefYBufSa != 0)
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefYBufSa);
#elif CONFIG_SYS_MEM_PHASE3
                    x_mem_aligned_free((void *)prJdecHalInfo->u4OutCoefYBufSa);
#else
                    x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefYBufSa);
#endif
                    #else
                    x_free_aligned_nc_mem((void *)prJdecHalInfo->u4OutCoefYBufSa);
                    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY
                    
                if(prJdecHalInfo->u4OutCoefCbBufSa != 0)
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefCbBufSa);
#elif CONFIG_SYS_MEM_PHASE3
                    x_mem_aligned_free((void *)prJdecHalInfo->u4OutCoefCbBufSa);
#else
                   x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefCbBufSa);
#endif
                    #else
                    x_free_aligned_nc_mem((void *)prJdecHalInfo->u4OutCoefCbBufSa);
                    #endif // CONFIG_DRV_LINUX_DATA_CONSISTENCY
                    
                if(prJdecHalInfo->u4OutCoefCrBufSa != 0)
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefCrBufSa);
#elif CONFIG_SYS_MEM_PHASE3
                    x_mem_aligned_free((void *)prJdecHalInfo->u4OutCoefCrBufSa);
#else
                     x_free_aligned_dma_mem((void *)prJdecHalInfo->u4OutCoefCrBufSa);                 
#endif
                    #else
                    x_free_aligned_nc_mem((void *)prJdecHalInfo->u4OutCoefCrBufSa);
                    #endif // CONFIG_DRV_LINUX_DATA_CONSISTENCY
                            
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)                            
                if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
//#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
//                    || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE
//                    || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
//#else
//                    )
//#endif
                {
                    UINT32 u4Temp;

                    u4Temp = u4Width * prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 2;
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Temp*2,128);
#elif CONFIG_SYS_MEM_PHASE3
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_mem_aligned_alloc(u4Temp*2,128);
#else
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Temp*2,128);
#endif
                    #else
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Temp*2,128);
                    #endif
                    prJdecHalInfo->u4OutCoefCbBufSa = prJdecHalInfo->u4OutCoefYBufSa;//(UINT32)x_alloc_aligned_nc_mem(u4Temp*2,2048);
                    prJdecHalInfo->u4OutCoefCrBufSa = prJdecHalInfo->u4OutCoefYBufSa;//(UINT32)x_alloc_aligned_nc_mem(u4Temp*2,2048);

                    VERIFY(prJdecHalInfo->u4OutCoefYBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCbBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCrBufSa  != 0);

                    // Clean coefficient buffer
                    if(!_fgJdecHalResume)
                        x_memset((void *)prJdecHalInfo->u4OutCoefYBufSa,0,
                                               u4Temp* 2);

                }
//                else
#endif    
                if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)               
                    ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) 
                    ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
#endif
                   )
                {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)                     
                    if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                    {
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
#elif CONFIG_SYS_MEM_PHASE3
                        prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                        prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                        prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
#else
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
#endif
                    #else
                    prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                    prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                    #endif // CONFIG_DRV_LINUX_DATA_CONSISTENCY
                   }
                   else 
                    #endif
                   {
                   #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                       prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                       prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                       prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
#elif CONFIG_SYS_MEM_PHASE3
                       #if  JDEC_SUPPORT_SLICED_COEF_BUFFER
                       {
            		       UINT32 u4Temp = 0;
            			UINT32 u4count = 0;
                            UINT32 u4McuRowCount = 0;
            				 
                           if ((u4Width * u4Height * 2) <= JDEC_MAX_COEF_BLOCK)
                           {
                               prJdecHalInfo->fgSlicedCoeff = FALSE;
                           }
                           else
                           {
                               prJdecHalInfo->fgSlicedCoeff = TRUE;
                           }

                           if (!prJdecHalInfo->fgSlicedCoeff)
                           {
                               prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                               prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                               prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                               prJdecHalInfo->u4BlkSize = 0; // no use
                               prJdecHalInfo->hTempY = 0;
                               prJdecHalInfo->hTempCb = 0;
                               prJdecHalInfo->hTempCr = 0;
                               //UTIL_Printf("[JDEC_HAL]Allocate continuous Y = 0x%x \n",prJdecHalInfo->u4OutCoefYBufSa);
                           }
                           else
                           {
            			u4Temp = u4Width * prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8 * 2;
            			u4Temp = (u4Temp+1023)/1024;	
                            u4McuRowCount = u4Height / u4Tmp;
            				 
            			   while( u4Temp!= 0 )
            		       { 
            		           u4Temp = u4Temp >> 1;
            		           u4count ++;
            			   }
        		      prJdecHalInfo->u4BlkSize = (UINT32)1024*u4JdecPow(2,u4count);
                           prJdecHalInfo->hTempY= x_kmem_block_sliced_alloc(u4McuRowCount * prJdecHalInfo->u4BlkSize, prJdecHalInfo->u4BlkSize);
            		      prJdecHalInfo->hTempCb= x_kmem_block_sliced_alloc(u4McuRowCount * prJdecHalInfo->u4BlkSize, prJdecHalInfo->u4BlkSize);
        		      prJdecHalInfo->hTempCr= x_kmem_block_sliced_alloc(u4McuRowCount * prJdecHalInfo->u4BlkSize, prJdecHalInfo->u4BlkSize);
        				   
                           prJdecHalInfo->u4OutCoefYBufSa= (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempY,0);
        		      prJdecHalInfo->u4OutCoefCbBufSa= (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempCb,0);
        		      prJdecHalInfo->u4OutCoefCrBufSa= (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempCr,0);

                                #if 0 // for debug memory for debug
                                {
                                    // For debug usage
                                    UINT32 u4Temp = 0;
                                    UINT32 u4Y = 0;
                                    UINT32 u4Cb =0;
                                    UINT32 u4Cr = 0;
                                    
                                for (u4Temp = 0; u4Temp < u4McuRowCount; u4Temp++)
                                {
                                    u4Y = (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempY, u4Temp * prJdecHalInfo->u4BlkSize);
                                    u4Cb = (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempCb,u4Temp * prJdecHalInfo->u4BlkSize);
                                    u4Cr = (UINT32)x_kmem_kernel_address(prJdecHalInfo->hTempCr,u4Temp * prJdecHalInfo->u4BlkSize);
                                    UTIL_Printf("[JDEC] Blk%d SA = 0x%x , 0x%x , 0x%x \n",u4Temp, u4Y, u4Cb,u4Cr);
                                }
                                }
                                #endif
    			           }
            			   
    			      }
    			      #else			  
                          prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                          prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
                          prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_mem_aligned_alloc(u4Width*u4Height*2,2048);
    			      #endif //JDEC_SUPPORT_SLICED_COEF_BUFFER
#else
                       prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                       prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
                       prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_dma_mem(u4Width*u4Height*2,2048);
#endif 
                   #else
                      prJdecHalInfo->u4OutCoefYBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                      prJdecHalInfo->u4OutCoefCbBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                      prJdecHalInfo->u4OutCoefCrBufSa = (UINT32)x_alloc_aligned_nc_mem(u4Width*u4Height*2,2048);
                   #endif  //CONFIG_DRV_LINUX_DATA_CONSISTENCY
                	
                   	}

                    VERIFY(prJdecHalInfo->u4OutCoefYBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCbBufSa  != 0);
                    VERIFY(prJdecHalInfo->u4OutCoefCrBufSa  != 0);

	#if  0//!JDEC_SUPPORT_SLICED_COEF_BUFFER 
                    // Clean coefficient buffer
                    if(!_fgJdecHalResume)
                        x_memset((void *)prJdecHalInfo->u4OutCoefYBufSa,0,
                                               u4Width * u4Height * 2);

                    if(!_fgJdecHalResume)
                        x_memset((void *)prJdecHalInfo->u4OutCoefCbBufSa,0,
                                               u4Width * u4Height * 2);

                    if(!_fgJdecHalResume)
                        x_memset((void *)prJdecHalInfo->u4OutCoefCrBufSa,0,
                                               u4Width * u4Height * 2);
	#endif 
                }
            }
            #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
            if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC) ||
               (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)  
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
               ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
                    )
            {
                // enable interrupt
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }
            #endif // !CONFIG_DRV_MT8520
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
       
#if 0
        // Set Huffman Table
        if((i4Ret = i4HwJdecSetHuffmanTable(u4JDecID,prJdecHalInfo->rPicInfo.prHuffTblInfo,
                                           (UINT8 *)(prJdecHalInfo->u4HwDcHuffTblSa),
                                           (UINT8 *)(prJdecHalInfo->u4HwAcHuffTblSa))) < 0)
            return i4Ret;
#endif        

        // Set Quantization Table
        if((i4Ret = i4HwJdecSetQuantTable(u4JDecID,prJdecHalInfo->rPicInfo.prQTblInfo)) < 0)
            return i4Ret;

        // Initialize Restart Interval Counter
        if(prJdecHalInfo->rPicInfo.u4RestartInterval>0)
        {
            prJdecHalInfo->u4RestartIntervalCnt = prJdecHalInfo->rPicInfo.u4RestartInterval;
        }

        //UTIL_Printf("[JDEC_DRV] HAL Input buffer SA= %x \n",prJdecHalInfo->rInputInfo.u4InFifoSa);
        // 4. Set Input Buffer Information to HW
        vHwJdecSetInputBuffer(u4JDecID,prJdecHalInfo->rInputInfo.u4InFifoSa,prJdecHalInfo->rInputInfo.u4InFifoSz);

        //UTIL_Printf("[JDEC_DRV] HAL rd point %x \n",prJdecHalInfo->rInputInfo.u4RdPoint);
        if(!prJdecHalInfo->fgReinit)
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rInputInfo.u4RdPoint,0); // Set VLD read pointer to bitstream start address.
        else
            i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rHwStatus.u4ReadAddrBytes,
                                                  prJdecHalInfo->rHwStatus.u4ReadAddrBits);
        if (i4Ret < 0)
            return i4Ret;
        //i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,prJdecHalInfo->rInputInfo.u4WrPoint);
        // Add by pingzhao ,temp solution,maybe other mode should set write pointer also
        #if 1
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)     
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
            )
        {
            i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,prJdecHalInfo->rInputInfo.u4WrPoint);
            //UTIL_Printf("[JDEC_DRV]HAL set write pointer %x \n",prJdecHalInfo->rInputInfo.u4WrPoint);
        }
        #endif

        // 5. Set Output Buffer Information to HW

        /*The last parameter is for last scan, if enhance mode, each scan is last scan, 
          and the cofficient buffer pitch should equal with color buffer pitch, for multi-collect
          mode, the previous scan's cofficient buffer pitch should equal with color buffer picth * 2 ,and
          the last scan's cofficient buffer pitch shoule equal with color buffer pitch*/
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
        i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                        &(prJdecHalInfo->rScanInfo),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE));
#else
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
#endif
        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
#else
           )
#endif
            i4HwJdecSetCoefPitchPerRow(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo);

    }

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
        vHwJdecForNewEnhance(u4JDecID, TRUE, TRUE);
#endif
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
        #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU))
        {
            vHwJdecSetAhAl(u4JDecID,0,0);
        }
        else
        #endif
        {
            vHwJdecSetAhAl(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Al);
        }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)     
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
           )
#endif
        {
            vHwJdecSetDcAc(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Ss);
        }
        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
#endif
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
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
	      ) 
        {
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
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)            
            (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
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
        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            i4HwJdecSetCoefMCUSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
            i4HwJdecSetCoefPitch(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, 
                                 prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo),
                                 prJdecHalInfo->eDecodingMode);
#else
            i4HwJdecSetCoefPitch(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
#endif
            vHwJdecSetPictureSizeForProg(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, &(prJdecHalInfo->rScanInfo));
        }
        
        //7. Set Coefficient buffer 
        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT ||
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
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
    if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)||
        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU))
    {
        vHwJdecSetSsSe(u4JDecID,0,0);
    }
    else
    #endif
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
    {
        if((prJdecHalInfo->rScanInfo.u1Ah == 0) && (prJdecHalInfo->rScanInfo.u1Ss != 0) && // AC first
           (prJdecHalInfo->u4EOBRUN != 0) && // EOBRUN
           ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Last Scan
           vHwJdecSetSsSe(u4JDecID,64,63);
        else
            vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
    }
#else
    {
        if((prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) &&
           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) &&
           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) &&
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
#endif
    // 5. Set Output Buffer Information to HW
    {
        BOOL fgLastScan = FALSE;

        if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
#else
            )
#endif
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
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
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
                                                fgLastScan,
                                                prJdecHalInfo->eDecodingMode,
                                                prJdecHalInfo->hTempY,
                                                prJdecHalInfo->hTempCb,
                                                prJdecHalInfo->hTempCr,
                                                prJdecHalInfo->fgSlicedCoeff);
#else
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
                                                fgLastScan,
                                                prJdecHalInfo->eDecodingMode);
#endif // JDEC_SUPPORT_SLICED_COEF_BUFFER
#else
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
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
                                                prJdecHalInfo->eDecodingMode,
                                                prJdecHalInfo->hTempY,
                                                prJdecHalInfo->hTempCb,
                                                prJdecHalInfo->hTempCr,
                                                prJdecHalInfo->fgSlicedCoeff);
#else
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
#endif //JDEC_SUPPORT_SLICED_COEF_BUFFER
#endif // (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)

        {
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) && // Multi-collect mode
            !((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Not Last Scan
        {
            // I don't know why must set it,
            // But if not set, it will write out to this address.
            vHwJdecSetOutputBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YCoeffBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CbCoeffBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CrCoeffBufSa,
                                             &(prJdecHalInfo->rScanInfo));
        } else
        {
            vHwJdecSetOutputBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CbBufSa,
                                             prJdecHalInfo->rOutputAddr.u4CrBufSa,
                                             &(prJdecHalInfo->rScanInfo));
        }

#if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8530)			
  #if  JDEC_SUPPORT_SLICED_COEF_BUFFER
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) && // Multi-collect mode
            !((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Not Last Scan
        {
            vHwJdecSetOutputCoefBufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YCoeffBufSa,
                                                 prJdecHalInfo->rOutputAddr.u4CbCoeffBufSa,
                                                 prJdecHalInfo->rOutputAddr.u4CrCoeffBufSa,
                                                 &(prJdecHalInfo->rScanInfo));
        }
  #endif
#endif

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
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
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
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530) 
    if(prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT && 
       prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
    {
    vHwJdecSetCurrMCUNum(u4JDecID,prJdecHalInfo->u4CurrMCUNum);
    i4HwJdecSetEOBRUN(u4JDecID,prJdecHalInfo->u4EOBRUN);
        
    }
    #else
      vHwJdecSetCurrMCUNum(u4JDecID,prJdecHalInfo->u4CurrMCUNum);
      i4HwJdecSetEOBRUN(u4JDecID,prJdecHalInfo->u4EOBRUN);
    #endif
    vHwJdecSetOutputNoneZeroHistoryBufOnly(u4JDecID,FALSE);

    if(prJdecHalInfo->fgReinit)
        i4HwJdecSetHwStatus(u4JDecID,&(prJdecHalInfo->rHwStatus));

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530) 
    // Enable error conceal
    vJDECErrorConceal(u4JDecID, TRUE, TRUE);
#endif

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    vJDECErrorConceal(u4JDecID, TRUE, TRUE,TRUE);
#endif

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
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
#endif
            #if PERFORMANCE_TEST
            HAL_GetTime(&_rJdecStratTime);
            #endif
            vHwJdecDecodePicture(u4JDecID);
            break;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            vHwJdecDecodeMCURow(u4JDecID,prJdecHalInfo->fgStartOfScan);
            prJdecHalInfo->fgStartOfScan = FALSE;
            break;
#endif
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


/// Get Jpeg HAL decode status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Decode_Status(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    BOOL fgFinish = FALSE;

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
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
#endif
            fgFinish = fgHwJdecIsDecodePictureFinish(u4JDecID);
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
#if 0
                UINT32 u4Bytes;
                UINT32 u4Bits;

                u4Bytes = u4VDecReadVldRPtr(0,u4JDecID,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa));

                if(u4Bytes >= 2)
                    u4Bytes -= 2;
                else
                    u4Bytes += _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz - 2;

                i4HwJdecSetBarrelShifterAddr(u4JDecID,_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa + u4Bytes,0);

                // Initialize VLD marker_found and encounter_marker register
                vHwJdecInitScan(u4JDecID);
#endif
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
    i4Ret= i4JDECReadVldRPtr(0,u4JDecID,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa), PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa + _prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSz - 1), &u4Bytes);
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

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    i4Ret = i4HwJdecSetBarrelShifterAddr(u4JDecID,u4Addr,0);

    return i4Ret;
}

UINT32 u4JDEC_HAL_Get_VLD_WPtr(UINT32 u4JDecID)
{
    UINT32 i4Ret;
    
    i4Ret = u4HwJdecGetVLDWritePtr(u4JDecID);

    return i4Ret;
}

UINT32 u4JDEC_HAL_Get_VLD_RPtr(UINT32 u4JDecID)
{
    UINT32 i4Ret;
    
    i4Ret = u4HwJdecGetVLDReadPtr(u4JDecID);

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
    //u4Addr = u4Addr ? PHYSICAL(u4Addr):0; 
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    u4Addr = (u4Addr + 15)/16 * 16; // 16byte align
    _prJdecHalInfo[u4JDecID]->rInputInfo.u4WrPoint = u4Addr;
    i4Ret = i4HwJdecSetVLDSwWritePointer(u4JDecID,TRUE,_prJdecHalInfo[u4JDecID]->rInputInfo.u4WrPoint);

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
{
    return i4HwJdecGetErrorStatus(u4JDecID);
}

void i4JDEC_HAL_PrintChkSum(UINT32 u4JDecID)
{
    vHwPrintChkSum(u4JDecID);
}
void vJDEC_HAL_StopPicModeDec(
    UINT32 u4JDecID)
{
    vJDECSTOPPicMode(u4JDecID);
}

INT32 i4JDEC_HAL_GET_STOP_STATUS(
    UINT32 u4JDecID
    )
{
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
	UTIL_Printf("vJDEC_HAL_GetIrq 00\n");

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
	UTIL_Printf("fgJDEC_HAL_SetIrq 00\n");

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
	UTIL_Printf("fgJDEC_HAL_DisIrq 00\n");

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


