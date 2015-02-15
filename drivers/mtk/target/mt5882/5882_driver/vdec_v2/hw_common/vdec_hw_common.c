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
#include "x_hal_5381.h"
#include "x_assert.h"

// *********************************************************************
// Function : void vVDecWriteVLD(UINT32 u4VDecID)
// Description : Write VLD register
// Parameter : u4VDecID : VLD ID [0:  VLD0; 1:  VLD1; 2: LAE VLD0; 3:LAE VLD1]
// Return    : None
// *********************************************************************
void vVDecWriteVLD_EX(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        vWriteReg(VLD_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VLD_REG_OFFSET0, u4Addr, u4Val);
    }
    else if(u4VDecID == 1)
    {
        vWriteReg(VLD_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VLD_REG_OFFSET1, u4Addr, u4Val);
    }
    else if(u4VDecID == 2)
    {
        vWriteReg(LAE_VLD_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLD_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_VLD_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLD_OFFSET1, u4Addr, u4Val);
    }
}


UINT32 u4VDecReadVLD_EX(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(VLD_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, VLD_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 1)
    {   
        u4Val = u4ReadReg(VLD_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, VLD_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 2)
    {   
        u4Val = u4ReadReg(LAE_VLD_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_VLD_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {   
        u4Val = u4ReadReg(LAE_VLD_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_VLD_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    //return 0;
}

void vVDecWriteVLDTOP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{

    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        vWriteReg(VLD_TOP_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VLD_TOP_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (u4VDecID == 1)
    {
        vWriteReg(VLD_TOP_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VLD_TOP_REG_OFFSET1, u4Addr, u4Val);
    }
    else if (u4VDecID == 2)
    {
        vWriteReg(LAE_VLDTOP_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLDTOP_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_VLDTOP_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLDTOP_OFFSET1, u4Addr, u4Val);
    }
}


UINT32 u4VDecReadVLDTOP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(VLD_TOP_REG_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, VLD_TOP_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if (u4VDecID == 1)
    {
        u4Val = u4ReadReg(VLD_TOP_REG_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, VLD_TOP_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else if (u4VDecID == 2)
    {
        u4Val = u4ReadReg(LAE_VLDTOP_OFFSET0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_VLDTOP_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = u4ReadReg(LAE_VLDTOP_OFFSET1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_VLDTOP_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    //return 0;
}


void vVDecWriteMC_EX(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (0 == u4VDecID)
    {
        vWriteReg(MC_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, MC_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
        vVDecSimDumpW(u4VDecID, MC_REG_OFFSET1, u4Addr, u4Val);
        vWriteReg(MC_REG_OFFSET1 + u4Addr, u4Val);
    }
    else
    {
        //lae do noting
    }
}


UINT32 u4VDecReadMC_EX(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    //UTIL_Printf("u4VDecID:%d, MC_REG_OFFSET0:0x%x\n",u4VDecID, MC_REG_OFFSET0);
    if (0 == u4VDecID)
    {
        u4Val = (u4ReadReg(MC_REG_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, MC_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if (1 == u4VDecID)
    {
        u4Val = (u4ReadReg(MC_REG_OFFSET1 + u4Addr));
        vVDecSimDumpR(u4VDecID, MC_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        //LOG(0,"err HW ID param !\n");
        //return 0;
        //lae do nothing
    }
    return 0;
}

void vVDecWritePP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (0 == u4VDecID)
    {
        vWriteReg(PP_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, PP_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
        vWriteReg(PP_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, PP_REG_OFFSET1, u4Addr, u4Val);
    }
    else
    {
        //lae do nothing
    }
}


UINT32 vVDecReadPP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif	
    if (0 == u4VDecID)
    {
        u4Val = (u4ReadReg(PP_REG_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, PP_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if (1 == u4VDecID)
    {
        u4Val = (u4ReadReg(PP_REG_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, PP_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        //LOG(0,"err HW ID param !\n");
        //return 0;
        //lae do nothing
    }
    return 0;
}


void vVDecWriteBS2(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (0 == u4VDecID)
    {
        vWriteReg(VDEC_BS2_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_BS2_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
        vWriteReg(VDEC_BS2_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_BS2_OFFSET1, u4Addr, u4Val);
    }
    else
    {
        //lae do nothing
    }
}


UINT32 u4VDecReadBS2(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(VDEC_BS2_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, VDEC_BS2_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if (u4VDecID == 1)
    {
        u4Val = (u4ReadReg(VDEC_BS2_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, VDEC_BS2_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        //LOG(0,"err HW ID param !\n");
        //return 0;
        //lae do nothing
    }
    return 0;
}


void vVDecWriteCRC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        vWriteReg(VDEC_CRC_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_CRC_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
        vWriteReg(VDEC_CRC_REG_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_CRC_REG_OFFSET1, u4Addr, u4Val);       
    }
    else
    {
        //lae do nothing
    }
}

void vVDecEnableCRC(UINT32 u4VDecID, BOOL fgEnable, BOOL fgCRCFromMC)
{
    UINT32 u4CRCSrc;
    UINT32 u4Val = 0;
    u4VDecID = 0;

    if (fgEnable)
    {
        u4Val = VDEC_CRC_EN;
    }

    if (fgCRCFromMC)
    {
        u4CRCSrc = u4Val | VDEC_CRC_SRC_MC;  // CRC input from MC
    }
    else
    {
        u4CRCSrc = u4Val | VDEC_CRC_SRC_PP;
    }

    vVDecWriteCRC(u4VDecID, VDEC_CRC_REG_EN, u4CRCSrc);
}

UINT32 u4VDecReadCRC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(VDEC_CRC_REG_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, VDEC_CRC_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else if (1 == u4VDecID)
    {
        u4Val = (u4ReadReg(VDEC_CRC_REG_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, VDEC_CRC_REG_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        //LOG(0,"err HW ID param !\n");
        //return 0;
        //lae do noting
    }
    return 0;
}

UINT32 u4VDecReadMCoreCRC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    u4VDecID = 0;
    u4Val = (u4ReadReg(MVDEC_TOP_OFFSET0 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_OFFSET0, u4Addr, u4Val);
    UNUSED(u4VDecID);
    return u4Val;
}

void vVDecWriteDV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        vWriteReg(DV_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, DV_REG_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(DV_REG_OFFSET1 + u4Addr, u4Val);
    }
}

UINT32 u4VDecReadDV(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(DV_REG_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, DV_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        return (u4ReadReg(DV_REG_OFFSET1 + u4Addr));
    }
    //return 0;
}

void vVDecWriteLAEVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        vWriteReg(LAE_VLD_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLD_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_VLD_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLD_OFFSET1, u4Addr, u4Val);
    }
}


void vVDecWriteLAEVLDTOP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        vWriteReg(LAE_VLDTOP_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLDTOP_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_VLDTOP_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_VLDTOP_OFFSET1, u4Addr, u4Val);
    }
}

void vVDecWriteLAELARB(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        vWriteReg(LAE_LARB_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_LARB_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_LARB_OFFSET1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_LARB_OFFSET1, u4Addr, u4Val);
    }
}

void vVDecWriteLAEMISC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
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

void vVDecWriteMCORETOP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    u4VDecID = 0;
    vWriteReg(MVDEC_TOP_OFFSET0 + u4Addr, u4Val);
    vVDecSimDumpW(u4VDecID, MVDEC_TOP_OFFSET0, u4Addr, u4Val);
    UNUSED(u4VDecID);
}

void vVDecWriteMCORETOPLARB(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    u4VDecID = 0;
    vWriteReg(MVDEC_TOP_LARB_OFFSET1 + u4Addr, u4Val);
    vVDecSimDumpW(u4VDecID, MVDEC_TOP_LARB_OFFSET1, u4Addr, u4Val);
    UNUSED(u4VDecID);
}

UINT32 u4VDecReadLAEVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(LAE_VLD_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_VLD_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = (u4ReadReg(LAE_VLD_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_VLD_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    //return 0;
}

UINT32 u4VDecReadLAEVLDTOP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(LAE_LARB_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_LARB_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = (u4ReadReg(LAE_LARB_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_LARB_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    //return 0;
}


UINT32 u4VDecReadLAELARB(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(LAE_VLDTOP_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_VLDTOP_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = (u4ReadReg(LAE_VLDTOP_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_VLDTOP_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
    //return 0;
}


UINT32 u4VDecReadLAEMISC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(LAE_MISC_OFFSET0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_MISC_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        u4Val = (u4ReadReg(LAE_MISC_OFFSET1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, LAE_MISC_OFFSET1, u4Addr, u4Val);
        return u4Val;
    }
//    return 0;
}


UINT32 u4VDecReadMCORETOP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    u4VDecID = 0;
    u4Val = (u4ReadReg(MVDEC_TOP_OFFSET0 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_OFFSET0, u4Addr, u4Val);
    UNUSED(u4VDecID);
    return u4Val;
}

UINT32 u4VDecReadMCORETOPLARB(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    u4VDecID = 0;
    u4Val = (u4ReadReg(MVDEC_TOP_LARB_OFFSET1 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_LARB_OFFSET1, u4Addr, u4Val);
    UNUSED(u4VDecID);
    return u4Val;
}

void vVDec_HAL_CRC_Enable(UINT32 u4VDecID, BOOL fgCRCType)
{
    if (fgCRCType)
    {
        vVDecWriteCRC(u4VDecID, VDEC_CRC_REG_EN, 0x01);
    }
    else
    {
        vVDecWriteCRC(u4VDecID, VDEC_CRC_REG_EN, 0x03);
    }
}


void vVDecWriteMISC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if(u4VDecID == 0)
    {
        vWriteReg(VDEC_MISC_BASE0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_MISC_BASE0, u4Addr, u4Val);
    }
    else if(u4VDecID == 1)
    {
        vWriteReg(VDEC_MISC_BASE1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_MISC_BASE1, u4Addr, u4Val);
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


UINT32 u4VDecReadMISC(UINT32 u4VDecID,UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(VDEC_MISC_BASE0 + u4Addr);
        vVDecSimDumpR(u4VDecID, VDEC_MISC_BASE0, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 1)
    {
        u4Val = u4ReadReg(VDEC_MISC_BASE1 + u4Addr);
        vVDecSimDumpR(u4VDecID, VDEC_MISC_BASE1, u4Addr, u4Val);
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
    //return 0;
}

#if VDEC_SUPPORT_IOMMU 

void vVDecWriteIOMMU(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    if (u4VDecID == 0)
    {
        vWriteReg(VDEC_IOMMU_BASE + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_IOMMU_BASE, u4Addr, u4Val);
    }
    else
    {
        
    }
}

UINT32 u4VDecReadIOMMU(UINT32 u4VDecID,UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(VDEC_IOMMU_BASE + u4Addr);
        vVDecSimDumpR(u4VDecID, VDEC_IOMMU_BASE, u4Addr, u4Val);
    }
    else
    {
        
    }
    return u4Val;
}
#endif


// *********************************************************************
// Function : void vVDecResetVLDHW(UINT32 u4VDecID)
// Description : Reset Video decode HW
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecResetHW(UINT32 u4VDecID, UINT32 u4VDecType)
{
    UINT32 u4Cnt = 0;

    // HW issue, wait for read pointer stable
    u4Cnt = 50000;
#ifdef VDEC_SIM_DUMP
    LOG(0, "if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n");
#endif
    if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK)) && (u4Cnt--));
    }

    if (u4VDecType==VDEC_FMT_H265)
    {
        //vVDecWriteHEVCMISC(u4VDecID, 0xF4, 0x0);
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0x101);
        //vVDecWriteHEVCMISC( u4VDecID, RW_VLD_PIC_W_MB,  0x0);
        //vVDecWriteHEVCMISC( u4VDecID, RW_VLD_BS_SPEEDUP,  0x0);
    } 
    else 
    {
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, (0x1 |(0x1<<8) |(0x1<<16)));
    }
    
    #if (CONFIG_DRV_FPGA_BOARD != 1)
    vVDEC_HAL_Ckgen(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
    vVDEC_HAL_PowerSave(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
    vVDEC_HAL_SysClkSel(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
    #endif
    
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
}


// *********************************************************************
// Function : void vVDecResetMultiCore(UINT32 u4VDecID)
// Description : Reset Dual Core
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecResetMultiCore(UINT32 u4VDecID)
{
    #ifdef VDEC_SIM_DUMP
    LOG(0, "vVDecResetMultiCore Start\n");
    #endif
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) | 0x01);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) & (0xFFFFFFFE));
    #ifdef VDEC_SIM_DUMP
    LOG(0, "vVDecResetMultiCore End\n");
    #endif
}

// *********************************************************************
// Function : void vVDecResetLAEMultiCore(UINT32 u4VDecID)
// Description : Reset Dual Core
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecResetLAEMultiCore(UINT32 u4VDecID)
{
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) | 0x10);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) & (0xFFFFFFEF));
}

// *********************************************************************
// Function : void vVDecTrigMultiCore(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr)
// Description : Trigger dual core decode
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecTrigMultiCore(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr, UINT32 u4McoreMode)
{
    #if 0
    
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_CABAC, u4CabacAddr);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_EC,u4ECAddr);
    
    // lae + single core mode, for tile (for oryx)
    if(u4McoreMode)
    {        
        //vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ActiveCore, 0x0); // default
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ActiveCore, (u4VDecID << 4)); // default
    }
    
    //disable cpu clear interrupt
    //vVDecWriteMISC(u4VDecID,(41 << 2),u4VDecReadMISC(u4VDecID,(41 << 2)) | (1 << 28));
    
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ErrTableSize, 0x120);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_EnableDecode,u4VDecReadMCORETOP(u4VDecID,(RW_MCORE_EnableDecode)) | 0x01);
    #endif
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_TriggerDecode,u4VDecReadMCORETOP(u4VDecID,(RW_MCORE_TriggerDecode)) | 0x01);
    //printk("wait(pic_finish == 1);\n");
}


// *********************************************************************
// Function : void vVDecConfigMultiCore(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr)
// Description : Config Multi Core register
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecConfigMultiCore(UINT32 u4VDecID,UINT32 u4MCoreMode)
{
//    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_CABAC, u4CabacAddr);
//    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_EC,u4ECAddr);
    
    // lae + single core mode, for tile (for oryx)
    if(u4MCoreMode)
    {        
        //vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ActiveCore, 0x0); // default
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ActiveCore, (u4VDecID << 4)); // default
    }
    
    //disable cpu clear interrupt
    //vVDecWriteMISC(u4VDecID,(41 << 2),u4VDecReadMISC(u4VDecID,(41 << 2)) | (1 << 28));
    
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_ErrTableSize, 0x120);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_EnableDecode,u4VDecReadMCORETOP(u4VDecID,(RW_MCORE_EnableDecode)) | 0x01);
}

// *********************************************************************
// Function : void vVDecSetMCoreLaeBuf(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr)
// Description : Set Lae decode buffer
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
extern void vVDecSetMCoreLaeBuf(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr)
{
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_CABAC, u4CabacAddr);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_EC,u4ECAddr);
}

// *********************************************************************
// Function : void vVDecSetVLDVFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
// Description : Set VFIFO start address and end address
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecSetVLDVFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_VSTART + (u4BSID << 10), u4VFifoSa >> 6);
    vVDecWriteVLD(u4VDecID, RW_VLD_VEND + (u4BSID << 10), u4VFifoEa >> 6);
}

void vVDecSetBS2VFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
{
    vVDecWriteBS2(u4VDecID, RW_VLD_VSTART + (u4BSID << 10), u4VFifoSa >> 6);
    vVDecWriteBS2(u4VDecID, RW_VLD_VEND + (u4BSID << 10), u4VFifoEa >> 6);
}

// *********************************************************************
// Function : BOOL fgVDecIsVLDFetchOk(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Check if VLD fetch is done
// Parameter : None
// Return    : TRUE: VLD fetch OK, FALSE: not OK
// *********************************************************************
BOOL fgVDecIsVLDFetchOk_EX(UINT32 u4BSID, UINT32 u4VDecID)
{
    if ((u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK + (u4BSID << 10)) & VLD_FETCH_OK) == 0)
    {
        return (FALSE);
    }
    return (TRUE);
}


BOOL fgVDecWaitVldFetchOk_EX(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Cnt;

#ifdef VDEC_SIM_DUMP
    LOG(0, "wait(`VDEC_INI_FETCH_RDY == 1);\n");
#endif
    if (!fgVDecIsVLDFetchOk(u4BSID, u4VDecID))
    {
        u4Cnt = 0;
        while (!fgVDecIsVLDFetchOk(u4BSID, u4VDecID))
        {
            u4Cnt++;
            if (u4Cnt >= 0x1000)
            {
                return (FALSE);
            }
        }
    }
    return (TRUE);
}


// *********************************************************************
// Function : void vMCSetOutputBuf(UINT32 u4VDecID, UINT32 u4OutBufIdx, UINT32 u4FRefBufIdx)
// Description : MC Set Output buffer index
// Parameter : dOutBufIdx: Output Buffer index, 0, 1, MC_DIG_BUF, MC_B_BUF
//             dFRefBufIdx: Forward reference buffer index
// Return    : None
// *********************************************************************
void vMCSetOutputBuf(UINT32 u4VDecID, UINT32 u4OutBufIdx, UINT32 u4FRefBufIdx)
{
    vVDecWriteMC(u4VDecID, RW_MC_OPBUF, u4OutBufIdx);
    vVDecWriteMC(u4VDecID, RW_MC_FWDP, u4FRefBufIdx);
}


// *********************************************************************
// Function : UINT32 dVLDGetBitS(UINT32 u4BSID, UINT32 u4VDecID, UINT32 dShiftBit)
// Description : Get Bitstream from VLD barrel shifter
// Parameter : dShiftBit: Bits to shift (0-32)
// Return    : barrel shifter
// *********************************************************************
UINT32 u4VDecVLDGetBitS(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBit)
{
    UINT32 u4RegVal;
    u4RegVal = u4VDecReadVLD(u4VDecID, RO_VLD_BARL + (u4BSID << 10) + (u4ShiftBit << 2));
    return (u4RegVal);
}

#if (!CONFIG_DRV_FPGA_BOARD)
void vVDecPowerDownHW(UINT32 u4VDecID)
{
    ///////////////////////////////////
    //add code for realchip
    ///////////////////////////////////
}

void vVDecSetVldMcClk(UINT32 u4VDecID,UINT32 u4CodecType)
{
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, (0x1 |(0x1<<8)));
    switch(u4CodecType)
    {
    ///////////////////////////////////
    //add code for realchip
    ///////////////////////////////////
    
    }
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
}

void vVDEC_HAL_Ckgen(UINT32 u4VDecID, ENUM_VDEC_FMT_T eCurFMT)
{
    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
    case VDEC_FMT_H265:
        #if defined(CC_MT5890)
        if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
        {
            IO_WRITE32(CKGEN_REG_LAE_SYS_CKCFG, 0, LAE_SYS_CK_SEL_FAST);
        }
        else
        #endif
        {
            #if (defined(CC_MT5882))
            vWriteReg(CKGEN_VDEC_SYS_CLK,((u4ReadReg(CKGEN_VDEC_SYS_CLK) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_FAST));
            #elif (defined(CC_MT5890))
            if(IS_IC_5861())
            {
                //492MHz
                vWriteReg(CKGEN_VDEC_SYS_CLK,((u4ReadReg(CKGEN_VDEC_SYS_CLK) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_FAST));    
            }
            else
            {   
                //0x0C ==>432MHz; 0x0E ==>504MHz
                vWriteReg(CKGEN_VDEC_SYS_CLK,((u4ReadReg(CKGEN_VDEC_SYS_CLK) & 0xFFFFFFF0) | 0xC/*VDEC_SYS_CK_SEL_FAST*/));
            }
            #endif
        }
        break;
    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        vWriteReg(CKGEN_VDEC_SYS_CLK,VDEC_SYS_CK_SEL_MID);
        vWriteReg(CKGEN_VDEC_SYS_CLK,14); //336M
        break;

    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        vWriteReg(CKGEN_VDEC_SYS_CLK,((u4ReadReg(CKGEN_VDEC_SYS_CLK) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_SLOW));
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        LOG(2, "%s: disabled format %d\n", __FUNCTION__, eCurFMT);
        break;

    default:
        LOG(0, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break;
    }
}

void vVDEC_HAL_SysClkSel(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT)
{

    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
    case VDEC_FMT_H265:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
        break;
    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_MID_CLK);
        break;
    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_SLOW_CLK);
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;

    default:
        LOG(0, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break;
    }
}

void vVDEC_HAL_PowerSave(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT)
{

    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
        //vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_H264);
        //vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_H264);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,0x00);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0x00);
        break;
    case VDEC_FMT_H265:
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_H265);
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_H265);
//        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_H265);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,0x00);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0x00);
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, 0x00);
    #ifdef CC_DUAL_CORE_DEC
        vVDecWriteMCORETOP(ucEsId, RW_VDEC_MCORE_PDN_CON_MODULE, 0x00);
    #endif
        break;
    case VDEC_FMT_MPV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MPV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MPV);
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_MPV);
        break;
        
    case VDEC_FMT_MP4:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MP4);
        x_thread_delay(100);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MP4);
        x_thread_delay(100);
		break;
    case VDEC_FMT_WMV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WMV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_WMV);
		break;
    case VDEC_FMT_VP8:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP8);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_VP8);
        break;
    case VDEC_FMT_RV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_RM);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_RM);
		break;
    case VDEC_FMT_AVS:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_AVS);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_AVS);
		break;
    case VDEC_FMT_VP6:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP6);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_VP6);
        break;
