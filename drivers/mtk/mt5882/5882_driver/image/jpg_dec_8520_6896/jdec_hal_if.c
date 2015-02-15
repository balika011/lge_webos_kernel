/*****************************************************************************
* LEGAL DISCLAIMER
*
* (Header of MediaTek Software/Firmware Release or Documentation)
*
* BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN
* "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER
* DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF
* ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE
* MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR
* ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO CONFORM TO
* A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR
* REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK
* FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH
* THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS
* PRINCIPLES.
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: vsv_input_api.c $
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
#include "x_hal_5381.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "jdec_hal_if.h"
#include "jdec_hal_errcode.h"
#include "jdec_hal.h"
#include "jpg_if.h"
#include "x_hal_926.h"


#define RO_VLD_VBAR         0xEC
#define RO_VLD_VRPTR        0xFC
#define RW_VLD_BITCOUNT     0x114
#define RW_VLD_WMV_ABS      0x23C


extern UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa);

// Work around for 16M limitation
static BOOL _fgJdecHalAllocate = FALSE;
static void *_pvJdecHalNoneZeroHistoryBuf[3];
static UINT32 _u4JdecHalOutCoefYBufSa[2];
static UINT32 _u4JdecHalOutCoefCbBufSa[2];
static UINT32 _u4JdecHalOutCoefCrBufSa[2];


typedef struct {
    JDEC_HAL_DEC_MODE_T eDecodingMode;          ///< Decoding Mode.
    JDEC_HAL_PIC_INFO_T rPicInfo;               ///< Input Jpeg picture information.
    JDEC_JFIF_SOS_T rScanInfo;                  ///< Input Jpeg picture Scan Infomation
//    JPEG_JFIF_SOS_T rScanInfo;                  ///< Input Jpeg picture Scan Infomation
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
    UINT32 u4OutCoefYBufSa;                     ///< Jpeg output progressive coefficient buffer Y start address
    UINT32 u4OutCoefCbBufSa;                    ///< Jpeg output progressive coefficient buffer Cb start address
    UINT32 u4OutCoefCrBufSa;                    ///< Jpeg output progressive coefficient buffer Cr start address
    BOOL fgReachMarker;
    BOOL fgReinit;                              ///< Re-initialization flag.
    JDEC_HAL_OUTPUT_ADDR_T rOutputAddr;         ///< Internal count for output address
    JDEC_HAL_HW_STATUS_T rHwStatus;             ///< Hardware status for re-initialization.
#ifdef CC_MT5363
    UINT8 au1HwDcHuffTbl[4];		    	
	UINT8 au1HwAcHuffTbl[4];             
#else
	UINT8 au1HwDcHuffTbl[1024 * 4];		    	///< HW format DC Huffman Table
	UINT8 au1HwAcHuffTbl[1024 * 4];             ///< HW format AC Huffman Table
#endif	
    UINT8 au1HwHuffTblAlign[15];                ///< For HW format Huffman Table align to 16 bytes.
} JDEC_HAL_INFO_T;


JDEC_HAL_INFO_T *_prJdecHalInfo[HW_JDEC_VLD_NUM];   // HAL local information for each HW.


void vHwClkVDecSwitchJpeg(void)
{
    // set mpg & jpg clock
#if defined(CC_MT5387) && !defined(CC_MT5363)        
   	IO_WRITE32(CKGEN_BASE, 0x224, 0x5);//sys 200
#elif defined(JPG_CLK_FROM_JPGDEC_CKCFG)   	
#if defined(CC_MT5398)|| defined(CC_MT5880)|| defined(CC_MT5881)
	IO_WRITE32(CKGEN_BASE, 0x394, 0x2);
#else
	IO_WRITE32(CKGEN_BASE, 0x310, 0x2);//has jpgdec clock source, 2-> 216Mhz
#endif
#elif defined(JPG_CLK_FROM_GRAPHIC)
    IO_WRITE32(CKGEN_BASE, 0x230, 0x1);//sys 240
#else                                
    IO_WRITE32(CKGEN_BASE, 0x224, 0x4);  //sys 200
#endif
}

INT32 i4JDEC_HAL_Pre_Init(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    // Set clock for Jpeg.
    vHwClkVDecSwitchJpeg();
//#ifndef CC_MT5392B
    // Power on VLD
    vHwJdecVLDPowerOn(u4JDecID,TRUE);
//#endif
    // VLD reset.
    vHwJdecHwReset(u4JDecID);

#ifndef JPG_PROGRASSIVE_V4
    // Clear SRAM
    vHwJdecSRAMClear(u4JDecID);
#endif

    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,TRUE);
    return S_JDEC_HAL_OK;
}

/// Initialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Init(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{    
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

	#ifdef JPEG_AUTO_TEST
	//init w_ptr to max value, avoid that progressive will be hanged when 0xfc-0xf8 = (0xb8-0xb4)*64, hw code issue.
	vHwJdecUpdateWptr(u4JDecID, 0xffffffff);
	#endif
	
    // Adjust this register to correctly decode.
    vHwJdecSetMiscReg(u4JDecID);

    // Set output to color buffer as raster scan mode.
    vHwJdecSetReadWriteOutputBufMode(u4JDecID);

    // Set VLD wait time out value.
    vHwJdecSetVLDWaitTimeoutValue(u4JDecID,0x3000);

#if 0
    // Variable initialization
    _prJdecHalInfo[u4JDecID] = (JDEC_HAL_INFO_T *)BSP_AllocAlignedDmaMemory(sizeof(JDEC_HAL_INFO_T),2048);
    x_memset(_prJdecHalInfo[u4JDecID],0,sizeof(JDEC_HAL_INFO_T));
#else
    if(!_fgJdecHalAllocate)
    {
        _prJdecHalInfo[0] = (JDEC_HAL_INFO_T *)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(sizeof(JDEC_HAL_INFO_T),2048));
        x_memset(_prJdecHalInfo[0],0,sizeof(JDEC_HAL_INFO_T));

        _prJdecHalInfo[1] =(JDEC_HAL_INFO_T *)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(sizeof(JDEC_HAL_INFO_T),2048));
        x_memset(_prJdecHalInfo[1],0,sizeof(JDEC_HAL_INFO_T));

#ifdef CHIPVERIFY
        _pvJdecHalNoneZeroHistoryBuf[0] = (void *)0x2bbc800;
        _pvJdecHalNoneZeroHistoryBuf[1] = (void *)0x2be4800;
        _pvJdecHalNoneZeroHistoryBuf[2] = (void *)0x2c0c800;
        x_memset(_pvJdecHalNoneZeroHistoryBuf[0],0, 0x28000);
        x_memset(_pvJdecHalNoneZeroHistoryBuf[1],0, 0x28000);
        x_memset(_pvJdecHalNoneZeroHistoryBuf[2],0, 0x28000);
        _u4JdecHalOutCoefYBufSa[0] = 0x2c34800;
        _u4JdecHalOutCoefCbBufSa[0] = 0x2eb4800;
        _u4JdecHalOutCoefCrBufSa[0] = 0x3134800;   
        x_memset((void *)_u4JdecHalOutCoefYBufSa[0], 0, 0x280000);
        x_memset((void *)_u4JdecHalOutCoefCbBufSa[0], 0, 0x280000);
        x_memset((void *)_u4JdecHalOutCoefCrBufSa[0], 0, 0x280000);
#else
        {
            UINT32 u4Comp;

            for(u4Comp=0; u4Comp<3; u4Comp++)
            {
                _pvJdecHalNoneZeroHistoryBuf[u4Comp] =
                    BSP_AllocAlignedDmaMemory(2000 * 2000/8,1);
                x_memset(_pvJdecHalNoneZeroHistoryBuf[u4Comp],0,2000 * 2000/8);
            }
        }

        _u4JdecHalOutCoefYBufSa[0] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefYBufSa[0],0,
                                   2000 * 2000 * 2);

        _u4JdecHalOutCoefCbBufSa[0] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefCbBufSa[0],0,
                                   2000 * 2000 * 2);

        _u4JdecHalOutCoefCrBufSa[0] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefCrBufSa[0],0,
                                   2000 * 2000 * 2);

        _u4JdecHalOutCoefYBufSa[1] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefYBufSa[0],0,
                                   2000 * 2000 * 2);

        _u4JdecHalOutCoefCbBufSa[1] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefCbBufSa[0],0,
                                   2000 * 2000 * 2);

        _u4JdecHalOutCoefCrBufSa[1] =
            BSP_AllocAlignedDmaMemory(2000 * 2000 * 2,2048);
        x_memset(_u4JdecHalOutCoefCrBufSa[0],0,
                                   2000 * 2000 * 2);
#endif
        _fgJdecHalAllocate = TRUE;
    }

    x_memset(_prJdecHalInfo[u4JDecID],0,sizeof(JDEC_HAL_INFO_T));
#endif

    return S_JDEC_HAL_OK;
}


/// Uninitialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
)
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

#if 0
    // Free none zero history buffer.
    if(_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
    {
        UINT32 u4Comp;

        for(u4Comp=0; u4Comp<3; u4Comp++)
        {
            if(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] != NULL)
            {
                x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp]);
                _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = NULL;
            }
        }

        if(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa != NULL)
        {
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
            _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = NULL;
        }

        if(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa != NULL)
        {
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
            _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = NULL;
        }

        if(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa != NULL)
        {
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);
            _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = NULL;
        }

    }

    x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]);
#endif


    vHwJdecSetVLDBarrelShifterToJpegMode(u4JDecID,FALSE);

    // Software reset VLD for other module
    vHwJdecHwReset(u4JDecID);

    // Power off
    vHwJdecVLDPowerOn(u4JDecID,FALSE);

    return S_JDEC_HAL_OK;
}


/// Set Jpeg decoding mode
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Decoding_Mode(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_DEC_MODE_T eDecodingMode                   ///< [IN] Jpeg decoding mode
)
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

#ifdef JPG_PROGRASSIVE_V4

    _prJdecHalInfo[u4JDecID]->eDecodingMode = eDecodingMode;

    if ((eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
        (eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
        (eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC))
    {
        vHwJdecSetNewBaseline(u4JDecID, TRUE);
    }
    else
    {
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
    }

#else
    _prJdecHalInfo[u4JDecID]->eDecodingMode = eDecodingMode;
#endif

    return S_JDEC_HAL_OK;
}


/// Set Jpeg input picture information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Pic_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_PIC_INFO_T *prPicInfo                      ///< [IN] Jpeg input picture information
)
{
    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    x_memcpy(&(_prJdecHalInfo[u4JDecID]->rPicInfo),prPicInfo,sizeof(JDEC_HAL_PIC_INFO_T));

#if 0
    // Allocate none zero history buffer when picture size is update.
    if(_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
    {
        UINT32 u4Comp;

        for(u4Comp=0; u4Comp<3; u4Comp++)
        {
            if(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] != NULL)
                x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp]);
        }

        if(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa != NULL)
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa);
        if(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa != NULL)
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa);
        if(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa != NULL)
            x_free_aligned_nc_mem(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa);

        {
            UINT32 u4Width,u4Height;
            UINT32 u4WidthPerMCU,u4HeightPerMCU;

            for(u4Comp=0; u4Comp<3; u4Comp++)
            {
                u4WidthPerMCU = (_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /
                                 _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[u4Comp].u1HSampFactor) * 8;
                u4HeightPerMCU = (_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor /
                                  _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->arSofComp[u4Comp].u1VSampFactor) * 8;
                u4Width = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU) * u4WidthPerMCU;
                u4Height = ((_prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU) * u4HeightPerMCU;
                _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] =
                    BSP_AllocAlignedDmaMemory(u4Width * u4Height/8,1);
                x_memset(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp],0,u4Width * u4Height/8);
            }
        }

        _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa =
            BSP_AllocAlignedDmaMemory(prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2,2048);
        x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa,0,
                                   prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2);

        _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa =
            BSP_AllocAlignedDmaMemory(prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2,2048);
        x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa,0,
                                   prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2);

        _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa =
            BSP_AllocAlignedDmaMemory(prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2,2048);
        x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa,0,
                                   prPicInfo->prSofInfo->u2ImageWidth * prPicInfo->prSofInfo->u2ImageHeight * 2);
    }
#else
        {
            UINT32 u4Comp;

            for(u4Comp=0; u4Comp<3; u4Comp++)
            {
                _prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp] = _pvJdecHalNoneZeroHistoryBuf[u4Comp];
                //x_memset(_prJdecHalInfo[u4JDecID]->pvNoneZeroHistoryBuf[u4Comp],0,2000 * 2000/8);
            }
        }

        _prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa = _u4JdecHalOutCoefYBufSa[u4JDecID];
//        x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefYBufSa,0,
    //                               2000 * 2000 * 2);

        _prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa = _u4JdecHalOutCoefCbBufSa[u4JDecID];
        //x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefCbBufSa,0,
            //                       2000 * 2000 * 2);

        _prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa = _u4JdecHalOutCoefCrBufSa[u4JDecID];
//        x_memset(_prJdecHalInfo[u4JDecID]->u4OutCoefCrBufSa,0,
    //                               2000 * 2000 * 2);


    {
        UINT32 u4CompId;
        JDEC_HAL_OUTPUT_ADDR_T *prOutputAddr = &(_prJdecHalInfo[u4JDecID]->rOutputAddr);
        JDEC_JFIF_SOF_T *prSofInfo = _prJdecHalInfo[u4JDecID]->rPicInfo.prSofInfo;

        for(u4CompId = 0;( (u4CompId<prSofInfo->u1NumComponents )&& (u4CompId < 3)); u4CompId++)
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

#endif

    return S_JDEC_HAL_OK;
}


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
    JPEG_JFIF_SOS_T *prScanInfo                         ///< [IN] Input Jpeg scan information
)
{
    UINT32 i;

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

//    x_memcpy(&(_prJdecHalInfo[u4JDecID]->rScanInfo),prScanInfo,sizeof(JDEC_JFIF_SOS_T));

    _prJdecHalInfo[u4JDecID]->rScanInfo.u1CompInScan = prScanInfo->u1CompInScan;

    for (i = 0; i < D_MAX_JPEG_COMPONENT; i++)
    {
        _prJdecHalInfo[u4JDecID]->rScanInfo.au1CompId[i] = prScanInfo->au1CompId[i];
        _prJdecHalInfo[u4JDecID]->rScanInfo.au1DcId[i] = prScanInfo->au1DcId[i];
        _prJdecHalInfo[u4JDecID]->rScanInfo.au1AcId[i] = prScanInfo->au1AcId[i];
    }
    
    _prJdecHalInfo[u4JDecID]->rScanInfo.u1Ss = prScanInfo->u1Ss;
    _prJdecHalInfo[u4JDecID]->rScanInfo.u1Se = prScanInfo->u1Se;
    _prJdecHalInfo[u4JDecID]->rScanInfo.u1Ah = (prScanInfo->u1AhAl & 0xf0) >> 4;
    _prJdecHalInfo[u4JDecID]->rScanInfo.u1Al = (prScanInfo->u1AhAl & 0x0f);
	
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

    x_memcpy(&(_prJdecHalInfo[u4JDecID]->rInputInfo),prInputInfo,sizeof(JDEC_HAL_INPUT_INFO_T));

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

    x_memcpy(&(_prJdecHalInfo[u4JDecID]->rOutputInfo),prOutputInfo,sizeof(JDEC_HAL_OUTPUT_INFO_T));

    return S_JDEC_HAL_OK;
}


/// Trigger Jpeg HAL decoding
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Decode(
    UINT32 u4JDecID,                                     ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_DEC_PROG_T eFlow                           ///< [IN] Jpeg flow, driver uses row buffer
)
{
    JDEC_HAL_INFO_T *prJdecHalInfo;
    INT32 i4Ret;

  //  UINT32 u4Tempval;
#ifndef JPG_PROGRASSIVE_V4
    UINT32 u4YBufSa,u4CbBufSa,u4CrBufSa;
    UINT32 u4YCoeffBufSa,u4CbCoeffBufSa,u4CrCoeffBufSa;
#endif

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;

    prJdecHalInfo = _prJdecHalInfo[u4JDecID];

#ifdef JPG_PROGRASSIVE_V4
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



#ifdef JPG_PROGRASSIVE_V4
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
                                                eFlow);
#else
	        UINT32 u4YBufSa,u4CbBufSa,u4CrBufSa;
	        UINT32 u4YCoeffBufSa,u4CbCoeffBufSa,u4CrCoeffBufSa;
	        i4HwJdecCalculateOutputBufAddr(u4JDecID,prJdecHalInfo->u4CurrMCUNum,
	                                                prJdecHalInfo->rPicInfo.prSofInfo,
	                                                &(prJdecHalInfo->rScanInfo),
	                                                &(prJdecHalInfo->rOutputInfo),
	                                                prJdecHalInfo->u4OutCoefYBufSa,
	                                                prJdecHalInfo->u4OutCoefCbBufSa,
	                                                prJdecHalInfo->u4OutCoefCrBufSa,
	                                                &u4YBufSa,
	                                                &u4CbBufSa,
	                                                &u4CrBufSa,
	                                                &u4YCoeffBufSa,
	                                                &u4CbCoeffBufSa,
	                                                &u4CrCoeffBufSa, eFlow); 
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
                ||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#ifdef JPG_PROGRASSIVE_V4
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
                UINT32 u4CleanSize;
                #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                UINT32 u4CoefBufMap = 0;
                #endif

                u4Tmp = (UINT32)(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8);
                u4Width = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageWidth + (u4Tmp - 1)) / u4Tmp) * u4Tmp;
                u4Tmp = (UINT32)(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8);
                u4Height = ((prJdecHalInfo->rPicInfo.prSofInfo->u2ImageHeight + (u4Tmp - 1)) / u4Tmp) * u4Tmp;

                // Clean none-zero history buffer
#ifdef JPG_PROGRASSIVE_V4
                if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
                    (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
#endif                
					{
    	                #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                        UINT32 u4NonzeMapAddr = 0;
                        u4NonzeMapAddr = (UINT32)BSP_MapReservedMem((void *)prJdecHalInfo->pvNoneZeroHistoryBuf[0], (u4Width * u4Height + 7)/8 * 3);
                        if(!u4NonzeMapAddr)
                        {
                            printk( "none zero dynamic map fail\n");
                            ASSERT(0);
                        }                        
                        #endif
	                for(u4Comp=0; u4Comp<3; u4Comp++)
	                {
	                    x_memset((void *)VIRTUAL((UINT32)prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp]),0,
	                                               (u4Width * u4Height + 7)/8);
	                    
	                    #if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
	                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)(prJdecHalInfo->pvNoneZeroHistoryBuf[u4Comp])), (UINT32)((u4Width * u4Height + 7)/8));
	                    #endif
	                }
                    
                        #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                        if(u4NonzeMapAddr)
                        {
                            BSP_UnMapReservedMem((void *)u4NonzeMapAddr);
                        }
                        #endif
                	}

                if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
						||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
						||(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                )
                {
                    u4CleanSize = (u4Width * u4Height) << 1;
                }
                else
                {
                    u4CleanSize = (u4Width * prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3) << 1;                    
                }
                
                #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                u4CoefBufMap = (UINT32)BSP_MapReservedMem((void *)prJdecHalInfo->u4OutCoefYBufSa, u4CleanSize * 3);
                if(!u4CoefBufMap)
                {
                    printk("coef dynamic map fail\n");
                    ASSERT(0);
                }                
                #endif
                
                // Clean coefficient buffer
                //if(!_fgJdecHalResume)
                    x_memset((void*)VIRTUAL((UINT32)prJdecHalInfo->u4OutCoefYBufSa),0,
                                           u4CleanSize);

                //if(!_fgJdecHalResume)
                    x_memset((void*)VIRTUAL((UINT32)prJdecHalInfo->u4OutCoefCbBufSa),0,
                                           u4CleanSize);

                //if(!_fgJdecHalResume)
                    x_memset((void*)VIRTUAL((UINT32)prJdecHalInfo->u4OutCoefCrBufSa),0,
                                           u4CleanSize);
                
                #if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((prJdecHalInfo->u4OutCoefYBufSa)), (UINT32)u4CleanSize);
                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((prJdecHalInfo->u4OutCoefCbBufSa)), (UINT32)u4CleanSize);
                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((prJdecHalInfo->u4OutCoefCrBufSa)), (UINT32)u4CleanSize);                
                #else
                    HalFlushInvalidateDCache();
                #endif
                
                #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                if(u4CoefBufMap)
                {
                    BSP_UnMapReservedMem((void *)u4CoefBufMap);
                }
                #endif
                
            }
        }

        // 1. Set Decodeing Mode to HW
        vHwJdecSetDecodeMode(u4JDecID,prJdecHalInfo->eDecodingMode);

        // 2. Set Picture Information to HW
        // Set picture information
        switch((ENUM_JPEG_FORMAT_T)prJdecHalInfo->rPicInfo.prSofInfo->eJpegFormat)
        {
            case E_JPG_BASELINE:
            case E_JPG_EX_SEQ_HUFFMAN:
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
                                           (UINT8 *)((((UINT32)prJdecHalInfo->au1HwDcHuffTbl+15)>>4)<<4),
                                           (UINT8 *)((((UINT32)prJdecHalInfo->au1HwAcHuffTbl+15)>>4)<<4))) < 0)
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

        // 4. Set Input Buffer Information to HW
        vHwJdecSetInputBuffer(u4JDecID,prJdecHalInfo->rInputInfo.u4InFifoSa,prJdecHalInfo->rInputInfo.u4InFifoSz);

    #ifdef JPEG_VLD_TO_PARSE_HEADER
    #else   /*JPEG_VLD_TO_PARSE_HEADER*/
        if(!prJdecHalInfo->fgReinit)
            i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rInputInfo.u4BitstreamSa,0); // Set VLD read pointer to bitstream start address.
        else
            i4HwJdecSetBarrelShifterAddr(u4JDecID,prJdecHalInfo->rHwStatus.u4ReadAddrBytes,
                                         prJdecHalInfo->rHwStatus.u4ReadAddrBits);
    #endif  /*JPEG_VLD_TO_PARSE_HEADER*/
        

        // 5. Set Output Buffer Information to HW
