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
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#include "vdec_info_h265.h"
#include "vdec_hw_h265.h"
#include "../vdec_debug.h"
//#include "x_hal_ic.h"

// *********************************************************************
// Function : void vVDecWriteHEVCVLD(UINT32 u4VDecID)
// Description : Write HEVC VLD register
// Parameter : u4VDecID : VLD ID [0: HEVC VLD0; 1: HEVC VLD1; 2: LAE VLD0; 3:LAE VLD1]
// Return    : None

void vVDecWriteHEVCVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if(u4VDecID == 0)
    {
        vWriteReg(HEVC_VLD_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_VLD_REG_OFFSET0, u4Addr, u4Val);
    }
    else if(u4VDecID == 1)
    {
        vWriteReg(HEVC_VLD_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_VLD_REG_OFFSET1, u4Addr, u4Val);
    }
    else if(u4VDecID == 2)
    {
        vWriteReg(LAE_HEVCVLD_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_HEVCVLD_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_HEVCVLD_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_HEVCVLD_OFFSET1, u4Addr, u4Val);
    }
}

UINT32 u4VDecReadHEVCVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(HEVC_VLD_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_VLD_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 1)
    {
        u4Val = u4ReadReg(HEVC_VLD_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_VLD_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 2)
    {
        u4Val = u4ReadReg(LAE_HEVCVLD_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_HEVCVLD_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(LAE_HEVCVLD_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_HEVCVLD_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }

}

void vVDecWriteHEVCMV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
     if(u4VDecID == 0)
     {
        vWriteReg(HEVC_MV_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_MV_REG_OFFSET0, u4Addr, u4Val);
     }
     else
     {
        vWriteReg(HEVC_MV_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_MV_REG_OFFSET1, u4Addr, u4Val);
     }
        
}

UINT32 u4VDecReadHEVCMV(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(HEVC_MV_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_MV_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(HEVC_MV_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_MV_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
}

void vVDecWriteHEVCPP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
     if(u4VDecID == 0)
     {
        vWriteReg(HEVC_PP_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_PP_REG_OFFSET0, u4Addr, u4Val);
     }
     else
     {
        vWriteReg(HEVC_PP_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_PP_REG_OFFSET1, u4Addr, u4Val);
     }

}


UINT32 u4VDecReadHEVCPP(UINT32 u4VDecID, UINT32 u4Addr)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
     UINT32 u4Val;
       
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(HEVC_PP_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_PP_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(HEVC_PP_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_PP_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
}


void vVDecWriteHEVCMISC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if(u4VDecID == 0)
    {
        vWriteReg(HEVC_MISC_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_MISC_REG_OFFSET0, u4Addr, u4Val);
    }
    else if(u4VDecID == 1)
    {
        vWriteReg(HEVC_MISC_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, HEVC_MISC_REG_OFFSET1, u4Addr, u4Val);
    }
    else if(u4VDecID == 2)
    {
        vWriteReg(LAE_MISC_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_MISC_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_MISC_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_MISC_OFFSET1, u4Addr, u4Val);
    }

}

UINT32 u4VDecReadHEVCMISC(UINT32 u4VDecID, UINT32 u4Addr)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
    UINT32 u4Val;

    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(HEVC_MISC_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_MISC_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 1)
    {
        u4Val = u4ReadReg(HEVC_MISC_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, HEVC_MISC_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 2)
    {
        u4Val = u4ReadReg(LAE_MISC_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_MISC_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(LAE_MISC_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_MISC_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }

}


void vVDecWriteLAEHEVCVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
     u4VDecID = 0;
    #endif
    if(u4VDecID == 0)
    {
        vWriteReg(LAE_HEVCVLD_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_HEVCVLD_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_HEVCVLD_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_HEVCVLD_OFFSET1, u4Addr, u4Val);
    }
}

UINT32 u4VDecReadLAEHEVCVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    UINT32 u4Val;

    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(LAE_HEVCVLD_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_HEVCVLD_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(LAE_HEVCVLD_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_HEVCVLD_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
}

// *********************************************************************
// Function : UINT32 dVLDGetBitS(UINT32 u4BSID,UINT32 u4VDecID,UINT32 dShiftBit)
// Description : Get Bitstream from VLD barrel shifter
// Parameter : dShiftBit: Bits to shift (0-32)
// Return    : barrel shifter
// *********************************************************************
UINT32 u4VDecHEVCVLDGetBitS(UINT32 u4BSID,UINT32 u4VDecID,UINT32 dShiftBit)
{

    UINT32 u4RegVal, u4index;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    for (u4index = 0; u4index<(dShiftBit/32);  u4index++ )
    {
        u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_BARL + (32<<2));
    }
    
    u4RegVal = u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_BARL + ((dShiftBit%32) << 2));
    return (u4RegVal);
}

// *********************************************************************
// Function : UINT32 u4VDecHEVCVLDShiftBits(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get HEVCVLD shift bits %64
// Parameter : None
// Return    : VLD Sum
// *********************************************************************
UINT32 u4VDecHEVCVLDShiftBits(UINT32 u4BSID, UINT32 u4VDecID)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    return ((u4VDecReadHEVCVLD(u4VDecID, RW_HEVLD_CTRL) >> 8) & 0x3F);

}

// *********************************************************************
// Function    : UINT32 u4VDECHEVCInitSearchStartCode(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get next start code
// Parameter   : u4BSID : Barrel shifter ID
//                   u4VDecID : VLD ID
//                    u4ShiftBit : Shift bits
// Return      : None
// *********************************************************************
UINT32 u4VDECHEVCInitSearchStartCode(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBit)
{
    UINT32 u4BitsCnt;
    UINT32 i = 0;
    UINT32 u4RetryNum = 0x100000;

#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDECHEVCInitSearchStartCode() Start!!\n");
#endif

    if ( u4ShiftBit > 0){
        vVDecWriteHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG,  HEVC_SC_START );
        for (i = 0; i < u4RetryNum; i++ ){
             if ((u4VDecReadHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG) & HEVC_SC_START) == 0)
                 break;
        }
        u4BitsCnt = u4VDecReadHEVCVLD( u4VDecID, HEVC_BITCNT_REG);
    }
    if ( i == u4RetryNum )
         LOG(0,"\n// [ERROR] u4VDECHEVCInitSearchStartCode() Fail!!!!!!!!!!!!!!\n\n");

    while( u4ShiftBit > (u4BitsCnt+16) ){
        vVDecWriteHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG,  HEVC_SC_START );
        for (i = 0; i < u4RetryNum; i++ ){
             if ((u4VDecReadHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG) & HEVC_SC_START) == 0)
                 break;
        }
        u4BitsCnt = u4VDecReadHEVCVLD( u4VDecID, HEVC_BITCNT_REG);
        
        if ( i == u4RetryNum ){
            LOG(0,"\n// [ERROR] u4VDECHEVCInitSearchStartCode() Fail!!!!!!!!!!!!!!\n\n");
            break;
        }
    }
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDECHEVCInitSearchStartCode() Done!!\n");
#endif
    return 1;
}



