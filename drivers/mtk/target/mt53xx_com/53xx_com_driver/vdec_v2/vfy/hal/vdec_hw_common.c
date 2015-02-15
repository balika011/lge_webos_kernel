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

#ifdef CONFIG_TV_DRV_VFY
#include "x_hal_5381.h"
#else
#include "x_hal_ic.h"
#endif //CONFIG_TV_DRV_VFY

#include "x_debug.h"
#include "x_ckgen.h"

#ifndef CONFIG_TV_DRV_VFY
#include <mach/cache_operation.h>
#endif //CONFIG_TV_DRV_VFY

#if CONFIG_DRV_VERIFY_SUPPORT
#include "vdec_verify_general.h"
#include "vdec_info_verify.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include "vdec_verify_mpv_prov.h"
extern int rand(void);

extern BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO);
extern void vVDecOutputDebugString(const CHAR * format, ...);
extern UINT32 u4BitstreamLoaded[2];
extern UINT32 u4OpenSimDump[2];
#endif


// *********************************************************************
// Function : void vVDecWriteVLD(UINT32 u4VDecID)
// Description : Write VLD register
// Parameter : u4VDecID : VLD ID [0:  VLD0; 1:  VLD1; 2: LAE VLD0; 3:LAE VLD1]
// Return    : None
// *********************************************************************
void vVDecWriteVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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


UINT32 u4VDecReadVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
void vVDecWriteVLDTOP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}
#endif

void vVDecWriteMC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
    u4VDecID = 0;
#endif

    if (0 == u4VDecID)
    {
        vWriteReg(MC_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, MC_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
#ifdef VDEC_PIP_WITH_ONE_HW
        UTIL_Printf("PIP_ONE_HW: Wrong HW ID!!!\n");
        VDEC_ASSERT(0);
#endif
        vVDecSimDumpW(u4VDecID, MC_REG_OFFSET1, u4Addr, u4Val);
        vWriteReg(MC_REG_OFFSET1 + u4Addr, u4Val);
    }
	else
	{
		//lae do noting
	}
}


UINT32 u4VDecReadMC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
		//lae do nothing
	}
}

void vVDecWritePP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
	#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
	UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
		//lae do nothing
	}
}


void vVDecWriteBS2(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    UINT32 u4Val;
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8560))
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
		//lae do nothing
	}
}


void vVDecWriteCRC(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
    u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        vWriteReg(VDEC_CRC_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDEC_CRC_REG_OFFSET0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
#ifdef VDEC_PIP_WITH_ONE_HW
        UTIL_Printf("PIP_ONE_HW: Wrong HW ID!!!\n");
        VDEC_ASSERT(0);
#endif
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
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))    
    u4VDecID = 0;
#endif
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
    UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
		//lae do noting
	}
}


UINT32 u4VDecReadMCoreCRC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    u4VDecID = 0;

    u4Val = (u4ReadReg(MVDEC_TOP_OFFSET0 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_OFFSET0, u4Addr, u4Val);
    return u4Val;  
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
void vVDecWriteDV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
    u4VDecID = 0;
#endif
    if (u4VDecID == 0)
    {
        vWriteReg(DV_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, DV_REG_OFFSET0, u4Addr, u4Val);
    }
    else
    {
#ifdef VDEC_PIP_WITH_ONE_HW
        UTIL_Printf("PIP_ONE_HW: Wrong HW ID!!!\n");
        VDEC_ASSERT(0);
#endif
        vWriteReg(DV_REG_OFFSET1 + u4Addr, u4Val);
    }
}

UINT32 u4VDecReadDV(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}
#endif

void vVDecWriteLAEVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}

void vVDecWriteMCORETOPLARB(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    u4VDecID = 0;
    vWriteReg(MVDEC_TOP_LARB_OFFSET1 + u4Addr, u4Val);
    vVDecSimDumpW(u4VDecID, MVDEC_TOP_LARB_OFFSET1, u4Addr, u4Val);
}

UINT32 u4VDecReadLAEVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}

UINT32 u4VDecReadLAEVLDTOP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}


UINT32 u4VDecReadLAELARB(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}


UINT32 u4VDecReadLAEMISC(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
}


UINT32 u4VDecReadMCORETOP(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    u4VDecID = 0;
    u4Val = (u4ReadReg(MVDEC_TOP_OFFSET0 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_OFFSET0, u4Addr, u4Val);
    return u4Val;
}

UINT32 u4VDecReadMCORETOPLARB(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
    u4VDecID = 0;
    u4Val = (u4ReadReg(MVDEC_TOP_LARB_OFFSET1 + u4Addr)); 
    vVDecSimDumpR(u4VDecID, MVDEC_TOP_LARB_OFFSET1, u4Addr, u4Val);
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
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
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
    #if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
     u4VDecID = 0;
    #endif
    UINT32 u4Val;

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
}

void vVDec_HAL_Enable_CalcCLK(UINT32 u4VDecID)
{
    vVDecWriteVLDTOP(u4VDecID,(21 << 2),1);
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
    UINT32 u4Val;
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
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8555)
void vVDecResetHW(UINT32 u4VDecID)
#else
void vVDecResetHW(UINT32 u4VDecID, UINT32 u4VDecType)
#endif
{
    UINT32 u4Cnt = 0;
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580))
    UINT32 u4VDecSysClk;
    UINT32 u4VDecPDNCtrl = 0;
#endif
    // HW issue, wait for read pointer stable
    u4Cnt = 50000;
#ifdef VDEC_SIM_DUMP
    UTIL_Printf("if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n");
#endif
    if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&AA_FIT_TARGET_SCLK)) && (u4Cnt--));
    }

    if (u4VDecType==VDEC_FMT_H265)
    {
    #if 1 //mt8135 flow
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0x101);
        vVDecWriteHEVCMISC(u4VDecID, 0xF4, 0x0);
        //vVDecWriteHEVCMISC( u4VDecID, RW_VLD_PIC_W_MB,  0x0);
        //vVDecWriteHEVCMISC( u4VDecID, RW_VLD_BS_SPEEDUP,  0x0);
    #else
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0x1);
    #endif
    } 
    else 
    {
    #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) || (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, (0x1 |(0x1<<8) |(0x1<<16)));
        mb();
    #else
        vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 1);
    #endif
        //UTIL_Printf("[RMVB] after set. \n");
    }
    #if (DTV_COMMON_CONFIG && (CONFIG_DRV_FPGA_BOARD != 1))
    vVDEC_HAL_Ckgen(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
    vVDEC_HAL_PowerSave(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
	if (_tVerMpvDecPrm[u4VDecID].fgIsUFOMode)
	{
		vVDEC_HAL_UFO_PowerSave(u4VDecID);
	}
    vVDEC_HAL_SysClkSel(u4VDecID,(ENUM_VDEC_FMT_T)u4VDecType);
	#endif
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
    if (u4VDecType==VDEC_FMT_AVS)
    {
        vVDecWriteDV(u4VDecID, 0xF0, 1); //Bypass latency buffer for Oryx project
    }
#endif
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
	UTIL_Printf("vVDecResetMultiCore Start\n");
	#endif
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) | 0x01);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_RESET,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_RESET) & (0xFFFFFFFE));	
	#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDecResetMultiCore End\n");
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
void vVDecConfigMultiCore(UINT32 u4VDecID, UINT32 u4CabacAddr,UINT32 u4ECAddr, UINT32 u4MCoreMode)
{
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_CABAC, u4CabacAddr);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_DEC_EC,u4ECAddr);
    
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
// Function : void vVDecSetVLDVFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
// Description : Set VFIFO start address and end address
// Parameter : u4VDecID : VLD ID
// Return    : None
// *********************************************************************
void vVDecSetVLDVFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_VSTART + (u4BSID << 10), u4VFifoSa >> 6);
    mb();
    vVDecWriteVLD(u4VDecID, RW_VLD_VEND + (u4BSID << 10), u4VFifoEa >> 6);
}

