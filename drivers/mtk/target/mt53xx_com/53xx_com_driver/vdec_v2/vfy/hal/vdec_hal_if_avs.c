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
#include "vdec_hw_common.h"
#include "vdec_hal_if_avs.h"
#include "vdec_hw_avs.h"
#include "vdec_hal_errcode.h"

#ifdef CONFIG_TV_DRV_VFY
#include "bd_drv_common.h"
#include "x_hal_5381.h"
#include "x_hal_926.h"
#else
#include "x_hal_ic.h"
#include "x_hal_1176.h"
#endif //CONFIG_TV_DRV_VFY

#include "x_debug.h"


#if CONFIG_DRV_VERIFY_SUPPORT
#include "vdec_verify_general.h"
#include "vdec_verify_mpv_prov.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include "x_printf.h"
#endif

#if VDEC_SUPPORT_IOMMU 
#include "vdec_hal_if_common.h"
#endif

extern BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO);
#endif

// **************************************************************************
// Function : INT32 i4VDEC_HAL_AVS_InitVDecHW(UINT32 u4Handle, VDEC_INFO_AVS_INIT_PRM_T *prAVSVDecInitPrm);
// Description :Initialize video decoder hardware only for AVS
// Parameter : u4VDecID : video decoder hardware ID
//                  prAVSVDecInitPrm : pointer to VFIFO info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_AVS_InitVDecHW(UINT32 u4VDecID, VDEC_INFO_AVS_VFIFO_PRM_T *prAVSVDecInitPrm)
{
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8555)
    vVDecResetHW(u4VDecID);
#else
    vVDecResetHW(u4VDecID, VDEC_FMT_AVS);
#endif

#if VDEC_SUPPORT_IOMMU
    vDEC_HAL_COMMON_IOMMUInit(VDEC_FMT_AVS);
    vDEC_HAL_COMMON_MMUFlush();
  
    vDEC_HAL_COMMON_PrintfMMUStatus();
#endif

#if AVS_CRC_GENERATE || AVS_CRC_COMPARE_SUPORT
    vVDecEnableCRC(u4VDecID, 1, 0);
#endif

    return  HAL_HANDLE_OK;
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_AVS_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read barrel shifter after shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window after shifting
// **************************************************************************
UINT32 u4VDEC_HAL_AVS_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal;

    u4RegVal = u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

    return (u4RegVal);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_AVS_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_AVS_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal0;

    u4RegVal0 = u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, 0);
    u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

    return (u4RegVal0);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_AVS_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Most significant (32 - u4ShiftBits) bits of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_AVS_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal0;

    u4RegVal0 = u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, 0);
    u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

    return (u4RegVal0 >> (32-u4ShiftBits));
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_AVS_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_AVS_BS_INIT_PRM_T *prAVSBSInitPrm);
// Description :Initialize barrel shifter with byte alignment
// Parameter :u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 prAVSBSInitPrm : pointer to AVS initialize barrel shifter information struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_AVS_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_AVS_BS_INIT_PRM_T *prAVSBSInitPrm)
{
    BOOL fgInitBSResult;

#if (CONFIG_DRV_VERIFY_SUPPORT) && (CONFIG_DRV_LINUX)
    //HalFlushInvalidateDCache();
#endif

    if (u4BSID == 0)
    {
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8555)
        vVDecResetHW(u4VDecID);
#else
        vVDecResetHW(u4VDecID, VDEC_FMT_AVS);
#endif
        fgInitBSResult = fgInitAVSBarrelShift1(u4VDecID, prAVSBSInitPrm);
    }
    else
    {
        fgInitBSResult = fgInitAVSBarrelShift2(u4VDecID, prAVSBSInitPrm);
    }

    if (fgInitBSResult)
    {
        return HAL_HANDLE_OK;
    }
    else
    {
        return INIT_BARRELSHIFTER_FAIL;
    }
}


void vVDEC_HAL_AVS_BitStuff_Check(UINT32 u4VDecID, UINT32 u4Val)
{
   vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_BITSTUFFING_SWITCH, u4Val);
}

// **************************************************************************
// Function : UINT32 u4VDEC_HAL_AVS_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits);
// Description :Read current read pointer
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 pu4Bits : read pointer value with remained bits
// Return      : Read pointer value with byte alignment
// **************************************************************************
UINT32 u4VDEC_HAL_AVS_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFIFOSa, UINT32 *pu4Bits)
{
    return u4VDecReadAVSVldRPtr(u4BSID, u4VDecID, pu4Bits, HW_ACCESSED_MEM(u4VFIFOSa));
}



UINT32 u4VDEC_AvsUeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{
    if (u4BSID == 0)
    {
        return u4VDecReadAVSVLD(u4VDecID, RO_AVS_VLD_UE);
    }
    else
    {
        return u4VDecReadAVSVLD(u4VDecID, RO_AVLD_2ND_UE);
    }
}

UINT32 u4VDEC_AvsSeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Temp;

    u4Temp = u4VDecReadAVSVLD(u4VDecID, RO_AVS_VLD_SE);

    return(u4Temp);
}

UINT32 u4VDEC_AVS_Search_SC(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4RetryNum = 0;
    UINT32 u4Temp = 1;

    vVDecWriteAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE, 1);
    UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
    while (1)
    {
        while(u4VDecReadAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE))
        {
           if (u4RetryNum == 0x100000)
           {
              UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SC, timeout\n", u4BSID);
              u4Temp = 0;
              break;
           }
           u4RetryNum++;
        }
        // Shift 0x000001 for read pointer
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(u4BSID, u4VDecID, 0);
        if (((u4Temp >> 8) == 0x000001) &&
            ((u4Temp & 0x000000FF) <= 0xB7))
        {
            #if VDEC_TEST_LOG
            UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SC, find SC: 0x%x\n", u4BSID, u4Temp);
            #endif
            
            //Find Start Code.
            break;
        }

        //Search Next Start Code.
        vVDecWriteAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE, 1);
        UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
    }

    return(u4Temp);
}

UINT32 u4VDEC_AVS_Search_SliceSC(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4RetryNum = 0;
    UINT32 u4Temp = 1;
    UINT32 u4Ret = TRUE;

    vVDecWriteAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE, 1);
#if DEBUG_DUMP_REG_ENABLE
    UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
#endif
    while (1)
    {
        while(u4VDecReadAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE))
        {
           if (u4RetryNum == 0x100000)
           {
              UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SliceSC, timeout\n", u4BSID);
              u4Ret = FALSE;
              break;
           }
           u4RetryNum++;
        }
        // Shift 0x000001 for read pointer
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(u4BSID, u4VDecID, 0);
        if (((u4Temp >> 8) == 0x000001) &&
            ((u4Temp & 0x000000FF) <= 0xAF))
        {
            //Find Slice Start Code.
            #if DEBUG_DUMP_REG_ENABLE
            UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SliceSC, find SC: 0x%x\n", u4BSID, u4Temp);
            #endif
            break;
        }
        else
        {
            #if DEBUG_DUMP_REG_ENABLE
            UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SliceSC, cannot find SC: 0x%x\n", u4BSID, u4Temp);
            #endif
        }
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(u4BSID, u4VDecID, 8);
        //Search Next Start Code.
        vVDecWriteAVSVLD(u4VDecID, RW_AVS_FIND_STARTCODE, 1);
#if DEBUG_DUMP_REG_ENABLE
        UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
#endif
    }

    return(u4Ret);
}

void vVDEC_HAL_AVS_HW_Reset(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4RegVal;
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 1);
    mb();
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
    u4RegVal = (u4VDecReadVLD(u4VDecID, RW_VLD_VDOUFM) & 0xFFFFF7FF) | (0x1 << 11);
    vVDecWriteVLD(u4VDecID, RW_VLD_VDOUFM, u4RegVal);
}

INT32 i4VDEC_HAL_AVS_SetBSInfo(UINT32 u4BSID, UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *prDecParam, VDEC_INFO_AVS_BS_INIT_PRM_T rAvsBSInitPrm)
{
    #if 0//def WAIT_SRAM_STABLE
    UINT32 u4Cnt = 0;
    #endif
    UINT32 u4Ret;
    
    #if DTV_COMMON_CONFIG
    //vVDecWriteAVSVLD(u4InstID, RW_AVS_VLD_CTRL, 0x1);
    
    vVDecWriteAVSVLD(u4InstID, RW_AVS_VLD_CTRL, 0x7);
    vVDecWriteVLDTOP(u4InstID, RW_VLD_TOP_ERR_RECONCEALMENT,  ((u4VDecReadVLDTOP(u4InstID, RW_VLD_TOP_ERR_RECONCEALMENT) & 0xCFFFFFFF) | 0x20000000));
    vVDecWriteVLDTOP(u4InstID, RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH,  ((u4VDecReadVLDTOP(u4InstID, RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH) & 0xFFFFFFFE)));
    vVDecWriteVLDTOP(u4InstID, RW_VLD_TOP_MODE_SWITCH,  ((u4VDecReadVLDTOP(u4InstID, RW_VLD_TOP_MODE_SWITCH) & 0xFFFFFFCF)));
    #else
    vVDecWriteAVSVLD(u4InstID, RW_AVS_VLD_CTRL, 0x8007);
    vVDecWriteAVSVLD(u4InstID, RW_AVS_VLD_MODE_SWITCH, 7);
    #endif

    // Set for VLD boundary check 0x5000000
    if(u4BSID == 1)
    {
    #if 0
        _VDEC_AvsVLDHWReset2(FALSE);
        while(!_VDEC_AvsResetVLD2(HW_ACCESSED_MEM(u4RDPtrAddr), HW_ACCESSED_MEM(u4WPtrAddr),
            HW_ACCESSED_MEM(u4BsFifoStart), HW_ACCESSED_MEM(u4BsFifoEnd)))
        {
            _VDEC_AvsVLDHWReset2(TRUE);
        }
    #endif
    }
    else
    {
        #if 0//def WAIT_SRAM_STABLE
#ifdef CC_AVS_EMULATION
        while(!_VDEC_AvsResetVLD1(prDecParam, HW_ACCESSED_MEM(u4RDPtrAddr), HW_ACCESSED_MEM(u4WPtrAddr),
            HW_ACCESSED_MEM(u4BsFifoStart), HW_ACCESSED_MEM(u4BsFifoEnd), HW_ACCESSED_MEM(0), u1ECCMode, u4ECType))
#else
        while(!_VDEC_AvsResetVLD1(prDecParam, HW_ACCESSED_MEM(u4RDPtrAddr), HW_ACCESSED_MEM(u4WPtrAddr),
            HW_ACCESSED_MEM(u4BsFifoStart), HW_ACCESSED_MEM(u4BsFifoEnd), HW_ACCESSED_MEM(0), u1ECCMode, u4ECType))
#endif

        UNUSED(u4Cnt);
        #else
        u4Ret = i4VDEC_HAL_AVS_InitBarrelShifter(u4BSID, u4InstID, &rAvsBSInitPrm);
        if( u4Ret != HAL_HANDLE_OK)
        {
            UTIL_Printf( "Barrel Shifter1 Init failed");
        }
        #endif
    }

    return 0;
}

