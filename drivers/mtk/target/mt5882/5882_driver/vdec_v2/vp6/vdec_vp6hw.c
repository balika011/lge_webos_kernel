/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright(c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp6hwdec.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp6hwdec.c
 *  This file contains implementation of Vp6 hw decoder.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_printf.h"
#include "vdec_drvif.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END
;
#include "vdec_vp6util.h"
#include "vdec_vp6dec.h"
#include "vdec_vp6psr.h"
#include "vdec_vp6hw.h"
#include "vdec_vp6if.h"

static BOOL _bVdecVP6LogSimPat = FALSE;

#if defined(CC_VP6_EMULATION) || defined(VDEC_DEC_REG_QUERY)
void _VDEC_VP6_LogSimPat(BOOL bLogEn)
{
    _bVdecVP6LogSimPat = bLogEn;
}
#endif

UINT32 _VDEC_VP6_IO_Read32(UINT32 u4Base, UINT32 u4Offset)
{
    UINT32 u4Val;

#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    u4Val = IO_READ32(u4Base, u4Offset);
    
    if(_bVdecVP6LogSimPat)
    {
        switch(u4Base)
        {
            case VLD_TOP_BASE:
                Printf( "RISCRead_VLD_TOP(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VLD_BASE:
                Printf( "RISCRead_VLD(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VLD2_BASE:
                Printf( "RISCRead_VLD2(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VP6_VLD_BASE:
                Printf( "RISCRead_VLD_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VP6_VLD2_BASE:
                Printf( "RISCRead_VLD2_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case MC_BASE:
                Printf( "RISCRead_MC(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VP6_MV_BASE:
                Printf( "RISCRead_MV_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case VDEC_MISC_BASE:
                Printf( "RISCRead_VDEC_MISC(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;

            case PP_BASE:
                Printf( "RISCRead_PP_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Val);
                break;
                
            default:
                break;
        }
        //x_thread_delay(1);
    }
    
    return u4Val;
}

void _VDEC_VP6_IO_Write32(UINT32 u4Base, UINT32 u4Offset, UINT32 u4Value)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    if(_bVdecVP6LogSimPat)
    {
        switch(u4Base)
        {
        case VLD_TOP_BASE:
            Printf( "RISCWrite_VLD_TOP(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
        
        case VLD_BASE:
            Printf( "RISCWrite_VLD(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;

        case VLD2_BASE:
            Printf( "RISCWrite_VLD2(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;

        case VP6_VLD_BASE:
            Printf( "RISCWrite_VLD_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;

        case VP6_VLD2_BASE:
            Printf( "RISCWrite_VLD2_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
            
        case MC_BASE:
            Printf( "RISCWrite_MC(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
        
        case VP6_MV_BASE:
            Printf( "RISCWrite_MV_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
        
        case VDEC_MISC_BASE:
            Printf( "RISCWrite_VDEC_MISC(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
        
        case PP_BASE:
            printf( "RISCWrite_PP_VP6(4*%d, 32'h%x);\n", u4Offset/4, u4Value);
            break;
            
        default:
            break;
        }
        //x_thread_delay(1);
    }

    IO_WRITE32(u4Base, u4Offset, u4Value);
}

void vVDecSetVLDVFIFO(UINT32 u4Id, UINT32 u4VFifoSa, UINT32 u4VFifoEa)
{
    vVDecWriteVLDEx(u4Id, RW_VLD_VSTART, u4VFifoSa >> 6);
    vVDecWriteVLDEx(u4Id, RW_VLD_VEND, u4VFifoEa >> 6);
}

BOOL fgVDecIsVLDFetchOk(UINT32 u4Id)
{
    if ((u4VDecReadVLDEx(u4Id, RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
    {
        return (FALSE);
    }
    return (TRUE);
}

BOOL fgVDecWaitVldFetchOk(UINT32 u4Id)
{
    UINT32 u4Cnt;
  
    if (!fgVDecIsVLDFetchOk(u4Id))
    {
        u4Cnt = 0;
        while (!fgVDecIsVLDFetchOk(u4Id))
        {
            u4Cnt++;
            if (u4Cnt >= WAIT_THRD)
            {
                return (FALSE);
            }
        }
    }
    return (TRUE);
}

void vVDecWriteVLDTop(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    VLD_TOP_WRITE32(u4Addr, u4Val);
}


UINT32 u4VDecReadVLDTop(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return VLD_TOP_READ32(u4Addr);
}

void vVDecWriteVLD(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    VLD_WRITE32(u4Addr, u4Val);
}

UINT32 u4VDecReadVLD(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return VLD_READ32(u4Addr);
}

void vVDecWriteVLDEx(UINT32 u4Id, UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    if (u4Id == 0)
    {
        VLD_WRITE32(u4Addr, u4Val);
    }
    else
    {
        VLD2_WRITE32(u4Addr, u4Val);
    }
}

UINT32 u4VDecReadVLDEx(UINT32 u4Id, UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    if (u4Id == 0)
    {
        return VLD_READ32(u4Addr);
    }
    else
    {
        return VLD2_READ32(u4Addr);
    }
}

void vVDecWriteVP6VLD(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    VP6_VLD_WRITE32(u4Addr, u4Val);
}

UINT32 u4VDecReadVP6VLD(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return VP6_VLD_READ32(u4Addr);
}

void vVDecWriteVP6VLDEx(UINT32 u4Id, UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    if (u4Id == 0)
    {
        VP6_VLD_WRITE32(u4Addr, u4Val);
    }
    else
    {
        VP6_VLD2_WRITE32(u4Addr, u4Val);
    }
}

UINT32 u4VDecReadVP6VLDEx(UINT32 u4Id, UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    if (u4Id == 0)
    {
        return VP6_VLD_READ32(u4Addr);
    }
    else
    {
        return VP6_VLD2_READ32(u4Addr);
    }
}

void vVDecWriteMC(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    MC_WRITE32(u4Addr, u4Val);
}

UINT32 u4VDecReadMC(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return MC_READ32(u4Addr);
}

#if 0
void vVDecWriteVP6MV(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    VP6_MV_WRITE32(u4Addr, u4Val);
}

UINT32 u4VDecReadVP6MV(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return VP6_MV_READ32(u4Addr);
}
#endif

void vVDecWriteVP6PP(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    PP_WRITE32(u4Addr, u4Val);
}

void vVDecWriteVDECMisc(UINT32 u4Addr, UINT32 u4Val)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    VDEC_MISC_WRITE32(u4Addr, u4Val);
}

#ifdef VP6_GEN_CRC
UINT32 u4VDecReadVDECMisc(UINT32 u4Addr)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return VDEC_MISC_READ32(u4Addr);
}
#endif

UINT32 u4VDecVP6VLDGetBits(UINT32 u4Id, UINT32 dShiftBit)
{
#ifdef TEST_ONOFF_RUNTIME
    _VDEC_CheckLockStatus(VLD0);
#endif
    return u4VDecReadVP6VLDEx(u4Id, RO_VP6_VLD_BARL + (dShiftBit << 2));
}

UINT32 u4VDecWaitVP6GetBitsReady(void)
{
    UINT32 u4Val = 0, u4Cnt = 0;

    while ((u4VDecReadVP6VLD(RW_VP6_GBFR) & GRDY) == 0)
    {
        u4Cnt++;
        if (u4Cnt >= WAIT_THRD)
        {
            LOG(1, "u4VDecWaitVP6GetBitsReady Fail\n");
            break;
        }
    }

    u4Val = u4VDecReadVP6VLD(RW_VP6_GBFR);   
    return u4Val;
}

UINT32 u4VDecVP6BOOLGetBits(UINT32 dShiftBit)
{
    UINT32 u4RegVal;
    UINT32 u4Tmp = 0;
    
    vVDecWriteVP6VLD(RW_VP6_BCPR, dShiftBit);
    u4Tmp       = u4VDecWaitVP6GetBitsReady();
    u4RegVal    = ((u4Tmp & 0xFF00) >> 8);
 
    return (u4RegVal);
}

#if 0
UINT32 u4VDecReadVP6VldRPtr(UINT32 *pu4Bits)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr, u4SramVal, u4SramCtrl;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
    UINT32 u4VStart, u4VEnd;
    UINT32 u4Cnt = 0;

    if (!pu4Bits)
    {
        return 0;
    }
    
    // HW issue, wait for read pointer stable
    if (u4VDecReadVLD(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((u4VDecReadVLD(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(1, "WaitSramStable Fail\n");
                break;
            }
        }
    }

    u4DramRptr  = u4VDecReadVLD(RO_VLD_VRPTR);
    u4SramVal   = u4VDecReadVLD(RO_VLD_VBAR);
    u4SramRptr  = (UINT32)(u4SramVal & 0xf);
    u4SramWptr  = (UINT32)((u4SramVal >> 8) & 0xf);

    if(u4SramWptr >= u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
    }

  	u4SramCtrl  = (u4SramVal >> 24) & 3;
    *pu4Bits    = u4VDecReadVLD(RO_VLD_SUM) & 0x3f;

  	u4ByteAddr  = u4DramRptr - u4SramDataSz * 16 + u4SramCtrl * 4 - 16 + ((*pu4Bits+7)/8);

    u4VStart    = u4VDecReadVLD(RW_VLD_VSTART) << 6;    
    u4VEnd      = u4VDecReadVLD(RW_VLD_VEND) << 6;

    if (u4ByteAddr < u4VStart)
    {
        u4ByteAddr = u4ByteAddr + (u4VEnd - u4VStart);
    }
    
    if(u4ByteAddr > u4VEnd)
    {
        u4ByteAddr = u4VStart + (u4ByteAddr - u4VEnd);
    }

    return (u4ByteAddr);
}
#endif

void _VDEC_SetVP6VldWPtr(UINT32 u4Id, UINT32 u4WPtr)
{
    UINT32 u4WPtrAlign, u4VStart, u4VEnd;

    u4WPtrAlign = ((u4WPtr + RPTR_ALIGN) & (~((UINT32)RPTR_ALIGN - 1))) + WPTR_OFFSET;

    u4VStart    = u4VDecReadVLDEx(u4Id, RW_VLD_VSTART)<<6;
    u4VEnd      = u4VDecReadVLDEx(u4Id, RW_VLD_VEND)<<6;    

    if(u4WPtrAlign < u4VStart)
    {
        u4WPtrAlign = u4VEnd - (u4VStart - u4WPtrAlign);
    }
    
    if(u4WPtrAlign > u4VEnd)
    {
        u4WPtrAlign = u4VStart + (u4WPtrAlign - u4VEnd);
    }
    
    vVDecWriteVLDEx(u4Id, WO_VLD_WPTR, PHYSICAL(u4WPtrAlign));
}

#ifdef VP6_ENABLE_POWER_DOWN
void _VDEC_VP6PowerDownSetting(void)
{
    LOG(7,"vp6 power down on\n");
	#if defined (VDEC_IS_POST_MT5882)
	     if(BSP_GetIcVersion() == IC_VER_5890_AC)
         {
			vVDecWriteVDECMisc(0xC8, 0xffbf);
			vVDecWriteVDECMisc(0x178, 0xfffffffb);
	     }
		 else
		 {
			vVDecWriteVDECMisc(0xC8, 0x1BF);
			vVDecWriteVDECMisc(0x178, 0x7B);
		 }
	#else
					
    vVDecWriteVDECMisc(0xC8, 0xBF);
	#endif
    
    #if defined(VDEC_VP6_HWDEBLOCK) || defined(VDEC_FORCE_DDR3)
	    #if defined (VDEC_IS_POST_MT5882)
			 if(BSP_GetIcVersion() == IC_VER_5890_AC)
         	{
				vVDecWriteVDECMisc(0xCC, 0x646A11C8);
			}
			else
			{
				vVDecWriteVDECMisc(0xCC, 0x646A1108);
			}
        #elif defined(VDEC_IS_POST_MT5398)
        	vVDecWriteVDECMisc(0xCC, 0x246A1108);
        #else
        	vVDecWriteVDECMisc(0xCC, 0x24EA9108);
        #endif
    #else
        if(DDR_III_x2 == DRVCUST_InitGet(eDramType))//Vp6IsSetDDR3Mode()
        {
        LOG(7,"vp6 in DDR3 mode\n");
		#if defined (VDEC_IS_POST_MT5882)
			if(BSP_GetIcVersion() == IC_VER_5890_AC)
         	{
				vVDecWriteVDECMisc(0xCC, 0x646A11C8);
			}
			else
			{
				vVDecWriteVDECMisc(0xCC, 0x646A1108);
			}
        #elif defined(VDEC_IS_POST_MT5398)
        	vVDecWriteVDECMisc(0xCC, 0x246A1108);
        #else
        	vVDecWriteVDECMisc(0xCC, 0x24EA9108);
        #endif
        }
        else
        {
        #if defined (VDEC_IS_POST_MT5882)
			if(BSP_GetIcVersion() == IC_VER_5890_AC)
         	{
				vVDecWriteVDECMisc(0xCC, 0x646A11C8);
			}
			else
			{
				vVDecWriteVDECMisc(0xCC, 0x646A1108);
			}
        #elif defined(VDEC_IS_POST_MT5398)
        	vVDecWriteVDECMisc(0xCC, 0x246A11E8);
        #else
        	vVDecWriteVDECMisc(0xCC, 0x24EA91E8);
        #endif
        }
    #endif
}

#else
void _VDEC_VP6PowerDownSetting(void)
{
}
#endif

void _VDEC_VP6VLDHWReset(UINT32 u4Id)
{
#ifdef TEST_ONOFF_RUNTIME
    VDEC_INFO_T *prVdecInfo;
    prVdecInfo = _VDEC_GetInfo();
    prVdecInfo->arVdecVldInfo[0].fgSwrst = TRUE;
#endif

    vVDecWriteVLDEx(u4Id, WO_VLD_SRST, 0x101);
    _VDEC_CkgenSel(0, VDEC_FMT_VP6, 0);
    _VDEC_VP6PowerDownSetting();
    _VDEC_SysClkSel(0, VDEC_FMT_VP6, 0);
    vVDecWriteVLDEx(u4Id, WO_VLD_SRST, 0);
}

#if defined(CC_VP6_EMULATION) || defined(VDEC_DEC_REG_QUERY)
void _VDEC_VP6DumpChksumReg(void)
{
    UINT32 i, u4Val;
    _VDEC_VP6_LogSimPat(TRUE);
    
    Printf("\n***** Dump VLD Chksum Reg *****\n");

    VLD_READ32(72*4);
    VLD_READ32(73*4);
    VLD_READ32(74*4);
    VLD_READ32(75*4);

    Printf("\n***** Dump VLD2 Chksum Reg *****\n");
    VLD2_READ32(72*4);
    VLD2_READ32(73*4);
    VLD2_READ32(74*4);
    VLD2_READ32(75*4);

    Printf("\n***** Dump MC Chksum Reg *****\n");
    MC_READ32(378*4);
    MC_READ32(379*4);
    MC_READ32(380*4);
    MC_READ32(381*4);
    MC_READ32(382*4);
    MC_READ32(383*4);
    MC_READ32(384*4);
    MC_READ32(385*4);
    MC_READ32(388*4);
    MC_READ32(390*4);
    MC_READ32(391*4);
    MC_READ32(392*4);
    MC_READ32(393*4);
    MC_READ32(394*4);
    MC_READ32(395*4);
    MC_READ32(396*4);
    MC_READ32(397*4);
    MC_READ32(479*4);
    MC_READ32(480*4);
    MC_READ32(481*4);
    MC_READ32(483*4);
    MC_READ32(498*4);

    Printf("\n***** Dump AVC_MV Chksum Reg *****\n");
    VP6_MV_READ32(147*4);
    VP6_MV_READ32(148*4);
    VP6_MV_READ32(149*4);
    VP6_MV_READ32(153*4);
    VP6_MV_READ32(154*4);

    Printf("\n***** Dump VLD_TOP Chksum Reg *****\n");
    VLD_TOP_READ32(41*4);
    VLD_TOP_READ32(42*4);
    VLD_TOP_READ32(43*4);
    VLD_TOP_READ32(44*4);
    
    Printf("\n***** Dump VP6_VLD Chksum Reg *****\n");
    VP6_VLD_READ32(67*4);
    VP6_VLD_READ32(68*4);
    VP6_VLD_READ32(69*4);
    VP6_VLD_READ32(70*4);
    VP6_VLD_READ32(71*4);
    VP6_VLD_READ32(72*4);

    _VDEC_VP6_LogSimPat(FALSE);
    
    Printf("\n***** Dump IS Chksum Reg *****\n");
    for (i = 0; i < 13; i++)
    {
        u4Val = PP_READ32(0x200 + 4*i);
        Printf( "RISCRead_IS(4*%d, 32'h%x);\n", i, u4Val);
    }
    
    Printf("\n***** Dump IQ Chksum Reg *****\n");
    u4Val = PP_READ32(0x500 + 4*3);
    Printf( "RISCRead_IQ(4*%d, 32'h%x);\n", 3, u4Val);
    u4Val = PP_READ32(0x500 + 4*5);
    Printf( "RISCRead_IQ(4*%d, 32'h%x);\n", 5, u4Val);
    
    Printf("\n***** Dump DCAC Chksum Reg *****\n");
    u4Val = PP_READ32(0x700 + 4*2);
    Printf( "RISCRead_DCAC(4*%d, 32'h%x);\n", 2, u4Val);
    u4Val = PP_READ32(0x700 + 4*15);
    Printf( "RISCRead_DCAC(4*%d, 32'h%x);\n", 15, u4Val);
    
    Printf("\n***** Dump IT Chksum Reg *****\n");
    u4Val = PP_READ32(0x900 + 4*0);
    Printf( "RISCRead_IT(4*%d, 32'h%x);\n", 0, u4Val);
    u4Val = PP_READ32(0x900 + 4*1);
    Printf( "RISCRead_IT(4*%d, 32'h%x);\n", 1, u4Val);
}

void _VDEC_VP6DumpInputWnd()
{
    Printf("*** VLD0 Input Window 0x%X\n", u4VDecVP6VLDGetBits(0,0));
    Printf("*** VLD1 Input Window 0x%X\n", u4VDecVP6VLDGetBits(1,0));

    return ;
}
#endif

