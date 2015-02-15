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
#include "vdec_hal_if_h264.h"
#include "vdec_hw_h264.h"
#include "vdec_hal_errcode.h"

#ifdef CONFIG_TV_DRV_VFY 
#include "x_hal_5381.h"
#include "x_hal_926.h"
#else
#include "x_hal_ic.h"
#include "x_hal_1176.h"
#endif // CONFIG_TV_DRV_VFY

#include "x_debug.h"

//extern VDEC_INFO_VERIFY_FILE_INFO_T _tRecFileInfo;
//extern VDEC_INFO_H264_FBUF_INFO_T _ptFBufInfo[17];
//extern char _bFileStr1[9][300];

#if CONFIG_DRV_VERIFY_SUPPORT
#include "vdec_verify_general.h"
#include "vdec_verify_mpv_prov.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include "x_printf.h"
#endif

extern BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO);
#endif

const CHAR ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const CHAR ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

#ifdef MPV_DUMP_H264_DEC_REG
void VDec_DumpH264Reg(UCHAR ucMpvId);
#endif

#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
#define fgIsMVCDecode(arg)  (u4VDecReadAVCVLD(arg, RW_AVLD_RM03R) & MVC_SWITCH)
#endif
UINT32 B0List[100] = {0};
UINT32 B1List[100] = {0};

// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_InitVDecHW(UINT32 u4Handle);
// Description :Initialize video decoder hardware only for H264
// Parameter : u4VDecID : video decoder hardware ID
//                  prH264VDecInitPrm : pointer to VFIFO info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H264_InitVDecHW(UINT32 u4VDecID)
{
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]InitVDecHW Start\n");
#endif
	if ((1 == _u4DualCoreEnable[0]) && (u4VDecID == 0))
	{
		vVDecResetMultiCore(u4VDecID);
	}
	vVDecResetHW(u4VDecID, VDEC_FMT_H264);
#if DTV_COMMON_CONFIG
	//    vVDecWriteMC(u4VDecID,RW_MC_PP_DRAM_ACCESS,ACCESS_ENABLE);
	//    vVDecWriteMC(u4VDecID,RW_AMV_EC_SETTING,AVOID_HANG);
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	//   vVDecWriteVLDTOP(u4VDecID, RO_VLD_TOP_TIMEOUT_THD, VLD_TOP_TIMEOUT_THD);
#else
	vVDecWriteAVCVLD(u4VDecID, RO_AVLD_TIMEOUT_THD, AVLD_TIMEOUT_THD);
#endif
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]InitVDecHW Done\n");
#endif
	return  HAL_HANDLE_OK;
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read barrel shifter after shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window after shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H264_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
	UINT32 u4RegVal;

	u4RegVal = u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

	return (u4RegVal);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H264_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
	UINT32 u4RegVal0;

	u4RegVal0 = u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, 0);
	u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

	return (u4RegVal0);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Most significant (32 - u4ShiftBits) bits of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H264_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
	UINT32 u4RegVal0;

	u4RegVal0 = u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, 0);
	u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);

	return (u4RegVal0 >> (32-u4ShiftBits));
}


// **************************************************************************
// Function : UINT32 bVDEC_HAL_H264_GetBitStreamFlg(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : MSB of barrel shifter input window before shifting
// **************************************************************************
BOOL bVDEC_HAL_H264_GetBitStreamFlg(UINT32 u4BSID, UINT32 u4VDecID)
{
	UINT32 u4RegVal;

	u4RegVal = u4VDEC_HAL_H264_GetBitStreamShift(u4BSID, u4VDecID, 1);
	return ((u4RegVal >> 31));
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_UeCodeNum(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Do UE variable length decoding
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Input window after UE variable length decoding
// **************************************************************************
UINT32 u4VDEC_HAL_H264_UeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{
	if (u4BSID == 0)
	{
	    return (u4VDecReadAVCVLD(u4VDecID,RO_AVLD_UE));
	}
	else
	{
	    u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_BARL);
	    return (u4VDecReadAVCVLD(u4VDecID,RO_AVLD_2ND_UE));
	} 
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_SeCodeNum(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Do SE variable length decoding
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Input window after SE variable length decoding
// **************************************************************************
INT32 i4VDEC_HAL_H264_SeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{
	if (u4BSID == 0)
	{
	    return ((INT32)u4VDecReadAVCVLD(u4VDecID, RO_AVLD_SE));
	}
	else
	{
	    u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_BARL);
	    return ((INT32)u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_SE));
	}   
}


// *********************************************************************
// Function    : UINT32 u4VDEC_HAL_H264_GetStartCode(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get next start code
// Parameter   : u4BSID : Barrel shifter ID
//                   u4VDecID : VLD ID
// Return      : None
// *********************************************************************
UINT32 u4VDEC_HAL_H264_GetStartCode(UINT32 u4BSID, UINT32 u4VDecID)
{
	UINT32 u4Temp;
	BOOL fgVLDRem03;

	u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(u4BSID, u4VDecID, 0);
	if (u4BSID == 0)
	{
	    fgVLDRem03 = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) >> 31;
	}
	else
	{
	    fgVLDRem03 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_2ND_CTRL) & 0x40);
	}

	while (((u4Temp >> 8) != START_CODE) || fgVLDRem03)
	{
	    u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(u4BSID, u4VDecID, 8);
	    if (u4BSID == 0)
	    {
	        fgVLDRem03 = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) >> 31;
	    }
	    else
	    {
	        fgVLDRem03 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_2ND_CTRL) & 0x40);
	    }
	}
	u4Temp = u4VDEC_HAL_H264_GetBitStreamShift(u4BSID, u4VDecID, 32);
	return u4Temp;
}


// *********************************************************************
// Function    : UINT32 u4VDEC_HAL_H264_GetStartCode(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get next start code
// Parameter   : u4BSID : Barrel shifter ID
//                   u4VDecID : VLD ID
// Return      : None
// *********************************************************************
UINT32 u4VDEC_HAL_H264_GetStartCode_8530 (UINT32 u4BSID, UINT32 u4VDecID)
{	
	UINT32 u4Temp = 0;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
	BOOL fgVLDRem03;
	UINT32 u4RetryNum = 0x100000;
	UINT32 i;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]GetStartCode Start\n");
#endif
#if AVC_SEARCH_START_CODE_WR
	vVDecWriteAVCVLD(u4VDecID,(186 << 2),u4VDecReadAVCVLD(u4VDecID,(186 << 2)) & 0xffffefff);
#endif

	if (u4BSID == 0)
	{
	   u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(u4BSID, u4VDecID, 0);
	   if (u4BSID == 0)
	   {
	       fgVLDRem03 = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) >> 31;
	   }
	   else
	   {
	       fgVLDRem03 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_2ND_CTRL) & 0x40);
	    }

	   if (((u4Temp >> 8) != START_CODE) || fgVLDRem03)
	   {           
		   vVDecWriteAVCVLD(u4VDecID,  RW_AVLD_FSSR, FW_SEARCH_START_CODE);

		   for(i=0; i < u4RetryNum; i++)
		   {
		      if ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_FSSR) & 0x1) == 0)
		      {              
		           break;
		      }
		      x_thread_delay(1);
		   }

		    fgVLDRem03 = (u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R) >> 11) & 0x1;
		    if (i == u4RetryNum && !fgVLDRem03)
		   {
		       UTIL_Printf("Can not find AVC start code\n");
		       VDEC_ASSERT(0);
		    }
	 	}
	 }
	 else
	 {
	     VDEC_ASSERT(0);
	 }
	     
	 u4Temp = u4VDEC_HAL_H264_GetBitStreamShift(u4BSID, u4VDecID, 32);    
#endif    
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]GetStartCode End\n");
#endif
	return u4Temp;
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H264_BS_INIT_PRM_T *prH264BSInitPrm);
// Description :Initialize barrel shifter with byte alignment
// Parameter :u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 prH264BSInitPrm : pointer to h264 initialize barrel shifter information struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H264_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H264_BS_INIT_PRM_T *prH264BSInitPrm)
{
	BOOL fgInitBSResult;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]i4VDEC_HAL_H264_InitBarrelShifter Start\n");
#endif
	if (u4BSID == 0)
	{     
	    fgInitBSResult = fgInitH264BarrelShift1(u4VDecID, prH264BSInitPrm);
	}
	else
	{
	    fgInitBSResult = fgInitH264BarrelShift2(u4VDecID, prH264BSInitPrm);
	}
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]i4VDEC_HAL_H264_InitBarrelShifter Done\n");
#endif
	if (fgInitBSResult)
	{
	    return HAL_HANDLE_OK;
	}
	else
	{
	    return INIT_BARRELSHIFTER_FAIL;
	}
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits);
// Description :Read current read pointer
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 pu4Bits : read pointer value with remained bits
// Return      : Read pointer value with byte alignment
// **************************************************************************
UINT32 u4VDEC_HAL_H264_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFIFOSa, UINT32 *pu4Bits)
{
	return u4VDecReadH264VldRPtr(u4BSID, u4VDecID, pu4Bits, PHYSICAL(u4VFIFOSa));
}


// **************************************************************************
// Function : void v4VDEC_HAL_H264_AlignRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4AlignType);
// Description :Align read pointer to byte,word or double word
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4AlignType : read pointer align type
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_AlignRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4AlignType)
{
	return;
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H264_GetBitcount(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Read barrel shifter bitcount after initializing 
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Current bit count
// **************************************************************************
UINT32 u4VDEC_HAL_H264_GetBitcount(UINT32 u4BSID, UINT32 u4VDecID)
{
	return HAL_HANDLE_OK;
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_ScalingList(UINT32 u4BSID, UINT32 u4VDecID, CHAR *pcScalingList, UINT32 u4SizeOfScalingList, BOOL *pfgUseDefaultScalingMatrixFlag);
// Description :Decode scaling list
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4SizeOfScalingList : size of scaling list
//                 pcScalingList : pointer to value of scaling list
//                 pfgUseDefaultScalingMatrixFlag : pointer to  flag to use default scaling list or not
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_ScalingList(UINT32 u4BSID, UINT32 u4VDecID, CHAR *pcScalingList, UINT32 u4SizeOfScalingList, BOOL *pfgUseDefaultScalingMatrixFlag)
{
	UINT32 i;
	UINT32 u4Scanj;
	INT32 i4LastScale;
	INT32 i4NextScale;
	INT32 i4DeltaScale;

	i4LastScale = 8;
	i4NextScale = 8;

	for (i=0; i<u4SizeOfScalingList; i++)
	{
	    u4Scanj = (u4SizeOfScalingList==16) ? ZZ_SCAN[i]:ZZ_SCAN8[i];
	    
	    if (i4NextScale != 0)
	    {
	        i4DeltaScale = i4VDEC_HAL_H264_SeCodeNum(u4BSID, u4VDecID);
	        i4NextScale = (i4LastScale + i4DeltaScale + 256) % 256;
	        *pfgUseDefaultScalingMatrixFlag = ((u4Scanj == 0) && (i4NextScale == 0)) ? TRUE : FALSE;
	    }
	    pcScalingList[u4Scanj] = (i4NextScale == 0)? i4LastScale : i4NextScale;
	    i4LastScale = pcScalingList[u4Scanj];
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_WriteScalingList(UINT32 u4VDecID, UINT32 u4Idx, CHAR *pcSlicePtr);
// Description :Write scaling list to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 u4Idx : scaling list index
//                 pcSlicePtr : pointer to scaling list matrix
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_WriteScalingList(UINT32 u4VDecID, UINT32 u4Idx, CHAR *pcSlicePtr)
{
	INT32 i;
	UINT32 u4Temp;

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("==========2 pix mode===========\n");
#endif
	vVDecWritePP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN, (vVDecReadPP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN)| 0x1));
	if (u4Idx < 6)
	{
		for (i=0; i<16; i=i+2)
		{
			u4Temp = ((UCHAR)pcSlicePtr[i+1] << 8) + (UCHAR)pcSlicePtr[i];
			vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, (u4Idx << 4) + i);
			vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
			//vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + (u4Idx << 4) + i);
			}
		}
	else
	{
		for (i=0; i<64; i=i+2)
		{
			u4Temp = ((UCHAR)pcSlicePtr[i+1] << 8) + (UCHAR)pcSlicePtr[i];
				vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 96 + ((u4Idx - 6) << 6) + i);
			vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
			//vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + (u4Idx << 4) + i);
		}
	}
	vVDecWritePP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN, (vVDecReadPP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN)& 0xfffffffe));