static void vVDEC_HAL_AVS_SetMvReg(UINT32 u4VDecID, const VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm)
{
    INT32 ai4BlkDistFw[4] = {0};
    INT32 ai4BlkDistBw[2] = {0};
    INT32 ai4DirectRatio[4] = {0};
    INT32 ai4MedRatioFw[4] = {0};
    INT32 ai4MedRatioBw[4] = {0};

    INT32 i4FwBlkDist1_0 = 0;
    INT32 i4FwBlkDist3_2 = 0;
    INT32 i4BwBlkDist1_0 = 0;
    INT32 i4FwMedRatio1_0 = 0;
    INT32 i4FwMedRatio3_2 = 0;
    INT32 i4BwMedRatio1_0 = 0;
    INT32 i4DirectRatio1_0 = 0;
    INT32 i4DirectRatio3_2 = 0;
    INT32 i = 0;
    UINT32 u4TopMvBufAddr, u4BottomMvBufAddr;
    VDEC_INFO_AVS_SEQ_HDR_T* prSeqHdr = prVDecAvsDecPrm->prSeqHdr;
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;

    u4TopMvBufAddr = PHYSICAL(prVDecAvsDecPrm->rAvsWorkBufSa.u4Mv1);
    u4BottomMvBufAddr = PHYSICAL(prVDecAvsDecPrm->rAvsWorkBufSa.u4Mv2);
    if ((!u4TopMvBufAddr) || (!u4BottomMvBufAddr))
    {
        UTIL_Printf( "//_VDEC_AvsSetMvReg NULL motion vector address\n");
        return;
    }

    if(prPicHdr->u4PBFieldEnhancedFlag)
    {
        vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_CTRL, u4VDecReadAVSMV(u4VDecID, RW_AVS_MV_CTRL) |AVS_PB_FIELD_ENHANCED);
    }

#if TOP_BOT_OUTPUT_SUPPORT
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_CTRL, u4VDecReadAVSMV(u4VDecID, RW_AVS_MV_CTRL) |AVS_TOP_BOT_INVERSE);
#if KUVISION_TOP_BOT_OUTPUT_SUPPORT
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_CTRL, u4VDecReadAVSMV(u4VDecID, RW_AVS_MV_CTRL) |AVS_BOT_FIELD_FIRST);
#endif
#endif

    if (prPicHdr->u4PicStruct != FRM_PIC)
    {
        if (prPicHdr->u4TFT)
        {
            if (!prPicHdr->fgSecField)
            {
                vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4TopMvBufAddr >> 4);
#if VDEC_TEST_LOG
                UTIL_Printf("//MV_TOPBufAddr >> 4\n");
#endif
            }
            else
            {
                if((prSeqHdr->u4ProfileID == GUANGBO) 
                     && (prPicHdr->u4PBFieldEnhancedFlag == 1) 
                     && (prPicHdr->u4PicCodingType == B_PIC)) //B frame
                {
                    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4TopMvBufAddr >> 4);
#if VDEC_TEST_LOG
                    UTIL_Printf("//MV_TOPBufAddr >> 4\n");
#endif
                }
                else
                {
                    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4BottomMvBufAddr >> 4);
#if VDEC_TEST_LOG
                    UTIL_Printf("//MV_BottomBufAddr >> 4\n");
#endif
                }
            }
        }
        else
        {
            if (!prPicHdr->fgSecField)
            {
                if((prSeqHdr->u4ProfileID == GUANGBO) 
                    && (prPicHdr->u4PBFieldEnhancedFlag == 1)
                    && (prPicHdr->u4PicCodingType == B_PIC))
                {
                    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4TopMvBufAddr >> 4);
#if VDEC_TEST_LOG
                    UTIL_Printf("//MV_TOPBufAddr >> 4\n");
#endif
                }
                else
                {
                    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4BottomMvBufAddr >> 4);
#if VDEC_TEST_LOG
                    UTIL_Printf("//MV_BottomBufAddr >> 4\n");
#endif
                }
            }
            else
            {
                vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4TopMvBufAddr >> 4);
#if VDEC_TEST_LOG
                UTIL_Printf("//MV_TOPBufAddr >> 4\n");
#endif
            }
        }
    }
    else
    {
        vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_ADDR, u4TopMvBufAddr >> 4);
#if VDEC_TEST_LOG
        UTIL_Printf("//MV_TOPBufAddr >> 4\n");
#endif
    }

    //x_memset(ai4BlkDistFw, 0, sizeof(INT32) * 4);
    //x_memset(ai4BlkDistBw, 0, sizeof(INT32) * 2);
    //x_memset(ai4DirectRatio, 0, sizeof(INT32) * 4);
    //x_memset(ai4MedRatioFw, 0, sizeof(INT32) * 4);
    //x_memset(ai4MedRatioBw, 0, sizeof(INT32) * 4);

    // If the decoding source is the bottom field or the second field,
    // the index distance should be picture distance * 2 + 1.
    // Otherwise, the index distance should be picture distance * 2
    if (prPicHdr->u4PicCodingType == I_PIC)
    {
        if ((prPicHdr->u4PicStruct == FRM_PIC) || (!prPicHdr->fgSecField))
        {
            #if VDEC_TEST_LOG
            UTIL_Printf("//I frame/I first field, PicStruct:%d\n", prPicHdr->u4PicStruct);
            #endif
        }
        else
        {
            ai4BlkDistFw[0] = 1 ;
            ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2);
            ai4BlkDistFw[2] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2) + 1;
            //if (AVS_LAST_P_IDX >= REF_LIST_1)
            {
                ai4BlkDistFw[3] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4PicDistance) * 2);
            }

            #if VDEC_TEST_LOG
            UTIL_Printf("//I second field\n");
            #endif
        }
    }
    else
    if(prPicHdr->u4PicCodingType == P_PIC)
    {
        if (prPicHdr->u4PicStruct == FRM_PIC)
        {
            ai4BlkDistFw[0] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2);
            //if (AVS_LAST_P_IDX >= REF_LIST_1)
            {
                ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4PicDistance) * 2);
            }

            #if VDEC_TEST_LOG
            UTIL_Printf( "//P Frame\n");
            #endif
        }
        else
        {
            if (prPicHdr->u4TFT)
            {
                if (!prPicHdr->fgSecField)
                {
                    ai4BlkDistFw[0] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2) - 1;
                    ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2);
                    //if (AVS_LAST_P_IDX >= REF_LIST_1)
                    {
                        ai4BlkDistFw[2] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4PicDistance) * 2) - 1;
                        ai4BlkDistFw[3] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4PicDistance) * 2);
                    }

                    #if VDEC_TEST_LOG
                    UTIL_Printf( "//P first top field\n");
                    #endif
                }
                else
                {
                    ai4BlkDistFw[0] = 1 ;
                    ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2);
                    ai4BlkDistFw[2] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4PicDistance) * 2) + 1;
                    //if (AVS_LAST_P_IDX >= REF_LIST_1)
                    {
                        ai4BlkDistFw[3] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4PicDistance) * 2);
                    }

                    #if VDEC_TEST_LOG
                    UTIL_Printf( "//P second bottom field\n");
                    #endif
                }
            }
        }
    }
    else
    {
        if (prPicHdr->u4PicStruct == FRM_PIC)
        {
            ai4BlkDistFw[0] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);
            ai4BlkDistBw[0] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2- (INT32)(prPicHdr->u4PicDistance) * 2);

            ai4DirectRatio[0] =	(INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2-
                (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);

            //f (AVS_BW_REF_IDX > REF_LIST_1)
            {
                ai4DirectRatio[1] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2-
                    (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2);
            }

            if (prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u1PicCodingType == I_PIC)
            {
                vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 1);
            }
            else
            {
                vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 0);
            }

            #if VDEC_TEST_LOG
            UTIL_Printf( "//B frame\n");
            #endif
        }
        else
        {
            if (prPicHdr->u4TFT)
            {
                // Top field
                if (!prPicHdr->fgSecField)
                {
                    ai4BlkDistFw[0] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2) - 1;
                    ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);

                    ai4BlkDistBw[0] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prPicHdr->u4PicDistance) * 2);
                    ai4BlkDistBw[1] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prPicHdr->u4PicDistance) * 2) + 1;

                    ai4DirectRatio[0] =	(INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2) - 1;
                    ai4DirectRatio[1] =	(INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);

                    //if (AVS_BW_REF_IDX > REF_LIST_1)
                    {
                        ai4DirectRatio[2] =	(INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2) - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2) - 1;
                        ai4DirectRatio[3] =	(INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2) - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2);
                    }


                    if (prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u1PicCodingType == I_PIC)
                    {
                        vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 1);
                    }
                    else
                    {
                        vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 0);
                    }

                    #if VDEC_TEST_LOG
                    UTIL_Printf( "//B first top field\n");
                    #endif
                }
                else
                {
                    ai4BlkDistFw[0] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);
                    ai4BlkDistFw[1] = (INT32)((INT32)(prPicHdr->u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2) + 1;

                    ai4BlkDistBw[0] = (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prPicHdr->u4PicDistance) * 2 -1;
                    ai4BlkDistBw[1] = (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prPicHdr->u4PicDistance) * 2;

                    if((prSeqHdr->u4ProfileID == GUANGBO) && (prPicHdr->u4PBFieldEnhancedFlag == 1))
                    {
                        ai4DirectRatio[0] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2) - 1;
                        ai4DirectRatio[1] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);
                        
                        //if (AVS_BW_REF_IDX > REF_LIST_1)
                        {
                            ai4DirectRatio[2] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2) - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2) - 1;
                            ai4DirectRatio[3] = (INT32)((INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2) - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2);
                        }
                        
                        if (prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u1PicCodingType == I_PIC)
                        {
                            vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 1);
                        }
                        else
                        {
                            vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 0);
                        }
                    }
                    else
                    {
                        ai4DirectRatio[0] =	1;
                        ai4DirectRatio[1] = (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2);
                        ai4DirectRatio[2] = (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4PicDistance) * 2) + 1;
    
                        //if (prVDecAvsDecPrm->u1LastRefIdx > REF_LIST_1)
                        {
                            ai4DirectRatio[3] = (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4PicDistance) * 2 - (INT32)(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4PicDistance) * 2;
                        }
    
                        vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_I_REF, 0);
                    }
                    #if VDEC_TEST_LOG
                    UTIL_Printf( "//B second bottom field\n");
                    #endif
                }
            }
            else
            {
            }
        }
    }

    for(i = 0; i < 4; i++)
    {
        ai4BlkDistFw[i] = (ai4BlkDistFw[i] + 512)%512;
        ai4DirectRatio[i] = (ai4DirectRatio[i] + 512)%512;

        if(ai4BlkDistFw[i] > 0)
        {
            ai4MedRatioFw[i] = 512/ai4BlkDistFw[i];
        }

        if(ai4DirectRatio[i] > 0)
        {
            ai4DirectRatio[i] = 16384/ai4DirectRatio[i];
        }
    }

    for(i = 0; i < 2; i++)
    {
        ai4BlkDistBw[i] = (ai4BlkDistBw[i] + 512)%512;

        if(ai4BlkDistBw[i] > 0)
        {
            ai4MedRatioBw[i] = 512 /ai4BlkDistBw[i];
        }
    }

    i4FwBlkDist1_0 = ai4BlkDistFw[0] | (ai4BlkDistFw[1] << 16);
    i4FwBlkDist3_2 = ai4BlkDistFw[2] | (ai4BlkDistFw[3] << 16);
    i4BwBlkDist1_0 = ai4BlkDistBw[0] | (ai4BlkDistBw[1] << 16);
    i4FwMedRatio1_0 = ai4MedRatioFw[0] | (ai4MedRatioFw[1] << 16);
    i4FwMedRatio3_2 = ai4MedRatioFw[2] | (ai4MedRatioFw[3] << 16);
    i4BwMedRatio1_0 = ai4MedRatioBw[0] | (ai4MedRatioBw[1] << 16);
    i4DirectRatio1_0 = ai4DirectRatio[0] | (ai4DirectRatio[1] << 16);
    i4DirectRatio3_2 = ai4DirectRatio[2] | (ai4DirectRatio[3] << 16);

    vVDecWriteAVSMV(u4VDecID, RW_AVS_BLK_FW_DISTANCE_1_0, i4FwBlkDist1_0);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_BLK_FW_DISTANCE_3_2, i4FwBlkDist3_2);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_BLK_BW_DISTANCE_1_0, i4BwBlkDist1_0);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_BLK_FW_MED_RATIO_1_0, i4FwMedRatio1_0);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_FW_MED_RATIO_3_2, i4FwMedRatio3_2);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_BW_MED_RATIO_1_0, i4BwMedRatio1_0);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_DIRECT_RATIO_1_0, i4DirectRatio1_0);
    vVDecWriteAVSMV(u4VDecID, RW_AVS_MV_DIRECT_RATIO_3_2, i4DirectRatio3_2);
}