void vVDecSetBS2VFIFO(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
{
    vVDecWriteBS2(u4VDecID, RW_VLD_VSTART + (u4BSID << 10), u4VFifoSa >> 6);
    mb();
    vVDecWriteBS2(u4VDecID, RW_VLD_VEND + (u4BSID << 10), u4VFifoEa >> 6);
}
#if 0
UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa)
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
    printk("if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n");
#endif
    if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (PROCESS_FLAG))
    {
        while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1)) && (u4Cnt--));
    }

    u4RegVal = u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10));
    vb_sram_ra = u4RegVal & 0x1F;
    vb_sram_wa = (u4RegVal >> 8) & 0x1F;
    seg_rcnt = (u4RegVal >> 24) & 0x3;

    u4SramRptr = vb_sram_ra;
    u4SramWptr = vb_sram_wa;
    u4SramCtr = seg_rcnt;
    u4DramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR + (u4BSID << 10));

    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 32 - (u4SramRptr - u4SramWptr);
    }
  
    *pu4Bits = u4VDecReadVLD(u4VDecID, RW_VLD_BITCOUNT + (u4BSID << 10)) & 0x3f;

    if (u4VDecReadVLD(u4VDecID, 4*59) & (0x1 << 28))
    {              
        if (u4VDecReadVLD(u4VDecID, RW_VLD_WMV_ABS + (u4BSID << 10)) & 0x1) // vc1 not ready @2011/08/11
        {
            u4BsBufLen = 6 * 4;
        }
        else
        {
            u4BsBufLen = 5 * 4;
        }
    }
    else // old case
    {
        if (u4VDecReadVLD(u4VDecID, RW_VLD_WMV_ABS + (u4BSID << 10)) & 0x1) // vc1 case
        {
            u4BsBufLen = 5 * 4;
        }
        else
        {
            u4BsBufLen = 4 * 4;
        }
    }    


    #ifdef VDEC_SIM_DUMP
      printk("<vdec> ReadVldRPtr, dRptr:0x%08X, sra:0x%08X, swa:0x%08X, scnt:0x%08X, sum:0x%08X, u4BsBufLen=%u\n", 
        u4DramRptr, vb_sram_ra, vb_sram_wa, seg_rcnt, *pu4Bits, u4BsBufLen);
    #endif

    u4ByteAddr = u4DramRptr - u4SramDataSz * 16 + u4SramCtr * 4 - u4BsBufLen + *pu4Bits/8;  
    
    *pu4Bits &= 0x7;

    if (u4ByteAddr < u4VFIFOSa)
    {
        u4ByteAddr = u4ByteAddr  
                     + ((u4VDecReadVLD(u4VDecID, RW_VLD_VEND + (u4BSID << 10)) << 6) - ((UINT32)u4VFIFOSa))
                     - u4VFIFOSa;
    }
    else
    {
        u4ByteAddr -= ((UINT32)u4VFIFOSa);
    }
    
    #ifdef VDEC_SIM_DUMP
      printk("<vdec> RdPtr=0x%08X (%u) @(%s, %d)\n", u4ByteAddr, u4ByteAddr, szFunction, i4Line);
    #endif

    return (u4ByteAddr);
}
#else
UINT32 u4VDecReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa)
{
		UINT32 u4DramRptr;
		UINT32 u4SramRptr, u4SramWptr;
		UINT32 u4SramDataSz;
		UINT32 u4ByteAddr;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
		UINT32 u4Cnt = 0;
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
		u4Cnt = 50000;
#ifdef VDEC_SIM_DUMP
		UTIL_Printf("if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n");
#endif
		if (u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & (PROCESS_FLAG))
		{
			while((!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1)) && (u4Cnt--));
		}
#else
		while (!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&1));
#endif
		u4DramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR + (u4BSID << 10));
		u4SramRptr = ((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))) & 0xf) * 4 +
			(((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))>>24)) & 0x3); //count in 128bits
		u4SramWptr = (((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10))>>8)) & 0xf) *4;
#else    
		while (!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL + (u4BSID << 10))&0x10000));
		u4DramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR + (u4BSID << 10));
		u4SramRptr = u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10)) & 0xff;
		u4SramWptr = (u4VDecReadVLD(u4VDecID, RO_VLD_VBAR + (u4BSID << 10)) >> 16) & 0xff;
#endif
		if (u4SramWptr > u4SramRptr)
		{
			u4SramDataSz = u4SramWptr - u4SramRptr;
		}
		else
		{
			u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
		}
		*pu4Bits = u4VDecReadVLD(u4VDecID, RW_VLD_BITCOUNT + (u4BSID << 10)) & 0x3f;
		if (u4VDecReadVLD(u4VDecID, RW_VLD_WMV_ABS + (u4BSID << 10)) & 0x1) // 03 replacement enable!
		{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
			u4ByteAddr = u4DramRptr - (u4SramDataSz + 5)* 4 + (*pu4Bits / 8) ; 
#else   
			u4ByteAddr = u4DramRptr - (u4SramDataSz + 4) * 4 + (*pu4Bits / 8);
#endif
		}
		else
		{
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
			u4ByteAddr = u4DramRptr - (u4SramDataSz + 4)* 4 + (*pu4Bits / 8); 
#else
			u4ByteAddr = u4DramRptr - (u4SramDataSz + 3)* 4 + (*pu4Bits / 8);
#endif
		}
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
#endif


// *********************************************************************
// Function : BOOL fgVDecIsVLDFetchOk(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Check if VLD fetch is done
// Parameter : None
// Return    : TRUE: VLD fetch OK, FALSE: not OK
// *********************************************************************
BOOL fgVDecIsVLDFetchOk(UINT32 u4BSID, UINT32 u4VDecID)
{
    if ((u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK + (u4BSID << 10)) & VLD_FETCH_OK) == 0)
    {
        return (FALSE);
    }
    return (TRUE);
}


BOOL fgVDecWaitVldFetchOk(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Cnt;

#ifdef VDEC_SIM_DUMP
    UTIL_Printf("wait(`VDEC_INI_FETCH_RDY == 1);\n");
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
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
#else
    u4Cnt = 0;
    // HW modification
    // read point may not stable after read fetck ok flag
    while (!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL + (u4BSID << 10))&0x10000))
    {
        u4Cnt++;
        if (u4Cnt >= 0x1000)
        {
            return (FALSE);
        }
    }
#endif   
    return (TRUE);
}


UINT32 u4VDec03Number(UINT32 u4Addr, UINT32 u4Range)
{
    INT32 i;
    UINT32 u403Cnt = 0;
    UINT32 u4ZeroCnt = 0;
    UCHAR *pucAddr;

    pucAddr = (UCHAR *)u4Addr;

    //BSP_dma_unmap_vaddr(u4Addr,u4Range,FROM_DEVICE);
    for ( i=0; i < u4Range; i++, pucAddr++)
    {
        if ((*pucAddr) == 0)
        {
            u4ZeroCnt++;
        }
        else
        {
            if(((*pucAddr) == 3) && (u4ZeroCnt >= 2))
            {
                u403Cnt++;
            }
            u4ZeroCnt = 0;
        }
    }
    //BSP_dma_map_vaddr(u4Addr,u4Range,FROM_DEVICE);
    return (u403Cnt);
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
    #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8580 && CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8561 && (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
    UCHAR ucTmp=0;
#endif
    //u4RegVal = u4VDecReadVLD(u4VDecID, RO_VLD_BARL + (u4BSID << 10));


    u4RegVal = u4VDecReadVLD(u4VDecID, RO_VLD_BARL + (u4BSID << 10) + (u4ShiftBit << 2));

#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8580 && CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8561 &&(CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))

    /* added by Alan Cheng for JPEG debug error according to GS Lin */

    ucTmp = (u4VDecReadVLD(u4VDecID, WO_VLD_RDY)&0x10000000)>>28;
    while (!ucTmp)
    {
        ucTmp = (u4VDecReadVLD(u4VDecID, WO_VLD_RDY)&0x10000000)>>28;
    }

#endif   

    //u4VDecReadVLD(u4VDecID, RO_VLD_BARL + (u4BSID << 10));
    /* end */

    return (u4RegVal);
}