#else	
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("==========1 pix mode===========\n");
#endif
	if (u4Idx < 6)
	{
	    u4Idx = (u4Idx << 4);
	    for(i=0; i<4; i++)
	    {
	        // add 16 for every list
	        u4Temp = (((UCHAR)pcSlicePtr[i<<2]) << 24) + (((UCHAR)pcSlicePtr[(i<<2) + 1]) << 16) +(((UCHAR)pcSlicePtr[(i<<2) + 2]) << 8) + (((UCHAR)pcSlicePtr[(i<<2) + 3]));
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x200 + u4Idx + (i << 2));
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + u4Idx + (i << 2));
	    }
	}
	else
	{
	    u4Idx = (u4Idx == 6) ? (u4Idx << 4) : ((u4Idx + 3) << 4); // 6=>16*6   7=>16*6+64(equal to 16*7+48)
	    for (i=0; i<16; i++)
	    {
	        // add 64 for every list
	        u4Temp = (((UCHAR)pcSlicePtr[i<<2]) << 24) + (((UCHAR)pcSlicePtr[(i<<2) + 1]) << 16) +(((UCHAR)pcSlicePtr[(i<<2) + 2]) << 8) + (((UCHAR)pcSlicePtr[(i<<2) + 3]));
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x200 + u4Idx + (i << 2));
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
	        vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + u4Idx + (i << 2));
	    }
	}
#endif
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_Reording(UINT32 u4VDecID);
// Description :Reference list reordering
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_Reording(UINT32 u4VDecID)
{
	UINT32 u4Cnt;

	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RPL_REORD, 1);
	u4Cnt = 0;
	while(1)
	{
	    if (u4Cnt == 100)
	    {
	        if (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_RPL_REORD))
	        {
	            break;
	        }
	        else
	        {
	            u4Cnt = 0;        
	        }
	    }
	    else
	    {
	        u4Cnt ++;
	    }
	}
}

// **************************************************************************
// Function : void vVDEC_HAL_H264_PredWeightTable(UINT32 u4VDecID);
// Description :Decode prediction weighting table
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_PredWeightTable(UINT32 u4VDecID)
{
	UINT32 u4Cnt;

	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_WEIGHT_PRED_TBL, 1);
	u4Cnt = 0;
	while(1)
	{
	    if (u4Cnt == 100)
	    {
	        if (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_WEIGHT_PRED_TBL))
	        {
	            break;
	        }
	        else
	        {
	            u4Cnt = 0;        
	        }
	    }
	    else
	    {
	        u4Cnt ++;
	    }
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_TrailingBits(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Remove traling bits to byte align
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_TrailingBits(UINT32 u4BSID, UINT32 u4VDecID)
{
	UINT32 u4Temp;

	u4Temp = 8 - (u4VDecAVCVLDShiftBits(u4BSID, u4VDecID) % 8);
	// at list trailing bit
	if (u4Temp < 8)
	{
	    u4Temp = u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, u4Temp);
	}
}


// **************************************************************************
// Function : BOOL bVDEC_HAL_H264_IsMoreRbspData(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Check whether there is more rbsp data
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Is morw Rbsp data or not
// **************************************************************************
BOOL bVDEC_HAL_H264_IsMoreRbspData(UINT32 u4BSID, UINT32 u4VDecID)
{
	UINT32 u4RemainedBits;
	UINT32 u4Temp;
	INT32 i;

	u4RemainedBits = (u4VDecAVCVLDShiftBits(u4BSID, u4VDecID) % 8); //0~7
	//u4RemainedBits = (8 - (((u4VDecReadAVCVLD(RW_AVLD_CTRL) >> 16) & 0x3F) % 8));  
	u4Temp = 0xffffffff;
	for (i=0; i<=u4RemainedBits; i++)
	{
	    u4Temp &= (~(1<<i));
	}

	if ((u4VDecAVCVLDGetBitS(u4BSID, u4VDecID, 0) & u4Temp) == (0x80000000))
	{
	    // no more
	    return FALSE;
	}
	else
	{
	    return TRUE;
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_InitPRefList(UINT32 u4VDecID, BOOL fgPicFrm, UINT32 u4MaxFrameNum, UINT32 u4CurrPicNum);
// Description :Set HW registers to initialize P reference list
// Parameter : u4VDecID : video decoder hardware ID
//                 fgPicFrm : flag of frame picture or not
//                 u4MaxFrameNum : maximium frame number
//                 u4CurrPicNum : current pic number
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_InitPRefList(UINT32 u4VDecID, BOOL fgPicFrm, UINT32 u4MaxFrameNum, UINT32 u4CurrPicNum)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
	int i;
#endif

	vVDecWriteMC(u4VDecID, RW_AMC_P_LIST0_FLD, 0);
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RESET_PIC_NUM, RESET_PIC_NUM);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)    
	for(i= 0; i<10000; i++)
	{
	     int a = 0;
	     if ( (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_RESET_PIC_NUM) & 0x1) == 0)
	     {
	        break;
	     }            

	     for (i = 0; i < 1000; i++)
	     {
	        a += 100;
	     }
	}

	if (i == 10000)
	{
	    UTIL_Printf("[VDEC][8550_AVC]: RW_AVLD_RESET_PIC_NUM: Pooling FAIL!!!\n");
	}
#endif
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MAX_PIC_NUM, fgPicFrm ? u4MaxFrameNum : (u4MaxFrameNum << 1));
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CUR_PIC_NUM, u4CurrPicNum);
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_SetPRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo);
// Description :Set HW registers related with P reference list
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetPRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	//UCHAR bRefPicIdx;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;
	BOOL fgLRefPic;
	UINT32 u4Param = 0;
	BOOL fgIsUFOMode;

	fgIsUFOMode = _tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH264DecPrm.fgIsUFOMode;
	ucFld = (prPRefPicListInfo->u4FBufInfo & 0xff);  
	//bRefPicIdx = ((prPRefPicListInfo->u4FBufInfo >> 8) & 0xff);  
	ucRegIdx = ((prPRefPicListInfo->u4FBufInfo >> 16) & 0xff);  

	ucFBufIdx = prPRefPicListInfo->ucFBufIdx;
	fgLRefPic = (prPRefPicListInfo->u4ListIdx > 3)? TRUE: FALSE;

	if (ucFld == FRAME)
	{    
		if (fgIsUFOMode)
		{
			vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prPRefPicListInfo->u4YLenOffset);
			vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prPRefPicListInfo->u4CLenOffset);
		}
		#ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
		vVDecWriteMC(u4VDecID, (728 << 2) + (ucRegIdx <<2), u4AbsDramANc((UINT32)prPRefPicListInfo->u4CStartAddr));		
		#else
	    //bRegIdx = bRefPicIdx;
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)  prPRefPicListInfo->u4FBufYStartAddr));
#else
	    vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), u4AbsDramANc((UINT32)  prPRefPicListInfo->u4FBufYStartAddr));    
#endif
		#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	    if(fgIsMVCDecode(u4VDecID))
	    {
	        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4PicNum & 0x7ffff)));
	        u4Param = (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4PicNum & 0x7ffff));
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	        //UTIL_Printf("SetPRefPicList1: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	    }
	    else
#endif            
	    {
	        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4PicNum & 0x7ffff)));
	        u4Param = (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4PicNum & 0x7ffff));
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	        //UTIL_Printf("SetPRefPicList2: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	    }

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp |= (fgLRefPic << 20);    
	    u4Temp |= ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<3), u4Temp);

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp |= (fgLRefPic << 20);    
	    u4Temp |= ((((ucFBufIdx)<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);
	}
	else
	{ 
	    if (ucFld == TOP_FIELD)
	    {
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prPRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prPRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, (728 << 2) + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	            u4Param = (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4TFldPicNum & 0x7ffff));
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	            //UTIL_Printf("SetPRefPicList3: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	        }
	        else
#endif            
	        {
	            //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	            u4Param = (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4TFldPicNum & 0x7ffff));
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	             //UTIL_Printf("SetPRefPicList4: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	        }
	        
	        //u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	        u4Temp = (fgLRefPic << 20);      
	        u4Temp |= ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara;
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<2), u4Temp);
	    }
	    else
	    {      
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prPRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prPRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, (728 << 2) + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	            u4Param = (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prPRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4BFldPicNum & 0x7ffff));
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	             //UTIL_Printf("SetPRefPicList4: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	        }
	        else
#endif            
	        {
	            //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	            u4Param = (fgLRefPic << 19) + (fgLRefPic? (prPRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prPRefPicListInfo->i4BFldPicNum & 0x7ffff));
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	             //UTIL_Printf("SetPRefPicList4: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);
	        }
	        
	        //u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	        u4Temp = (fgLRefPic << 20);      
	        u4Temp |= (((ucFBufIdx<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara;
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<2), u4Temp);
	        
	        vVDecWriteMC(u4VDecID, RW_AMC_P_LIST0_FLD, u4VDecReadMC(u4VDecID, RW_AMC_P_LIST0_FLD) | (0x1 << ucRegIdx));        
	    }     
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_SetPOC(UINT32 u4VDecID, VDEC_INFO_H264_POC_PRM_T *prPOCInfo);
// Description :Set POC number to HW registers
// Parameter : u4VDecID : video decoder hardware ID
//                 prPOCInfo : pointer to information of current POC
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetPOC(UINT32 u4VDecID, VDEC_INFO_H264_POC_PRM_T *prPOCInfo)
{
	INT32 i4CurrPOC;

	if (prPOCInfo->fgIsFrmPic)
	{
	    i4CurrPOC = prPOCInfo->i4POC;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_TFLD_POC, prPOCInfo->i4TFldPOC & 0x3ffff);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_BFLD_POC, prPOCInfo->i4BFldPOC & 0x3ffff);  
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_POC, prPOCInfo->i4POC & 0x3ffff);
	}
	else
	{
	    i4CurrPOC = (prPOCInfo->ucPicStruct == TOP_FIELD)? prPOCInfo->i4TFldPOC : prPOCInfo->i4BFldPOC;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_POC, i4CurrPOC & 0x3ffff);
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_InitBRefList(UINT32 u4VDecID);
// Description :Set HW registers to initialize B reference list
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_InitBRefList(UINT32 u4VDecID)
{
	vVDecWriteMC(u4VDecID, RW_AMC_B_LIST0_FLD, 0);
	vVDecWriteMC(u4VDecID, RW_AMC_B_LIST1_FLD, 0); 
}


// **************************************************************************
// Function : BOOL bVDEC_HAL_H264_LAE_SetBRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_B_REF_PRM_T *prBRefPicListInfo);
// Description :Set LAE HW registers related with B reference list
// Parameter : u4VDecID : video decoder hardware ID
//                 prBRefPicListInfo : pointer to information of b reference list
// Return      : None
// **************************************************************************
BOOL bVDEC_HAL_H264_LAE_SetBRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_B_REF_PRM_T *prBRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	//UCHAR bRefPicIdx;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;
	BOOL fgLRefPic;  
	
#if (CONFIG_DRV_VERIFY_SUPPORT) && (!VDEC_DRV_PARSER)
	VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *) &(_tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH264DecPrm);
#if (!VDEC_H264_REDUCE_MV_BUFF)
	prH264DecPrm->prCurrFBufInfo->u4MvStartAddr = (prH264DecPrm->prCurrFBufInfo->u4YStartAddr + ((prH264DecPrm->prCurrFBufInfo->u4DramPicSize * 3) >>1));