UINT32 u4WeightingQuantParamDefault[6] = {128, 98,106,116,116,128};
UINT32 u4WeightingQuantParamBase1[6] = {135,143,143,160,160,213};
UINT32 u4WeightingQuantParamBase2[6] = {128, 98,106,116,116,128};
UINT32 u4CurrentWeightingQuantMatrix[64];
UINT32 u4WeightingQuantMatrix[3][64];
UINT32 u4WeightingQuantParam[3][6];
UINT32 u4WeightingQuantModeScene[3][64] =
{
    { 	
        //Mode0
        0,0,0,4,4,4,5,5,
        0,0,3,3,3,3,5,5,
        0,3,2,2,1,1,5,5,
        4,3,2,2,1,5,5,5,
        4,3,1,1,5,5,5,5,
        4,3,1,5,5,5,5,5,
        5,5,5,5,5,5,5,5,
        5,5,5,5,5,5,5,5
    },
    
    {  
        //Mode1
        0,0,0,4,4,4,5,5,
        0,0,4,4,4,4,5,5,
        0,3,2,2,2,1,5,5,
        3,3,2,2,1,5,5,5,
        3,3,2,1,5,5,5,5,
        3,3,1,5,5,5,5,5,
        5,5,5,5,5,5,5,5,
        5,5,5,5,5,5,5,5
    },
    
    {   
        //Mode2
        0,0,0,4,4,3,5,5,
        0,0,4,4,3,2,5,5,
        0,4,4,3,2,1,5,5,
        4,4,3,2,1,5,5,5,
        4,3,2,1,5,5,5,5,
        3,2,1,5,5,5,5,5,
        5,5,5,5,5,5,5,5,
        5,5,5,5,5,5,5,5
    }
};
// **************************************************************************
// Function : static void vVDEC_HAL_AVS_WeightQuantCal(UINT32 u4VDecID, const VDEC_HAL_AVS_DEC_PRM_T* prVDecAvsDecPrm)
// Description : Calculate Weighting Quant Matrix when WeightingQuantFlag is TRUE
// Parameter : u4VDecID : video decoder hardware ID
// Return : None
// **************************************************************************
static void vVDEC_HAL_AVS_WeightQuantCal(UINT32 u4VDecID, const VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm)
{
    UINT32 u4CurrentSceneModel;
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr;

    prPicHdr = prVDecAvsDecPrm->prPicHdr;
    u4CurrentSceneModel = prPicHdr->u4WeightingQuantModel;


    if(prPicHdr->u4WeightingQuantFlag)
    {
        UINT32 u4Idx,u4Idx2,u4Idx3;
        
        //Step1: Init Param
        //UTIL_Printf("//Init Weighting Quant Matrix\n"); 

        for(u4Idx = 0; u4Idx < 64; u4Idx++)
        {
            u4CurrentWeightingQuantMatrix[u4Idx] = 128;
        }
        
        for(u4Idx = 0; u4Idx < 3; u4Idx++)
        {
            for(u4Idx2 = 0; u4Idx2 < 64; u4Idx2++)
            {
                u4WeightingQuantMatrix[u4Idx][u4Idx2] = 128;
            }
        }

        for(u4Idx = 0; u4Idx < 6; u4Idx++)
        {
            u4WeightingQuantParam[0][u4Idx] = 128;
            u4WeightingQuantParam[1][u4Idx] = u4WeightingQuantParamBase1[u4Idx];
            u4WeightingQuantParam[2][u4Idx] = u4WeightingQuantParamBase2[u4Idx];
        }
        
        //Step2:Cal wqP of Current Picture
        if(prPicHdr->u4WeightingQuantParamIndex == 0)
        {
            for(u4Idx = 0; u4Idx < 6; u4Idx++)
            {
                u4WeightingQuantParam[0][u4Idx] = u4WeightingQuantParamDefault[u4Idx];
            }
        }
        else if(prPicHdr->u4WeightingQuantParamIndex == 1)
        {
            for(u4Idx = 0; u4Idx < 6; u4Idx++)
            {
                u4WeightingQuantParam[1][u4Idx] = prPicHdr->u4WeightingQuantParamDelta1[u4Idx] + u4WeightingQuantParamBase1[u4Idx];
            }
        }
        else if(prPicHdr->u4WeightingQuantParamIndex == 2)
        {
            for(u4Idx = 0; u4Idx < 6; u4Idx++)
            {
                u4WeightingQuantParam[2][u4Idx] = prPicHdr->u4WeightingQuantParamDelta2[u4Idx] + u4WeightingQuantParamBase2[u4Idx];
            }
        }

        //Step3:Cal wqM8x8[m] according to CurrentSceneModel
        if((u4CurrentSceneModel < 3) && (prPicHdr->u4WeightingQuantParamIndex <3)) //Boyahualu bitstream, u4CurrentSceneModel is 3, old Spec
        {
            for(u4Idx = 0; u4Idx < 3; u4Idx++) 
            {
                for(u4Idx2 = 0; u4Idx2 < 8; u4Idx2++)
                {
                    for(u4Idx3 = 0; u4Idx3 < 8; u4Idx3++)
                    {
                        u4WeightingQuantMatrix[u4Idx][u4Idx2*8+u4Idx3]=(u4WeightingQuantParam[u4Idx][u4WeightingQuantModeScene[u4CurrentSceneModel][u4Idx2*8+u4Idx3]]); 
                    }
                }
            }
    
            for(u4Idx = 0; u4Idx < 64; u4Idx++)
            {
                u4CurrentWeightingQuantMatrix[u4Idx] = u4WeightingQuantMatrix[prPicHdr->u4WeightingQuantParamIndex][u4Idx];
            }
        }
        else
        {
            UTIL_Printf("//AVSPlus Bitstream Over Spec!!\n"); 
        }
    }
    else
    {
    //nothing
    }

}