#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
void vVDecPowerDownHW(UINT32 u4VDecID)
{
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
    //UTIL_Printf("vVDecPowerDownHW, u4VDecID:%d, to set RW_PDN_CRTL2\n", u4VDecID);

    vVDecWriteDV( u4VDecID, RW_PDN_CRTL2, 0xFFFFFFFF ); 

    //UTIL_Printf("vVDecPowerDownHW, u4VDecID:%d, RW_PDN_CRTL2 value:0x%x\n", u4VDecID, u4VDecReadDV(u4VDecID, RW_PDN_CRTL2));
#else
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) 
    if(fgRunBelowSegmentAfter(IC_8580_VER_B))
    {
        UTIL_Printf("vVDecPowerDownHW, u4VDecID:%d, to set RW_PDN_CRTL2\n", u4VDecID);
        
        #if 1//VDEC_VFY_TEST_POWER_DOWN_CTRL
        vVDecWriteDV( u4VDecID, RW_PDN_CRTL2, 0xFFFFFFFF ); 
        #endif
        
//        vVDecWriteDV( u4VDecID, RW_PDN_CRTL1, 0xFFFFFFFF ); 
    }
    else
    #endif
    {
        UTIL_Printf("vVDecPowerDownHW, u4VDecID:%d, to set RW_PDN_CTRL\n", u4VDecID);
        
        vVDecWriteDV( u4VDecID, RW_PDN_CTRL, (u4VDecReadDV(u4VDecID, RW_PDN_CTRL) | 0x003FFFFF) );
    }
#endif
}
#endif

// *********************************************************************
// Function : void vVLDLoadQuanMat(BOOL fgIntra)
// Description : load Q matrix
// Parameter : fgIntra: TRUE: Load Intra Matrix, FALSE: Load Non-Intra
// Return    : None
// *********************************************************************
UINT32 _u4VldDummy=0;
#if (CONFIG_DRV_ONLY || CONFIG_DRV_VERIFY_SUPPORT)
void vVLDLoadQuanMat(UINT32 u4VDecID, BOOL fgIntra)
{
    // hardware will get 64 byte auto
    // use _dwVldDummy to prevent code being optimize by compiler
    if(fgIntra)
    {
        _u4VldDummy = u4VDecReadVLD(u4VDecID, RW_VLD_TABLIM);
    }
    else
    {
        _u4VldDummy = u4VDecReadVLD(u4VDecID, RW_VLD_TABLNIM);
    }
}
#endif

void vWriteVDSCL(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
    VDEC_ASSERT(0);
#endif

#if (CONFIG_DRV_VERIFY_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560))
    u4VDecID = 0;
#endif


    if (u4VDecID == 0)
    {
        vWriteReg(VDSCL_REG_OFFSET0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, VDSCL_REG_OFFSET0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(VDSCL_REG_OFFSET1 + u4Addr, u4Val);
    }
}


UINT32 u4ReadVDSCL(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val;
#if (CONFIG_DRV_VERIFY_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560))
    u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        u4Val = (u4ReadReg(VDSCL_REG_OFFSET0 + u4Addr));
        vVDecSimDumpR(u4VDecID, VDSCL_REG_OFFSET0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        return (u4ReadReg(VDSCL_REG_OFFSET1 + u4Addr));
    }
}


void vVDSCLReset(UINT32 u4VDecID)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_ACTIVATE, RW_VDSCL_SW_RESET);
}


void vVDSCLSetEnable(UINT32 u4VDecID, BOOL fgEnable)
{
    if (fgEnable)
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_ACTIVATE, u4ReadVDSCL(u4VDecID, RW_VDSCL_ACTIVATE) | RW_VDSCL_EN);
    }
    else
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_ACTIVATE, u4ReadVDSCL(u4VDecID, RW_VDSCL_ACTIVATE) & (~RW_VDSCL_EN));
    }
}


void vVDSCLSetConvert(UINT32 u4VDecID, BOOL fgConvert)
{
    if (fgConvert)
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_ACTIVATE, u4ReadVDSCL(u4VDecID, RW_VDSCL_ACTIVATE) | RW_VDSCL_CONVERT);
    }
    else
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_ACTIVATE, u4ReadVDSCL(u4VDecID, RW_VDSCL_ACTIVATE) & (~RW_VDSCL_CONVERT));
    }
}


void vVDSCLSetPicStruct(UINT32 u4VDecID, EPicStruct ePicStruct)
{
    if (ePicStruct > 3) 
    {
        return;
    }
    vWriteVDSCL(u4VDecID, RW_VDSCL_TYPE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TYPE) & (~RW_VDSCL_PIC_STRU)) | ((UINT32)ePicStruct));
}

#if VDEC_REMOVE_UNUSED_FUNC
void vVDSCLSetRasterScanOut(UINT32 u4VDecID, BOOL fgRasterScan)
{
    if (fgRasterScan)
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_TYPE, u4ReadVDSCL(u4VDecID, RW_VDSCL_TYPE) | RW_VDSCL_RASTER_SCAN);
    }
    else
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_TYPE, u4ReadVDSCL(u4VDecID, RW_VDSCL_TYPE) & (~RW_VDSCL_RASTER_SCAN));
    }
}
#endif

void vVDSCLSetAddrSwap(UINT32 u4VDecID, UINT32 u4Mode)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    if (u4Mode > 7)
#else
    if (u4Mode > 3)
#endif
    {
        return;
    }

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    if(u4Mode)
    {
        if(u4Mode == ASM_1)
        {
            u4Mode = RW_VDSCL_ADDR_SWAP_MODE1;
        }
        else
        {
            u4Mode = RW_VDSCL_ADDR_SWAP_MODE2;
        }
    }
#endif
    vWriteVDSCL(u4VDecID, RW_VDSCL_MEM_IF, (u4ReadVDSCL(u4VDecID, RW_VDSCL_MEM_IF) & (~RW_VDSCL_ADDR_SWAP_MODE2)) | u4Mode);
}


void vVDSCLSetBoundPad(UINT32 u4VDecID, EPadMode ePadMode, UCHAR ucYColor, UCHAR ucCbColor, UCHAR ucCrColor)
{
    switch (ePadMode)
    {
    case PAD_MODE_NO_PAD:
        vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) | RW_VDSCL_BOUND_NO_PAD);
        break;
    case PAD_MODE_EXTEND:
        vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) & (~RW_VDSCL_BOUND_NO_PAD));
        vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) & (~RW_VDSCL_BOUND_PAD_EN));
        break;
    case PAD_MODE_COLOR:
        vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) & (~RW_VDSCL_BOUND_NO_PAD));
        vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) | RW_VDSCL_BOUND_PAD_EN);
        {
            UINT32 u4Y,u4Cb,u4Cr;

            u4Y = ucYColor;
            u4Cb = ucCbColor;
            u4Cr = ucCrColor;

            vWriteVDSCL(u4VDecID, RW_VDSCL_BOUND, (u4ReadVDSCL(u4VDecID, RW_VDSCL_BOUND) & (~RW_VDSCL_BOUND_PAD_Y) & (~RW_VDSCL_BOUND_PAD_CB) & (~RW_VDSCL_BOUND_PAD_CR)) | (u4Y<<16) | (u4Cb<<8) | u4Cr);
        }
        break;
    }
}


void vVDSCLSetSrcPicSize(UINT32 u4VDecID, UINT32 u4SrcWidth, UINT32 u4SrcHeight)
{
    if ((u4SrcWidth>0xFFF) || (u4SrcHeight>0xFFF))
    {
        return;
    }
    vWriteVDSCL(u4VDecID, RW_VDSCL_SRC_SIZE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_SRC_SIZE) & (~RW_VDSCL_SRC_WIDTH) & (~RW_VDSCL_SRC_HEIGHT)) | (u4SrcWidth<<16) | u4SrcHeight);
}