#endif
#else
	//VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
	ucFld = (prBRefPicListInfo->u4FBufInfo & 0xff);  
	//bRefPicIdx = ((prBRefPicListInfo->u4FBufInfo >> 8) & 0xff);  
	ucRegIdx = ((prBRefPicListInfo->u4FBufInfo >> 16) & 0xff);	
	ucFBufIdx = prBRefPicListInfo->ucFBufIdx;
	fgLRefPic = (prBRefPicListInfo->u4ListIdx > 3)? TRUE: FALSE;

	if (ucFld == FRAME)
	{
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4PicNum & 0x7ffff)));
		
		u4Temp = (((prBRefPicListInfo->i4TFldPOC <= prBRefPicListInfo->i4BFldPOC) && (prBRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
		u4Temp |= (fgLRefPic << 20);
		u4Temp += ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
		B0List[(ucRegIdx<<3)] = u4Temp;
		u4Temp = (((prBRefPicListInfo->i4TFldPOC <= prBRefPicListInfo->i4BFldPOC) && (prBRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
		u4Temp |= (fgLRefPic << 20);
		u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
		B0List[(ucRegIdx<<3)+4] = u4Temp;

		// B_1
		ucFBufIdx = prBRefPicListInfo->ucFBufIdx1;
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum1 & 0x7ffff) : (prBRefPicListInfo->i4PicNum1 & 0x7ffff)));
		u4Temp = (((prBRefPicListInfo->i4TFldPOC1 <= prBRefPicListInfo->i4BFldPOC1) && (prBRefPicListInfo->i4TFldPOC1 != 0x7fffffff))? 1: 0) << 21;
		u4Temp |= (fgLRefPic << 20);
		u4Temp += ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara1 + (prBRefPicListInfo->i4TFldPOC1 & 0x3ffff);
		B1List[(ucRegIdx<<3)] = u4Temp;
		u4Temp = (((prBRefPicListInfo->i4TFldPOC1 <= prBRefPicListInfo->i4BFldPOC1) && (prBRefPicListInfo->i4TFldPOC1 != 0x7fffffff))? 1: 0) << 21;
		u4Temp |= (fgLRefPic << 20);
		u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara1 + (prBRefPicListInfo->i4BFldPOC1 & 0x3ffff);
		B1List[(ucRegIdx<<3)+4] = u4Temp;
	}
	else
	{
	// B_0
		if ((prBRefPicListInfo->u4ListIdx == 0) || (prBRefPicListInfo->u4ListIdx == 1) || (prBRefPicListInfo->u4ListIdx == 4) || (prBRefPicListInfo->u4ListIdx == 5))
		{ 	 
		//u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
			if (ucFld == TOP_FIELD)
			{
				vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff)));
			  	u4Temp = ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
			  	u4Temp |= (fgLRefPic << 20);
			  	B0List[(ucRegIdx<<2)] = u4Temp;
			}
			else
			{		 
				vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
			  	u4Temp = (((ucFBufIdx<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
			  	u4Temp |= (fgLRefPic << 20);
			  	B0List[(ucRegIdx<<2)] = u4Temp;		 
			}
		}
		// B_1	
		if ((prBRefPicListInfo->u4ListIdx == 2) || (prBRefPicListInfo->u4ListIdx == 3) || (prBRefPicListInfo->u4ListIdx == 4) || (prBRefPicListInfo->u4ListIdx == 5))
		{ 	 
		//u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
			if (ucFld == TOP_FIELD)
			{
				vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff))); 	 
			  	u4Temp = ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
			  	u4Temp |= (fgLRefPic << 20);
			  	B1List[(ucRegIdx<<2)] = u4Temp;
			}
			else
			{			
				vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
			  	u4Temp = (((ucFBufIdx<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
			  	u4Temp |= (fgLRefPic << 20);
				B1List[(ucRegIdx<<2)] = u4Temp;
			}
		}
	}
	if (B0List[(ucRegIdx<<2)] != B1List[(ucRegIdx<<2)])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// **************************************************************************
// Function : BOOL bVDEC_HAL_H264_SetBRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_B_REF_PRM_T *prBRefPicListInfo);
// Description :Set HW registers related with B reference list
// Parameter : u4VDecID : video decoder hardware ID
//                 prBRefPicListInfo : pointer to information of b reference list
// Return      : None
// **************************************************************************
BOOL bVDEC_HAL_H264_SetBRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_B_REF_PRM_T *prBRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	//UCHAR bRefPicIdx;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;
	BOOL fgLRefPic;  
	BOOL fgIsUFOMode;
	UINT32 u4FieldDistance = 4;
	
	fgIsUFOMode = _tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH264DecPrm.fgIsUFOMode;
#if (CONFIG_DRV_VERIFY_SUPPORT) && (!VDEC_DRV_PARSER)
	VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *) &(_tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH264DecPrm);
#if (!VDEC_H264_REDUCE_MV_BUFF)
	prH264DecPrm->prCurrFBufInfo->u4MvStartAddr = (prH264DecPrm->prCurrFBufInfo->u4YStartAddr + ((prH264DecPrm->prCurrFBufInfo->u4DramPicSize * 3) >>1));
#endif
#else
	//VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
	ucFld = (prBRefPicListInfo->u4FBufInfo & 0xff);  
	//bRefPicIdx = ((prBRefPicListInfo->u4FBufInfo >> 8) & 0xff);  
	ucRegIdx = ((prBRefPicListInfo->u4FBufInfo >> 16) & 0xff);  
	ucFBufIdx = prBRefPicListInfo->ucFBufIdx;
	fgLRefPic = (prBRefPicListInfo->u4ListIdx > 3)? TRUE: FALSE;

#if (CONFIG_DRV_VERIFY_SUPPORT)
#if VDEC_H264_REDUCE_MV_BUFF
	 if (prH264DecPrm->fgIsReduceMVBuffer)
	 {
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_REDUCE_BMV,  (u4VDecReadAVCMV(u4VDecID, RW_AMV_REDUCE_BMV) | EN_AMV_REDUCE_BMV ));
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
		u4FieldDistance = 2;
#else
	    u4FieldDistance= 1;
#endif
	 }
#endif
#else//Driver flow
#if VDEC_DRV_MV_BUFF_REDUCE
	  if(prBRefPicListInfo->fgMVBufferReduce)
	  {
		vVDecWriteAVCMV(u4VDecID, RW_AMV_REDUCE_BMV,  (u4VDecReadAVCMV(u4VDecID, RW_AMV_REDUCE_BMV) | EN_AMV_REDUCE_BMV ));
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
			u4FieldDistance = 2;
#else
		   u4FieldDistance= 1;
#endif
	  }
#else
	  //do nothing
#endif
#endif

	if (ucFld == FRAME)
	{
		if (fgIsUFOMode)
		{
			vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset);
			vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset);
		}
		#ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr));
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr));
		#else
	    // B_0
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#else
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#endif
		#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	    if(fgIsMVCDecode(u4VDecID))
	    {
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4PicNum & 0x7ffff)));        
	    }
	    else
#endif            
	    {
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4PicNum & 0x7ffff)));
	    }
	    u4Temp = (((prBRefPicListInfo->i4TFldPOC <= prBRefPicListInfo->i4BFldPOC) && (prBRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
	    u4Temp |= (fgLRefPic << 20);
	    u4Temp += ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
		vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<3), u4Temp);
	    u4Temp = (((prBRefPicListInfo->i4TFldPOC <= prBRefPicListInfo->i4BFldPOC) && (prBRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
	    u4Temp |= (fgLRefPic << 20);
	    u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
		vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);


	    // B_1
	    ucFBufIdx = prBRefPicListInfo->ucFBufIdx1;
		if (fgIsUFOMode)
		{
			vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset1);
			vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset1);
		}
		#ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr1));
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr1));
		#else
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr1));    
#else
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr1));    
#endif
		#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	    if(fgIsMVCDecode(u4VDecID))
	    {
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum1 & 0x7ffff) : (prBRefPicListInfo->i4PicNum1 & 0x7ffff)));        
	    }
	    else
#endif            
	    {
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4LongTermPicNum1 & 0x7ffff) : (prBRefPicListInfo->i4PicNum1 & 0x7ffff)));
	    }
	    u4Temp = (((prBRefPicListInfo->i4TFldPOC1 <= prBRefPicListInfo->i4BFldPOC1) && (prBRefPicListInfo->i4TFldPOC1 != 0x7fffffff))? 1: 0) << 21;
	    u4Temp |= (fgLRefPic << 20);
	    u4Temp += ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara1 + (prBRefPicListInfo->i4TFldPOC1 & 0x3ffff);
		vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<3), u4Temp);
	    u4Temp = (((prBRefPicListInfo->i4TFldPOC1 <= prBRefPicListInfo->i4BFldPOC1) && (prBRefPicListInfo->i4TFldPOC1 != 0x7fffffff))? 1: 0) << 21;
	    u4Temp |= (fgLRefPic << 20);
	    u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara1 + (prBRefPicListInfo->i4BFldPOC1 & 0x3ffff);
		vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);

#if CONFIG_DRV_VIRTUAL_ADDR
	    u4Temp = PHYSICAL((UINT32) prBRefPicListInfo->u4FBufMvStartAddr1);
#else
	    u4Temp = u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufMvStartAddr1);
#endif
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<3), u4Temp >> 4);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<3) + 4, (u4Temp >> 4) + u4FieldDistance);
	}
	else
	{
	  // B_0
	  if ((prBRefPicListInfo->u4ListIdx == 0) || (prBRefPicListInfo->u4ListIdx == 1) || (prBRefPicListInfo->u4ListIdx == 4) || (prBRefPicListInfo->u4ListIdx == 5))
	  {      
	    //u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
	    if (ucFld == TOP_FIELD)
	    {
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#else
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	        }
	        else
#endif            
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	        }
	      u4Temp = ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
	      u4Temp |= (fgLRefPic << 20);
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<2), u4Temp);
	    }
	    else
	    {        
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#else
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	        }
	        else
#endif            
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	        }
	      u4Temp = (((ucFBufIdx<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
	      u4Temp |= (fgLRefPic << 20);
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<2), u4Temp);
	      
	      vVDecWriteMC(u4VDecID, RW_AMC_B_LIST0_FLD, u4VDecReadMC(u4VDecID, RW_AMC_B_LIST0_FLD) | (0x1 << ucRegIdx));        
	    }
	  }
	  // B_1    
	  if ((prBRefPicListInfo->u4ListIdx == 2) || (prBRefPicListInfo->u4ListIdx == 3) || (prBRefPicListInfo->u4ListIdx == 4) || (prBRefPicListInfo->u4ListIdx == 5))
	  {      
	    //u4Temp = (((_ptFBufInfo[ucFBufIdx].i4TFldPOC <= _ptFBufInfo[ucFBufIdx].i4BFldPOC) && (_ptFBufInfo[ucFBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21;
	    if (ucFld == TOP_FIELD)
	    {
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#else
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) +(fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	        }
	        else
#endif            
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4TFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4TFldPicNum & 0x7ffff)));
	        }
#if CONFIG_DRV_VIRTUAL_ADDR
	      u4Temp = PHYSICAL((UINT32) prBRefPicListInfo->u4FBufMvStartAddr);
#else
	      u4Temp = u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufMvStartAddr);
#endif
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<2), (u4Temp >> 4));      
	      u4Temp = ((ucFBufIdx)<<1<<22) + prBRefPicListInfo->u4TFldPara + (prBRefPicListInfo->i4TFldPOC & 0x3ffff);
	      u4Temp |= (fgLRefPic << 20);
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<2), u4Temp);
	    }
	    else
	    {
	    	if (fgIsUFOMode)
			{
				vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prBRefPicListInfo->u4YLenOffset);
				vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prBRefPicListInfo->u4CLenOffset);
			}
			#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prBRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#else
	      	vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	        if(fgIsMVCDecode(u4VDecID))
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) +(fgLRefPic << 20) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	        }
	        else
#endif            
	        {
	            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (fgLRefPic << 19) + (fgLRefPic? (prBRefPicListInfo->i4BFldLongTermPicNum & 0x7ffff) : (prBRefPicListInfo->i4BFldPicNum & 0x7ffff)));
	        }

#if CONFIG_DRV_VIRTUAL_ADDR
	      u4Temp = PHYSICAL((UINT32) prBRefPicListInfo->u4FBufMvStartAddr);
#else  
	      u4Temp = u4AbsDramANc((UINT32) prBRefPicListInfo->u4FBufMvStartAddr);