// **************************************************************************
// Function : static void vVDEC_HAL_AVS_FillQTable2Sram(UINT32 u4VDecID, const VDEC_HAL_AVS_DEC_PRM_T* prVDecAvsDecPrm);
// Description : Fill Q table value
// Parameter : u4VDecID : video decoder hardware ID
// Return : None
// **************************************************************************
static void vVDEC_HAL_AVS_FillQTable2Sram(UINT32 u4VDecID, const VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm)
{
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
    UINT32 u4DequantTable[64] =
    {
        32768,36061,38968,42495,46341,50535,55437,60424,
        32932,35734,38968,42495,46177,50535,55109,59933,
        65535,35734,38968,42577,46341,50617,55027,60097,
        32809,35734,38968,42454,46382,50576,55109,60056,
        65535,35734,38968,42495,46320,50515,55109,60076,
        65535,35744,38968,42495,46341,50535,55099,60087,
        65535,35734,38973,42500,46341,50535,55109,60097,
        32771,35734,38965,42497,46341,50535,55109,60099
    }; //According to Spec
    UINT32 u4ShiftTable[64] =
    {
        14,14,14,14,14,14,14,14,
        13,13,13,13,13,13,13,13,
        13,12,12,12,12,12,12,12,
        11,11,11,11,11,11,11,11,
        11,10,10,10,10,10,10,10,
        10, 9, 9, 9, 9, 9, 9, 9,
         9, 8, 8, 8, 8, 8, 8, 8,
         7, 7, 7, 7, 7, 7, 7, 7 
    }; //According to Spec
    	
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr;
    UINT32 u4DequantTableVal, u4ShiftTableVal, u4CurrentWeightingQuantMatrixVal;
    UINT32 u4Idx;
    
    prPicHdr = prVDecAvsDecPrm->prPicHdr;

    //UTIL_Printf("//Fill QuantTable to Sdram, u4WeightingQuantFlag: %d\n", prPicHdr->u4WeightingQuantFlag);     
    if(prPicHdr->u4WeightingQuantFlag)
    {
        vVDecWriteAVSPP(u4VDecID, 321*4, 1); //set iq_sram_32b_risc_ctrl_en= 1
        	
        for(u4Idx = 0; u4Idx < 64; u4Idx ++)
        {
            u4DequantTableVal = (u4DequantTable[u4Idx] & 0xFFFF);
            u4ShiftTableVal = (u4ShiftTable[u4Idx] & 0xFF);
            u4CurrentWeightingQuantMatrixVal = (u4CurrentWeightingQuantMatrix[0] & 0xFF);
            	
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, u4Idx);
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE,
                                            ((u4DequantTableVal << 16) |(u4ShiftTableVal << 8) | u4CurrentWeightingQuantMatrixVal));
        }
        
        for(u4Idx = 0; u4Idx < 64; u4Idx ++)
        {
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, (64+u4Idx));
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, u4CurrentWeightingQuantMatrix[u4Idx]);
        }
        
        vVDecWriteAVSPP(u4VDecID, 321*4, 0); //set iq_sram_32b_risc_ctrl_en= 0
        vVDecWriteAVSPP(u4VDecID, 329*4, 1); //set weighting_quant_flag(PP_329[0])= 1
    }
    else
    {
        vVDecWriteAVSPP(u4VDecID, 321*4, 1); //set iq_sram_32b_risc_ctrl_en= 1
        
        for(u4Idx = 0; u4Idx < 64; u4Idx ++)
        {
            u4DequantTableVal = (u4DequantTable[u4Idx] & 0xFFFF);
            u4ShiftTableVal = (u4ShiftTable[u4Idx] & 0xFF);

            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, u4Idx);
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 
                                            ((u4DequantTableVal << 16) |(u4ShiftTableVal << 8) | 0x80));
        }
        
        for(u4Idx = 64; u4Idx < 128; u4Idx ++)
        {
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, u4Idx);
            vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 128);
        }
        	
        vVDecWriteAVSPP(u4VDecID, 321*4, 0); //set iq_sram_32b_risc_ctrl_en= 0
        vVDecWriteAVSPP(u4VDecID, 329*4, 0); //set weighting_quant_flag(PP_329[0])= 0
    }
    
#else
    // Because I don't want to waste memory, I use hard codes for Q table
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 0);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xdd38ff);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 4);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x5678d08);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 8);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xa49638ff);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 12);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x6167451d);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 16);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff963851);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 20);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x5b9f3c1);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 24);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x299638d6);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 28);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x2e904598);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 32);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff9638ff);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 36);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xf05345ac);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 40);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xffa038ff);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 44);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x5673bb7);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 48);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff963d04);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 52);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x56745c1);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 56);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x3963501);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 60);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x56745c3);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 64);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x808c98a5);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 68);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d8ec);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 72);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x808b98a5);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 76);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb4c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 80);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff8b98a6);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 84);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d6ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 88);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x808b98a5);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 92);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 96);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff8b98a5);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 100);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb4c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 104);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff8b98a5);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 108);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 112);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xff8b98a6);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 116);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 120);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x808b98a6);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 124);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb5c5d7ea);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 128);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xe0e0e0e);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 132);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xe0e0e0e);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 136);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xd0d0d0d);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 140);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xd0d0d0d);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 144);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xd0c0c0c);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 148);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xc0c0c0c);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 152);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb0b0b0b);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 156);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb0b0b0b);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 160);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xb0a0a0a);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 164);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xa0a0a0a);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 168);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0xa090909);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 172);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x9090909);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 176);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x9080808);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 180);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x8080808);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 184);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x7070707);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE_POS, 188);
    vVDecWriteVLD(u4VDecID, RW_AVS_VLD_QUANT_TABLE, 0x7070707);
#endif
}

static void vVDEC_HAL_AVS_SetMcReg(UINT32 u4VDecID, const VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm)
{
    UINT32 u4WidthMb;
    UINT32 u4HeightMb;
    //UINT32 u4FbWidth, u4FbHeight;
#if AVS_MC_REFERENCE_INIT
    UINT32 u4Temp, u4RefInitYAddr, u4RefInitCAddr;
#endif
    VDEC_INFO_AVS_SEQ_HDR_T *prSeqHdr = prVDecAvsDecPrm->prSeqHdr;
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;
    
    u4HeightMb = (UINT32)((prSeqHdr->u4LastVSize + 15) >> 4);
    
#if DTV_COMMON_CONFIG 
    #if 0 // todo: need setting after get line size for swap mode 2
    if (prVDecAvsDecPrm->ucAddrSwapMode == ADDRSWAP_DDR3)
    {
        u4WidthMb = (UINT32)((prSeqHdr->u4LineSize + 15) >> 4);
        u4WidthMb = ((u4WidthMb + 3) / 4) * 4;
        
        VLD_WRITE32(RW_VLD_MCPICSZ, u4WidthMb);

        #if VDEC_DDR3_SUPPORT
        MC_WRITE32(RW_MC_MCPICSZ, (((prDecPrm->u4LineSize + 63) >> 6) << 2));
        #else
        MC_WRITE32(RW_MC_MCPICSZ, ((prDecPrm->u4LineSize + 15) >> 4));
        #endif

        VLD_TOP_WRITE32(RW_VLD_TOP_TOTAL_MBS_IN_PIC,  u4WidthMb * u4HeightMb);
    }
    else
    #endif
    {
        u4WidthMb = (UINT32)((prSeqHdr->u4LastHSize + 15) >> 4);
        
        vVDecWriteVLD(u4VDecID, RW_VLD_MCPICSZ, u4WidthMb);
        vVDecWriteMC(u4VDecID, RW_MC_MCPICSZ, u4WidthMb);
        vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC, u4WidthMb * u4HeightMb);
    }
#else
    // notify 
    #if 0
    if (prDecParam->ucAddrSwapMode == ADDR_MODE_5351_NORMAL)
    {
        u4WidthMb = (UINT32)((prVDecAvsDecPrm->u4LineSize + 15) >> 4);
        u4WidthMb = ((u4WidthMb + 3) / 4) * 4;
        vVDecWriteVLD(u4VDecID, RW_VLD_MCPICSZ, u4WidthMb);
        LOG(6, "vVDecWriteVLD(u4VDecID, %d, %d)\n", RW_VLD_MCPICSZ >> 2, u4WidthMb);
    }
    else
    #endif
    {
        u4WidthMb = (UINT32)((prSeqHdr->u4LastHSize + 15) >> 4);
        vVDecWriteVLD(u4VDecID, RW_VLD_MCPICSZ, u4WidthMb);
        vVDecWriteMC(u4VDecID, RW_MC_MCPICSZ, u4WidthMb);
    }
#endif

    // Set frame width and height
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, prSeqHdr->u4LastHSize);
    #if AVS_TEST_HD //DTV_COMMON_CONFIG
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, ((prSeqHdr->u4LastVSize + 31) / 32)*32);
    #else
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, prSeqHdr->u4LastVSize);
    #endif

    // For debug usage
    vVDecWriteMC(u4VDecID, 0x788, 3);
    vVDecWriteMC(u4VDecID, 0x790, 3);

    // Enable PP
    vVDecWriteMC(u4VDecID, RW_AVS_MC_PP_ENABLE, prVDecAvsDecPrm->fgEnPP);

    // Set the size of luma
    //u4FbWidth = prSeqHdr->u2WidthDec;
    //u4FbHeight = prSeqHdr->u2WidthDec;
    //vVDecWriteMC(u4VDecID, RW_AVS_MC_LUMA_SIZE, (u4FbWidth * (u4FbHeight)));
#if AVS_SUPPORT_YC_SEPERATE
    //Set the size of luma
    vVDecWriteMC(u4VDecID, RW_AVS_MC_LUMA_SIZE, 0x440000);
#else
    vVDecWriteMC(u4VDecID, RW_AVS_MC_LUMA_SIZE, prVDecAvsDecPrm->prCurrFBufInfo->u4CAddrOffset);
#endif

    // Disable MC Write
    vVDecWriteMC(u4VDecID, RW_AVS_MC_DISABLE_WRITE, 1);

#if AVS_SUPPORT_YC_SEPERATE
    vVDecWriteMC(u4VDecID, RW_AVS_MC_YC_SEPERATE_SWITCH, YC_SEPERATE_ENABLE);
#endif

#if AVS_MC_REFERENCE_INIT
    //Assign an reasonable dram address to MC Refence register
    if(prPicHdr->u4PicCodingType != B_PIC) //I/P pic use last P pic
    {
        u4RefInitYAddr = PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr);
        u4RefInitCAddr = PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr);
    }
    else //B Pic use forward reference picture
    {
        u4RefInitYAddr = PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr);
        u4RefInitCAddr = PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr);
    }

    for(u4Temp = 0; u4Temp < 96; u4Temp ++)
    {
        vVDecWriteMC(u4VDecID, 4*(247 + u4Temp), u4RefInitYAddr);
        vVDecWriteMC(u4VDecID, 4*(728 + u4Temp), u4RefInitCAddr);
    }
