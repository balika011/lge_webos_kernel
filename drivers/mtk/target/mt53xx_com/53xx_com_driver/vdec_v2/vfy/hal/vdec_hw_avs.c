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
#include "vdec_info_avs.h"
#include "vdec_hw_avs.h"

#ifdef CONFIG_TV_DRV_VFY
#include "x_hal_5381.h"
#else
#include "x_hal_ic.h"
#endif //CONFIG_TV_DRV_VFY

#if CONFIG_DRV_VERIFY_SUPPORT
#include "vdec_verify_general.h"
#include "vdec_info_verify.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#endif

extern void vVDecOutputDebugString(const CHAR * format, ...);
extern BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO);
extern void vVDecOutputDebugString(const CHAR * format, ...);
#endif

void vVDecWriteAVSVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        vWriteReg(AVS_VLD_REG_OFFSET0 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVS_VLD_REG_OFFSET0, u4Addr, u4Val);
#endif
        //UTIL_Printf("[VDEC]0x      %x (AVS vld Register %d) =  0x%x\n", u4Addr, u4Addr/4, u4Val );
    }
    else
    {
        vWriteReg(AVS_VLD_REG_OFFSET1 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVS_VLD_REG_OFFSET1, u4Addr, u4Val);
#endif
    }
}

UINT32 u4VDecReadAVSVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4RegVal = 0;
#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        u4RegVal = (u4ReadReg(AVS_VLD_REG_OFFSET0 + u4Addr));
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVS_VLD_REG_OFFSET0, u4Addr, u4RegVal);
#endif
        return u4RegVal; 
    }
    else
    {
        u4RegVal = u4ReadReg(AVS_VLD_REG_OFFSET1 + u4Addr);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVS_VLD_REG_OFFSET1, u4Addr, u4RegVal);
#endif
        return u4RegVal;
    }
}

void vVDecWriteAVSMV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        vWriteReg(AVC_MV_REG_OFFSET0 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVC_MV_REG_OFFSET0, u4Addr, u4Val);
#endif
    }
    else
    {
        vWriteReg(AVC_MV_REG_OFFSET1 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVC_MV_REG_OFFSET1, u4Addr, u4Val);
#endif
    }
}

UINT32 u4VDecReadAVSMV(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4RegVal = 0;

#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        u4RegVal = (u4ReadReg(AVC_MV_REG_OFFSET0 + u4Addr));
          
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVC_MV_REG_OFFSET0, u4Addr, u4RegVal);
#endif
        return u4RegVal;
    }
    else
    {
        u4RegVal = u4ReadReg(AVC_MV_REG_OFFSET1 + u4Addr);

#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVC_MV_REG_OFFSET1, u4Addr, u4RegVal);
#endif
        return u4RegVal;
    }
}

void vVDecWriteAVSPP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        vWriteReg(AVS_PP_REG_OFFSET0 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVS_PP_REG_OFFSET0, u4Addr, u4Val);
#endif
    }
    else
    {
        vWriteReg(AVS_PP_REG_OFFSET1 + u4Addr, u4Val);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpW(u4VDecID, AVS_PP_REG_OFFSET1, u4Addr, u4Val);
#endif
    }
}

UINT32 u4VDecReadAVSPP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4RegVal = 0;
#if (CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        u4RegVal = u4ReadReg(AVS_PP_REG_OFFSET0 + u4Addr);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVS_PP_REG_OFFSET0, u4Addr, u4RegVal);
#endif
        return u4RegVal;
    }
    else
    {
        u4RegVal = u4ReadReg(AVS_PP_REG_OFFSET0 + u4Addr);
#ifdef VDEC_SIM_DUMP
        vVDecSimDumpR(u4VDecID, AVS_PP_REG_OFFSET1, u4Addr, u4RegVal);
#endif
        return u4RegVal;
    }
}