#endif
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<2), (u4Temp >> 4) + u4FieldDistance);
	      u4Temp = (((ucFBufIdx<<1) + 1)<<22) + prBRefPicListInfo->u4BFldPara + (prBRefPicListInfo->i4BFldPOC & 0x3ffff);
	      u4Temp |= (fgLRefPic << 20);
	      vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<2), u4Temp);

	      vVDecWriteMC(u4VDecID, RW_AMC_B_LIST1_FLD, u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD) | (0x1 << ucRegIdx));        
	    }
	  }
	}
	if (u4VDecReadAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<2)) != u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<2)))
	{
	  return TRUE;
	}
	else
	{
	  return FALSE;
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_B1ListSwap(UINT32 u4VDecID, BOOL fgIsFrmPic);
// Description :Swap B1 reference list1 
// Parameter : u4VDecID : video decoder hardware ID
//                 fgIsFrmPic : flag to frame picture or not
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_B1ListSwap(UINT32 u4VDecID, BOOL fgIsFrmPic)
{
	UINT32 u4Temp;

	if (fgIsFrmPic)
	{
	    u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + 4, u4Temp);  
		#ifdef Y_C_SEPERATE
	    u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR);
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4));
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4, u4Temp); 
		#endif
		
	    u4Temp = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL);    
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4));
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4, u4Temp);        
	      
	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 8));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 8, u4Temp);        
	      
	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 12));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 12, u4Temp); 
	      
	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 8));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 8, u4Temp);        
	    
	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 4);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 4, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 12));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 12, u4Temp); 
	}
	else
	{
	    u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + 4, u4Temp);        
		#ifdef Y_C_SEPERATE
	    u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR);
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4));
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4, u4Temp); 
		#endif
	      
	    u4Temp = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL);    
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4));
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4, u4Temp);        
	      
	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4, u4Temp);        

	    u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 4));
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + 4, u4Temp);        
	 
	    u4Temp = u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD) & 1; // bit 0
	    // Write bit 1 to bit 0
	    vVDecWriteMC(u4VDecID, RW_AMC_B_LIST1_FLD,  (u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD) & (~1)) | ((u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD)  >> 1) & 1)); 
	    vVDecWriteMC(u4VDecID, RW_AMC_B_LIST1_FLD,  (u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD) & (~2)) | (u4Temp << 1)); 
	}
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_SetSPSAVLD(UINT32 u4VDecID, VDEC_INFO_H264_SPS_T *prSPS);
// Description :Set SPS data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prSPS : pointer to sequence parameter set struct
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetSPSAVLD(UINT32 u4VDecID, VDEC_INFO_H264_SPS_T *prSPS)
{
	UINT32 u4SPSInfo;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetSPSAVLD start\n");
#endif
	u4SPSInfo = (prSPS->u4ChromaFormatIdc & 0x3); // 1~0
	u4SPSInfo |= ((prSPS->u4Log2MaxFrameNumMinus4 & 0xf)<< 2); //5~2
	u4SPSInfo |= ((prSPS->u4PicOrderCntType & 0x3) << 6); //7~6
	u4SPSInfo |= ((prSPS->u4Log2MaxPicOrderCntLsbMinus4 & 0xf) << 8);
	u4SPSInfo |= (prSPS->fgDeltaPicOrderAlwaysZeroFlag << 12);
	u4SPSInfo |= ((prSPS->u4NumRefFrames & 0x1f) << 13);
	u4SPSInfo |= (prSPS->fgFrameMbsOnlyFlag << 18);
	u4SPSInfo |= (prSPS->fgMbAdaptiveFrameFieldFlag << 19);  
	u4SPSInfo |= (prSPS->fgDirect8x8InferenceFlag << 20);    
	u4SPSInfo |= (1 << 21);    
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SPS, u4SPSInfo);
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_PIC_MB_SIZE_M1, ((prSPS->u4PicHeightInMapUnitsMinus1) << VLD_TOP_PIC_HEIGHT_IN_MBS_POS) | prSPS->u4PicWidthInMbsMinus1);  
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PIC_SIZE, ((prSPS->u4PicHeightInMapUnitsMinus1) << AVLD_PIC_HEIGHT_IN_MBS_POS) | prSPS->u4PicWidthInMbsMinus1);  
#endif

#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetSPSAVLD end\n");
#endif
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_SetPPSAVLD(UINT32 u4VDecID, BOOL fgUserScalingMatrixPresentFlag,
//    BOOL *fgUserScalingListPresentFlag, VDEC_INFO_H264_PPS_T *prVDecH264PPS);
// Description :Set PPS data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prPPS : pointer to picture parameter set struct
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetPPSAVLD(UINT32 u4VDecID, BOOL fgUserScalingMatrixPresentFlag,
BOOL *pfgUserScalingListPresentFlag, VDEC_INFO_H264_PPS_T *prVDecH264PPS)
{
	UINT32 u4PPSInfo;
	INT32 i;

#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetPPSAVLD start\n");
#endif
	u4PPSInfo = prVDecH264PPS->fgEntropyCodingModeFlag;
	u4PPSInfo |= (prVDecH264PPS->fgPicOrderPresentFlag << 1);
	u4PPSInfo |= (prVDecH264PPS->fgWeightedPredFlag << 2);
	u4PPSInfo |= ((prVDecH264PPS->u4WeightedBipredIdc & 0x03) << 3);  
	u4PPSInfo |= ((prVDecH264PPS->i4PicInitQpMinus26 & 0x3f) << 5);  
	u4PPSInfo |= ((prVDecH264PPS->i4ChromaQpIndexOffset & 0x1f)<< 11);    
	u4PPSInfo |= (prVDecH264PPS->fgDeblockingFilterControlPresentFlag << 16);
	u4PPSInfo |= (prVDecH264PPS->fgConstrainedIntraPredFlag << 17);
	u4PPSInfo |= (prVDecH264PPS->fgTransform8x8ModeFlag << 18);  
	u4PPSInfo |= ((prVDecH264PPS->i4SecondChromaQpIndexOffset & 0x1f) << 19);  

	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PPS_1 , u4PPSInfo);

	u4PPSInfo = prVDecH264PPS->u4NumRefIdxL0ActiveMinus1;
	u4PPSInfo |= (prVDecH264PPS->u4NumRefIdxL1ActiveMinus1 << 5);
	for (i=0; i<8; i++)
	{
	  u4PPSInfo |= (pfgUserScalingListPresentFlag[i] << (10 + i));  
	}
	u4PPSInfo |= (fgUserScalingMatrixPresentFlag << 18);  

	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PPS_2 , u4PPSInfo);

#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetPPSAVLD end\n");
#endif
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_SetSHDRAVLD1(UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr);
// Description :Set part of slice header data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prSliceHdr : pointer to slice parameter set struct
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetSHDRAVLD1(UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr)
{
	UINT32 u4SHDRInfo;

#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetSHDRAVLD1 start\n");
#endif
	u4SHDRInfo = prSliceHdr->u4FirstMbInSlice & 0x1fff;
	u4SHDRInfo |= ((prSliceHdr->u4SliceType & 0xf) << 13);
	u4SHDRInfo |= (prSliceHdr->fgFieldPicFlag << 17);
	u4SHDRInfo |= (prSliceHdr->fgBottomFieldFlag << 18);
	u4SHDRInfo |= (prSliceHdr->fgDirectSpatialMvPredFlag << 19);
	u4SHDRInfo |= ((prSliceHdr->u4NumRefIdxL0ActiveMinus1 & 0x1f) << 20);
	u4SHDRInfo |= ((prSliceHdr->u4NumRefIdxL1ActiveMinus1 & 0x1f) << 25);  

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PIC_SIZE, (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_PIC_SIZE) &(~0x1FFFF)) | (u4SHDRInfo&0x1FFF));  
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1, (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_SHDR_1) &(~0xFFFFE000)) | (u4SHDRInfo&0xFFFFE000));  
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1, u4SHDRInfo);  
#endif

#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_SetSHDRAVLD1 end\n");
#endif
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_FGTSetting(UINT32 u4VDecID, VDEC_INFO_H264_FGT_PRM_T *prFGTPrm);
// Description :Set film gram hardware registers
// Parameter : prFGTPrm : pointer to H264 film gram info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H264_FGTSetting(UINT32 u4VDecID, VDEC_INFO_H264_FGT_PRM_T *prFGTPrm)
{
  // Trun off then on to reset FGT module
  //vVDecWriteAVCFG(RW_FGT_MODE, FGT_SCR_PP);

    vVDecWriteAVCFG(u4VDecID, RW_FGT_MODE, 0);
    vVDecWriteAVCFG(u4VDecID, RW_FGT_MODE, prFGTPrm->ucDataScr);

    if (prFGTPrm->ucDataScr)
    {
        vVDecWriteAVCFG(u4VDecID, RW_FGT_DRAM_CTRL, 0x0100); 
#if CONFIG_DRV_VIRTUAL_ADDR
        vVDecWriteAVCFG(u4VDecID, RW_FGT_OUT_Y_ADDR, PHYSICAL((UINT32) prFGTPrm->pucFGTTrgYAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_OUT_C_ADDR, PHYSICAL((UINT32) prFGTPrm->pucFGTTrgCAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_IN_Y_ADDR, PHYSICAL((UINT32) prFGTPrm->pucFGTScrYAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_IN_C_ADDR, PHYSICAL((UINT32) prFGTPrm->pucFGTScrCAddr)>>4);  
#else
        vVDecWriteAVCFG(u4VDecID, RW_FGT_OUT_Y_ADDR, u4AbsDramANc((UINT32) prFGTPrm->pucFGTTrgYAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_OUT_C_ADDR, u4AbsDramANc((UINT32) prFGTPrm->pucFGTTrgCAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_IN_Y_ADDR, u4AbsDramANc((UINT32) prFGTPrm->pucFGTScrYAddr)>>4);
        vVDecWriteAVCFG(u4VDecID, RW_FGT_IN_C_ADDR, u4AbsDramANc((UINT32) prFGTPrm->pucFGTScrCAddr)>>4);  
#endif
        vVDecWriteAVCFG(u4VDecID, RW_FGT_MB_SIZE, (prFGTPrm->ucMBYSize << 8) +prFGTPrm->ucMBXSize);    
        vVDecWriteAVCFG(u4VDecID, RW_FGT_SEI_CTRL_A, prFGTPrm->u4Ctrl);
    }
    return HAL_HANDLE_OK;
}

extern int vVDecDumpBuf( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout);

// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_LAEDecStart(UINT32 u4VDecID, VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm);
// Description :Set video decoder LAE hardware registers to decode for H264
// Parameter : ptHalDecH264Info : pointer to H264 decode info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H264_LAEDecStart(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
	UINT32 u4SHDRInfo;
		
#if (!CONFIG_DRV_VERIFY_SUPPORT)
	UINT32	aAVCVLD84 = 0;
#endif    

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	UINT32 u4total_mbs_in_pic = 0;
#endif
	VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecH264DecPrm);	
#if (CONFIG_DRV_VERIFY_SUPPORT)
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_BUSY_THRESHOLD, 0xFFFFFFFF);
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MC_BUSY_THRESHOLD, 0x1000);
#endif
#else
		// set MC wait timeout threshold
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_BUSY_THRESHOLD, 0xFFFFFFFF);
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MC_BUSY_THRESHOLD, 0x8000);
#endif
#endif
	  
#if (!CONFIG_DRV_VERIFY_SUPPORT)
	if(prH264DecPrm->ucECLevel == 0)
	{
			// Default No EC, Mask for selected types
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, (~(CABAC_ALIGN_BIT_ERR | CABAC_ZERO_WORD_ERR | AVLD_4BLOCKS_SKIP_CHK | NO_NEXT_START_CODE)));
	}
	else
	{
		//vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) | AVC_ERR_CONCEALMENT); 	   
		aAVCVLD84 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) | AVC_ERR_CONCEALMENT);
		aAVCVLD84 |= AVC_SUM6_APPEND_INV;	
			#ifdef MEM_PAUSE_SUPPORT
		aAVCVLD84 |= AVC_NOT_CHK_DATA_VALID;
			#else
		aAVCVLD84 &= (0x7FFFFFFF);
			#endif	
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, aAVCVLD84);			
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK,  (~(CABAC_ALIGN_BIT_ERR | CABAC_ZERO_WORD_ERR | AVLD_4BLOCKS_SKIP_CHK | NO_NEXT_START_CODE)));
	}
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0xa89ec007);
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, (~(AVLD_MB_END_CHK |AVLD_4BLOCKS_SKIP_CHK)) );    
#endif    
	  
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)  	
#ifdef MEM_PAUSE_SUPPORT
	// Turn off find start code function
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFFDF) );	  
	//vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,	RW_AVLD_RM03R)) & 0xFFFFFCDF) );
		#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFCDD) );
		#endif	 