#endif

    // Set Reference Buffer
    if (prPicHdr->u4PicStruct == FRM_PIC)
    {
        if (prPicHdr->u4PicCodingType == I_PIC)
        {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr) );

#if AVS_SUPPORT_YC_SEPERATE
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr) );
#endif
        }
        else if (prPicHdr->u4PicCodingType == P_PIC)
        {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr) );

            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr));

#if AVS_SUPPORT_YC_SEPERATE
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));

            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr));
#endif
        }
        else if (prPicHdr->u4PicCodingType == B_PIC)
        {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr));

            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr));

#if AVS_SUPPORT_YC_SEPERATE
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr));
            
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr));
#endif
        }
    }
    else
    {
        if ((prPicHdr->u4PicCodingType == I_PIC) && (prPicHdr->fgSecField))
        {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_Y_ADDR, PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufY));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));

            //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
            {
                vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_Y_ADDR,
                    PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr));
            }


#if AVS_SUPPORT_YC_SEPERATE
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_C_ADDR, 
                PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufC));
            
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
            
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
            
            //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
            {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr));
            }
#endif

#if TOP_BOT_OUTPUT_SUPPORT
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0x5);
#else
            // bottom Field
            vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0xa);
#endif
        }
        else if (prPicHdr->u4PicCodingType == P_PIC)
        {
            if (prPicHdr->u4TFT)
            {
                if (!prPicHdr->fgSecField)
                {
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_Y_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_Y_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));

                    //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
                    {
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_Y_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr));
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_Y_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr));
                    }

#if AVS_SUPPORT_YC_SEPERATE
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_C_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_C_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
                    
                    //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
                    {
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_C_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr));
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_C_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr));
                    }
#endif

#if TOP_BOT_OUTPUT_SUPPORT
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0xa);
#else
                    // Top Field
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0x5);
#endif
                }
                else
                {
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_Y_ADDR, PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufY));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_Y_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_Y_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr));

                    //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
                    {
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_Y_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr));
                    }

#if AVS_SUPPORT_YC_SEPERATE
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC0_C_ADDR, PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufC));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC1_C_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC2_C_ADDR,
                        PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr));
                    
                    //if (prVDecAvsDecPrm->u1LastRefIdx >= REF_LIST_1)
                    {
                        vVDecWriteMC(u4VDecID, RW_AVS_MC_P_LIST0_PIC3_C_ADDR,
                            PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr));
                    }
#endif
#if TOP_BOT_OUTPUT_SUPPORT
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0x5);
#else
                    // bottom Field
                    vVDecWriteMC(u4VDecID, RW_AVS_MC_P_FIELD_DESCRIPTION, 0xa);
#endif
                }
            }
        }
        else if (prPicHdr->u4PicCodingType == B_PIC)
        {
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC1_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr));

            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC0_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC1_Y_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr));

#if AVS_SUPPORT_YC_SEPERATE
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST0_PIC1_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr));
            
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC0_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr));
            vVDecWriteMC(u4VDecID, RW_AVS_MC_B_LIST1_PIC1_C_ADDR,
                PHYSICAL(prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr));
#endif

#if TOP_BOT_OUTPUT_SUPPORT
            vVDecWriteMC(u4VDecID, 0x568, 2);
            vVDecWriteMC(u4VDecID, 0x56C, 1);
#else
            vVDecWriteMC(u4VDecID, 0x568, 1);
            vVDecWriteMC(u4VDecID, 0x56C, 2);
#endif
        }
    }

    if (prPicHdr->u4PicCodingType == B_PIC)
    {
        vVDecWriteMC(u4VDecID, RW_AVS_MC_B_REF, 1);
    }
}

static void vVDEC_HAL_AVS_SetPpReg(UINT32 u4VDecID, const VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm)
{
    UINT32 u4Height;
    UINT32 u4WidthMB, u4HeightMB;
    VDEC_INFO_AVS_SEQ_HDR_T *prSeqHdr;
    VDEC_INFO_AVS_PIC_HDR_T* prPicHdr;
    prSeqHdr = prVDecAvsDecPrm->prSeqHdr;
    prPicHdr = prVDecAvsDecPrm->prPicHdr;

    // Set AVS mode for PP
    vVDecWriteAVSPP(u4VDecID, RW_AVS_PP_AVS_MODE, 0x100);

    // Set PP buffer
    vVDecWriteMC(u4VDecID, RW_AVS_PP_LUMA_ADDR, PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufY) >> 9);
    vVDecWriteMC(u4VDecID, RW_AVS_PP_CHROM_ADDR, PHYSICAL((UINT32)prVDecAvsDecPrm->pucDecWorkBufC) >> 8);

    //For UFO Mode
    if(prVDecAvsDecPrm->fgIsUFOMode)
    {
        UTIL_Printf("Enable UFO Mode\n");
        vVDecWriteMC(u4VDecID, 4*664, 0x11);
    
        vVDecWriteMC(u4VDecID, 4*698, PHYSICAL(prVDecAvsDecPrm->prCurrFBufInfo->u4YLenAddr));
        vVDecWriteMC(u4VDecID, 4*699, PHYSICAL(prVDecAvsDecPrm->prCurrFBufInfo->u4CLenAddr));
        vVDecWriteMC(u4VDecID, 4*825, prVDecAvsDecPrm->prCurrFBufInfo->u4YLenOffset);
        vVDecWriteMC(u4VDecID, 4*826, prVDecAvsDecPrm->prCurrFBufInfo->u4CLenOffset);

        vVDecWriteAVSPP(u4VDecID, AVS_PP_ERROR_HANDLE_MODE, 
			u4VDecReadAVSPP(u4VDecID, AVS_PP_ERROR_HANDLE_MODE)|D_PP_USE_FOR_DDR2);
    }

    vVDecWriteMC(u4VDecID, 0x250, 1);
    // PP Write Enable.
    vVDecWriteMC(u4VDecID, 0x220, 1);

    if (!prPicHdr->u4LoopFilterDisable)
    {
        vVDecWriteMC(u4VDecID, 0x238, 3);
    }
    else
    {
        vVDecWriteMC(u4VDecID, 0x238, 0);
    }

    if (!prPicHdr->u4ProgFrm)
    {
        u4Height = prSeqHdr->u4LastVSize/2;
    }
    else
    {
        u4Height = prSeqHdr->u4LastVSize;
    }

    u4WidthMB = ((prSeqHdr->u4LastHSize + 15)/16) - 1;
    u4HeightMB = ((u4Height + 15)/16) - 1;

    vVDecWriteMC(u4VDecID, 0x260, u4WidthMB);
    vVDecWriteMC(u4VDecID, 0x264, u4HeightMB);
    u4WidthMB += 1;

#if 0
    if (prVDecAvsDecPrm->u4AddrMode == ADDR_MODE_5351_NORMAL)
    {
        u4WidthMB = ((prVDecAvsDecPrm->u4LineSize + 15)/16) - 1;
        u4WidthMB = ((u4WidthMB + 3) / 4) * 4;
        vVDecWriteMC(u4VDecID, 0x22C, u4WidthMB);
    }
    else
#endif
    {
        vVDecWriteMC(u4VDecID, 0x22C, u4WidthMB);
    }
}


#if AVS_TEST_DRAM_BUSY
void vVDEC_AVSSetDramBusy()
{
    vWriteReg(0x07200+0x18, u4ReadReg(0x07200 + 0x18) | (1 << 25) | (1 << 31));

    vWriteReg(0x07200+0x10, 0x0f6cb2f0);
    vWriteReg(0x07200+0x14, 0x10000);

    vWriteReg(0x07100+0x4, u4ReadReg(0x07100 + 0x4) & 0xFFFFFFF);

    vWriteReg(0x07100+0x14, 0xFFFFFF11);

    return;
}

#endif

INT32 i4VDEC_HAL_AVS_DecStart(UINT32 u4BsID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    UINT32 u4Temp;
    UINT32 u4PicStruct;
    UINT32 u4Php = 0;
    UINT32 u4MbWidthMinusOne;
    UINT32 u4MbHeightMinusOne;
    //UINT32 u4WorkingSize, u4WorkingAddr;
    UINT32 u4PicCodingType;
    UINT32 u4ChromaQuantParamDeltaCb;
    UINT32 u4ChromaQuantParamDeltaCr;


#if (CONFIG_DRV_VERIFY_SUPPORT) && (!VDEC_DRV_PARSER)
    VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = (VDEC_INFO_AVS_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecAVSDecPrm);
#else
    VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = (VDEC_INFO_AVS_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
    VDEC_INFO_AVS_SEQ_HDR_T *prSeqHdr = prVDecAvsDecPrm->prSeqHdr;
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;
    //UINT32 u4Rptr, u4Wptr;

   //VDEC_DISP_PIC_INFO_T rDispInfo = {0};

    u4PicCodingType = prPicHdr->u4PicCodingType;

    u4MbWidthMinusOne = ((prSeqHdr->u4LastHSize + 15) >> 4) - 1;

    if (prPicHdr->u4PicStruct != FRM_PIC)
    {
        u4MbHeightMinusOne = ((prSeqHdr->u4LastVSize + 15) >> 5) - 1;
    }
    else
    {
        u4MbHeightMinusOne = ((prSeqHdr->u4LastVSize + 15) >> 4) - 1;
    }

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && CONFIG_DRV_FTS_SUPPORT)
    // set letterbox detection parameter
    vVDECSetLetetrBoxDetPrm(u4VDecID, &prDecPrm->rLBDPrm);
#endif

    // Write Addressing Mode
    

#if DTV_COMMON_CONFIG
    vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, prDecPrm->ucAddrSwapMode);
    
    #if VDEC_DDR3_SUPPORT    
    vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFF8) | (prDecPrm->ucAddrSwapMode & 0x7)));
    #else
    vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, (prDecPrm->ucAddrSwapMode & 0x7));
    #endif    
#else
    vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, prDecPrm->ucAddrSwapMode);
    vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFF8) |prDecPrm->ucAddrSwapMode));