// *********************************************************************
// Function : BOOL fgH265VLDInitBarrelShifter1(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VLDRdPtr, UINT32 u4VLDWrPtr)
// Description : Init HW Barrel Shifter
// Parameter : u4Ptr: Physical DRAM Start Address to fill Barrel Shifter
// Return    : TRUE: Initial Success, Fail: Initial Fail
// *********************************************************************
BOOL fgH265VLDInitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4FIFOSa, UINT32 u4VLDRdPtr, UINT32 u4VLDWrPtr)
{
    UINT32 u4RdptrLsb = 0,u4ShiftByteAcc = 0,u4EmuCnt = 0,u4RegVal = 0;
    BOOL fgFetchOK = FALSE;
    UINT32 u4Cnt;
    // prevent initialize barrel fail
    //for (i = 0; i < 1; i++)
    {
        u4Cnt = 50000;
        if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (1<<15))
    	{
    	    while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1)) && (u4Cnt--));
    	}
        // read pointer
        vVDecWriteVLD(u4VDecID, RW_VLD_RPTR , u4VLDRdPtr);
  
        // bitstream DMA async_FIFO  local reset
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST , 1 << 8);
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST , 0);

        //Just for emulation test,driver need remove this register setting
        #if (CONFIG_DRV_FPGA_BOARD)
        //vVDecWriteVLD(u4VDecID,RW_VLD_BS_SPEEDUP,1000);
        #endif

        // start to fetch data
        vVDecWriteVLD(u4VDecID, RW_VLD_PROC, VLD_INIFET);
        if (fgVDecWaitVldFetchOk(u4BSID, u4VDecID))
        {
            fgFetchOK = TRUE;
        }
    }

    if (!fgFetchOK)
    {
        LOG(2,"[VDEC-ERR] VLD Fetch Fail VdecId %d!\n",u4VDecID);
        return (FALSE);
    }
    
    // initial barrel shifter
    vVDecWriteVLD(u4VDecID, RW_VLD_PROC + (u4BSID << 10), VLD_INIBR);

    // FW_shift precise to byte address

    u4RdptrLsb = u4VLDRdPtr & 0x0F;
    #if 0
    while(u4ShiftByteAcc < u4RdptrLsb)
    {
        u4RegVal = u4VDecReadHEVCVLD(u4VDecID,RW_HEVLD_CTRL);
        u4EmuCnt = ((u4RegVal >> 24) & 0x01) + ((u4RegVal >> 25) & 0x01) + \
                    ((u4RegVal >> 26) & 0x01) + ((u4RegVal >> 27) & 0x01);
        u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 8);
        u4ShiftByteAcc ++;
        if(u4ShiftByteAcc%4 == 0)
        {
            LOG(2,"shifter %d emu %d\n",u4ShiftByteAcc,u4EmuCnt);
            u4ShiftByteAcc += u4EmuCnt;
        }
    }
    #else
    while ((u4ShiftByteAcc + u4EmuCnt) < u4RdptrLsb)
    {
        u4RegVal = u4VDecReadHEVCVLD(u4VDecID,RW_HEVLD_CTRL);
        if( (u4RegVal >> (24 + (u4ShiftByteAcc % 4)+u4EmuCnt))&0x1) 
        {
            u4EmuCnt ++;
        } 
        else 
        {
            u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 8);
            u4ShiftByteAcc ++;

            if (u4ShiftByteAcc%4 == 0)
            {
                u4ShiftByteAcc += (((u4RegVal>>24)&0x1) + ((u4RegVal>>25)&0x1) + ((u4RegVal>>26)&0x1) + ((u4RegVal>>27)&0x1)); 
                u4EmuCnt -= (((u4RegVal>>24)&0x1) + ((u4RegVal>>25)&0x1) + ((u4RegVal>>26)&0x1) + ((u4RegVal>>27)&0x1)); 
            }
        }
    }
    #endif
    //u4VDECHEVCInitSearchStartCode( u4BSID, u4VDecID, (u4VLDRdPtr&0xF) * 8  );