#endif

	//SetSHDRAVLD2	
	u4SHDRInfo = (prH264DecPrm->prSliceHdr->i4SliceQpDelta) & 0x7f;
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->u4DisableDeblockingFilterIdc & 0x3) << 7);
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->i4SliceAlphaC0OffsetDiv2 & 0xf) << 9);  
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->i4SliceBetaOffsetDiv2 & 0xf) << 13);	
	u4SHDRInfo |= ((prH264DecPrm->ucNalRefIdc & 0x3) << 17);  
	u4SHDRInfo |= (prH264DecPrm->fgIsIDRPic << 19);  
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->u4CabacInitIdc & 0x3) << 20);
	  
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_2, u4SHDRInfo);
	if (prH264DecPrm->prPPS->fgEntropyCodingModeFlag) // CABAC only
	{
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_INIT_CTX_SRAM, 0x1);  
	}
	  
		#ifdef MEM_PAUSE_SUPPORT
	while(u4VDecReadAVCVLD(u4VDecID, RO_AVLD_STATUS) & RO_AVLD_STALL);
		#endif
	  
#ifdef MPV_DUMP_H264_CHKSUM
	vVDEC_HAL_H264_VDec_ReadCheckSum1(u4VDecID);
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	if (prH264DecPrm->prSPS->fgFrameMbsOnlyFlag == 0 && prH264DecPrm->prSliceHdr->fgFieldPicFlag == 0)
	{
		u4total_mbs_in_pic = ((prH264DecPrm->prSPS->u4PicHeightInMapUnitsMinus1<<1) + 2)*(prH264DecPrm->prSPS->u4PicWidthInMbsMinus1 + 1); 
	}
	else
	{
		u4total_mbs_in_pic = (prH264DecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)*(prH264DecPrm->prSPS->u4PicWidthInMbsMinus1 + 1); 
	}
		 
	 vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC, u4total_mbs_in_pic);	
	 vVDecWriteVLDTOP(u4VDecID, RO_VLD_TOP_TIMEOUT_THD, VLD_TOP_TIMEOUT_THD);
#endif

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)	
	vVDecWriteVLDTOP(u4VDecID, AVC_VLD_TOP_PIC_PIX_SIZE, ((((prDecPrm->u4PicW) & 0xffff) << 0 ) | (((prDecPrm->u4PicH) & 0xffff) << 16 )));  
#endif
	if (1 == prH264DecPrm->fgIs16BitMode)
	{
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 0);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 0);
	}
	if (1 == prH264DecPrm->fgIsCompactMode)
	{
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 1);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 1);
	}
	if (1 == prH264DecPrm->fgIsReorderMode)
	{
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 0);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 2);
	}
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
#ifdef MPV_DUMP_H264_DEC_REG
		//UTIL_Printf("{H264 DEC} u4InstID = 0x%x, start decode\n", u4VDecID);
		//vVDEC_HAL_H264_VDec_DumpReg(u4VDecID);
#endif
		
#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
	{
		HAL_GetTime(&rTimeStartDec[u4VDecID]);
			//UTIL_Printf("second:ms %08d:%08d",rTimeStartDec[u4VDecID].u4Seconds,rTimeStartDec[u4VDecID].u4Micros);
	}
#endif
	//mb();
	if (14 == _u4FileCnt[0])
	{
		//vVDecDumpBuf(0, _pucVFifo[0], 157286400, _u4FileCnt[0], 3, 0);
	}
	vVDecWriteMCORETOP(u4VDecID, (15 << 2), 0x17200707);
	UTIL_Printf("mcore_top_%d, %x\n", 15, u4VDecReadMCORETOP(u4VDecID, (15 << 2)));
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PROC, 0x1); 
	  
	return HAL_HANDLE_OK;
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H264_DecStart(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
// Description :Set video decoder hardware registers to decode for H264
// Parameter : ptHalDecH264Info : pointer to H264 decode info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H264_DecStart(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
	UINT32 u4SHDRInfo;

#if VDEC_DDR3_SUPPORT
	UINT32 u4DDR3_PicWdith;
	UINT32 aMc406;
#endif

#if (!CONFIG_DRV_VERIFY_SUPPORT)
	UINT32  aAVCVLD84 = 0;
#endif    

#if (((CONFIG_DRV_VERIFY_SUPPORT) && (VDEC_VER_COMPARE_CRC)) || VDEC_DRV_COMPARE_CRC || CONFIG_DRV_VERIFY_SUPPORT)
	UINT32 u4CRCSrc = 0x1; 
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	UINT32 u4total_mbs_in_pic = 0;
#endif

	UINT32 u4FieldDistance = 4;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("Before trigger decode start\n");
#endif
#if (CONFIG_DRV_VERIFY_SUPPORT) && (!VDEC_DRV_PARSER)
	VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecH264DecPrm);
	//prH264DecPrm->prCurrFBufInfo->u4YStartAddr = prH264DecPrm->u4DecWorkBuf;
	prH264DecPrm->prCurrFBufInfo->u4CAddrOffset = prH264DecPrm->prCurrFBufInfo->u4DramPicSize;
#if (!VDEC_H264_REDUCE_MV_BUFF)
	prH264DecPrm->prCurrFBufInfo->u4MvStartAddr = (prH264DecPrm->prCurrFBufInfo->u4YStartAddr + ((prH264DecPrm->prCurrFBufInfo->u4DramPicSize * 3) >>1));
#endif
#else
	VDEC_INFO_H264_DEC_PRM_T *prH264DecPrm = (VDEC_INFO_H264_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif  
#if (CONFIG_DRV_VERIFY_SUPPORT)
#if VDEC_H264_REDUCE_MV_BUFF
	 if (prH264DecPrm->fgIsReduceMVBuffer)
	 {
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_REDUCE_BMV,  (u4VDecReadAVCMV(u4VDecID, RW_AMV_REDUCE_BMV) | EN_AMV_REDUCE_BMV ));
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
		u4FieldDistance= 2;  
#else
	    u4FieldDistance= 1;  
#endif
	 }
#endif
#else//Driver flow
#if VDEC_DRV_MV_BUFF_REDUCE
	if(prH264DecPrm->fgIsReduceMVBuffer)
	{
	vVDecWriteAVCMV(u4VDecID, RW_AMV_REDUCE_BMV,  (u4VDecReadAVCMV(u4VDecID, RW_AMV_REDUCE_BMV) | EN_AMV_REDUCE_BMV ));  
		#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
		u4FieldDistance= 2;  
		#else
		u4FieldDistance= 1;  
		#endif
	}
#else
	//do nothing
#endif

#endif
#ifdef Y_C_SEPERATE
	vVDecWriteMC(u4VDecID, 0xB5C, ((u4VDecReadMC(u4VDecID, 0xB5C))|1));
#endif
#if (CONFIG_DRV_VERIFY_SUPPORT)
#if PARA8_BEHAVIOR_TEST
	vVDecWriteMC(u4VDecID,0x998,u4VDecReadMC(u4VDecID,0x998)|(1<<4));
#endif
#endif

#if (((CONFIG_DRV_VERIFY_SUPPORT) && (VDEC_VER_COMPARE_CRC)) || VDEC_DRV_COMPARE_CRC || CONFIG_DRV_VERIFY_SUPPORT)
	//u4CRCSrc = (VDEC_CRC_EN | VDEC_CRC_SRC_MC);   //CRC input from MC
	u4CRCSrc = (VDEC_CRC_EN | VDEC_CRC_SRC_PP);  //CRC input from PP
	vVDecWriteCRC(u4VDecID, 0x4, u4CRCSrc);
#endif

	// addr swap mode
	vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, 4);

#if (CONFIG_DRV_VERIFY_SUPPORT)
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_BUSY_THRESHOLD, 0xFFFFFFFF);
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MC_BUSY_THRESHOLD, 0x1000);
#endif
#else
	// set MC wait timeout threshold
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_BUSY_THRESHOLD, 0xFFFFFFFF);
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MC_BUSY_THRESHOLD, 0x8000);
#endif
#endif
	// Fld & Frame height same?
	vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, prDecPrm->u4PicW);
	vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, prDecPrm->u4PicH);  

#if (!CONFIG_DRV_VERIFY_SUPPORT)
	if(prH264DecPrm->ucECLevel == 0)
	{
	     // Default No EC, Mask for selected types
	    if (1 == _u4DualCoreEnable[0])
	    {
	    	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK,  0x0);
	    }
		else
		{
	    	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, (~(CABAC_ALIGN_BIT_ERR | CABAC_ZERO_WORD_ERR | AVLD_4BLOCKS_SKIP_CHK | NO_NEXT_START_CODE)));
		}
	}
	else
	{
	    //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) | AVC_ERR_CONCEALMENT);        
	    aAVCVLD84 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) | AVC_ERR_CONCEALMENT);
	    aAVCVLD84 |= AVC_SUM6_APPEND_INV;

#ifdef MEM_PAUSE_SUPPORT
	    aAVCVLD84 |= AVC_NOT_CHK_DATA_VALID;
#else
	    aAVCVLD84 &= (0x7FFFFFFF);
#endif

	   	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, aAVCVLD84);
	   	if (1 == _u4DualCoreEnable[0])
	   	{	   		
	   		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK,  0x0);
	   	}
		else
	   	{
	   		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK,  (~(CABAC_ALIGN_BIT_ERR | CABAC_ZERO_WORD_ERR | AVLD_4BLOCKS_SKIP_CHK | NO_NEXT_START_CODE)));
	   	}
	 }
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0xa89ec007);
	if (1 == _u4DualCoreEnable[0])
	{		
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK,  0x0);
	}
	else
	{
		vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, (~(AVLD_MB_END_CHK |AVLD_4BLOCKS_SKIP_CHK)) );    
	}
#endif    

	// turn of DRAM wrapper when  current frame is Mono
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
	if (prH264DecPrm->prSPS->u4ChromaFormatIdc == 0)
	{
	    vVDecWriteMC(u4VDecID, RW_MC_WRAPPER_SWITCH, 1);
	}
	else
	{
	    vVDecWriteMC(u4VDecID, RW_MC_WRAPPER_SWITCH, 0);
	}
#endif
	// Only one case needs to turn off deblocking
	if(0)//prH264DecPrm->prSliceHdr->u4DisableDeblockingFilterIdc == 1)
	{
	    //test only
	    vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 0);  
	    vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 0);  
	    vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, 0);            
	}
	else
	{
	    vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 1); 
	#ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr) >> 9);
		vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4CStartAddr) >> 8);  
	#else
	#if CONFIG_DRV_VIRTUAL_ADDR
		vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr) >> 9);  
		vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, u4AbsDramANc((UINT32) (prH264DecPrm->prCurrFBufInfo->u4YStartAddr + prH264DecPrm->prCurrFBufInfo->u4CAddrOffset)) >> 8);  
	#else
		vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr) >> 9);  
		vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, u4AbsDramANc((UINT32) (prH264DecPrm->prCurrFBufInfo->u4YStartAddr + prH264DecPrm->prCurrFBufInfo->u4CAddrOffset)) >> 8);  
	#endif
	#endif
	    vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 1); 
	    vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, DBLK_Y | DBLK_C);            
	    vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15)>> 4) - 1);    
	    vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (((prDecPrm->u4PicH >> (1-(prH264DecPrm->fgIsFrmPic))) + 15)>> 4) - 1);    
	    vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, ((prDecPrm->u4PicW + 15)>> 4));            
	}
	if (prH264DecPrm->fgIsUFOMode)
	{
		vVDecWriteMC(u4VDecID, RW_MC_UFO_ONOFF, 0x11);
		memset(prH264DecPrm->prCurrFBufInfo->u4YStartAddr, 0, prH264DecPrm->prCurrFBufInfo->u4DramPicSize);
		memset(prH264DecPrm->prCurrFBufInfo->u4CStartAddr, 0, (prH264DecPrm->prCurrFBufInfo->u4DramPicSize)/2);
		memset(prH264DecPrm->prCurrFBufInfo->u4YLenStartAddr, 0, prH264DecPrm->prCurrFBufInfo->u4YLen);
		memset(prH264DecPrm->prCurrFBufInfo->u4CLenStartAddr, 0, prH264DecPrm->prCurrFBufInfo->u4CLen);
		vVDecWriteMC(u4VDecID, RW_MC_Y_LEN, PHYSICAL((UINT32)prH264DecPrm->prCurrFBufInfo->u4YLenStartAddr));
		vVDecWriteMC(u4VDecID, RW_MC_C_LEN, PHYSICAL((UINT32)prH264DecPrm->prCurrFBufInfo->u4CLenStartAddr));
	}