void vVDSCLSetTrgPicSize(UINT32 u4VDecID, UINT32 u4TrgWidth, UINT32 u4TrgHeight)
{
    if ((u4TrgWidth>0xFFF) || (u4TrgHeight>0xFFF)) 
    {
        return;
    }
    vWriteVDSCL(u4VDecID, RW_VDSCL_TG_SIZE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TG_SIZE) & (~RW_VDSCL_TG_WIDTH) & (~RW_VDSCL_TG_HEIGHT)) | (u4TrgWidth<<16) | u4TrgHeight);
}


void vVDSCLSetTrgPicOffset(UINT32 u4VDecID, UINT32 u4TrgX, UINT32 u4TrgY)
{
    if ((u4TrgX>0xFFF) || (u4TrgY>0xFFF))
    {
        return;
    }
    vWriteVDSCL(u4VDecID, RW_VDSCL_TG_OFFSET,(u4TrgX<<16) | u4TrgY);
}


void vVDSCLSetTrgBufWidth(UINT32 u4VDecID, UINT32 u4TrgBufWidth)
{
    if (u4TrgBufWidth>0xFFF) 
    {
        return;
    }
    vWriteVDSCL(u4VDecID, RW_VDSCL_TG_BUF_LEN, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TG_BUF_LEN) & (~RW_VDSCL_TRG_BUF_LEN)) | (u4TrgBufWidth>>4));
}


void vVDSCLSetTrgYBufBaseAddr(UINT32 u4VDecID, UINT32 u4YBufBaseAddr)
{
    //if(u4YBufBaseAddr>0xFFFFFF) return;

    vWriteVDSCL(u4VDecID, RW_VDSCL_TG_Y_OW_BASE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TG_Y_OW_BASE) & (~RW_VDSCL_TG_Y_OW_ADDR_BASE)) | (u4YBufBaseAddr>>4));
}


void vVDSCLSetTrgCBufBaseAddr(UINT32 u4VDecID, UINT32 u4CBufBaseAddr)
{
    //if(u4CBufBaseAddr>0xFFFFFF) return;

    vWriteVDSCL(u4VDecID, RW_VDSCL_TG_C_OW_BASE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TG_C_OW_BASE) & (~RW_VDSCL_TG_C_OW_ADDR_BASE)) | (u4CBufBaseAddr>>4));
}


void vVDSCLSetHScaleFactor(UINT32 u4VDecID, UINT32 u4HScaleFactorY, UINT32 u4HScaleFactorC)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_H_SCL_Y, (u4ReadVDSCL(u4VDecID, RW_VDSCL_H_SCL_Y) & (~RW_VDSCL_H_SCL_Y_FACTOR)) | u4HScaleFactorY);
    vWriteVDSCL(u4VDecID, RW_VDSCL_H_SCL_C, (u4ReadVDSCL(u4VDecID, RW_VDSCL_H_SCL_C) & (~RW_VDSCL_H_SCL_C_FACTOR)) | u4HScaleFactorC);
}


void vVDSCLSetHScaleOffset(UINT32 u4VDecID, UINT32 u4HScaleOffsetY, UINT32 u4HScaleOffsetC)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_H_SCL_Y, (u4ReadVDSCL(u4VDecID, RW_VDSCL_H_SCL_Y) & (~RW_VDSCL_H_SCL_Y_OFFSET)) | (u4HScaleOffsetY<<16));
    vWriteVDSCL(u4VDecID, RW_VDSCL_H_SCL_C, (u4ReadVDSCL(u4VDecID, RW_VDSCL_H_SCL_C) & (~RW_VDSCL_H_SCL_C_OFFSET)) | (u4HScaleOffsetC<<16));
}


void vVDSCLSetVScaleFactor(UINT32 u4VDecID, UINT32 u4VScaleFactorY, UINT32 u4VScaleFactorC)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_V_SCL_Y, (u4ReadVDSCL(u4VDecID, RW_VDSCL_V_SCL_Y) & (~RW_VDSCL_V_SCL_Y_FACTOR)) | u4VScaleFactorY);
    vWriteVDSCL(u4VDecID, RW_VDSCL_V_SCL_C, (u4ReadVDSCL(u4VDecID, RW_VDSCL_V_SCL_C) & (~RW_VDSCL_V_SCL_C_FACTOR)) | u4VScaleFactorC);
}