#ifdef JPG_PROGRASSIVE_V4
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

#else
        i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                        prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                        &(prJdecHalInfo->rScanInfo),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE),
                                        (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE));
#endif

        if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
           || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        {
            i4HwJdecSetCoefPitchPerRow(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo);
        }

    }

#ifdef JPG_PROGRASSIVE_V4
    if(prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE ||
       prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
    	{
        	vHwJdecForNewEnhance(u4JDecID, TRUE, TRUE);
    	}
#endif


    // Scan info update
    if(prJdecHalInfo->fgStartOfScan || prJdecHalInfo->fgReinit)
    {
        // Initialize VLD marker_found and encounter_marker register
        vHwJdecInitScan(u4JDecID);
    
        // 2. Set Picture Information to HW
        if((i4Ret = i4HwJdecSetHuffmanTable(u4JDecID,prJdecHalInfo->rPicInfo.prHuffTblInfo,
                                           (UINT8 *)((((UINT32)prJdecHalInfo->au1HwDcHuffTbl+15)>>4)<<4),
                                           (UINT8 *)((((UINT32)prJdecHalInfo->au1HwAcHuffTbl+15)>>4)<<4))) < 0)
            return i4Ret;
        i4HwJdecSetCurrUsedQuantTable(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo,&(prJdecHalInfo->rScanInfo));

        // 3. Set Scan Information to HW
        // Set Ah, Al, Ss, Se information
        vHwJdecSetAhAl(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Al);
        vHwJdecSetDcAc(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ah,(UINT32)prJdecHalInfo->rScanInfo.u1Ss);
        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#ifdef JPG_PROGRASSIVE_V4
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
#endif
        )
        {
            if ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))
                vHwJdecSetLastScan(u4JDecID,TRUE);
            else
                vHwJdecSetLastScan(u4JDecID,FALSE);
        } 
		 else
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
        /*
        if ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0)) // Last scan
            i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                            prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                            prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                            TRUE,TRUE);
		*/
        if (((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))// Last scan
	         || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) //for enhance every scan is last scan