// *********************************************************************
// Function : UINT32 dVLDGetBitS(UINT32 u4BSID,UINT32 u4VDecID,UINT32 dShiftBit)
// Description : Get Bitstream from VLD barrel shifter
// Parameter : dShiftBit: Bits to shift (0-32)
// Return    : barrel shifter
// *********************************************************************
UINT32 u4VDecAVSVLDGetBitS(UINT32 u4BSID,UINT32 u4VDecID,UINT32 dShiftBit)
{
    UINT32 u4RegVal;
    
    //UTIL_Printf("[AVS%d] u4VDecAVSVLDGetBitS, dShiftBit:0x%x\n", 
    //  u4BSID, dShiftBit);
    // print input window
    //UTIL_Printf("[AVS%d] u4VDecAVSVLDGetBitS, Input Window: 0x%08x\n", u4VDecID, u4VDecReadVLD(u4VDecID, 0xF0)); 
    
    if (u4BSID == 0)
    {
        u4RegVal = u4VDecReadAVSVLD(u4VDecID, RO_AVLD_BARL  + (dShiftBit << 2));
    }
    else
    {
        u4RegVal = u4VDecReadAVSVLD(u4VDecID, RO_AVLD_2ND_BARL  + (dShiftBit << 2));
    }
    return (u4RegVal);
}

// *********************************************************************
// Function : UINT32 u4VDecAVSVLDShiftBits(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get AVSVLD shift bits %64
// Parameter : None
// Return    : VLD Sum
// *********************************************************************
UINT32 u4VDecAVSVLDShiftBits(UINT32 u4BSID, UINT32 u4VDecID)
{
    if (u4BSID==0)
    {
        return ((u4VDecReadAVSVLD(u4VDecID, RW_AVLD_CTRL) >> 16) & 0x3F);
    }
    else
    {
        return (u4VDecReadAVSVLD(u4VDecID, RW_AVLD_2ND_CTRL) & 0x3F);
    }
}


#if DTV_COMMON_CONFIG
#define RPTR_ALIGN   16
#define WPTR_OFFSET  512
#define WPTR_SETBY_RISC_EN ((UINT32)0x1 << 18)

void vVDEC_SetAvsVldWPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4WPtr)
{
    UINT32 u4WPtrAlign, u4VStart, u4VEnd;

    u4WPtrAlign = ((u4WPtr + RPTR_ALIGN) & (~((UINT32)RPTR_ALIGN - 1))) + WPTR_OFFSET;


    u4VStart = u4VDecReadVLD(u4VDecID, RW_VLD_VSTART)<<6;
    u4VEnd = u4VDecReadVLD(u4VDecID, RW_VLD_VEND)<<6;    

    UTIL_Printf("[AVS] vVDEC_SetAvsVldWPtr, VStart:0x%x, VEnd:0x%x, Wptr:0x%x, WptrAlign:0x%x, ASYNC:0x%x\n", 
      u4VStart, u4VEnd, u4WPtr, u4WPtrAlign, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC)); 

    if(u4WPtrAlign < u4VStart)
    {
        u4WPtrAlign = u4VEnd - (u4VStart - u4WPtrAlign);
    }
    
    if(u4WPtrAlign > u4VEnd)
    {
        u4WPtrAlign = u4VStart + (u4WPtrAlign - u4VEnd);
    }

    //vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) | WPTR_SETBY_RISC_EN);
    //vVDecWriteVLD(u4VDecID, WO_VLD_WPTR, u4WPtrAlign);

    vVDecWriteVLD(u4VDecID, WO_VLD_WPTR + (u4BSID << 10), u4WPtr); // 0xFFFFFFFF for test only
    vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC + (u4BSID << 10), u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) | 0x1040104);
}
#endif