#endif
    
    u4Temp = u4VDecReadMC(u4VDecID, 0x5E4);
    u4Temp |= (0x1000);
    vVDecWriteMC(u4VDecID, 0x5E4, u4Temp);

    // Set some VLD registers
    #if DTV_COMMON_CONFIG
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, 0x7);

    // todo:
    #if 1
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH, (u4VDecReadVLDTOP(u4VDecID, RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH) & 0xFFFFFFFE));
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PIC_SIZE, (((u4MbHeightMinusOne & 0x1FF) << 16) | (u4MbWidthMinusOne & 0x1FF)));
    #else
    VLD_TOP_WRITE32(RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH, (VLD_TOP_READ32(RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH) & 0xFFFFFFFE ));
    VLD_TOP_WRITE32(RW_VLD_TOP_PIC_SIZE, (((u4MbHeightMinusOne & 0x1FF) << 16) | (u4MbWidthMinusOne & 0x1FF)));
    #endif

    vVDecWriteVLD(u4VDecID, RW_VLD_SRAM_TEST_WRITE_ADDR, 0x0);

    //Error Concealment Setting for project after Oryx
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_ERR_TYPE_ENABLE, 0xFF5D7FFF);
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_ERR_RECONCEALMENT, 0x20000101);
    vVDecWriteAVSVLD(u4VDecID, 0xB4, 0x7530); //mvd_err_chk_upper_bnd, MVD upper boundary 
    vVDecWriteAVSVLD(u4VDecID, 0xB8, 0x7530); //mvd_err_chk_lower_bnd, MVD lower boundary
    vVDecWriteAVSVLD(u4VDecID, 0x170, 0x10); //bit4, aec_coeff_clip_mode
    #else
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, 0x8007);       
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PIC_SIZE, ((u4MbHeightMinusOne << 8) | u4MbWidthMinusOne));
    #endif

    // pic type/pic struct/pic reference/skip mode flag/fix pic qp
    if (prPicHdr->u4PicStruct == FRM_PIC)
    {
        u4PicStruct = FRM_PIC;
    }
    else
    {
#if BOT_FIELD_FIRST_AVS_SUPPORT
        prPicHdr->u4TFT = 1;
#endif

        if (prPicHdr->u4TFT)
        {
            if (!prPicHdr->fgSecField)
            {
                u4PicStruct = TOP_FLD_PIC; // top field
            }
            else
            {
                u4PicStruct = BTM_FLD_PIC; //bottom field
            }
        }
        else
        {
#if TOP_BOT_OUTPUT_SUPPORT
            prPicHdr->u4TFT = 1;
#endif

            if (!prPicHdr->fgSecField)
            {
                u4PicStruct = BTM_FLD_PIC; // bottom field
            }
            else
            {
                u4PicStruct = TOP_FLD_PIC; //top field

                // bottom field reconstructs from top field
                if (prPicHdr->u4PicCodingType == I_PIC)
                {
                    //u4PicCodingType = P_PIC;
                }
            }
        }
    }

    u4Php = ((u4PicCodingType & 3) |
                 ((u4PicStruct & 3) << 2) |
                 ((prPicHdr->u4PicRefFg & 1) << 4) |
                 ((prPicHdr->u4SkipModeFg & 1) << 5) |
                 ((prPicHdr->u4FixedPicQP & 1) << 6) |
                 ((prPicHdr->fgSecField & 1) << 7) |
                 ((prPicHdr->u4PicQP & 0x3f) << 8) |
                 ((prPicHdr->i4AlphaCOffset & 0x1f) << 16) |
                 ((prPicHdr->i4BetaOffset & 0x1f) << 24));
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PHP, u4Php);

#if DTV_COMMON_CONFIG  
    if (prVDecAvsDecPrm->prPicHdr->u4PicCodingType == I_PIC)
    {
        #ifdef CUR_CHIP_5881
        vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, u4VDecReadAVSVLD(u4VDecID, RW_AVS_VLD_CTRL) & (~(0x3<<12))); //bit[13:12] -> 0
        #endif
    }
    else
    {
        #ifdef CUR_CHIP_5881
        {
        UINT32 u4Value = u4VDecReadAVSVLD(u4VDecID, RW_AVS_VLD_CTRL); 

        u4Value &= (~(0x3<<12)); //bit[13:12] -> 0        
        
        if (prPicHdr->u4PicStruct == FRM_PIC)
        {
            vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, u4Value | (0x1<<12)); //bit[13:12] -> 1
        }
        else
        {
            vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, u4Value | (0x3<<12)); //bit[13:12] -> 3
        }
        }
        #endif
    }

    if(prSeqHdr->u4ProfileID == GUANGBO)
    {
        vVDecWriteAVSVLD(u4VDecID,RW_AVS_VLD_CTRL, u4VDecReadAVSVLD(u4VDecID, RW_AVS_VLD_CTRL) |RW_AVS_GUANGBO_MODE_ENABLE);

        u4ChromaQuantParamDeltaCb = (prPicHdr->u4ChromaQuantParamDeltaCb & 0x3F);
        u4ChromaQuantParamDeltaCr = (prPicHdr->u4ChromaQuantParamDeltaCr & 0x3F);
        vVDecWriteAVSVLD(u4VDecID,RW_AVS_VLD_WEIGHT_QUANT,(u4ChromaQuantParamDeltaCr<<8 |u4ChromaQuantParamDeltaCb));
        if(prPicHdr->u4AceEnable)
        {
            vVDecWriteAVSVLD(u4VDecID,RW_AVS_VLD_CTRL, u4VDecReadAVSVLD(u4VDecID, RW_AVS_VLD_CTRL) |RW_AVS_AEC_ENABLE);
        }
    }

    // Get/Set working buffer for VLD prediction module
    //prDecPrm->u4VldPredBufAddr = u4WorkingAddr;
    
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PRED_BASE_ADDR, 0);
    
    #if VDEC_AVS_EMU
    // Get/Set working buffer for VLD prediction module
    //vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PRED_BASE_ADDR, PHYSICAL(prVDecAvsDecPrm->rAvsWorkBufSa.u4PredSa));
    #endif

    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_SRAM_BYPASS, (u4VDecReadVLDTOP(u4VDecID, RW_VLD_TOP_SRAM_BYPASS) & 0xBFFFFFFF));
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_SRAM_BYPASS, (u4VDecReadVLDTOP(u4VDecID, RW_VLD_TOP_SRAM_BYPASS) & 0x7FFFFFFF));
#else
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_ERR_CONCEALMENT, AVS_ERR_CONCEALMENT_ENABLE);

    #if VDEC_AVS_EMU //(CONFIG_DRV_FPGA_BOARD)
    // Get/Set working buffer for VLD prediction module
    //vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PRED_BASE_ADDR, ( (PHYSICAL(prVDecAvsDecPrm->rAvsWorkBufSa.u4PredSa)) | 0x40000000));
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PRED_BASE_ADDR, PHYSICAL(prVDecAvsDecPrm->rAvsWorkBufSa.u4PredSa));
    #else
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_PRED_BASE_ADDR, 0);
    #endif
#endif

    //Need set as 1 before trigger Decode
    vVDEC_HAL_AVS_BitStuff_Check(u4VDecID, 1);

    // Set MV Related Settings
    vVDEC_HAL_AVS_SetMvReg(u4VDecID, prVDecAvsDecPrm);

    // Fill Quantization Table
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
    vVDEC_HAL_AVS_WeightQuantCal(u4VDecID, prVDecAvsDecPrm);
#endif
    vVDEC_HAL_AVS_FillQTable2Sram(u4VDecID, prVDecAvsDecPrm);

    // Set MC Related Settings
    vVDEC_HAL_AVS_SetMcReg(u4VDecID, prVDecAvsDecPrm);

    // Set PP Related Settings
    vVDEC_HAL_AVS_SetPpReg(u4VDecID, prVDecAvsDecPrm);

    //Add for Pana_Error_Concealment_Cut.avs
#if 0
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_ERROR_TYPE_ENABLE, (u4VDecReadVLDTOP(u4VDecID, RW_VLD_TOP_ERROR_TYPE_ENABLE))&0xfffdffff);
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_DEDERR_TYPE, (u4VDecReadVLDTOP(u4VDecID, RW_VLD_TOP_DEDERR_TYPE))&0xfff9ffff);
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_ERR_RECONCEALMENT, 0x20000203);
#endif
    //Error Concealment Setting for project after Oryx
    vVDecWriteAVSPP(u4VDecID, AVS_PP_ERROR_HANDLE_MODE, u4VDecReadAVSPP(u4VDecID, AVS_PP_ERROR_HANDLE_MODE) |ERR_HANDLE_CURR_MB_ROW_ENABLE);

#if (CONFIG_DRV_VERIFY_SUPPORT) && (CONFIG_DRV_LINUX)
    //HalFlushInvalidateDCache();
#endif

#if AVS_TEST_DRAM_BUSY
    vVDEC_AVSSetDramBusy();
#endif

    // print input window
    UTIL_Printf("//[#%d] Input Window: 0x%08x, 0x%08x\n", _u4AvsTriggerCnt[u4VDecID], u4VDecReadVLD(u4VDecID, 0xF0), u4VDecReadAVSVLD(u4VDecID, 0x00)); 

#if DEBUG_DUMP_REG_ENABLE
    vVdecAvsDumpReg(u4VDecID, 1);
#endif

#if 0 //DUMP_DRAM_BUFFER
    {
        UCHAR *ucStartAddr;
        UINT32 u4Size;
        
        ucStartAddr = (UCHAR *)_pucVFifo[u4VDecID];
        
        u4Size = ((UINT32)(_pucAddressSwapBuf[u4VDecID] - ucStartAddr) + 0x660000);
        
        UTIL_Printf("//Start to dump vdec dram, StartAddr: 0x%8x, Size: 0x%8x\n", ucStartAddr, u4Size);
        
        fgAVSPlusDumpMem(u4VDecID, ucStartAddr, u4Size, _u4AvsTriggerCnt[u4VDecID], DP_TOTAL_DRAM );
    }
#endif

#if TOP_BOT_OUTPUT_SUPPORT
    prPicHdr->u4TFT = 0;
#endif
#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
    HAL_GetTime(&rTimeStartDec[u4VDecID]);
    //UTIL_Printf("second:ms %08d:%08d",rTimeStartDec[u4VDecID].u4Seconds,rTimeStartDec[u4VDecID].u4Micros);