#ifdef JPG_PROGRASSIVE_V4	         
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
             || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif             
	      ) 
        {
                 i4HwJdecSetPitchPerRow(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutYBufPitch,
                                                 prJdecHalInfo->rOutputInfo.u4OutCbBufPitch,
                                                 prJdecHalInfo->rOutputInfo.u4OutCrBufPitch,
                                                 &(prJdecHalInfo->rScanInfo),
                                                 TRUE, TRUE);
        }

        // 6. HW related setting
        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)         
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
           || (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
           )        
        	{
        		vHwJdecSetNoneZeroHistoryBuf(u4JDecID,(UINT32)(prJdecHalInfo->pvNoneZeroHistoryBuf[prJdecHalInfo->rScanInfo.au1CompId[0]]));
        	}
        
        if(!prJdecHalInfo->fgReinit)
            prJdecHalInfo->u4CurrMCUNum = 0;
        vHwJdecCleanInterBuf(u4JDecID,prJdecHalInfo->fgStartOfScan);

        
#ifdef JPG_PROGRASSIVE_V4
        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            i4HwJdecSetCoefMCUSize(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));

            i4HwJdecSetCoefPitch(u4JDecID,prJdecHalInfo->rPicInfo.prSofInfo, prJdecHalInfo->rPicInfo.prSofInfo->arSofComp,&(prJdecHalInfo->rScanInfo));
            
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
    #ifdef JPG_PROGRASSIVE_V4
	    {
	        if((prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) &&
	           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) &&
	           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) &&
	           (prJdecHalInfo->eDecodingMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
	        {
	            if((prJdecHalInfo->rScanInfo.u1Ah == 0) && (prJdecHalInfo->rScanInfo.u1Ss != 0) && // AC first
	               (prJdecHalInfo->u4EOBRUN != 0) && // EOBRUN
	               ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Last Scan
	            	{
	                    vHwJdecSetSsSe(u4JDecID,64,63);
	            	}	                    
		     		else //sun modified
		     		{
			  			vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
		     		}
	        }
	        else
	        {
	            vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
	        }
	    }
    #else
	    if((prJdecHalInfo->rScanInfo.u1Ah == 0) && (prJdecHalInfo->rScanInfo.u1Ss != 0) && // AC first
	       (prJdecHalInfo->u4EOBRUN != 0) && // EOBRUN
	       ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Last Scan
	        vHwJdecSetSsSe(u4JDecID,64,63);
	    else
	        vHwJdecSetSsSe(u4JDecID,(UINT32)prJdecHalInfo->rScanInfo.u1Ss,(UINT32)prJdecHalInfo->rScanInfo.u1Se);
    #endif
    
    // 5. Set Output Buffer Information to HW
    {

        #ifdef JPG_PROGRASSIVE_V4
        BOOL fgLastScan = FALSE;

        if (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT
            || prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
        {
            if ((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))
            {
                fgLastScan = TRUE;
                //Printf("fgLastScan [%d][%d]\n", prJdecHalInfo->rScanInfo.au1CompId[0], prJdecHalInfo->rOutputAddr.u4MCUy);
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
		 #endif

		 #ifdef JPG_PROGRASSIVE_V4
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
                                                eFlow);		 
		 #else
        i4HwJdecCalculateOutputBufAddr(u4JDecID,prJdecHalInfo->u4CurrMCUNum,
                                                prJdecHalInfo->rPicInfo.prSofInfo,
                                                &(prJdecHalInfo->rScanInfo),
                                                &(prJdecHalInfo->rOutputInfo),
                                                prJdecHalInfo->u4OutCoefYBufSa,
                                                prJdecHalInfo->u4OutCoefCbBufSa,
                                                prJdecHalInfo->u4OutCoefCrBufSa,
                                                &u4YBufSa,
                                                &u4CbBufSa,
                                                &u4CrBufSa,
                                                &u4YCoeffBufSa,
                                                &u4CbCoeffBufSa,
                                                &u4CrCoeffBufSa, eFlow);
		#endif

        
#ifndef JPG_PROGRASSIVE_V4
        if ((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) && // Multi-collect mode
            !((prJdecHalInfo->rScanInfo.u1Se == 63) && (prJdecHalInfo->rScanInfo.u1Al == 0))) // Not Last Scan
        {
            vHwJdecSetOutputBufAddr(u4JDecID,u4YCoeffBufSa,
                                             u4CbCoeffBufSa,
                                             u4CrCoeffBufSa,
                                             &(prJdecHalInfo->rScanInfo));
        } 
        else
        {
            vHwJdecSetOutputBufAddr(u4JDecID,u4YBufSa,
                                             u4CbBufSa,
                                             u4CrBufSa,
                                             &(prJdecHalInfo->rScanInfo));
        }

        if((prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
           (prJdecHalInfo->eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
        {
            vHwJdecSetOutputCoefBufAddr(u4JDecID,u4YCoeffBufSa,
                                                 u4CbCoeffBufSa,
                                                 u4CrCoeffBufSa,
                                                 &(prJdecHalInfo->rScanInfo));
        }
#endif
    }

    switch(prJdecHalInfo->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
            {
                vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
                                                      prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
                vHwJdecSetMCUWidth(u4JDecID, (UINT32)((prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8),
                                             (UINT32)((prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8),
                                             (UINT32)((prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8));
            }
            break;
            
#ifdef JPG_PROGRASSIVE_V4
       case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:

            #ifdef JPG_PROGRASSIVE_VFY_FRAME_CKSUM            
            {
					if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
					{
						switch(prJdecHalInfo->rScanInfo.au1CompId[0])
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
								
								if (0)//(prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
								{
									prJdecHalInfo->rOutputAddr.u4MCUy++;
								}
							}
							else  // not the first row
							{
								if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
								{
									// setup bank 0 for row 2, 4, 6, 8...
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
								                   prJdecHalInfo->rOutputAddr.u4YBufSa,
								                   prJdecHalInfo->rOutputAddr.u4YBufSa);
								}
								else
								{
									// setup bank 1 for row 1, 3, 5, 7...
									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
								                   prJdecHalInfo->rOutputAddr.u4YBufSa,
								                   prJdecHalInfo->rOutputAddr.u4YBufSa);              
								}
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
									
									if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
									{
										prJdecHalInfo->rOutputAddr.u4MCUy++;
									}
								}
								else  // not the first row
								{
									if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
									{
										vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
									                prJdecHalInfo->rOutputAddr.u4CbBufSa,
									                prJdecHalInfo->rOutputAddr.u4CbBufSa);
									}
									else
									{
										vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CbBufSa,
									                prJdecHalInfo->rOutputAddr.u4CbBufSa,
									                prJdecHalInfo->rOutputAddr.u4CbBufSa);             
									}
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
									
									if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
									{
									prJdecHalInfo->rOutputAddr.u4MCUy++;
									}
								}
								else  // not the first row
								{
									if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
									                     prJdecHalInfo->rOutputAddr.u4CrBufSa,
									                     prJdecHalInfo->rOutputAddr.u4CrBufSa);
									else
									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4CrBufSa,
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

							if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
							{
								prJdecHalInfo->rOutputAddr.u4MCUy++;
							}
						}
						else // not the first row
						{
							if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
							{
								vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
							                        prJdecHalInfo->rOutputAddr.u4CbBufSa,
							                        prJdecHalInfo->rOutputAddr.u4CrBufSa);
							}
							else
							{
								vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputAddr.u4YBufSa,
							                       prJdecHalInfo->rOutputAddr.u4CbBufSa,
							                       prJdecHalInfo->rOutputAddr.u4CrBufSa);                          
							}
						}
						
						vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
						            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
						            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
					}
            }
            #else
{
					if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
					{
						switch(prJdecHalInfo->rScanInfo.au1CompId[0])
						{
							case 0:
							if( prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
							{
								vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
								                    prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
								                    prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa);

								//prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch*8;

								vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
								                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
								                              prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa);
								
								if (0)//(prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
								{
									prJdecHalInfo->rOutputAddr.u4MCUy++;
								}
							}
							else  // not the first row
							{
								if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
								{
									// setup bank 0 for row 2, 4, 6, 8...
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
								                   prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
								                   prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa);
								}
								else
								{
									// setup bank 1 for row 1, 3, 5, 7...
									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
								                   prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
								                   prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa);              
								}
							}     
							
							break;
							
							case 1:
								if( prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
								{
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa);

									//prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch*8;

									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa);
									
									if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
									{
										prJdecHalInfo->rOutputAddr.u4MCUy++;
									}
								}
								else  // not the first row
								{
									if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
									{
										vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
									                prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
									                prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa);
									}
									else
									{
										vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
									                prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
									                prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa);             
									}
								}    
								
								break;
							case 2:
								if(prJdecHalInfo->rOutputAddr.u4MCUy == 0 )  //the first row
								{
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);

									prJdecHalInfo->rOutputAddr.u4CrBufSa +=  prJdecHalInfo->rOutputInfo.u4OutCrBufPitch*8;

									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
									                 prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);
									
									if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
									{
									prJdecHalInfo->rOutputAddr.u4MCUy++;
									}
								}
								else  // not the first row
								{
									if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
									vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
									                     prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa,
									                     prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
									else
									vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
									                       prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa,
									                       prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);             
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
							vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
							                           prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
							                           prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
                                                /*    
							prJdecHalInfo->rOutputAddr.u4YBufSa +=  prJdecHalInfo->rOutputInfo.u4OutYBufPitch
							                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8);
							prJdecHalInfo->rOutputAddr.u4CbBufSa += prJdecHalInfo->rOutputInfo.u4OutCbBufPitch
							                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor * 8);
							prJdecHalInfo->rOutputAddr.u4CrBufSa += prJdecHalInfo->rOutputInfo.u4OutCrBufPitch
							                                        *(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor * 8);
                                                */
							vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
							                          prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
							                          prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);

							if(0)// (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
							{
								prJdecHalInfo->rOutputAddr.u4MCUy++;
							}
						}
						else // not the first row
						{
							if((prJdecHalInfo->rOutputAddr.u4MCUy)%2)
							{
								vHwJdecSetOutputBank0BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank0YBufSa,
							                        prJdecHalInfo->rOutputInfo.u4OutBank0CbBufSa,
							                        prJdecHalInfo->rOutputInfo.u4OutBank0CrBufSa);
							}
							else
							{
								vHwJdecSetOutputBank1BufAddr(u4JDecID,prJdecHalInfo->rOutputInfo.u4OutBank1YBufSa,
							                       prJdecHalInfo->rOutputInfo.u4OutBank1CbBufSa,
							                       prJdecHalInfo->rOutputInfo.u4OutBank1CrBufSa);                          
							}
						}
						
						vHwJdecSetMCUWidth(u4JDecID,(prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*8,
						            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor)*8,
						            (prJdecHalInfo->rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor)*8);
					}
            }

            #endif
            break;
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        {
                if(prJdecHalInfo->rScanInfo.u1CompInScan == 1)
                {
                    switch(prJdecHalInfo->rScanInfo.au1CompId[0])
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
                    switch(prJdecHalInfo->rScanInfo.au1CompId[0])
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
    #ifdef JPG_PROGRASSIVE_V4
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

#ifdef JPG_PROGRASSIVE_V4
	vJDECErrorConceal(u4JDecID, TRUE, TRUE,TRUE);
#endif
#if 0

    for (u4Tempval = 0; u4Tempval < 37; u4Tempval++)
    {
        //x_thread_delay(5);
        Printf("[JPG]  decoding Reg 0x%08x = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
               (0x140b0 + (u4Tempval*16 + 0)),
               IO_READ32(0xf00140b0, (u4Tempval*16 + 0)),
               IO_READ32(0xf00140b0, (u4Tempval*16 + 4)),
               IO_READ32(0xf00140b0, (u4Tempval*16 + 8)),
               IO_READ32(0xf00140b0, (u4Tempval*16 + 12)));
    }

#endif
  
    // 7. Trigger Decoding
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
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:        	
            vHwJdecDecodePicture(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            vHwJdecDecodeMCURow(u4JDecID,prJdecHalInfo->fgStartOfScan);
            prJdecHalInfo->fgStartOfScan = FALSE;
            break;
         default:
            break;
    }

    // 8. Status Update
    prJdecHalInfo->fgDecoded = TRUE;
    prJdecHalInfo->fgReinit = FALSE;
    switch(prJdecHalInfo->eDecodingMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            prJdecHalInfo->u4CurrMCUNum++;
			 break;
#ifdef JPG_PROGRASSIVE_V4
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            prJdecHalInfo->u4CurrMCUNum += prJdecHalInfo->rOutputAddr.u4MCUWidth;
            prJdecHalInfo->rOutputAddr.u4MCUx = 0;
            //prJdecHalInfo->rOutputAddr.u4MCUy++;
            if (prJdecHalInfo->rOutputAddr.u4CombMCUWidth > 0)//Temp Solution!
            {
            prJdecHalInfo->rOutputAddr.u4MCUy++;
            }
            break;
#endif            
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
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            fgFinish = fgHwJdecIsDecodeMCURowFinish(u4JDecID);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
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

    if(u4JDecID >= HW_JDEC_VLD_NUM)
        return E_JDEC_HAL_FAIL;
    if(pu4AddrBytes == NULL)
        return E_JDEC_HAL_FAIL;
    if(pu4AddrBits == NULL)
        return E_JDEC_HAL_FAIL;

    u4Bytes = u4VDecReadVldRPtr(0,u4JDecID,&u4Bits,PHYSICAL(_prJdecHalInfo[u4JDecID]->rInputInfo.u4InFifoSa));

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
    x_memcpy(&(prJdecHalInfo->rHwStatus),prHwStatus,sizeof(JDEC_HAL_HW_STATUS_T));

    return S_JDEC_HAL_OK;
}


//extern UINT32 _MPV_VldRead32(UCHAR ucMpvId, UINT16 u2Offset);
extern INLINE UINT32 u4HwJdecRead32(UINT32 u4HwId, UINT32 u4Offset);

UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, 
UINT32 u4VFIFOSa)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
#ifdef CC_MT5363
    UINT32 u4Reg;
    u4Reg = u4HwJdecRead32(u4VDecID, RO_VLD_VBAR + (u4BSID << 10));
    u4SramRptr =  ((u4Reg & 0xf) << 2) + ((u4Reg >> 24) & 0x3);
    u4SramWptr = ((u4Reg >> 8) & 0xf) << 2;
#else      
    u4SramRptr = u4HwJdecRead32(u4VDecID, (UINT32)(RO_VLD_VBAR + (u4BSID << 10))) & 0xff;
    u4SramWptr = (u4HwJdecRead32(u4VDecID, (UINT32)(RO_VLD_VBAR + (u4BSID << 10))) >> 16) & 0xff;
#endif
    u4DramRptr = u4HwJdecRead32(u4VDecID, (UINT32)(RO_VLD_VRPTR + (u4BSID << 10)));
    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
    }
  
    *pu4Bits = u4HwJdecRead32(u4VDecID, (UINT32)(RW_VLD_BITCOUNT + (u4BSID << 10))) & 0x3f;
  
    if (u4HwJdecRead32(u4VDecID, (UINT32)(RW_VLD_WMV_ABS + (u4BSID << 10))) & 0x1) // 03 replacement enable!
    {
        u4ByteAddr = u4DramRptr - (u4SramDataSz + 4) * 4 + (*pu4Bits / 8);
    }
    else
    {
        u4ByteAddr = u4DramRptr - (u4SramDataSz + 3)* 4 + (*pu4Bits / 8);
    }
    *pu4Bits &= 0x7;
    
    u4ByteAddr -= ((UINT32)u4VFIFOSa);

    return (u4ByteAddr);
}