#ifdef VDEC_SIM_DUMP
    LOG(0,"// {H265 DEC >> } u4InstID = 0x%x, Input Window: 0x%08x\n", u4VDecID, u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_BARL));
#endif
    return (TRUE);
}


// *********************************************************************
// Function : BOOL fgInitH265BarrelShift(UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm)
// Description : Reset VLD2
// Parameter : None
// Return    : None
// *********************************************************************
BOOL fgInitH265BarrelShift(UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm)
{

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_CTRL, HEVC_EN);
    
    if(prH265BSInitPrm->u4DualCoreEnable)
    {
        vVDecWriteVLD(u4VDecID, 80*4,(u4VDecReadVLD(u4VDecID,80*4) | (1 << 20)));
        vVDecWriteBS2(u4VDecID, 80*4,(u4VDecReadBS2(u4VDecID,80*4) | (1 << 20)));
    }
    
    vVDecWriteVLD(u4VDecID, 59*4, (0x1<<28));

    if(prH265BSInitPrm->u4DualCoreEnable)
    {
        vVDecWriteBS2(u4VDecID, 59*4,(u4VDecReadBS2(u4VDecID,59*4) | (1 << 28)));
    }
    
    vVDecSetVLDVFIFO(0, u4VDecID, PHYSICAL(prH265BSInitPrm->u4VFifoSa), PHYSICAL((UINT32) prH265BSInitPrm->u4VFifoEa));
    //vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PRED_ADDR, u4AbsDramANc((UINT32) prH265BSInitPrm->u4PredSa));   
    if(prH265BSInitPrm->u4DualCoreEnable)
    {
        vVDecSetBS2VFIFO(0, u4VDecID, PHYSICAL(prH265BSInitPrm->u4VFifoSa), PHYSICAL((UINT32) prH265BSInitPrm->u4VFifoEa));
    }