void vVDSCLSetVScaleOffset(UINT32 u4VDecID, UINT32 u4VScaleOffsetY, UINT32 u4VScaleOffsetC)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_V_SCL_Y, (u4ReadVDSCL(u4VDecID, RW_VDSCL_V_SCL_Y) & (~RW_VDSCL_V_SCL_Y_OFFSET)) | (u4VScaleOffsetY<<16));
    vWriteVDSCL(u4VDecID, RW_VDSCL_V_SCL_C, (u4ReadVDSCL(u4VDecID, RW_VDSCL_V_SCL_C) & (~RW_VDSCL_V_SCL_C_OFFSET)) | (u4VScaleOffsetC<<16));
}

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
void vVDSCLSetScalerHParm(UINT32 u4VDecID, UINT32 u4SrcWidth, UINT32 u4TrgWidth)
#else
void vVDSCLSetScalerHParm(UINT32 u4VDecID, UINT32 u4SrcWidth, UINT32 u4TrgWidth, VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
#endif
{
    UINT32 u4HScalerFactor,u4HScalerOffset;

    if (u4SrcWidth == 0)
    {
        return;
    }
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)   
    u4HScalerFactor = ((2048 * u4TrgWidth) + (u4SrcWidth>>1)) / u4SrcWidth;
    u4HScalerOffset = 2048 - u4HScalerFactor;
    //u4HScalerOffset = 0;
#else
    u4HScalerFactor = ((0x8000 * u4TrgWidth) + (u4SrcWidth>>1)) / u4SrcWidth;
#ifdef DOWN_SCALE_SUPPORT
    prDownScalerPrm->u4SclYOffH = (UINT32)(((UINT32) rand())%u4HScalerFactor);
    prDownScalerPrm->u4SclCOffH = prDownScalerPrm->u4SclYOffH ;
#endif
    u4HScalerOffset = 0x8000 - u4HScalerFactor + prDownScalerPrm->u4SclYOffH;
#endif

    vVDSCLSetHScaleFactor(u4VDecID, u4HScalerFactor, u4HScalerFactor);
    vVDSCLSetHScaleOffset(u4VDecID, u4HScalerOffset, u4HScalerOffset);
}

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
void vVDSCLSetScalerVParm(UINT32 u4VDecID, UINT32 u4SrcHeight, UINT32 u4TrgHeight, BOOL fgIs422, BOOL fgIsMbaff, UINT32 u4OffY, UINT32 u4OffC)
{
    UINT32 u4VScalerFactorY,u4VScalerOffsetY;
    UINT32 u4VScalerFactorC,u4VScalerOffsetC;

    if (u4SrcHeight == 0)
    {
        return;
    }

    //u4VScalerFactorY = ((2048 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
    if (0 == fgIsMbaff)
    {
        u4VScalerFactorY = ((2048 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
    }
    else
    {
        // If has remainder, plus 1
        u4VScalerFactorY = (2048 * u4TrgHeight + (u4SrcHeight - 1)) / u4SrcHeight; // by Subrina, 11212007
    }
    u4VScalerOffsetY = 0;
    if (fgIs422)
    {
        //u4VScalerFactorC = ((4096 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
        if (0 == fgIsMbaff)
        {
            u4VScalerFactorC = ((4096 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
        }
        else
        {
            // If has remainder, plus 1
            u4VScalerFactorC = (4096 * u4TrgHeight + (u4SrcHeight - 1)) / u4SrcHeight; // by Subrina, 11212007
        }
        u4VScalerOffsetC = 0;
    }
    else // 420
    {
#if 0 //ndef NEW_VDSCL_PARAM	
        u4VScalerFactorC = u4VScalerFactorY;
        u4VScalerOffsetC = u4VScalerOffsetY;
#else  
        u4VScalerFactorC = u4VScalerFactorY;
        u4VScalerOffsetC = u4OffC; 
        u4VScalerOffsetY = u4OffY;
#endif
    }

    vVDSCLSetVScaleFactor(u4VDecID, u4VScalerFactorY, u4VScalerFactorC);
    vVDSCLSetVScaleOffset(u4VDecID, u4VScalerOffsetY, u4VScalerOffsetC);

}
#else
void vVDSCLSetScalerVParm(UINT32 u4VDecID, UINT32 u4SrcHeight, UINT32 u4TrgHeight, BOOL fgIs422, BOOL fgIsMbaff, UINT32 u4OffY, UINT32 u4OffC, VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
{
    UINT32 u4VScalerFactorY,u4VScalerOffsetY;
    UINT32 u4VScalerFactorC,u4VScalerOffsetC;


    if (u4SrcHeight == 0)
    {
        return;
    }

    u4VScalerFactorY = ((0x8000 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
#if 0
    if (0 == fgIsMbaff)
    {
        u4VScalerFactorY = ((0x8000 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
    }
    else
    {
        u4Remainer = (0x8000 * u4TrgHeight) % u4SrcHeight;
        if (0 == u4Remainer)
        {
            u4VScalerFactorY = (0x8000 * u4TrgHeight) / u4SrcHeight; // by Subrina, 11212007
        }
        else
        {
            u4VScalerFactorY = (0x8000 * u4TrgHeight) / u4SrcHeight + 1; // by Subrina, 11212007
        }
    }
#endif

    if (fgIs422)
    {
        u4VScalerFactorC = ((0x10000 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
#if 0
        if (0 == fgIsMbaff)
        {
            u4VScalerFactorC = ((0x10000 * u4TrgHeight) + (u4SrcHeight>>1)) / u4SrcHeight;
        }
        else
        {
            u4Remainer = (0x10000 * u4TrgHeight) % u4SrcHeight;
            if (0 == u4Remainer)
            {
                u4VScalerFactorC = (0x10000 * u4TrgHeight) / u4SrcHeight; // by Subrina, 11212007
            }
            else
            {
                u4VScalerFactorC = (0x10000 * u4TrgHeight) / u4SrcHeight + 1; // by Subrina, 11212007
            }
        }
#endif
#ifdef DOWN_SCALE_SUPPORT
        u4VScalerOffsetC = (UINT32)(((UINT32) rand())%u4VScalerFactorC); 
        u4VScalerOffsetY = (UINT32)(((UINT32) rand())%u4VScalerFactorY);
#else
        u4VScalerOffsetC = (u4VScalerFactorC -1); 
        u4VScalerOffsetY = (u4VScalerFactorY-1);
#endif
    }
    else // 420
    {
        u4VScalerFactorC = u4VScalerFactorY;
#ifdef DOWN_SCALE_SUPPORT
        u4VScalerOffsetC = (UINT32)(((UINT32) rand())%u4VScalerFactorC); 
        u4VScalerOffsetY = (UINT32)(((UINT32) rand())%u4VScalerFactorY);
#else
        u4VScalerOffsetC = (u4VScalerFactorC-1); 
        u4VScalerOffsetY = (u4VScalerFactorY-1);
#endif
    }


    prDownScalerPrm->u4SclYOffV = u4VScalerOffsetY;
    prDownScalerPrm->u4SclCOffV = u4VScalerOffsetC;

    vVDSCLSetVScaleFactor(u4VDecID, u4VScalerFactorY, u4VScalerFactorC);
    vVDSCLSetVScaleOffset(u4VDecID, u4VScalerOffsetY, u4VScalerOffsetC);
}
#endif

void vVDSCLSetSrcOffset(UINT32 u4VDecID, UINT32 u4SrcYOffH, UINT32 u4SrcYOffV, UINT32 u4SrcCOffH, UINT32 u4SrcCOffV)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_SRC_OFFSET,  (((u4SrcCOffH&0xF)<<12)|((u4SrcCOffV&0xF)<<8)|((u4SrcYOffH&0xF)<<4)|(u4SrcYOffV&0xF)));
}

void vVDSCLSetTmpBufBaseAddr(UINT32 u4VDecID, UINT32 u4TmpBufBaseAddr)
{
    vWriteVDSCL(u4VDecID, RW_VDSCL_TMP_OW_BASE, (u4ReadVDSCL(u4VDecID, RW_VDSCL_TMP_OW_BASE) & (~RW_VDSCL_TMP_OW_ADDR_BASE)) | (u4TmpBufBaseAddr >> 4));
}

#if VDEC_REMOVE_UNUSED_FUNC
BOOL fgVDSCLIsDone(UINT32 u4VDecID)
{
    return (u4ReadVDSCL(u4VDecID, RO_VDSCL_DONE) & RO_VDSCL_DONE_FRAME);
}
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
void vVDSCLSetLumaKey(UINT32 u4VDecID, BOOL fgLumaKeyEn, UINT16 u2Key)
{
    if (fgLumaKeyEn)
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_LUMA_KEY, (0x110001 | (u2Key << 0x8)) );
    }
    else
    {
        vWriteVDSCL(u4VDecID, RW_VDSCL_LUMA_KEY, 0);
    }
}
#endif

// **************************************************************************
// Function : void vVDECSetDownScalerPrm(UINT32 u4VDecID,VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
// Description :Set down scaler hardware registers
// Parameter : u4VDecID : video decoder hardware ID
//                 prDownScalerPrm : pointer to down scaler prameter struct
// Return      : None
// **************************************************************************
void vVDECSetDownScalerPrm(UINT32 u4VDecID,VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
{
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8555)

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    UINT32 src_h, tg_h;
    UINT32 u4VDSCL_SRC_OFFSET = 0;
#endif


    // down scaler
    vVDSCLReset(u4VDecID);
    vVDSCLSetEnable(u4VDecID, prDownScalerPrm->fgDSCLEn);
    if (prDownScalerPrm->fgDSCLEn)
    {
        vVDSCLSetAddrSwap(u4VDecID, prDownScalerPrm->ucAddrSwapMode);
        vWriteVDSCL(u4VDecID, RW_VDSCL_TYPE, (prDownScalerPrm->ucScrAgent << 2) | 
            (prDownScalerPrm->ucSpectType << 5) | 
            (prDownScalerPrm->fgYOnly << 7) |
            (prDownScalerPrm->ucScanType << 16) |
            (prDownScalerPrm->ucVdoFmt << 17));

        vVDSCLSetBoundPad(u4VDecID, PAD_MODE_NO_PAD, 0, 0, 0);

        vVDSCLSetSrcPicSize(u4VDecID, prDownScalerPrm->u4SrcWidth, prDownScalerPrm->u4SrcHeight);
        vVDSCLSetTrgBufWidth(u4VDecID, prDownScalerPrm->u4DispW);
        vVDSCLSetTrgPicSize(u4VDecID, prDownScalerPrm->u4TrgWidth, prDownScalerPrm->u4TrgHeight);
        vVDSCLSetTrgPicOffset(u4VDecID, prDownScalerPrm->u4TrgOffH, prDownScalerPrm->u4TrgOffV);
        vVDSCLSetTrgYBufBaseAddr(u4VDecID, u4AbsDramANc((UINT32) prDownScalerPrm->u4TrgYAddr));
        vVDSCLSetTrgCBufBaseAddr(u4VDecID, u4AbsDramANc((UINT32) prDownScalerPrm->u4TrgCAddr));
        vVDSCLSetTmpBufBaseAddr(u4VDecID, u4AbsDramANc((UINT32) prDownScalerPrm->u4WorkAddr));
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)       
        vVDSCLSetScalerHParm(u4VDecID, prDownScalerPrm->u4SrcWidth, prDownScalerPrm->u4TrgWidth);
#else
        vVDSCLSetScalerHParm(u4VDecID, prDownScalerPrm->u4SrcWidth, prDownScalerPrm->u4TrgWidth, prDownScalerPrm);
#endif        

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
        prDownScalerPrm->u4QY = 0;
        prDownScalerPrm->u4QC = 0;
        prDownScalerPrm->u4R_norm = 0;
        if (prDownScalerPrm->ucPicStruct == BTM_FLD_PIC)
        {
            src_h = prDownScalerPrm->u4SrcHeight;
            tg_h = prDownScalerPrm->u4TrgHeight;
            prDownScalerPrm->u4QY = (src_h - tg_h) /(tg_h * 2 );
            prDownScalerPrm->u4QC = (src_h - tg_h) /(tg_h * 2 );
            prDownScalerPrm->u4R_norm = ( ((src_h - tg_h) - (prDownScalerPrm->u4QY * (tg_h *2))) * 2048 ) / (2 * src_h );           
        }

        prDownScalerPrm->u4OffY = prDownScalerPrm->u4R_norm;
        prDownScalerPrm->u4OffC = prDownScalerPrm->u4R_norm;

        if (prDownScalerPrm->ucScrAgent == VDSCL_SRC_PP)
        { 
            //source PP
            if (prDownScalerPrm->ucSpectType == VDSCL_SPEC_WMV)
            {
                //WMV case, max src_v_ofty = 4, max src_v_oftc = 0
                if (prDownScalerPrm->u4QY > 4)
                {
                    prDownScalerPrm->u4QY = 4;
                }

                prDownScalerPrm->u4QC = 0;        	
            }
            else
            {
                //h264 case, max src_v_ofty = 6, max src_v_oftc = 3
                if (prDownScalerPrm->u4QY > 6)
                {
                    prDownScalerPrm->u4QY = 6;
                }

                if (prDownScalerPrm->u4QC > 3)
                {
                    prDownScalerPrm->u4QC = 3;
                }
            }

        }
        else
        { 
            //source MC
            //for MC output, max src_v_ofty = 8, max src_v_oftc = 4      
            if (prDownScalerPrm->u4QY > 8)
            {
                prDownScalerPrm->u4QY = 8;
            }

            if (prDownScalerPrm->u4QC > 4)
            {
                prDownScalerPrm->u4QC = 4;
            }
        }

        //vWriteVDSCL(u4VDecID, RW_VDSCL_SRC_OFFSET, u4ReadVDSCL(u4VDecID, RW_VDSCL_SRC_OFFSET)  | (prDownScalerPrm->u4QC<<8) | (prDownScalerPrm->u4QY));
        u4VDSCL_SRC_OFFSET = (u4ReadVDSCL(u4VDecID, RW_VDSCL_SRC_OFFSET) & 0xFFFF0000);
        u4VDSCL_SRC_OFFSET |=  ( (prDownScalerPrm->u4QC<<8) | (prDownScalerPrm->u4QY));
        vWriteVDSCL(u4VDecID,RW_VDSCL_SRC_OFFSET, u4VDSCL_SRC_OFFSET);

        vVDSCLSetScalerVParm(u4VDecID, prDownScalerPrm->u4SrcHeight, prDownScalerPrm->u4TrgHeight, \
            prDownScalerPrm->ucVdoFmt, prDownScalerPrm->fgMbaff, \
            prDownScalerPrm->u4OffY, prDownScalerPrm->u4OffC);
#else
        vVDSCLSetSrcOffset(u4VDecID, prDownScalerPrm->u4SrcYOffH, prDownScalerPrm->u4SrcYOffV, prDownScalerPrm->u4SrcCOffH, prDownScalerPrm->u4SrcCOffV);

        vVDSCLSetScalerVParm(u4VDecID, prDownScalerPrm->u4SrcHeight, prDownScalerPrm->u4TrgHeight, \
            prDownScalerPrm->ucVdoFmt, prDownScalerPrm->fgMbaff, \
            prDownScalerPrm->u4OffY, prDownScalerPrm->u4OffC, prDownScalerPrm);
#endif

        if (prDownScalerPrm->fgEnColorCvt)
        {
            vVDSCLSetConvert(u4VDecID, TRUE);
        }
        else
        {
            vVDSCLSetConvert(u4VDecID, FALSE);
        }

        switch (prDownScalerPrm->ucPicStruct)
        {
        case TOP_FIELD:
            vVDSCLSetPicStruct(u4VDecID, PIC_STRUCT_TOP);
            break;
        case BOTTOM_FIELD:
            vVDSCLSetPicStruct(u4VDecID, PIC_STRUCT_BOTTOM);
            break;
        case FRAME:
            vVDSCLSetPicStruct(u4VDecID, PIC_STRUCT_FRAME);
            break;
        default:
            vVDSCLSetPicStruct(u4VDecID, PIC_STRUCT_TOP_BOTTOM);
            break;
        }

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
        vVDSCLSetLumaKey(u4VDecID, prDownScalerPrm->fgLumaKeyEn, prDownScalerPrm->u2LumaKeyValue);
#endif  
    }  

#endif
    return;
}


#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
void vVDECSetLetetrBoxDetPrm(UINT32 u4VDecID, VDEC_INFO_LBD_PRM_T *prLBDPrm)
{
    vVDecWriteDV(u4VDecID, RW_VEC_LBOX_UP_CFG , (prLBDPrm->u4UpBoundEnd << 16) | (prLBDPrm->u4UpBoundStart));
    vVDecWriteDV(u4VDecID, RW_VEC_LBOX_LOW_CFG , (prLBDPrm->u4LowBoundStart << 16) | (prLBDPrm->u4LowBoundEnd));
    vVDecWriteDV(u4VDecID, RW_VEC_LBOX_SIDE_CFG , (prLBDPrm->u4LeftBound << 16) | (prLBDPrm->u4RightBound));
    vVDecWriteDV(u4VDecID, RW_VEC_LBOX_THD_OFFSET, (prLBDPrm->u4YOffset << 24) | (prLBDPrm->u4COffset << 16)
        | (prLBDPrm->u4YValueThd << 8) | (prLBDPrm->u4CValueThd << 0));
    vVDecWriteDV(u4VDecID, RW_VEC_LBOX_CNT_THD, (prLBDPrm->u4YCntThd << 16) | (prLBDPrm->u4CCntThd));
}


void vVDECReadLetetrBoxDetResult(UINT32 u4VDecID, UINT32 *u4YResult, UINT32 *u4CResult)
{
    *u4YResult = u4VDecReadDV(u4VDecID, RO_VEC_LBOX_Y_LINE);
    *u4CResult = u4VDecReadDV(u4VDecID, RO_VEC_LBOX_C_LINE);
}
#endif


#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && (!CONFIG_DRV_FPGA_BOARD))
void vVDecSetVldMcClk(UINT32 u4VDecID,UINT32 u4CodecType)
{
	
}
#endif


#if DTV_COMMON_CONFIG
void vVDEC_HAL_Ckgen(UINT32 u4VDecID, ENUM_VDEC_FMT_T eCurFMT)
{
    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
        #if DUAL_VDEC_TEST
        vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | 0xC)); //432MHz
        #else
        vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_FAST));
        #endif
		if (VDEC_LAE0 == u4VDecID)
		{
			vWriteReg(CKGEN_VDEC_LAE_CKCFG, ((u4ReadReg(CKGEN_VDEC_LAE_CKCFG) & 0xFFFFFFFC) | 0x1));
		}
        break;

    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        vWriteReg(CKGEN_VDEC_SYS_CKCFG,VDEC_SYS_CK_SEL_MID);
        //vWriteReg(CKGEN_VDEC_SYS_CKCFG,14); //336M
        UTIL_Printf("CKGEN_VDEC_SYS_CKCFG = 0x%x\n",u4ReadReg(CKGEN_VDEC_SYS_CKCFG));
        break;

    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_SLOW));
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        LOG(2, "%s: disabled format %d\n", __FUNCTION__, eCurFMT);
        break;
    case VDEC_FMT_H265:
        if (VDEC_LAE0 == u4VDecID)
    	{
    		vWriteReg(CKGEN_VDEC_LAE_CKCFG, ((u4ReadReg(CKGEN_VDEC_LAE_CKCFG) & 0xFFFFFFFC) | 0x1));
    		//LOG(0,"VDEC Read LAE Ckgen 0x%x Val 0x%x\n",(IO_BASE_ADDRESS + CKGEN_VDEC_LAE_CKCFG),u4ReadReg(CKGEN_VDEC_LAE_CKCFG));
    	}
    	else
    	{
        	#if DUAL_VDEC_TEST
        	//vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_SLOW));
        	//vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | 0xC)); //432MHz    avc + hevc clock setting
        	vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | VDEC_SYS_CK_SEL_SLOW));//avs + hevc clock seting
        	#else
    //      vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | 0xE)); //504MHz
            vWriteReg(CKGEN_VDEC_SYS_CFG,((u4ReadReg(CKGEN_VDEC_SYS_CFG) & 0xFFFFFFF0) | 0xC)); //432MHz
            //UTIL_Printf("2Vdec CKGEN_VDEC_SYS_CKCFG = 0x%x\n",u4ReadReg(CKGEN_VDEC_SYS_CFG));
            #endif
            
        }
        
        break;
    default:
        LOG(0, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break;
    }

    return TRUE;
}