INT32 i4JDEC_HAL_Set_Buf(UINT32 u4JDecID, VOID **paCoefBuf, VOID **paNzBuf)
{
    _u4JdecHalOutCoefYBufSa[u4JDecID] = (UINT32)paCoefBuf[0];
    _u4JdecHalOutCoefCbBufSa[u4JDecID] = (UINT32)paCoefBuf[1];
    _u4JdecHalOutCoefCrBufSa[u4JDecID] = (UINT32)paCoefBuf[2];
    _pvJdecHalNoneZeroHistoryBuf[0] = paNzBuf[0];
    _pvJdecHalNoneZeroHistoryBuf[1] = paNzBuf[1];    
    _pvJdecHalNoneZeroHistoryBuf[2] = paNzBuf[2];    
    _prJdecHalInfo[0] = (JDEC_HAL_INFO_T *)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(sizeof(JDEC_HAL_INFO_T),2048));
    x_memset(_prJdecHalInfo[0],0,sizeof(JDEC_HAL_INFO_T));
    _fgJdecHalAllocate = TRUE;
    return 0;
}

INT32 i4JDEC_HAL_Unset_Buf(UINT32 u4JDecID)
{
    _u4JdecHalOutCoefYBufSa[u4JDecID] = 0;
    _u4JdecHalOutCoefCbBufSa[u4JDecID] = 0;
    _u4JdecHalOutCoefCrBufSa[u4JDecID] = 0;
    _pvJdecHalNoneZeroHistoryBuf[0] = 0;
    _pvJdecHalNoneZeroHistoryBuf[1] = 0;    
    _pvJdecHalNoneZeroHistoryBuf[2] = 0;    
    BSP_FreeAlignedDmaMemory((UINT32)_prJdecHalInfo[0]);
    _fgJdecHalAllocate = FALSE;
    return 0;
}