#ifdef VDEC_SIM_DUMP
     LOG(0,"// [INFO] u4VFifoSa = 0x%08x, u4VFifoEa = 0x%08x\n", PHYSICAL(prH265BSInitPrm->u4VFifoSa), PHYSICAL((UINT32) prH265BSInitPrm->u4VFifoEa) );
#endif
    
    // Reset HEVC VLD Sum
    /*
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_RESET_SUM, HEVLD_RESET_SUM_ON);
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_RESET_SUM, HEVLD_RESET_SUM_OFF);  
    */
    
    if (!fgH265VLDInitBarrelShifter(0, u4VDecID, PHYSICAL((UINT32) prH265BSInitPrm->u4VFifoSa), PHYSICAL((UINT32) prH265BSInitPrm->u4VLDRdPtr), PHYSICAL(prH265BSInitPrm->u4VLDWrPtr)))
    {
        return FALSE;
    }
    return TRUE;
}


UINT32 u4VDecReadH265VldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
    UINT32 u4RegVal, u4SramCtr;
	  UINT32 vb_sram_ra, vb_sram_wa, seg_rcnt;
    UINT32 u4Cnt = 0;
    UINT32 u4BsBufLen = 0;

    // HW issue, wait for read pointer stable
    u4Cnt = 50000;
    //if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (1<<15))
    
    #ifdef VDEC_SIM_DUMP
    LOG(0,"if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n");
    #endif
    
    if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (PROCESS_FLAG))
    {
        while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1)) && (u4Cnt--));
    }
    
    u4RegVal = u4VDecReadVLD(u4VDecID, RO_VLD_VBAR );
    vb_sram_ra = u4RegVal & 0x1F;
    vb_sram_wa = (u4RegVal >> 8) & 0x1F;
    seg_rcnt = (u4RegVal >> 24) & 0x3;


    u4SramRptr = vb_sram_ra;
    u4SramWptr = vb_sram_wa;
    u4SramCtr = seg_rcnt;
    u4DramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR );

    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
	    u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
    }

    (*pu4Bits) = u4VDecHEVCVLDShiftBits(u4BSID,u4VDecID);
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// <vdec> ReadH265VldRPtr, dRptr:0x%08X, sra:0x%08X, swa:0x%08X, scnt:0x%08X, sum:0x%08X\n", 
                                        u4DramRptr, vb_sram_ra, vb_sram_wa, seg_rcnt, *pu4Bits);
    #endif
    
    
    u4BsBufLen = 7 * 4;
    u4ByteAddr = u4DramRptr - u4SramDataSz * 16 + u4SramCtr * 4 - u4BsBufLen + (*pu4Bits)/8;  
    
    (*pu4Bits) &= 0x7;


    //HEVC "03" consumsion align 4
    u4ByteAddr = (u4ByteAddr >> 2) << 2;

    //printk("[INFO] Calculated u4ByteAddr: 0x%08X,  u4VFIFOSa: 0x%08X\n", u4ByteAddr, u4VFIFOSa );

    if (u4ByteAddr < u4VFIFOSa)
    {
    
        u4ByteAddr = u4ByteAddr  
                     + ((u4VDecReadVLD(u4VDecID, RW_VLD_VEND) << 6) - ((UINT32)u4VFIFOSa))
                     - u4VFIFOSa;
    }
    else
    {
        u4ByteAddr -= ((UINT32)u4VFIFOSa);
    }
    
    #ifdef VDEC_SIM_DUMP
	LOG(0,"// [INFO] Return u4ByteAddr: 0x%08X, Input window: 0x%08X\n", u4ByteAddr, u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_BARL) );
    #endif
    
    return (u4ByteAddr);
}