#if DTV_COMMON_CONFIG
	//for dtv only
	vVDecWriteMC(u4VDecID, RW_MC_WRAPPER_SWITCH, 0);
	vVDecWriteMC(u4VDecID,RW_MC_RR_CTRL,(u4VDecReadMC(u4VDecID,RW_MC_RR_CTRL)& 0xFF000000) | 0x01);
	vVDecWriteMC(u4VDecID,RW_MC_PP_RR_CTRL,(u4VDecReadMC(u4VDecID,RW_MC_PP_RR_CTRL)& 0xFF000000) | 0x01);
	vVDecWriteMC(u4VDecID,RW_MC_PS_CMB22,((prDecPrm->u4PicW + 15) >> 4));
	vVDecWritePP(u4VDecID,RW_PP_ADDRSWAP_SEL,0x01);
#endif

#if (CONFIG_DRV_FPGA_BOARD)
	vVDecWriteMC(u4VDecID, RW_MC_MODE_CTL, MC_QIU_BANK4|MC_QIU_BANK8|MC_DRAM_REQ_DELAY_1T|MC_DRAM_REQ_MERGE_OFF|MC_MV_MERGE_OFF);
#endif

#ifdef Y_C_SEPERATE
	vVDecWriteMC(u4VDecID, RW_AMC_Y_OUT_ADDR, PHYSICAL((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr));
#else
#if CONFIG_DRV_VIRTUAL_ADDR
	vVDecWriteMC(u4VDecID, RW_AMC_Y_OUT_ADDR, PHYSICAL((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr));
#else
	vVDecWriteMC(u4VDecID, RW_AMC_Y_OUT_ADDR, u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4YStartAddr));
#endif
	vVDecWriteMC(u4VDecID, RW_AMC_CBCR_OFFSET, prH264DecPrm->prCurrFBufInfo->u4CAddrOffset);  
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)  
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB, ((prDecPrm->u4PicBW + 15)>> 4));  
#else
	vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, ((prDecPrm->u4PicBW + 15)>> 4));  
#endif

#ifdef MEM_PAUSE_SUPPORT
	// Turn off find start code function
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFFDF) );    
	//vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFCDF) );
#else
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFCDD) );
#endif

#endif

#if CONFIG_DRV_VIRTUAL_ADDR
	vVDecWriteAVCMV(u4VDecID, RW_AMV_WR_ADDR, 
	                      ((PHYSICAL((UINT32) prH264DecPrm->prCurrFBufInfo->u4MvStartAddr) >> 4)) + ((prDecPrm->ucPicStruct == BOTTOM_FIELD)? u4FieldDistance : 0));
#else
	vVDecWriteAVCMV(u4VDecID, RW_AMV_WR_ADDR, 
	                      ((u4AbsDramANc((UINT32) prH264DecPrm->prCurrFBufInfo->u4MvStartAddr) >> 4)) + ((prDecPrm->ucPicStruct == BOTTOM_FIELD)? u4FieldDistance : 0));
#endif

#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
	vVDecWriteAVCMV(u4VDecID, RW_AMV_MVC_CTRL, (u4VDecReadAVCMV(u4VDecID,  RW_AMV_MVC_CTRL)) | FW_NO_MV_MVC);
#endif

	//SetSHDRAVLD2  
	u4SHDRInfo = (prH264DecPrm->prSliceHdr->i4SliceQpDelta) & 0x7f;
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->u4DisableDeblockingFilterIdc & 0x3) << 7);
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->i4SliceAlphaC0OffsetDiv2 & 0xf) << 9);  
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->i4SliceBetaOffsetDiv2 & 0xf) << 13);  
	u4SHDRInfo |= ((prH264DecPrm->ucNalRefIdc & 0x3) << 17);  
	u4SHDRInfo |= (prH264DecPrm->fgIsIDRPic << 19);  
	u4SHDRInfo |= ((prH264DecPrm->prSliceHdr->u4CabacInitIdc & 0x3) << 20);

	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_2, u4SHDRInfo);
	if (prH264DecPrm->prPPS->fgEntropyCodingModeFlag) // CABAC only
	{
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_INIT_CTX_SRAM, 0x1);  
	}

#ifdef MEM_PAUSE_SUPPORT
	while(u4VDecReadAVCVLD(u4VDecID, RO_AVLD_STATUS) & RO_AVLD_STALL);
#endif

#ifdef MPV_DUMP_H264_DEC_REG
	//VDec_DumpH264Reg(u4VDecID);
#endif
#ifdef MPV_DUMP_H264_CHKSUM
	vVDEC_HAL_H264_VDec_ReadCheckSum1(u4VDecID);
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530) && (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890))
	vVDecWriteMC(0, 0x5E4, (u4VDecReadMC(0, 0x5E4) |(0x1 <<12)) );
	vVDecWriteMC(0, 0x660, (u4VDecReadMC(0, 0x660) |(0x80000000)) );
#ifndef VDEC_PIP_WITH_ONE_HW
	vVDecWriteMC(1, 0x5E4, (u4VDecReadMC(1, 0x5E4) |(0x1 <<12)) );
	vVDecWriteMC(1, 0x660, (u4VDecReadMC(1, 0x660) |(0x80000000)) );    
#endif
#endif

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
	vVDecWriteMC(u4VDecID, 0x5E4, (u4VDecReadMC(u4VDecID, 0x5E4) |(0x1 <<12)) );
	vVDecWriteMC(u4VDecID, 0x660, (u4VDecReadMC(u4VDecID, 0x660) |(0x80000000)) );
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
	//     vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL,  
	//             ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFF8) |prDecPrm->ucAddrSwapMode));
#ifdef DTV_COMMON_CONFIG
	//for dtv path debug
	vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL,0x04);
	vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL,(u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL) |
	                            PP_WRAPPER_Y_LINES | PP_WRAPPER_C_LINES));
#endif
#endif 
	// turn off test mode 0x834[4]
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8560)
	 vVDecWriteMC(u4VDecID, RW_MC_DDR3_EN, (u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)  & 0xFFFFFFEF));
#endif

#if VDEC_DDR3_SUPPORT
	  u4DDR3_PicWdith = (((prDecPrm->u4PicBW + 63) >> 6) << 2);
	  vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, u4DDR3_PicWdith);  
	  #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	  vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB, u4DDR3_PicWdith);  
	  #else
	  vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, u4DDR3_PicWdith);                  
	  #endif

	 //Turn on DDR3 mode
	 vVDecWriteMC(u4VDecID, RW_MC_DDR_CTRL0, 
	           ((u4VDecReadMC(u4VDecID, RW_MC_DDR_CTRL0)  & 0xFFFFFFFE) |0x1));

	 vVDecWriteMC(u4VDecID, RW_MC_DDR_CTRL1, 
	           ((u4VDecReadMC(u4VDecID, RW_MC_DDR_CTRL1)  & 0xFFFFFFFE) |0x1));

	 aMc406 = u4VDecReadMC(u4VDecID, (406<<2));
	 aMc406 &= 0xFFFFFFEF;
	 vVDecWriteMC(u4VDecID, (406<<2), aMc406);
	 #if ((CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) && (CONFIG_DRV_FPGA_BOARD))
	 vVDecWriteMC(u4VDecID,RW_MC_DDR3_EN, (u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)  | 0x1));
	 #else
	 //Turn-on DDR3, Set 0x834[0] = 0
	 vVDecWriteMC(u4VDecID, RW_MC_DDR3_EN, (u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)  & 0xFFFFFFFE));
	 #endif
	//Post-process enable
	vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, (u4VDecReadMC(u4VDecID, RW_MC_PP_ENABLE)  | 0x1));

	//Writeback by PP
	vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 0x00000001);
#endif

#if CONFIG_DRV_VERIFY_SUPPORT
	//vVDEC_HAL_H264_VDec_DumpReg(u4VDecID, TRUE);
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	if (prH264DecPrm->prSPS->fgFrameMbsOnlyFlag == 0 && prH264DecPrm->prSliceHdr->fgFieldPicFlag == 0)
	 {
	    u4total_mbs_in_pic = ((prH264DecPrm->prSPS->u4PicHeightInMapUnitsMinus1<<1) + 2)*(prH264DecPrm->prSPS->u4PicWidthInMbsMinus1 + 1); 
	 }
	 else
	 {
	    u4total_mbs_in_pic = (prH264DecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)*(prH264DecPrm->prSPS->u4PicWidthInMbsMinus1 + 1); 
	 }
	 
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC, u4total_mbs_in_pic);  
	vVDecWriteVLDTOP(u4VDecID, RO_VLD_TOP_TIMEOUT_THD, VLD_TOP_TIMEOUT_THD);
#endif

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)	
	vVDecWriteVLDTOP(u4VDecID, AVC_VLD_TOP_PIC_PIX_SIZE, ((((prDecPrm->u4PicW) & 0xffff) << 0 ) | (((prDecPrm->u4PicH) & 0xffff) << 16 )));  
#endif

	if (1 == _u4DualCoreEnable[0])
	{
		vVDecWriteAVCMV(u4VDecID, RW_AMV_CFG, (u4VDecReadAVCMV(u4VDecID, RW_AMV_CFG) | 0x5000));
	}
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
#ifdef MPV_DUMP_H264_DEC_REG
	//UTIL_Printf("{H264 DEC} u4InstID = 0x%x, start decode\n", u4VDecID);
	//vVDEC_HAL_H264_VDec_DumpReg(u4VDecID);
#endif

#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
	{
	    HAL_GetTime(&rTimeStartDec[u4VDecID]);
	    //UTIL_Printf("second:ms %08d:%08d",rTimeStartDec[u4VDecID].u4Seconds,rTimeStartDec[u4VDecID].u4Micros);
	}
#endif
	//mb();
	if (1 == prH264DecPrm->fgIs16BitMode)
	{
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 0);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 0);
	}
	if (1 == prH264DecPrm->fgIsCompactMode)
	{
		memset(prH264DecPrm->prCurrFBufInfo->u4YStartAddr, 0, prH264DecPrm->prCurrFBufInfo->u4DramPicSize);
		memset(prH264DecPrm->prCurrFBufInfo->u4CStartAddr, 0, (prH264DecPrm->prCurrFBufInfo->u4DramPicSize)/2);
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 1);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 1);
	}
	if (1 == prH264DecPrm->fgIsReorderMode)
	{
		vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
		vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 0);
		vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 2);
	}
	
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)	
	if (prDecPrm->u4PicW < 176)
	{
		vVDecWriteMISC(u4VDecID, (60 << 2),(u4VDecReadMISC(u4VDecID, (60 << 2))| 0x1));
	}
#endif
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("Before trigger decode end\n");
#endif
	return HAL_HANDLE_OK;
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_TriggerDualCore(UINT32 u4VDecID);
// Description :trigger dual core decoded start
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_TriggerDualCore(UINT32 u4VDecID)
{	
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_TriggerDualCore start\n");
#endif
	#if AVC_NEW_CRC_COMPARE
	if (1 == _u4DualCoreEnable[0])
	{		
		vVDecWriteMCORETOP(u4VDecID, MCORE_CRC_EN, 0x1);
	}
	#endif
	vVDecWriteMCORETOP(u4VDecID, RW_MCORE_DEC_CABAC, PHYSICAL(_pucBSCABACBuff[0]));
	vVDecWriteMCORETOP(u4VDecID, RW_MCORE_DEC_EC, PHYSICAL(_pucECInfoBuff[0]));
	vVDecWriteMCORETOP(u4VDecID, (18 << 2), 0x120);
	vVDecWriteMCORETOP(u4VDecID, (24 << 2), u4VDecReadMCORETOP(u4VDecID,(24 << 2)) | 0x01);
	vVDecWriteMCORETOP(u4VDecID, (25 << 2), u4VDecReadMCORETOP(u4VDecID,(25 << 2)) | 0x01);
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_TriggerDualCore end\n");
#endif
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_TriggerDecode(UINT32 u4VDecID);
// Description :trigger decoded start
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_TriggerDecode(UINT32 u4VDecID)
{
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_TriggerDecode start\n");
#endif
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PROC, 0x1);
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vVDEC_HAL_H264_TriggerDecode end\n");
#endif
}