// *********************************************************************
// Function : BOOL fgH264VLDInitBarrelShifter1(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VLDRdPtr, UINT32 u4VLDWrPtr)
// Description : Init HW Barrel Shifter
// Parameter : u4Ptr: Physical DRAM Start Address to fill Barrel Shifter
// Return    : TRUE: Initial Success, Fail: Initial Fail
// *********************************************************************
BOOL fgAVSVLDInitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4FIFOSa, UINT32 u4VLDRdPtr, UINT32 u4VLDWrPtr)
{
    UINT32 u4ByteAddr;
    UINT32 u4TgtByteAddr;
    INT32 i;
    BOOL fgFetchOK = FALSE;
    UINT32 u4Cnt = 0;
    
    // prevent initialize barrel fail
    for (i = 0; i < 5; i++)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_RPTR + (u4BSID << 10), u4VLDRdPtr);
        #if 0//DTV_COMMON_CONFIG
        vVDEC_SetAvsVldWPtr(u4BSID, u4VDecID, u4VLDWrPtr);
        #else
        vVDecWriteVLD(u4VDecID, WO_VLD_WPTR + (u4BSID << 10), u4VLDWrPtr);
        #if DTV_COMMON_CONFIG

        #else
        vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC + (u4BSID << 10), u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) | 0x1040104);
        #endif
        #endif        
      
        // start to fetch data
        vVDecWriteVLD(u4VDecID, RW_VLD_PROC + (u4BSID << 10), VLD_INIFET);
        if (fgVDecWaitVldFetchOk(u4BSID, u4VDecID))
        {
            fgFetchOK = TRUE;
            //UTIL_Printf("[AVS] fgAVSVLDInitBarrelShifter, BS: %d, fetch OK!!\n", u4BSID); 
            break;
        }
    }

    if (!fgFetchOK)
    {
        UTIL_Printf("//[AVS] fgAVSVLDInitBarrelShifter, BS: %d, fetch NG!!\n", u4BSID); 
        return (FALSE);
    }
  
    vVDecWriteVLD(u4VDecID, RW_VLD_PROC + (u4BSID << 10), VLD_INIBR);
    
    if (u4BSID == 0)
    {
      #if DTV_COMMON_CONFIG // only test, refer to TV code
      while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D) == 0)
      {
          u4Cnt++;
          if(u4Cnt >= 0x1000)
          {
              LOG(4, "//===SRAM !Stable Fail 0xF4=0x%x, Rptr 0xB0=0x%x, Wptr 0x110=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                  u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_RPTR), u4VDecReadVLD(u4VDecID, WO_VLD_WPTR), u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR),
                  u4VDecReadVLD(u4VDecID, RO_VLD_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK));
              break;
          }    
      }
      
      LOG(4, "//===SRAM !Stable inquiry, 0xF4=0x%x, Rptr 0xB0=0x%x, Wpt 0x110=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                  u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_RPTR), u4VDecReadVLD(u4VDecID, WO_VLD_WPTR), u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR),
                  u4VDecReadVLD(u4VDecID, RO_VLD_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK));

      #if VDEC_TEST_LOG
      UTIL_Printf("//[AVS] fgAVSVLDInitBarrelShifter, BS: %d, u4VLDRdPtr:0x%x, wait SRAM stable!!\n", u4BSID, u4VLDRdPtr); 
      #endif

      u4Cnt = 0;
      #endif     
      
      // HW workaround
      // can not reset sum off until barrel shifter finish initialization 
      #if DTV_COMMON_CONFIG
      //UTIL_Printf("[AVS] fgAVSVLDInitBarrelShifter, BS: %d, SYS_CKCFG value: %d\n", u4BSID, u4ReadReg(CKGEN_REG_VDEC_SYS_CKCFG));      
      //vWriteReg(CKGEN_REG_VDEC_SYS_CKCFG,VDEC_SYS_CK_SEL_SLOW);
      //UTIL_Printf("[AVS] fgAVSVLDInitBarrelShifter, BS: %d, SYS_CKCFG value: %d\n", u4BSID, u4ReadReg(CKGEN_REG_VDEC_SYS_CKCFG));  
      #else //#if DTV_COMMON_CONFIG
      while((((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0xf) * 4) + 
            ((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) >> 24) & 0x3)) !=4)    
      {
              u4Cnt++;
              if(u4Cnt >= 0x1000)
              {
                  UTIL_Printf("//[AVS] fgAVSVLDInitBarrelShifter, BS: %d, fetch OK, but wait SRAM stable FAIL!! - 2\n", u4BSID); 
                  break;
              }      
      }
      #endif //#if DTV_COMMON_CONFIG
    }

    // move range 0~15 bytes
    u4TgtByteAddr = u4VLDRdPtr & 0xf;
    u4ByteAddr = u4VLDRdPtr & 0xfffffff0;
    while (u4TgtByteAddr)
    {
        u4TgtByteAddr --;
        u4ByteAddr ++;
        u4VDecAVSVLDGetBitS(u4BSID, u4VDecID, 8);
    }  

    // Reset byte counts
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CLR_BYTE_COUNT + (u4BSID << 10), 1);
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CLR_BYTE_COUNT + (u4BSID << 10), 0);
    
    return (TRUE);
}