void vVDEC_HAL_SysClkSel(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT)
{
    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
		if (VDEC_LAE0 == ucEsId)
		{
			vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
		}
        break;

    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_MID_CLK);
        mb();
        UTIL_Printf("Clk_sel = 0x%x\n",u4VDecReadMISC(ucEsId,VDEC_SYS_CLK_SEL));
        break;

    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_SLOW_CLK);
        //UTIL_Printf("vVDEC_HAL_SysClkSel = 0x%x\n",u4VDecReadMISC(ucEsId, VDEC_SYS_CLK_SEL));
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;
    case VDEC_FMT_H265:
        #if DUAL_VDEC_TEST
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_SLOW_CLK);//avs + hevc setting
        #else
        vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
        #endif
		if (VDEC_LAE0 == ucEsId)
		{
			vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
		}
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
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_H264);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_H264);		
		#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_H264);
		#endif
		if (VDEC_LAE0 == ucEsId)
		{
			vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_H264);
        	vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_H264_LAE);			
		}
        break;
	case VDEC_FMT_H265:
	    if (_u4DualCoreEnable[0])
	    {
	        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,0xFF);
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0x35E30180);
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE2,0x20);
	    }
	    else
	    {
	        #if 0
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,0xFF);
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0x77E30180);
//            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE2,0x60);
            //if UFO mode 
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE2,0x20);
            #else
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,0x00);
            vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0x00);
            vVDecWriteMISC(ucEsId,(RW_VDEC_MISC_PDN_CON_MODULE2),0x00);    
            #endif
        }
        break;
    case VDEC_FMT_MPV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MPV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MPV);
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_MPV);
        break;
        
    case VDEC_FMT_MP4:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_MP4);
        //x_thread_delay(100);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_MP4);
        //x_thread_delay(100);
		#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_MP4);
        //x_thread_delay(100);
		#endif
        break;
    case VDEC_FMT_WMV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WMV);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_WMV);
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_WMV);
        #endif
		break;
    case VDEC_FMT_VP8:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP8);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_VP8);			
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5882)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_VP8);
        #endif
        break;

    case VDEC_FMT_RV:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_RM);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_RM);
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
		vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_RM);
        #endif
		break;
    case VDEC_FMT_AVS:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_AVS);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_AVS);
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_AVS);
        #endif
        //UTIL_Printf("vVDEC_HAL_PowerSave RW_VDEC_MISC_PDN_CON_SPEC = 0x%x\n",u4VDecReadMISC(ucEsId, RW_VDEC_MISC_PDN_CON_SPEC));
        //UTIL_Printf("vVDEC_HAL_PowerSave RW_VDEC_MISC_PDN_CON_MODULE = 0x%x\n",u4VDecReadMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE));
		break;
    case VDEC_FMT_VP6:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_VP6);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,PDN_MODULE_VP6);
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5882)
        vVDecWriteMISC(ucEsId, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_VP6);
        #endif
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