// **************************************************************************
// Function : void v4VDEC_HAL_H264_GetMbxMby(UINT32 u4VDecID, UINT32 *pu4Mbx, UINT32 *pu4Mby);
// Description :Read current decoded mbx and mby
// Parameter : u4VDecID : video decoder hardware ID
//                 u4Mbx : macroblock x value
//                 u4Mby : macroblock y value
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_GetMbxMby(UINT32 u4VDecID, UINT32 *pu4Mbx, UINT32 *pu4Mby)
{
	*pu4Mbx = u4VDecReadMC(u4VDecID, RO_MC_MBX);
	*pu4Mby = u4VDecReadMC(u4VDecID, RO_MC_MBY);
}


// **************************************************************************
// Function : BOOL fgVDEC_HAL_H264_DecPicComplete(UINT32 u4VDecID);
// Description :Check if all video decoder modules are complete
// Parameter : u4VDecID : video decoder hardware ID
// Return      : TRUE: Decode complete, FALSE: Not yet
// **************************************************************************
BOOL fgVDEC_HAL_H264_DecPicComplete(UINT32 u4VDecID)
{
	if(u4VDecReadAVCVLD(u4VDecID, RO_AVLD_STATE_INFO) & AVLD_PIC_FINISH)
	{
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}
}

// **************************************************************************
// Function : void u4VDEC_HAL_H264_GetErrMsg(UINT32 u4VDecID);
// Description :Read h264 error message after decoding end
// Parameter : u4VDecID : video decoder hardware ID
// Return      : H264 decode error message
// **************************************************************************
UINT32 u4VDEC_HAL_H264_GetErrMsg(UINT32 u4VDecID)
{
	return u4VDecReadAVCVLD(u4VDecID, RO_AVLD_ERR_ACCUMULATOR);
}


// **************************************************************************
// Function : void u4VDEC_HAL_H264_GetErrMsg(UINT32 u4VDecID);
// Description :Read h264 error message after decoding end
// Parameter : u4VDecID : video decoder hardware ID
// Return      : H264 decode error message
// **************************************************************************
BOOL fgVDEC_HAL_H264_ChkErrInfo(UINT32 ucBsId, UINT32 u4VDecID, UINT32 u4DecErrInfo, UINT32 u4ECLevel)
{
	UINT32 u4Data; 
	BOOL fgIsVDecErr;

	fgIsVDecErr = TRUE;

	switch(u4ECLevel)
	{
	    case 2:
	        // Ignore the real non-NextStartCode condition
	        if( (u4DecErrInfo == (CABAC_ZERO_WORD_ERR|NO_NEXT_START_CODE))
	        // Add For CQ: 31166, 31113 Customer_B_B_K: AVCHD Disc            
	        || (u4DecErrInfo == (CABAC_ZERO_WORD_ERR))
	        )            	
	        {
	            fgIsVDecErr = FALSE;
	        }
	    case 0:
	    case 1:
	    default:
	        if (u4DecErrInfo == CABAC_ZERO_WORD_ERR)
	        {
	            vVDEC_HAL_H264_TrailingBits(ucBsId, u4VDecID);
	            u4Data = u4VDEC_HAL_H264_ShiftGetBitStream(ucBsId, u4VDecID, 0);
	            if(((u4Data >> 8) == START_CODE) || (u4Data == 0x00000000) || (u4Data == START_CODE))
	            {
	                fgIsVDecErr = FALSE;
	            }
	        }
	        else if (u4DecErrInfo == NO_NEXT_START_CODE) // don't care "No next start code"
	        {
	            fgIsVDecErr = FALSE;
	        }
	        else if ((u4DecErrInfo == CABAC_ALIGN_BIT_ERR) && (!(u4VDecReadAVCVLD(u4VDecID, RW_AVLD_ERR_MASK) & CABAC_ALIGN_BIT_ERR))) // don't care "No next start code"
	        {
	            fgIsVDecErr = FALSE;
	        } 
	        else if ((u4DecErrInfo == AVLD_4BLOCKS_SKIP_CHK) && (!(u4VDecReadAVCVLD(u4VDecID, RW_AVLD_ERR_MASK) & AVLD_4BLOCKS_SKIP_CHK)))
	        {
	            fgIsVDecErr = FALSE;
	        } 
	        else if((u4DecErrInfo & u4VDecReadAVCVLD(u4VDecID, RW_AVLD_ERR_MASK)) == 0)
	        {
	            fgIsVDecErr = FALSE;
	        }
	        break;
	}

	return fgIsVDecErr;
}

#if (VDEC_MVC_SUPPORT || (CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
// **************************************************************************
// Function : void vVDEC_HAL_H264_SetInterViewPRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo);
// Description :Set HW registers related with InterView Ref Pic
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_MVC_Switch(UINT32 u4VDecID, BOOL fgIsMVCDec)
{
	if(fgIsMVCDec)
	{
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R) | (MVC_SWITCH | HEADER_EXT_SWITCH | REORDER_MVC_SWITCH));    
	}
	else
	{
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R) & (~(MVC_SWITCH | HEADER_EXT_SWITCH | REORDER_MVC_SWITCH)));    
	}
	vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R) & (~VIEW_REORDER_SWITCH));    
}

// **************************************************************************
// Function : void vVDEC_HAL_H264_SetInterViewPRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo);
// Description :Set HW registers related with InterView Ref Pic
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetInterViewPRefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;
	UINT32 u4Param;

	ucFld = (prPRefPicListInfo->u4FBufInfo & 0xff);  
	ucRegIdx = ((prPRefPicListInfo->u4FBufInfo >> 16) & 0xff);  
	ucFBufIdx = prPRefPicListInfo->ucFBufIdx;

	if (ucFld == FRAME)
	{    
	    //bRegIdx = bRefPicIdx;
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)  prPRefPicListInfo->u4FBufYStartAddr));    
#else
	    vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx <<2), u4AbsDramANc((UINT32)  prPRefPicListInfo->u4FBufYStartAddr));    
#endif
	     //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));
	     u4Param = (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff);
	     vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	      //UTIL_Printf("SetInterPRefPicList1: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp |= ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<3), u4Temp);

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp |= ((((ucFBufIdx)<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara;
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);
	}
	else
	{ 
	    if (ucFld == TOP_FIELD)
	    {
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
	        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));
	        u4Param = (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff);
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	        //UTIL_Printf("SetInterPRefPicList2: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);


	        u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	        u4Temp |= ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara;
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<2), u4Temp);
	    }
	    else
	    {        
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
	        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	        u4Param = (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff);
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2), u4Param);
	        //UTIL_Printf("SetInterPRefPicList3: [0x%X] = 0x%X\n", RW_AVLD_REORD_P0_RPL + (ucRegIdx<<2),  u4Param);

	        u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	        u4Temp |= (((ucFBufIdx<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara;
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA + (ucRegIdx<<2), u4Temp);
	        
	        vVDecWriteMC(u4VDecID, RW_AMC_P_LIST0_FLD, u4VDecReadMC(u4VDecID, RW_AMC_P_LIST0_FLD) | (0x1 << ucRegIdx));        
	    }     
	}
}

// **************************************************************************
// Function : void vVDEC_HAL_H264_SetInterViewB0RefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo);
// Description :Set HW registers related with InterView Ref Pic
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetInterViewB0RefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;

	ucFld = (prPRefPicListInfo->u4FBufInfo & 0xff);  
	ucRegIdx = ((prPRefPicListInfo->u4FBufInfo >> 16) & 0xff);  
	ucFBufIdx = prPRefPicListInfo->ucFBufIdx;

	if (ucFld == FRAME)
	{
	    // B_0
	    #ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
		#else
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
		#endif
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp += ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara + (prPRefPicListInfo->i4TFldPOC & 0x3ffff);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<3), u4Temp);

	    // Always set Tfld first & !LRef in Interview
	    u4Temp = (1 << 21);
	    u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara + (prPRefPicListInfo->i4BFldPOC & 0x3ffff);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);
	}
	else
	{
	    if (ucFld == TOP_FIELD)
	    {
	    	#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	        u4Temp = ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara + (prPRefPicListInfo->i4TFldPOC & 0x3ffff);
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<2), u4Temp);
	    }
	    else
	    {     
	    #ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST0 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
		#endif
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	        u4Temp = (((ucFBufIdx<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara + (prPRefPicListInfo->i4BFldPOC & 0x3ffff);
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (ucRegIdx<<2), u4Temp);

	        vVDecWriteMC(u4VDecID, RW_AMC_B_LIST0_FLD, u4VDecReadMC(u4VDecID, RW_AMC_B_LIST0_FLD) | (0x1 << ucRegIdx));        
	    }
	}
}

// **************************************************************************
// Function : void vVDEC_HAL_H264_SetInterViewB1RefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo);
// Description :Set HW registers related with InterView Ref Pic
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H264_SetInterViewB1RefPicListReg(UINT32 u4VDecID, VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo)
{
	UCHAR ucFBufIdx;
	UCHAR ucFld;  
	UCHAR ucRegIdx;
	UINT32 u4Temp;

	ucFld = (prPRefPicListInfo->u4FBufInfo & 0xff);  
	ucRegIdx = ((prPRefPicListInfo->u4FBufInfo >> 16) & 0xff);  
	ucFBufIdx = prPRefPicListInfo->ucFBufIdx;

	if (ucFld == FRAME)
	{
	    // B_1
	    ucFBufIdx = prPRefPicListInfo->ucFBufIdx;
		#ifdef Y_C_SEPERATE
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
		vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
		#else
#if CONFIG_DRV_VIRTUAL_ADDR
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
		#endif
	    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp += ((ucFBufIdx)<<1<<22) + prPRefPicListInfo->u4TFldPara + (prPRefPicListInfo->i4TFldPOC & 0x3ffff);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<3), u4Temp);

	    u4Temp = (((prPRefPicListInfo->i4TFldPOC <= prPRefPicListInfo->i4BFldPOC) && (prPRefPicListInfo->i4TFldPOC != 0x7fffffff))? 1: 0) << 21;    
	    u4Temp += ((((ucFBufIdx)<<1) + 1)<<22) + prPRefPicListInfo->u4BFldPara + (prPRefPicListInfo->i4BFldPOC & 0x3ffff);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<3) + 4, u4Temp);

#if CONFIG_DRV_VIRTUAL_ADDR
	    u4Temp = PHYSICAL((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#else
	    u4Temp = u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#endif
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<3), u4Temp >> 4);
	    vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<3) + 4, (u4Temp >> 4) + 4); 
	}
	else
	{
	    if (ucFld == TOP_FIELD)
	    {
	    	#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

	        u4Temp = (1<<19) + ((ucFBufIdx)<<1<<22)+ (prPRefPicListInfo->i4TFldPOC & 0x3ffff);
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<2), u4Temp);

#if CONFIG_DRV_VIRTUAL_ADDR
	        u4Temp = PHYSICAL((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#else            
	        u4Temp = u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#endif
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<2), (u4Temp >> 4));      
	    }
	    else
	    {
	    	#ifdef Y_C_SEPERATE
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4YStartAddr));
			vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + (ucRegIdx <<2), PHYSICAL((UINT32)prPRefPicListInfo->u4CStartAddr));
			#else
#if CONFIG_DRV_VIRTUAL_ADDR
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), PHYSICAL((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#else
	        vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + (ucRegIdx<<2), u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufYStartAddr));    
#endif
			#endif
	        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + (ucRegIdx<<2), (1 << 19) + (2 << 20) + (prPRefPicListInfo->u4ViewId & 0x7ffff));