// *********************************************************************
// Function : BOOL fgInitH264BarrelShift1(UINT32 u4VDecID, VDEC_INFO_H264_BS_INIT_PRM_T *prH264BSInitPrm)
// Description : Reset VLD2
// Parameter : None
// Return    : None
// *********************************************************************
BOOL fgInitAVSBarrelShift1(UINT32 u4VDecID, VDEC_INFO_AVS_BS_INIT_PRM_T *prAvsBSInitPrm)
{   
    #if DTV_COMMON_CONFIG
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, 0x1);
    
    #ifdef CUR_CHIP_5881
    vVDecWriteVLD(u4VDecID, RO_VLD_VBAR, u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) | (0x1<<28));
    #endif
    #else
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_CTRL, 0x8007);
    vVDecWriteAVSVLD(u4VDecID, RW_AVS_VLD_MODE_SWITCH, 7);
    #endif
    
    vVDecSetVLDVFIFO(0, u4VDecID, HW_ACCESSED_MEM(prAvsBSInitPrm->u4VFifoSa), HW_ACCESSED_MEM((UINT32) prAvsBSInitPrm->u4VFifoEa));
     
    if (!fgAVSVLDInitBarrelShifter(0, u4VDecID, HW_ACCESSED_MEM((UINT32) prAvsBSInitPrm->u4VFifoSa), HW_ACCESSED_MEM((UINT32) prAvsBSInitPrm->u4VLDRdPtr), HW_ACCESSED_MEM(prAvsBSInitPrm->u4VLDWrPtr)))
    {
        return FALSE;
    }
    return TRUE;
}

// *********************************************************************
// Function : BOOL fgInitH264BarrelShift2(UINT32 u4RDPtrAddr)
// Description : Reset VLD2
// Parameter : None
// Return    : None
// *********************************************************************
BOOL fgInitAVSBarrelShift2(UINT32 u4VDecID, VDEC_INFO_AVS_BS_INIT_PRM_T *prAvsBSInitPrm)
{    
    //UTIL_Printf("VDEC: INIT AVS Barrel Shift2\n");
    VDEC_ASSERT(0);
    return TRUE;
}

 UINT32 u4VDecReadAVSVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
      #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
    UINT32 u4Cnt;
    #endif
  
    // HW issue, wait for read pointer stable
       #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
      u4Cnt = 50000;
      if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (1<<15))
      	{
      	while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1)) && (u4Cnt--));
      	}
      #else
    while (!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&1));
      #endif
      
      #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
      if (0 == u4Cnt)
      {
        UTIL_Printf("[AVS%d] u4VDecReadAVSVldRPtr, RO_VLD_SRAMCTRL time out!\n", u4BSID);
      }
      #endif

    u4DramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR + (u4BSID << 10));
    u4SramRptr = ((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))) & 0xf) * 4 +
                     (((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))>>24)) & 0x3); //count in 128bits
    u4SramWptr = (((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))>>8)) & 0xf) *4;

    //UTIL_Printf("[AVS%d] u4VDecReadAVSVldRPtr, VBAR:0x%x, DramRptr:0x%x, SramRptr:0x%x, SramWptr:0x%x\n", 
    //  u4BSID, u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10)), u4DramRptr, u4SramRptr, u4SramWptr);

    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
    }

    *pu4Bits = (u4VDecReadAVSVLD(u4VDecID, RW_AVS_VLD_CTRL + (u4BSID << 10)) >> 16) & 0x3f;

    //UTIL_Printf("[AVS%d] u4VDecReadAVSVldRPtr, u4SramDataSz:0x%x, *pu4Bits:0x%x\n", u4BSID, u4SramDataSz, *pu4Bits);
      
      #if 1 //def MEM_PAUSE_SUPPORT  
      
      #ifdef CUR_CHIP_5881
      u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 7) * 4)) + (*pu4Bits / 8);
      #else
      u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 5) * 4)) + (*pu4Bits / 8);
      #endif

      //UTIL_Printf("[AVS%d] u4VDecReadAVSVldRPtr, u4ByteAddr:0x%x, u4VFIFOSa:0x%x, 1\n", u4BSID, u4ByteAddr, u4VFIFOSa);
      #else
      u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 4) * 4)) + (*pu4Bits / 8);

      UTIL_Printf("[AVS%d] u4VDecReadAVSVldRPtr, u4ByteAddr:0x%x, u4VFIFOSa:0x%x, used, 2\n", u4BSID, u4ByteAddr, u4VFIFOSa);
      #endif
  
    *pu4Bits &= 0x7;

    if (u4ByteAddr < u4VFIFOSa)
    {
        u4ByteAddr = u4ByteAddr + 
                            ((u4VDecReadVLD(u4VDecID, RW_VLD_VEND + (u4BSID << 10)) << 6) - ((UINT32)u4VFIFOSa))
                            - u4VFIFOSa;
    }
    else
    {
        u4ByteAddr -= ((UINT32)u4VFIFOSa);
    }
  
    return (u4ByteAddr);
}