//    case VDEC_FMT_WEBP:
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WEBP);
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_WEBP);
//		break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;

    default:
        LOG(0, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break; 
    }
    
}

void vVDEC_HAL_PowerSaveClear(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT)
{

    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_H264);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_H264);
        break;

    case VDEC_FMT_MPV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MPV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MPV);
        break;
    case VDEC_FMT_MP4:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MP4);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MP4);
        break;
    case VDEC_FMT_WMV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WMV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_WMV);
        break;
    case VDEC_FMT_VP8:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP8);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0xFFFFFFFF);
        break;

    case VDEC_FMT_RV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_RM);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_RM);
        break;
    case VDEC_FMT_AVS:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_AVS);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_AVS);
        break;
    case VDEC_FMT_VP6:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP6);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0xFFFFFFFF);
        break;
//    case VDEC_FMT_WEBP:
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WEBP);
//        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0xFFFFFFFF);
//		break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;

    default:
        LOG(1, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break; 
    }
    
}
#endif

void vVDEC_HAL_SetLAEDecAddr(UINT32 u4VDecID,UINT32 u4CABACAddr,UINT32 u4ECAddr)
{
    if(u4VDecID == VDEC_LAE0)
    {
        vVDecWriteMCORETOP(u4VDecID, RW_MCORE_CABAC0, PHYSICAL(u4CABACAddr));
        vVDecWriteMCORETOP(u4VDecID, RW_MCORE_EC0, PHYSICAL(u4ECAddr));
    }
    else if(u4VDecID == VDEC_LAE1)
    {
        vVDecWriteMCORETOP(u4VDecID, RW_MCORE_CABAC1, u4CABACAddr);
        vVDecWriteMCORETOP(u4VDecID, RW_MCORE_EC1, u4ECAddr);
    }
    else
    {
        ASSERT(0);
    }
}