#if CONFIG_DRV_VIRTUAL_ADDR
	        u4Temp = PHYSICAL((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#else
	        u4Temp = u4AbsDramANc((UINT32) prPRefPicListInfo->u4FBufMvStartAddr);
#endif
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_ADDR + (ucRegIdx<<2), (u4Temp >> 4) + 4);

	        u4Temp = (1<<19) + (((ucFBufIdx<<1) + 1)<<22) + (prPRefPicListInfo->i4BFldPOC & 0x3ffff);
	        vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (ucRegIdx<<2), u4Temp);

	        vVDecWriteMC(u4VDecID, RW_AMC_B_LIST1_FLD, u4VDecReadMC(u4VDecID, RW_AMC_B_LIST1_FLD) | (0x1 << ucRegIdx));        
	    }
	}
}

#endif


void vVDEC_HAL_H264_VDec_PowerDown(UCHAR u4VDecID)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
	vVDecWriteVLD(u4VDecID, WO_VLD_SRST, (0x1 |(0x1<<8)));
#else
	vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 1);
#endif
	vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
	 vVDecPowerDownHW(u4VDecID);
#endif
}

#ifdef MPV_DUMP_H264_DEC_REG
static UINT32 _aru4DumpH264Data[0x500];
// *********************************************************************
// Function :void VDec_DumpH264Reg(UCHAR ucMpvId)
// Description : Set RW_MC_BREF
// Parameter : fgIVop: indicate the backward reference picture is an I-VOP
// Return    : None
// *********************************************************************
void VDec_DumpH264Reg(UCHAR ucMpvId)
{
	UINT32 i;
	if (VDEC_LAE0 == ucMpvId)
	{
		for(i = 33; i < 42; i ++)
		{
		    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(2,(i*4)));
		}
		for(i = 146; i < 188; i ++)
		{
		    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(2,(i*4)));
		}
		for(i = 44; i < 85; i ++)
		{
		    UTIL_Printf("vld #%d = (0x%x)\n",i,u4VDecReadVLD(2,(i*4)));
		}
		for (i = 0; i < 256; i++)
		{
			UTIL_Printf("MCORETOP #%d = (0x%x)\n", i, u4VDecReadMCORETOP(0, (i*4)));
		}
	}
	else
	{
		for (i = 0;i < 90; i++)
		{
			UTIL_Printf("MISC #%d = (0x%x)\n", i, u4VDecReadMISC(0, (i*4)));
		}
		for(i = 33; i < 42; i ++)
		{
		    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(0,(i*4)));
		}
		for(i = 146; i < 188; i ++)
		{
		    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(0,(i*4)));
		}
		for(i = 369; i < 398; i ++)
		{
		    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(0,(i*4)));
		}
		for(i = 44; i < 85; i ++)
		{
		    UTIL_Printf("vld #%d = (0x%x)\n",i,u4VDecReadVLD(0,(i*4)));
		}
		for(i = 470; i < 488; i ++)
		{
		    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(0,(i*4)));
		}
		for(i = 526; i < 531; i ++)
		{
		    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(0,(i*4)));
		}
		for (i = 32; i < 42; i++)
		{
			UTIL_Printf("PP #%d = (0x%x)\n", i, vVDecReadPP(0,(i*4)));
		}
		for (i = 784; i < 789; i++)
		{
			UTIL_Printf("PP #%d = (0x%x)\n", i, vVDecReadPP(0,(i*4)));
		}
		for (i = 695; i < 698; i++)
		{
			UTIL_Printf("MC #%d = (0x%x)\n", i, u4VDecReadMC(0, (i*4)));
		}
		for (i = 136; i < 158; i++)
		{
			UTIL_Printf("MV #%d = (0x%x)\n", i, u4VDecReadAVCMV(0, (i*4)));
		}
		if (1 == _u4DualCoreEnable[0])
		{
			UTIL_Printf("~~~~~~~~core1 part~~~~~~~~~~~~\n");
			for (i = 0;i < 90; i++)
			{
				UTIL_Printf("MISC #%d = (0x%x)\n", i, u4VDecReadMISC(1, (i*4)));
			}
			for(i = 33; i < 42; i ++)
			{
			    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(1,(i*4)));
			}
			for(i = 146; i < 188; i ++)
			{
			    UTIL_Printf("avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(1,(i*4)));
			}
			for(i = 369; i < 398; i ++)
			{
			    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(1,(i*4)));
			}
			for(i = 44; i < 85; i ++)
			{
			    UTIL_Printf("vld #%d = (0x%x)\n",i,u4VDecReadVLD(1,(i*4)));
			}
			for(i = 470; i < 488; i ++)
			{
			    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(1,(i*4)));
			}
			for(i = 526; i < 531; i ++)
			{
			    UTIL_Printf("MC #%d = (0x%x)\n",i,u4VDecReadMC(1,(i*4)));
			}
			for (i = 32; i < 42; i++)
			{
				UTIL_Printf("PP #%d = (0x%x)\n", i, vVDecReadPP(1,(i*4)));
			}
			for (i = 784; i < 789; i++)
			{
				UTIL_Printf("PP #%d = (0x%x)\n", i, vVDecReadPP(1,(i*4)));
			}
			for (i = 695; i < 698; i++)
			{
				UTIL_Printf("MC #%d = (0x%x)\n", i, u4VDecReadMC(1, (i*4)));
			}
			for (i = 136; i < 158; i++)
			{
				UTIL_Printf("MV #%d = (0x%x)\n", i, u4VDecReadAVCMV(1, (i*4)));
			}
			for (i = 0; i < 256; i++)
			{
				UTIL_Printf("MCORETOP #%d = (0x%x)\n", i, u4VDecReadMCORETOP(0, (i*4)));
			}
		}
	}
}
#endif

#if CONFIG_DRV_VERIFY_SUPPORT
UINT32 u4VDEC_HAL_H264_VDec_ReadFinishFlag(UINT32 u4VDecID)
{
	return u4VDecReadAVCVLD(u4VDecID, RO_AVLD_COMPLETE);
}

void vVDEC_HAL_H264_VDec_DumpReg(UINT32 u4VDecID)
{
#ifdef MPV_DUMP_H264_DEC_REG
	VDec_DumpH264Reg((UCHAR)u4VDecID);
#endif
}

void vVDEC_HAL_H264_VDec_ReadCheckSum(UINT32 u4VDecID, UINT32 *pu4CheckSum)
{
	UINT32  u4Temp,u4Cnt;

	u4Temp = 0;
	*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f4);
	pu4CheckSum ++;
	u4Temp ++;
	*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f8);    
	pu4CheckSum ++;
	u4Temp ++;
	*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x608);    
	pu4CheckSum ++;
	u4Temp ++;
	*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x60c);        
	pu4CheckSum ++;
	u4Temp ++;

	//MC  378~397  
	for(u4Cnt=378; u4Cnt<=397; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadMC(u4VDecID, (u4Cnt<<2));
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>MC<============");
	UTIL_Printf("[MC]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	pu4CheckSum ++;   
	u4Temp ++;
	}

	//AVC VLD  165~179
	for(u4Cnt=165; u4Cnt<=179; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));    
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>AVC VLD<============");
	UTIL_Printf("[AVLD]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	  pu4CheckSum ++;  
	  u4Temp ++;
	}

	//MV  147~151
	for(u4Cnt=147; u4Cnt<=151; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));  
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>AVCMV<============");
	UTIL_Printf("[AVCMV]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	pu4CheckSum ++;       
	u4Temp ++;
	}

	//IP  212    
	*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (212 << 2));
	pu4CheckSum ++;  
	u4Temp ++;

	//IQ  235~239
	for(u4Cnt=235; u4Cnt<=239; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));   
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>IQ<============");
	UTIL_Printf("[AVCMV]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	pu4CheckSum ++;     
	u4Temp ++;
	}    

	//IS  241~245
	for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>IS<============");
	UTIL_Printf("[AVCMV]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	pu4CheckSum ++;     
	u4Temp ++;
	}

	//IP 212~220
	for(u4Cnt=212; u4Cnt<=220; u4Cnt++)
	{
	*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));
#if AVC_CHKSUM_PRNT
	UTIL_Printf("==========>IP<============");
	UTIL_Printf("[AVCMV]reg#%d(0x%x) = 0x%x\n",u4Cnt,u4Cnt<<2,*pu4CheckSum);
#endif
	pu4CheckSum ++;     
	u4Temp ++;
	}

	while(u4Temp < MAX_CHKSUM_NUM)
	{
	*pu4CheckSum = 0;            
	pu4CheckSum ++;   
	u4Temp ++;
	}  
}

BOOL fgVDEC_HAL_H264_VDec_CompCheckSum(UINT32 *pu4DecCheckSum, UINT32 *pu4GoldenCheckSum)
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

#endif

#ifdef MPV_DUMP_H264_CHKSUM
#define MAX_CHKSUM_NUM 80
	UINT32 _u4DumpChksum[2][MAX_CHKSUM_NUM];
	void vVDEC_HAL_H264_VDec_ReadCheckSum1(UINT32 u4VDecID)
	{
		UINT32  u4Temp,u4Cnt;

		UINT32 *pu4CheckSum = _u4DumpChksum[0];
		    
		u4Temp = 0;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f4);
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f8);    
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x608);    
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x60c);        
		pu4CheckSum ++;
		u4Temp ++;

		//MC  378~397  
		for(u4Cnt=378; u4Cnt<=397; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadMC(u4VDecID, (u4Cnt<<2));
		    pu4CheckSum ++;   
		    u4Temp ++;
		}

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (44<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (45<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (46<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		//VLD  58~63
		for(u4Cnt=58; u4Cnt<=63; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		*pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, 0x84);            
		pu4CheckSum ++;  
		u4Temp ++;

		//AVC VLD  148~152
		for(u4Cnt=148; u4Cnt<=155; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		//AVC VLD  165~179
		for(u4Cnt=165; u4Cnt<=179; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		//MV  147~151
		for(u4Cnt=147; u4Cnt<=151; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;       
		    u4Temp ++;
		}

		//IP  212    
		*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (212 << 2));
		pu4CheckSum ++;  
		u4Temp ++;

		//IQ  235~239
		for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;     
		    u4Temp ++;
		}    

		//IS  241~245
		for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;     
		    u4Temp ++;
		}

		while(u4Temp < MAX_CHKSUM_NUM)
		{
		    *pu4CheckSum = 0;            
		    pu4CheckSum ++;   
		    u4Temp ++;
		}  
	}

	void vVDEC_HAL_H264_VDec_ReadCheckSum2(UINT32 u4VDecID)
	{
		UINT32  u4Temp,u4Cnt;

		UINT32 *pu4CheckSum = _u4DumpChksum[1];
		    
		u4Temp = 0;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f4);
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f8);    
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x608);    
		pu4CheckSum ++;
		u4Temp ++;
		*pu4CheckSum = u4VDecReadMC(u4VDecID, 0x60c);        
		pu4CheckSum ++;
		u4Temp ++;

		//MC  378~397  
		for(u4Cnt=378; u4Cnt<=397; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadMC(u4VDecID, (u4Cnt<<2));
		    pu4CheckSum ++;   
		    u4Temp ++;
		}

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (44<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (45<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		*pu4CheckSum = u4VDecReadVLD(u4VDecID, (46<<2));        
		pu4CheckSum ++;
		u4Temp ++;

		//VLD  58~63
		for(u4Cnt=58; u4Cnt<=63; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		*pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, 0x84);            
		pu4CheckSum ++;  
		u4Temp ++;

		//AVC VLD  148~152
		for(u4Cnt=148; u4Cnt<=155; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		//AVC VLD  165~179
		for(u4Cnt=165; u4Cnt<=179; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;  
		    u4Temp ++;
		}

		//MV  147~151
		for(u4Cnt=147; u4Cnt<=151; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;       
		    u4Temp ++;
		}

		//IP  212    
		*pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (212 << 2));
		pu4CheckSum ++;  
		u4Temp ++;

		//IQ  235~239
		for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;     
		    u4Temp ++;
		}    

		//IS  241~245
		for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
		{
		    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));            
		    pu4CheckSum ++;     
		    u4Temp ++;
		}

		while(u4Temp < MAX_CHKSUM_NUM)
		{
		    *pu4CheckSum = 0;            
		    pu4CheckSum ++;   
		    u4Temp ++;
		}  
	}
#endif