void vVDEC_HAL_UFO_PowerSave(UCHAR ucEsId)
{
	#if  TWO_THREAD_ENABLE
	vVDecWriteMCORETOP(ucEsId,RW_VDEC_UFO_MCORE_TOP_CON_MODULE, PDN_UFO_MODULE_TWO_SPEC);
	#else
	if (1 == _u4DualCoreEnable[0])
	{
		vVDecWriteMCORETOP(ucEsId,RW_VDEC_UFO_MCORE_TOP_CON_MODULE, PDN_UFO_MODULE_DUAL_CORE);
	}
	else if (VDEC_VLD0 == ucEsId)
	{	
		vVDecWriteMCORETOP(ucEsId,RW_VDEC_UFO_MCORE_TOP_CON_MODULE, PDN_UFO_MODULE_CORE0_ONLY);
	}
	else if (VDEC_VLD1 == ucEsId)
	{
		vVDecWriteMCORETOP(ucEsId,RW_VDEC_UFO_MCORE_TOP_CON_MODULE, PDN_UFO_MODULE_CORE1_ONLY);
	}
	else
	{
		vVDecWriteMCORETOP(ucEsId,RW_VDEC_UFO_MCORE_TOP_CON_MODULE, PDN_UFO_MODULE_LAE_CORE0);
	}
	#endif
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
    case VDEC_FMT_WEBP:
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_SPEC,PDN_SPEC_WEBP);
        vVDecWriteMISC(ucEsId,RW_VDEC_MISC_PDN_CON_MODULE,0xFFFFFFFF);
		break;

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


#ifdef VDEC_SIM_DUMP

#define DUMP_FOR_SIM 1