void vVDec_HAL_Enable_CalcCLK(UINT32 u4VDecID)
{
    vVDecWriteVLDTOP(u4VDecID,(21 << 2),1);
}

#ifdef VDEC_SIM_DUMP

#define DUMP_FOR_SIM 1

void vVDecSimWDump(const char * szFunction,UINT32 u4VDecID,const char * szOffset,UINT32 u4Addr,UINT32 u4Val, UINT32 u4BaseReg)
{
    if (szFunction)
    {
        if (VDEC_LAE0 == u4VDecID)//(DUMP_FOR_SIM)
        {
            //printk("RISCWrite(`%s + 4*%d, %d );  //u4val = 0x%08X /* 0x%08X */\n", szOffset, u4Addr>>2, u4Val, u4BaseReg);
            //printk("RISCWrite(`%s + 4*%d, %d );  //u4val = 0x%08X\n", szOffset, u4Addr>>2, u4Val, u4Val);
            #ifdef REG_LOG_NEW
            if (_fgRegLogConsole[u4VDecID])
            #endif
            {
                switch (u4BaseReg)
                {
                    case DV_REG_OFFSET0:
                    case DV_REG_OFFSET1:
                        printf("        RISCWrite_MISC_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case LAE_MISC_OFFSET0:
                    case LAE_MISC_OFFSET1:
                        printf("        RISCWrite_LAE_MISC_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case VLD_REG_OFFSET0:
                    case VLD_REG_OFFSET1:
                        printf("        RISCWrite_VLD_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case VLD_TOP_REG_OFFSET0:
                    case VLD_TOP_REG_OFFSET1:
                        printf("        RISCWrite_VLD_TOP_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case MC_REG_OFFSET0:
                    case MC_REG_OFFSET1:
                        printf("        RISCWrite_MC_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case AVC_MV_BASE0:
                    case AVC_MV_BASE1:
                         LOG(0,"        RISCWrite_MV_%d( %d, %d );  //u4val = 0x%08X\n", u4VDecID, u4Addr >> 2, u4Val, u4Val);
                        break;
                    case HEVC_VLD_REG_OFFSET0:
                    case HEVC_VLD_REG_OFFSET1:
                        printf("        RISCWrite_HEVC_VLD_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case LAE_HEVCVLD_OFFSET0:
                    case LAE_HEVCVLD_OFFSET1:
                        LOG(0,"        RISCWrite_LEHEVC_VLD_%d( %d, %d); /* u4val 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val);
                        break;
                    case LAE_VLD_OFFSET0:
                    case LAE_VLD_OFFSET1:
                        printf("        RISCWrite_LE_VLD_%d( %d, %d); //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val, u4BaseReg);
                        break;
                    case LAE_VLDTOP_OFFSET0:
                    case LAE_VLDTOP_OFFSET1:
                        LOG(0,"        RISCWrite_LE_VLDTOP_%d( %d, %d); /* u4val 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val);
                        break;
                    case HEVC_PP_REG_OFFSET0:
                    case HEVC_PP_REG_OFFSET1:
                        LOG(0,"        RISCWrite_PP_%d( %d, %d); /* u4val 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val);
                        break;
                    case MVDEC_TOP_OFFSET0:
                        printf("        RISCWrite_MCore%d( %d, %d); //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val, u4BaseReg);
                        break;
                    case AVC_VLD_BASE0:
                    case AVC_VLD_BASE1:
                        printf("        RISCWrite_AVC_VLD_%d( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                    case VDEC_BS2_OFFSET0:
                    case VDEC_BS2_OFFSET1:
                        printf("        RISCWrite_BS2_%d( %d, %d); //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val, u4BaseReg);
                        break;
                    case LAE_AVCVLD_BASE0:
                    case LAE_AVCVLD_BASE1:
                        printf("        RISCWrite_LAE_AVCVLD_%d( %d, %d); //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val,u4Val, u4BaseReg);
                        break;                    
                    default:
                        printf("        RISCWrite_Others( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                        break;
                }
            }            
            #ifdef REG_LOG_NEW
            else
            {
                switch (u4BaseReg)
                {
                case DV_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_MISC( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case VLD_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_VLD( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case VLD_TOP_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_VLD_TOP( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case MC_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_MC( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case AVC_MV_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_MV( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case RM_VDEC_PP_BASE:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_PP( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                    break;
                case HEVC_VLD_REG_OFFSET0:
                     _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_HEVC_VLD( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
                     break;
                default:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCWrite_Others( %d, %d );  //u4val = 0x%08X /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4Val, u4BaseReg);
                    break;
                }
            }
            #endif
        }
        else
        {
            //UTIL_Printf("%s(u4VDecID, 0x%x, 0x%x); /* 0x%08X */\n", szFunction, u4Addr, u4Val, u4BaseReg);
            //if (VDEC_LAE0 == u4VDecID)
                //LOG(0, "RISCWrite %x, %d, %x\n", u4BaseReg, u4Addr >> 2, u4Val);
        }
    }
    else
    {
    }
}

void vVDecSimRDump(const char * szFunction,UINT32 u4VDecID,const char * szOffset,UINT32 u4Addr,UINT32 u4Val, UINT32 u4BaseReg)
{
    if (szFunction)
    {
        if (VDEC_LAE0 == u4VDecID)//(DUMP_FOR_SIM)
        {
            //printk("RISCRead(`%s + 4*%d, %d );  //u4val = 0x%08X /* 0x%08X */\n", szOffset, u4Addr>>2, u4Val, u4Val, u4BaseReg);
            //printk("RISCRead(`%s + 4*%d, %d );  //u4val = 0x%08X\n", szOffset, u4Addr>>2, u4Val, u4Val);
            #ifdef REG_LOG_NEW
            if (_fgRegLogConsole[u4VDecID])
            #endif
            {
                switch (u4BaseReg)
                {
                    case DV_REG_OFFSET0:
                    case DV_REG_OFFSET1:
                        printf("        RISCRead_MISC_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case LAE_MISC_OFFSET0:
                    case LAE_MISC_OFFSET1:
                        printf("        RISCRead_LAE_MISC_%d( %d);  /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case VLD_REG_OFFSET0:
                    case VLD_REG_OFFSET1:
                        printf("        RISCRead_VLD_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case VLD_TOP_REG_OFFSET0:
                    case VLD_TOP_REG_OFFSET1:
                        printf("        RISCRead_VLD_TOP_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case MC_REG_OFFSET0:
                    case MC_REG_OFFSET1:
                        printf("        RISCRead_MC_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case AVC_MV_BASE0:
                    case AVC_MV_BASE1:
                        LOG(0,"        RISCRead_MV_%d( %d); /* return 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val);
                        break;
                    case HEVC_VLD_REG_OFFSET0:
                    case HEVC_VLD_REG_OFFSET1:
                        printf("        RISCRead_HEVC_VLD_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case LAE_HEVCVLD_OFFSET0:
                    case LAE_HEVCVLD_OFFSET1:
                        printf("        RISCRead_LEHEVC_VLD_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case LAE_VLD_OFFSET0:
                    case LAE_VLD_OFFSET1:
                        printf("        RISCRead_LE_VLD_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case LAE_VLDTOP_OFFSET0:
                    case LAE_VLDTOP_OFFSET1:
                        printf("        RISCRead_LE_VLDTOP_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case HEVC_PP_REG_OFFSET0:
                    case HEVC_PP_REG_OFFSET1:
                        printf("        RISCRead_PP_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case MVDEC_TOP_OFFSET0:
                        printf("        RISCRead_MCore%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case VDEC_BS2_OFFSET0:
                    case VDEC_BS2_OFFSET1:
                        printf("        RISCRead_BS2_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case AVC_VLD_BASE0:
                    case AVC_VLD_BASE1:
                        printf("        RISCRead_AVC_VLD_%d( %d); /* return 0x%08X */ /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    case LAE_AVCVLD_BASE0:
                    case LAE_AVCVLD_BASE1:
                        printf("        RISCRead_LAE_AVCVLD_%d( %d); //u4val = 0x%08X /* base 0x%08X */\n", u4VDecID, u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                    default: 
                        printf("        RISCRead_Others( %d);  /* return 0x%08X */ /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4BaseReg);
                        break;
                }
            }
            #ifdef REG_LOG_NEW
            else
            {
                switch (u4BaseReg)
                {
                case DV_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_MISC( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case VLD_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_VLD( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case VLD_TOP_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_VLD_TOP( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case MC_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_MC( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case AVC_MV_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_MV( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case RM_VDEC_PP_BASE:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_PP( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                case HEVC_VLD_REG_OFFSET0:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_HEVC_VLD( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
                    break;
                default:
                    _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "        RISCRead_Others( %d);  /* return 0x%08X */ /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4BaseReg);
                    break;
                }
            }
            
            #endif
        }
        else
        {
            //UTIL_Printf("%s(u4VDecID, 0x%x); /* return 0x%x */ /* 0x%08X */\n", szFunction, u4Addr, u4Val, u4BaseReg);
            //if (VDEC_LAE0 == u4VDecID)
                //LOG(0, "RISCRead %x, %d, %x\n", u4BaseReg, u4Addr >> 2, u4Val);
        }
    }
    else
    {
    }
}
#endif