#endif

//Add for Test
    {
    UINT32 u4VldBit;
    
    _u4AvsVldPtrBeforeTrig[u4VDecID] = u4VDEC_HAL_AVS_ReadRdPtr(0, u4VDecID, (UINT32)_pucVFifo[u4VDecID], &u4VldBit);
    }

    // Trigger Decoder
    mb();
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_DECSTART, 1);

#if VDEC_TEST_LOG
    UTIL_Printf("//Start to trigger HW[%d]\n", _u4AvsTriggerCnt[u4VDecID]);
    UTIL_Printf("wait(`VDEC_INT == 1);\n");
#endif
    _u4AvsTriggerCnt[u4VDecID]++;

     return 0;
}

// **************************************************************************
// Function : void u4VDEC_HAL_AVS_GetErrMsg(UINT32 u4VDecID);
// Description :Read AVS error message after decoding end
// Parameter : u4VDecID : video decoder hardware ID
// Return      : AVS decode error message
// **************************************************************************
BOOL fgVDEC_HAL_AVS_ChkErrInfo(UINT32 ucBsId, UINT32 u4VDecID, UINT32 u4DecErrInfo, UINT32 u4ECLevel)
{
    BOOL fgIsVDecErr = FALSE;
    return fgIsVDecErr;
}


void vVDEC_HAL_AVS_GetMbxMby(UINT32 u4VDecID, UINT32 *pu4Mbx, UINT32 *pu4Mby)
{
    //UINT32 u4Tmp;
    VDEC_ASSERT(pu4Mby!=NULL);
    VDEC_ASSERT(pu4Mby!=NULL);

    //u4Tmp = u4VDecReadAVSVLD(u4VDecID, RO_AVLD_SMKR);
    //*pu4Mbx = u4Tmp>>24;
    //*pu4Mby = (u4Tmp<<8)>>24;
    *pu4Mbx = u4VDecReadMC(u4VDecID, RO_MC_MBX);
    *pu4Mby = u4VDecReadMC(u4VDecID, RO_MC_MBY);
}

void vVDEC_HAL_AVS_GetErrInfo(UINT32 u4VDecID, VDEC_INFO_AVS_ERR_INFO_T *prAvsErrInfo)
{
    UINT32 u4RegVal = 0;

    //u4RegVal = u4VDecReadVLD(u4VDecID, RO_AVS_ERROR_ACC);
    u4RegVal = u4VDecReadAVSVLD(u4VDecID, RO_AVS_ERROR_ACC);
    prAvsErrInfo->u4AvsErrCnt = (u4RegVal & 0xFFFF5FFF);
    prAvsErrInfo->u4AvsErrRow = 0;
    prAvsErrInfo->u2AvsMBErrCnt =0 ;

    if(prAvsErrInfo->u4AvsErrCnt != 0)
    {
        UTIL_Printf("//u4AvsErrCnt: 0x%8x, u4RegVal: 0x%8x\n", prAvsErrInfo->u4AvsErrCnt, u4RegVal);
    }
    return;
}

// **************************************************************************
// Function : UINT32 u4VDEC_HAL_VP6_GetErrType(UINT32 u4VDecID);
// Description :Read Vp6 error type after decoding end
// Parameter : u4VDecID : video decoder hardware ID
// Return      : Vp6 decode error type value
// **************************************************************************
UINT32 u4VDEC_HAL_AVS_GetErrType(UINT32 u4VDecID, VDEC_INFO_AVS_ERR_INFO_T *prAvsErrInfo)
{
    UINT32 u4RegVal = 0;

    u4RegVal= u4VDecReadAVSVLD(u4VDecID, RO_AVS_ERROR_TYPE);
    prAvsErrInfo->u4AvsErrType = u4RegVal;
    return u4RegVal;
}

#if CONFIG_DRV_VERIFY_SUPPORT
UINT32 u4VDEC_HAL_AVS_VDec_ReadFinishFlag(UINT32 u4VDecID)
{
#if 0
    return u4VDecReadAVSVLD(u4VDecID, RO_AVLD_COMPLETE);
#else
    UINT32 u4Temp;
    
    u4Temp =  u4VDecReadMISC(u4VDecID, 4*41); //MISC_41[16] to judge decode finish
    
    return u4Temp;
#endif
}

BOOL fgVDEC_HAL_AVS_VDec_CompCheckSum(UINT32 *pu4DecCheckSum, UINT32 *pu4GoldenCheckSum)
{
  if((*pu4GoldenCheckSum) != (*pu4DecCheckSum))
  {
    return (FALSE);
  }
  pu4GoldenCheckSum ++;
  pu4DecCheckSum ++;
  if((*pu4GoldenCheckSum) != (*pu4DecCheckSum))
  {
    return (FALSE);
  }
  pu4GoldenCheckSum ++;
  pu4DecCheckSum ++;
  if((*pu4GoldenCheckSum) != (*pu4DecCheckSum))
  {
    return (FALSE);
  }
  pu4GoldenCheckSum ++;
  pu4DecCheckSum ++;
  if((*pu4GoldenCheckSum) != (*pu4DecCheckSum))
  {
    return (FALSE);
  }
  pu4GoldenCheckSum ++;
  pu4DecCheckSum ++;
  return (TRUE);
}

void vVDEC_HAL_AVS_VDec_DumpReg(
     UINT32 u4VDecID,
     BOOL fgBefore
)
{
    INT32 i;
    UINT32 u4Data;
    
    INT32 i4VldStart = 42;
    INT32 i4VldEnd = 255;
    
    INT32 i4McStart = 0;
    INT32 i4McEnd = 640; //0xA00

    Printf("AVS Dump Register: ");
    if (fgBefore == TRUE)
    {
        Printf("Before Decode\n");
    }
    else
    {
        Printf("After Decode\n");
    }

    Printf("[R] VLD Registers\n");
    
    for (i= i4VldStart; i<=i4VldEnd; i++)
    {        
        if (u4VDecID == 0)
        {
            u4Data = u4ReadReg(VLD_REG_OFFSET0 + (i<<2));

           
        }
        else
        {
            #ifdef VDEC_PIP_WITH_ONE_HW
            UTIL_Printf("PIP_ONE_HW: Wrong HW ID!!!\n");
            VDEC_ASSERT(0);
            #endif 
            
            u4Data = u4ReadReg(VLD_REG_OFFSET1 + (i<<2));
        }
        
        #if DTV_COMMON_CONFIG
        //vVDecSimDumpR(u4VDecID, VLD_REG_OFFSET0, (i<<2), u4Data);
        //vVDecPrintRegSetting(u4VDecID, VLD_REG_OFFSET0, (i<<2), u4Data, FALSE);
        #else
        Printf("%04d (0x%04X) = 0x%08X\n", i, (i<<2), u4Data);
        #endif
    }

    Printf("[R] MC Registers\n");
    
    for (i=i4McStart; i<=i4McEnd; i++)
    {
        if (u4VDecID == 0)
        {
            u4Data = u4ReadReg(MC_REG_OFFSET0 + (i<<2));
        }
        else
        {
            #ifdef VDEC_PIP_WITH_ONE_HW
            UTIL_Printf("PIP_ONE_HW: Wrong HW ID!!!\n");
            VDEC_ASSERT(0);
            #endif 
            
            u4Data = u4ReadReg(MC_REG_OFFSET1 + (i<<2));
        }
        
        #if DTV_COMMON_CONFIG
        //vVDecSimDumpR(u4VDecID, MC_REG_OFFSET0, (i<<2), u4Data);
        //vVDecPrintRegSetting(u4VDecID, MC_REG_OFFSET0, (i<<2), u4Data, FALSE);
        #else
        Printf("%04d (0x%04X) = 0x%08X\n", i, (i<<2), u4Data);
        #endif
    }
}
#endif