void vVDecSimWDump(const char * szFunction,UINT32 u4VDecID,const char * szOffset,UINT32 u4Addr,UINT32 u4Val, UINT32 u4BaseReg)
{
	if (szFunction)
	{
		if (DUMP_FOR_SIM)
		{
			//printk("RISCWrite(`%s + 4*%d, %d );  //u4val = 0x%08X /* 0x%08X */\n", szOffset, u4Addr>>2, u4Val, u4BaseReg);
			//printk("RISCWrite(`%s + 4*%d, %d );  //u4val = 0x%08X\n", szOffset, u4Addr>>2, u4Val, u4Val);
        #ifdef REG_LOG_NEW
			if (_fgRegLogConsole[u4VDecID])
        #endif
			{
				switch (u4BaseReg)
				{
				case VDEC_MISC_BASE0:
				case VDEC_MISC_BASE1:
					printk("		RISCWrite(`VDEC_MISC_BASE%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_MISC_OFFSET0:
					printk("		RISCWrite(`LAE_MISC_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VLD_REG_OFFSET0:
				case VLD_REG_OFFSET1:
					printk("		RISCWrite(`VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VLD_TOP_REG_OFFSET0:
				case VLD_TOP_REG_OFFSET1:
					printk("		RISCWrite(`VLD_TOP_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case MC_REG_OFFSET0:
				case MC_REG_OFFSET1:
					printk("		RISCWrite(`MC_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case AVC_MV_REG_OFFSET0:
				case AVC_MV_REG_OFFSET1:
					printk("		RISCWrite(`AVC_MV_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case HEVC_VLD_REG_OFFSET0:
            	case HEVC_VLD_REG_OFFSET1:
					printk("		RISCWrite(`HEVC_VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_HEVCVLD_OFFSET0:
            	case LAE_HEVCVLD_OFFSET1:
					printk("		RISCWrite(`LAE_HEVCVLD_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;				  
				case LAE_VLDTOP_OFFSET0:
                case LAE_VLDTOP_OFFSET1:
					printk("		RISCWrite(`LAE_VLDTOP_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;				  
				case PP_REG_OFFSET0:
				case PP_REG_OFFSET1:
					printk("		RISCWrite(`PP_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case MVDEC_TOP_OFFSET0:
					printk("		RISCWrite(`MVDEC_TOP_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case AVS_VLD_REG_OFFSET0:
					printk("		RISCWrite(`AVS_VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case AVC_VLD_REG_OFFSET0:
				case AVC_VLD_REG_OFFSET1:
					printk("		RISCWrite(`AVC_VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case RM_VLD_REG_OFFSET0:
					printk("		RISCWrite(`RM_VLD_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VP8_VLD_REG_OFFSET0:
					printk("		RISCWrite(`VP8_VLD_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VP8_VLD2_REG_OFFSET0:
					printk("		RISCWrite(`VP8_VLD2_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				default:
					printk("		RISCWrite(`others%d + 4*%d, 32'h%x) base 0x%08x\n", u4VDecID,u4Addr >> 2, u4Val,u4BaseReg);
					break;
				}
			}			 
        #ifdef REG_LOG_NEW
			else
			{
				switch (u4BaseReg)
				{
				case DV_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_MISC( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case VLD_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_VLD( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case VLD_TOP_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_VLD_TOP( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case MC_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_MC( %d, %d );	//u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case AVC_MV_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_MV( %d, %d );	//u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case RM_VDEC_PP_BASE:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_PP( %d, %d );	//u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					break;
				case HEVC_VLD_REG_OFFSET0:
					 _u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], "		RISCWrite_HEVC_VLD( %d, %d );  //u4val = 0x%08X\n", u4Addr >> 2, u4Val, u4Val);
					 break;
				default:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCWrite_Others( %d, %d );	//u4val = 0x%08X /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4Val, u4BaseReg);
					break;
				}
			}
        #endif
		}
		else
		{
			//UTIL_Printf("%s(u4VDecID, 0x%x, 0x%x); /* 0x%08X */\n", szFunction, u4Addr, u4Val, u4BaseReg);
			UTIL_Printf("%s(u4VDecID, 0x%x, 0x%x); \n", szFunction, u4Addr, u4Val);
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
		if (DUMP_FOR_SIM)
		{
			//printk("RISCRead(`%s + 4*%d, %d );  //u4val = 0x%08X /* 0x%08X */\n", szOffset, u4Addr>>2, u4Val, u4Val, u4BaseReg);
			//printk("RISCRead(`%s + 4*%d, %d );  //u4val = 0x%08X\n", szOffset, u4Addr>>2, u4Val, u4Val);
        #ifdef REG_LOG_NEW
			if (_fgRegLogConsole[u4VDecID])
        #endif
			{
				switch (u4BaseReg)
				{
				case VDEC_MISC_BASE0:
				case VDEC_MISC_BASE1:
					printk("		RISCRead(`VDEC_MISC_BASE%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_MISC_OFFSET0:
                case LAE_MISC_OFFSET1:
					printk("		RISCRead(`LAE_MISC_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VLD_REG_OFFSET0:
				case VLD_REG_OFFSET1:
					printk("		RISCRead(`VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case VLD_TOP_REG_OFFSET0:
				case VLD_TOP_REG_OFFSET1:
					printk("		RISCRead(`VLD_TOP_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case MC_REG_OFFSET0:
				case MC_REG_OFFSET1:
					printk("		RISCRead(`MC_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case AVC_MV_REG_OFFSET0:
				case AVC_MV_REG_OFFSET1:
					printk("		RISCRead(`AVC_MV_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case HEVC_VLD_REG_OFFSET0:
                case HEVC_VLD_REG_OFFSET1:
					printk("		RISCRead(`HEVC_VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_HEVCVLD_OFFSET0:
                case LAE_HEVCVLD_OFFSET1:
					printk("		RISCRead(`LAE_HEVCVLD_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_VLD_OFFSET0:
                case LAE_VLD_OFFSET1:
					printk("		RISCRead(`LAE_VLD_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case LAE_VLDTOP_OFFSET0:
                case LAE_VLDTOP_OFFSET1:
					printk("		RISCRead(`LAE_VLDTOP_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case PP_REG_OFFSET1:
				case PP_REG_OFFSET0:
					printk("		RISCRead(`PP_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case MVDEC_TOP_OFFSET0:
					printk("		RISCRead(`MVDEC_TOP_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case AVC_VLD_REG_OFFSET0:
				case AVC_VLD_REG_OFFSET1:					
					printk("		RISCRead(`AVC_VLD_REG_OFFSET%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				case RM_VLD_REG_OFFSET0:
					printk("		RISCRead(`RM_VLD_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;

				case VP8_VLD_REG_OFFSET0:
					printk("		RISCRead(`VP8_VLD_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;

				case VP8_VLD2_REG_OFFSET0:
					printk("		RISCRead(`VP8_VLD2_REG_OFFSET0%d + 4*%d, 32'h%x)\n", u4VDecID,u4Addr >> 2, u4Val);
					break;
				default:
					printk("		RISCRead_Others( %d);  /* return 0x%08X */ /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4BaseReg);
					break;
				}
			}
        #ifdef REG_LOG_NEW
			else
			{
				switch (u4BaseReg)
				{
				case DV_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_MISC( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case VLD_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_VLD( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case VLD_TOP_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_VLD_TOP( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case MC_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_MC( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case AVC_MV_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_MV( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case RM_VDEC_PP_BASE:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_PP( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				case HEVC_VLD_REG_OFFSET0:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_HEVC_VLD( %d); /* return 0x%08X */\n", u4Addr >> 2, u4Val);
					break;
				default:
					_u4RegisterLogLen[u4VDecID] += sprintf(_pucRegisterLog[u4VDecID]+_u4RegisterLogLen[u4VDecID], " 	   RISCRead_Others( %d);  /* return 0x%08X */ /* base 0x%08X */\n", u4Addr >> 2, u4Val, u4BaseReg);
					break;
				}
			}
			
        #endif
		}
		else
		{
			//UTIL_Printf("%s(u4VDecID, 0x%x); /* return 0x%x */ /* 0x%08X */\n", szFunction, u4Addr, u4Val, u4BaseReg);
			UTIL_Printf("%s(u4VDecID, 0x%x); /* return 0x%x */ \n", szFunction, u4Addr, u4Val);
		}
	}
	else
	{
	}
}




UINT32 _u4SimDumpCnt = 0;
UINT32 _u4SimDumpMax = 0xffffffff;
void vVDecSimDump(UINT32 u4VDecID, UINT32 u4OffsetAddr, UINT32 u4Addr, UINT32 u4Val)
{
    if(_u4SimDumpCnt > _u4SimDumpMax)
    {
        return;
    }
    switch(u4OffsetAddr)
    {
    case VLD_REG_OFFSET0:
//	 case VLD_REG_OFFSET1:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VLD + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
    case VLD_TOP_REG_OFFSET0:
//	 case VLD_TOP_REG_OFFSET1:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VLD_TOP + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;
#endif

    case MC_REG_OFFSET0:
//        case MC_REG_OFFSET1:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_MC + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

         
    case DV_REG_OFFSET0:
//        case DV_REG_OFFSET1:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_DV + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

    case AVC_VLD_REG_OFFSET0:
//        case AVC_VLD_REG_OFFSET1:
        if(u4Addr >= RW_AVLD_CTRL)
        {
            UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_AVC_VLD + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        }
        if(u4Addr >= RW_AVLD_PROC)
        {
            _u4SimDumpCnt ++;
        }
        break;
    case AVC_MV_REG_OFFSET0:
//        case AVC_MV_REG_OFFSET1:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_AVC_MV + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

	case VP6_VLD_REG_OFFSET0:
        UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP6_VLD + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

	case VP8_VLD_REG_OFFSET0:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP8_VLD + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

	case VP6_PP_REG_OFFSET0:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_PP + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

	//case VP6_MV_REG_OFFSET0:
	//	UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP6_MV + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
    //    break;

	case VP6_DCAC_REG_OFFSET0:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP6_DCAC + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;

	case VP6_VLD2_REG_OFFSET:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP6/VP8_VLD2 + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;
		
	case VP6_VLD2_SHIFT_OFFSET:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_VP6_VLD2SHIFT + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;
    #if 0
	case VDEC_MISC_BASE0:
		UTIL_Printf("RISCWrite('VDEC_FULL_ADDR_MISC + 4*%d, 32'h%x); \n", u4Addr>>2, u4Val);
        break;
	#endif
    }
    
    //x_thread_delay(4);  //In wait state
}
#endif