#if AVS_ESA_MEASURE
// *********************************************************************
// Function : vVdecMeasureAVSESA(UINT32 u4VDecID)
// Description : Measure AVS ESA/AVS/AVS_P16 speed/bw measurement
// Parameter : u4VDecID
// Return    : None
// *********************************************************************
VOID vVdecMeasureAVSESA(UINT32 u4VDecID)
{
    UTIL_Printf("//ESA1 %10d,%10d,%10d\n",
    	u4VDecReadMC(u4VDecID, 0x770),
    	u4VDecReadMC(u4VDecID, 0x8B8),
    	u4VDecReadMC(u4VDecID, 0xA28)
    );

    UTIL_Printf("//ESA2 %4d,%4d,%10d,%10d, %2d\n",
    	u4VDecReadMC(u4VDecID, 0x28),
    	u4VDecReadMC(u4VDecID, 0x2C),
    	u4VDecReadMC(u4VDecID, 0x9E0),
    	u4VDecReadMC(u4VDecID, 0x9E4),
        u4VDecReadVLDTOP(u4VDecID, 0xF0)
        );

    UTIL_Printf("//ESA3 %10d,%10d,%10d,%10d,%10d,%10d, %10d\n",
    	u4VDecReadMC(u4VDecID, 0x9E8),
    	u4VDecReadMC(u4VDecID, 0x9EC),
    	u4VDecReadMC(u4VDecID, 0x9F0),
    	u4VDecReadMC(u4VDecID, 0x7B4),
    	u4VDecReadMC(u4VDecID, 0x774),
    	u4VDecReadMC(u4VDecID, 0x778),
        u4VDecReadAVSVLD(u4VDecID, 0x138) //AVS_VLD_78
    	);

}
#endif
// *********************************************************************
// Function : Void vVdecAvsDumpReg(UINT32 u4BeoforeTrigger)
// Description : DUMP REG
// Parameter : None
// Return    : None
// *********************************************************************
VOID vVdecAvsDumpReg(UINT32 u4InstID, UINT32 u4BeoforeTrigger)
{
    UINT32 u4Temp;
    UINT32 u4RegVal, u4RegVal1;
    
    if(u4BeoforeTrigger)
    {
        UTIL_Printf("//DUMP Register Before HW Trigger\n");
    }
    else
    {
        UTIL_Printf("//DUMP Register After Decode Finish\n");
    }
    
    UTIL_Printf("//--------AVS_VLD REGISTER DUMP--------\n");
    
    u4RegVal = u4VDecReadAVSVLD(u4InstID, 0);
    UTIL_Printf("//RISCRead_AVS_VLD[%d] = 0x%8x    \n", 0, u4RegVal);
    x_thread_delay(1);
    for(u4Temp = 33; u4Temp < 49; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadAVSVLD(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadAVSVLD(u4InstID, (u4Temp+1)<<2);
        UTIL_Printf("//RISCRead_AVS_VLD[%d] = 0x%8x    RISCRead_AVS_VLD[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1),u4RegVal1);
        x_thread_delay(1);
    }
    
    u4Temp = 49;
    u4RegVal = u4VDecReadAVSVLD(u4InstID, u4Temp<<2);
    UTIL_Printf("//RISCRead_AVS_VLD[%d] = 0x%8x    \n", u4Temp, u4RegVal);
    x_thread_delay(1);

    for(u4Temp = 52; u4Temp < 256; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadAVSVLD(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadAVSVLD(u4InstID, (u4Temp+1)<<2);
        UTIL_Printf("//RISCRead_AVS_VLD[%d] = 0x%8x	 RISCRead_AVS_VLD[%d] = 0x%8x	 \n", 
                               u4Temp, u4RegVal, (u4Temp+1),u4RegVal1);
        x_thread_delay(1);
    }

    UTIL_Printf("//--------VLD REGISTER DUMP--------\n");
    for(u4Temp = 33; u4Temp < 255; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadVLD(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadVLD(u4InstID, (u4Temp+1)<<2);
        UTIL_Printf("//RISCRead_VLD[%d] = 0x%8x    RISCRead_VLD[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
    u4Temp = 255;
    u4RegVal = u4VDecReadVLD(u4InstID, u4Temp<<2);
    UTIL_Printf("//RISCRead_VLD[%d] = 0x%8x    \n", u4Temp, u4RegVal);
    x_thread_delay(1);
    

    UTIL_Printf("//--------MC REGISTER DUMP--------\n");
    for(u4Temp = 0; u4Temp < 800; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadMC(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadMC(u4InstID, (u4Temp+1)<<2);
        UTIL_Printf("//RISCRead_MC[%d] = 0x%8x    RISCRead_MC[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
    u4Temp = 800;
    u4RegVal = u4VDecReadVLD(u4InstID, u4Temp<<2);
    UTIL_Printf("//RISCRead_MC[%d] = 0x%8x    \n", u4Temp, u4RegVal);
    x_thread_delay(1);
    

    UTIL_Printf("//--------PP REGISTER DUMP--------\n");
    for(u4Temp = 0; u4Temp <1024; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadAVSPP(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadAVSPP(u4InstID, (u4Temp + 1)<<2);
        UTIL_Printf("//RISCRead_PP[%d] = 0x%8x    RISCRead_PP[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
    
    UTIL_Printf("//--------MV REGISTER DUMP--------\n");
    for(u4Temp = 0; u4Temp < 256; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadAVSMV(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadAVSMV(u4InstID, (u4Temp + 1)<<2);
        UTIL_Printf("//RISCRead_MV[%d] = 0x%8x    RISCRead_MV[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
    
    UTIL_Printf("//--------VLD_TOP REGISTER DUMP--------\n");
    for(u4Temp = 0; u4Temp < 40; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadVLDTOP(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadVLDTOP(u4InstID, (u4Temp + 1)<<2);
        UTIL_Printf("//RISCRead_VLD_TOP[%d] = 0x%8x    RISCRead_VLD_TOP[%d] = 0x%8x    \n", 
			u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
    u4Temp = 40;
    u4RegVal = u4VDecReadVLDTOP(u4InstID, u4Temp<<2);
    UTIL_Printf("//RISCRead_VLD_TOP[%d] = 0x%8x    \n", u4Temp, u4RegVal);
    x_thread_delay(1);

    UTIL_Printf("//--------MISC REGISTER DUMP--------\n");
    for(u4Temp = 0; u4Temp < 64; u4Temp=u4Temp+2)
    {
        u4RegVal = u4VDecReadMISC(u4InstID, u4Temp<<2);
        u4RegVal1 = u4VDecReadMISC(u4InstID, (u4Temp + 1)<<2);
        UTIL_Printf("//RISCRead_MISC[%d] = 0x%8x    RISCRead_MISC[%d] = 0x%8x    \n", 
        u4Temp, u4RegVal, (u4Temp+1), u4RegVal1);
        x_thread_delay(1);
    }
        
}

#if DUMP_DRAM_BUFFER
#ifdef SATA_HDD_FS_SUPPORT
extern BOOL fgHDDFsWriteFile(UINT32 u4InstID, CHAR *strFileName, void* pvAddr, UINT32 u4Length, INT32 *pi4FileId);
#else
extern FILE *fopen(const UINT8 *filename, const UINT8 *mode);
extern INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fseek(FILE *stream, INT32 offset, INT32 whence);
extern INT32 fclose(FILE *stream);
extern INT64 ftell(FILE *stream);
#endif
BOOL fgAVSPlusDumpMem( UINT32 u4InstID, UCHAR* ucAddr, UINT32 u4Size , UINT32  u4Field_Num , UINT32 u4Type)
{
    UCHAR fpDumpFileVdecDram[200] = "d:\\ChkFolder\\vDecTotalDram_";
    UCHAR fpDumpFileY_Cur[200] = "d:\\ChkFolder\\vDecCurY_";
    UCHAR fpDumpFileC_Cur[200] = "d:\\ChkFolder\\vDecCurC_";
    UCHAR fpDumpFileY_PreFW[200] = "d:\\ChkFolder\\vDecPreFWY_";
    UCHAR fpDumpFileC_PreFW[200] = "d:\\ChkFolder\\vDecPreFWC_";
    UCHAR fpDumpFileY_FWRef[200] = "d:\\ChkFolder\\vDecFWRefY_";
    UCHAR fpDumpFileC_FWRef[200] = "d:\\ChkFolder\\vDecFWRefC_";
    UCHAR fpDumpFileY_BWRef[200] = "d:\\ChkFolder\\vDecBWRefY_";
    UCHAR fpDumpFileC_BWRef[200] = "d:\\ChkFolder\\vDecBWRefC_";
    UCHAR fpDumpFileMV_1[200] = "d:\\ChkFolder\\vDecMVBuffer1_";
    UCHAR fpDumpFileMV_2[200] = "d:\\ChkFolder\\vDecMVBuffer2_";
    UCHAR fpDumpFilePred[200] = "d:\\ChkFolder\\vDecPred_";
    UCHAR fpDumpFileCRC[200] = "d:\\ChkFolder\\CRC_";

    UCHAR fpDumpFileY_BeforeTrig[200] = "d:\\ChkFolder\\vDecCurY_BeforeTrig_";
    UCHAR fpDumpFileC_BeforeTrig[200] = "d:\\ChkFolder\\vDecCurC_BeforeTrig_";
    //UCHAR fpDumpFilePic1Y[200] ="d:\\ChkFolder\\PicY_1.bin";

    UCHAR *fpDump;
    UINT32 u4ReadSize;
    UINT32 u4Temp;
    FILE *pFile = NULL;
    //char pBitstream_name[200] = {0};
#ifdef SATA_HDD_FS_SUPPORT
    INT32 i4Field = 0;
    BOOL fgResult;
#endif

    //x_memcpy(pBitstream_name , _bFileStr1[u4InstID][1]+12 , (strlen(_bFileStr1[u4InstID][1]) -38) );
    //pBitstream_name[(strlen(_bFileStr1[u4InstID][1]) -38)] = '\0';

    switch(u4Type)
    {
        default:
            fpDump = NULL;
            break;

        case CRC_GEN:
            fpDump = fpDumpFileCRC;
            break;

        case DP_TOTAL_DRAM:
            fpDump = fpDumpFileVdecDram;
            break;

        case DP_CUR_YBUFFER:
            fpDump = fpDumpFileY_Cur;
            break;
        
        case DP_CUR_CBUFFER:
            fpDump = fpDumpFileC_Cur;
            break;

        case DP_PREVFW_Y:
            fpDump = fpDumpFileY_PreFW;
            break;
        
        case DP_PREVFW_C :
            fpDump = fpDumpFileC_PreFW;
            break;
        
        case DP_FWREF_Y:
            fpDump = fpDumpFileY_FWRef;
            break;
        
        case DP_FWREF_C:
            fpDump = fpDumpFileC_FWRef;
            break;
        
        case DP_BWREF_Y:
            fpDump = fpDumpFileY_BWRef;
            break;
        
        case DP_BWREF_C:
            fpDump = fpDumpFileC_BWRef;
            break;
        
        case DP_MV1:
            fpDump = fpDumpFileMV_1;
            break;
        
        case DP_MV2:
            fpDump = fpDumpFileMV_2;
            break;

        case DP_PRED:
            fpDump = fpDumpFilePred;
            break;

        case DP_YBUFFER_BEFORE_TRIGHW:
            fpDump = fpDumpFileY_BeforeTrig;
            break;
        
        case DP_CBUFFER_BEFORE_TRIGHW:
            fpDump = fpDumpFileC_BeforeTrig;
            break;	
    }

    if(fpDump == NULL)
    {
        UTIL_Printf("//Dump Type No Exist!\n");
        return FALSE;
    }
    u4Temp = strlen(fpDump);
    //u4Temp += sprintf(fpDump + u4Temp,"%s_",pBitstream_name);
    u4Temp += sprintf(fpDump + u4Temp,"%d",u4Field_Num);
    u4Temp += sprintf(fpDump + u4Temp,"%s",".bin");

#ifdef SATA_HDD_FS_SUPPORT
    fgResult = fgHDDFsWriteFile(u4InstID, (char* )fpDump, ucAddr, u4Size, &i4Field);
    
    return fgResult;
#else
    pFile = fopen(fpDump,"wb");
    if(pFile == NULL)
    {
        UTIL_Printf("//Create file error !\n");
        return FALSE;
    }

    u4ReadSize = fwrite ((char* )(ucAddr), 1, u4Size, pFile);
    UTIL_Printf("//Read file len = %d @ 0x%x\n",u4ReadSize,(UINT32)ucAddr);
    fclose(pFile);

    return TRUE;
#endif
}
#endif

