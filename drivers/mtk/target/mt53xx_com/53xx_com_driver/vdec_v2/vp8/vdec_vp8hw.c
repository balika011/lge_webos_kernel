/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp8hw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8hw.c
 *  This source file include vp8 hw setting  function 
 */

 
#include "x_assert.h"
#include "x_printf.h"
#include "vdec_drvif.h"
     LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
     LINT_EXT_HEADER_END
#include "../vdec_debug.h"
#include "vdec_vp8webp_if.h"
#include "vdec_vp8util.h"
#include "vdec_vp8hw.h"

#ifdef VDEC_VP8HW_DEBUG
static BOOL _fgVdecVPLogSimPat=FALSE;
#endif

EXTERN VDEC_VP8_INFO_T *VDEC_Vp8GetLockedInstance(void);

#ifdef USE_VP68_SW_VLD
#define USE_VP6_BOOL_DECODER 1
static BOOL_VLD_T _rBoolVld[2];
static BERRAL_SHIFTER _rBerralShift[2];
#if USE_VP6_BOOL_DECODER
static const UCHAR vp8dx_bitreader_norm[]=
{
    0,  // Invalid (should never be used).
    7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static INT32 _SwBoolDecode(UINT32 u4VldId,INT32 i4Prob)
{
   BOOL_VLD_T *pBr=&_rBoolVld[u4VldId];
   UINT8 u4RetVal=0;
   UINT32 u4Range=pBr->u4Range;
   UINT32 u4Value=pBr->u4Value;
   INT32  i4Count=pBr->i4Count;

  // perform the actual decodin
   UINT32 u4Split=1+(((u4Range-1)*i4Prob)>>8);
   if(u4Value>=u4Split<<24)
   {
     u4RetVal=1;
     u4Range-=u4Split;
     u4Value-=u4Split<<24;
   }
   else
   {
     u4Range=u4Split;
   }

  if(u4Range<0x80)
  {
     UINT32 u4Index=vp8dx_bitreader_norm[u4Range];
     u4Range<<=u4Index;
     u4Value<<=u4Index;
     i4Count-=u4Index;
     if(i4Count<= 0) 
     {
        u4Value|=((*((UCHAR *)pBr->u4ReadPtr))<<(-i4Count));
        VDEC_RPOS_INC(pBr->u4ReadPtr,1,pBr->u4BufStart,pBr->u4BufEnd);
        i4Count += 8;
     }
  }

   pBr->u4Range=u4Range;
   pBr->u4Value=u4Value;
   pBr->i4Count=i4Count;
   return(u4RetVal);
}

static INT32 _SwBoolDecode128(UINT32 u4VldId)
{
    BOOL_VLD_T *pBr=&_rBoolVld[u4VldId];
    UINT8 u4RetVal=0;
    UINT32 u4Range=pBr->u4Range;
    UINT32 u4Value=pBr->u4Value;
    INT32  i4Count=pBr->i4Count;

    // perform the actual decoding
    UINT32 u4Split=(u4Range+1)>>1;
    if (u4Value>=(u4Split<<24))
    {
        u4RetVal=1;
        u4Range-=u4Split;
        u4Value-=(u4Split<<24);
    }    
    else
    {
        u4Range=u4Split;
    }

    u4Range<<=1;
    u4Value<<=1;

    if (--i4Count==0)
    {
        u4Value|=(*((UCHAR *)pBr->u4ReadPtr));
        VDEC_RPOS_INC(pBr->u4ReadPtr,1,pBr->u4BufStart,pBr->u4BufEnd);
        i4Count=8;
    }

    pBr->u4Range=u4Range;
    pBr->u4Value=u4Value;
    pBr->i4Count=i4Count;
    return(u4RetVal);
}

static UINT32 _SwBoolReadValue(UINT32 u4VldId,INT32 i4Bits)
{
    UINT32 u4Ret = 0;
    INT32 i4BitIndex;
    for ( i4BitIndex=i4Bits-1; i4BitIndex>=0; i4BitIndex-- )
    {
        u4Ret |= (_SwBoolDecode128(u4VldId)<<i4BitIndex);
    }
    return u4Ret;
}

#else
static const UINT8 vp8dx_bitreader_norm[256] =
{
    0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int _SwBoolDecode(UINT32 u4VldId,INT32 i4Prob)
{
  UINT8 uRetVal=0;
  BOOL_VLD_T *pBr=&_rBoolVld[u4VldId];
  UINT32 u4Range=pBr->u4Range;
  UINT32 u4Value=pBr->u4Value;
  INT32 i4Count=pBr->i4Count;
  UINT32 u4Split=1+(((u4Range-1)*i4Prob)>>8);
  UINT32 u4BigSplit=(u4Split<<8);
  
  register UINT32 u4Shift=0;
  u4Range=u4Split;

  if(u4Value>=u4BigSplit)
  {
     u4Range=pBr->u4Range-u4Split;
     u4Value-=u4BigSplit;
     uRetVal=1;
  }

  u4Shift=vp8dx_bitreader_norm[u4Range];
  u4Range<<=u4Shift;
  u4Value<<=u4Shift;
  i4Count-=u4Shift;
  if(i4Count<= 0) 
  {
    u4Value |= ((*((UCHAR *)pBr->u4ReadPtr))<<(-i4Count));
    VDEC_RPOS_INC(pBr->u4ReadPtr,1,pBr->u4BufStart,pBr->u4BufEnd);
    i4Count += 8;
  }

  pBr->u4Range=u4Range;
  pBr->u4Value=u4Value;
  pBr->i4Count=i4Count;
  return(uRetVal);
}
static UINT32 _SwBoolReadValue(UINT32 u4VldId,INT32 i4Bits)
{
    UINT32 u4Ret = 0;
    INT32 i4BitIndex;
    for ( i4BitIndex=i4Bits-1; i4BitIndex>=0; i4BitIndex-- )
    {
        u4Ret |= (_SwBoolDecode(u4VldId,0x80)<<i4BitIndex);
    }
    return u4Ret;
}
#endif


static UINT32 _SwBerrelShift(UINT32 u4VldId,INT32 i4Bits)
{
    BERRAL_SHIFTER *prShift=&_rBerralShift[u4VldId];
    INT32 i4Aailable = prShift->i4BitsAvailable;
    UINT32 u4Value=prShift->u4Value;
    UINT32 u4RetVal = 0;
    if(i4Aailable < i4Bits )
    {
        // Need more bits from input buffer...
        u4RetVal = u4Value >> (32-i4Aailable);
        i4Bits -= i4Aailable;
        u4RetVal <<= i4Bits;
        VDEC_INTEGER(u4Value,prShift->u4ReadPtr,4,prShift->u4BufStart,prShift->u4BufEnd);
        i4Aailable = 32;
    }
    
    u4RetVal |= u4Value >> (32-i4Bits);
    prShift->u4Value= u4Value<<i4Bits;
    prShift->i4BitsAvailable= i4Aailable-i4Bits;
    return u4RetVal;  
}

VOID _VDEC_VP68_InitSwBoolDecoder(UINT32 u4VldId,UINT32 u4ReadPtr,
   UINT32 u4BufStart,UINT32 u4BufEnd)
{
  UINT32 u4Value=0;
#if USE_VP6_BOOL_DECODER
  _rBoolVld[u4VldId].u4BufStart=u4BufStart;
  _rBoolVld[u4VldId].u4BufEnd=u4BufEnd;
  _rBoolVld[u4VldId].u4LowValue=0;
  _rBoolVld[u4VldId].u4Range=255;
  _rBoolVld[u4VldId].i4Count=8;
  VDEC_INTEGER(u4Value,u4ReadPtr,4,u4BufStart,u4BufEnd);
  _rBoolVld[u4VldId].u4Value=u4Value;
  _rBoolVld[u4VldId].u4ReadPtr=u4ReadPtr;
#else
  _rBoolVld[u4VldId].u4BufStart=u4BufStart;
  _rBoolVld[u4VldId].u4BufEnd=u4BufEnd;
  _rBoolVld[u4VldId].u4LowValue=0;
  _rBoolVld[u4VldId].u4Range=255;
  _rBoolVld[u4VldId].i4Count=0;
  _rBoolVld[u4VldId].u4Value=(*((UCHAR *)u4ReadPtr))<<8;
  VDEC_RPOS_INC(u4ReadPtr,1,u4BufStart,u4BufEnd);
  _rBoolVld[u4VldId].u4ReadPtr=u4ReadPtr;  
#endif
}

VOID _VDEC_VP68_InitSwBarrelShifter(UINT32 u4VldId,UINT32 u4ReadPtr,
    UINT32 u4BufStart,UINT32 u4BufEnd)
{
   UINT32 u4Value=0;
   _rBerralShift[u4VldId].u4BufStart=u4BufStart;
   _rBerralShift[u4VldId].u4BufEnd=u4BufEnd;  
   VDEC_INTEGER(u4Value,u4ReadPtr,4,u4BufStart,u4BufEnd);
   _rBerralShift[u4VldId].u4ReadPtr=u4ReadPtr;
   _rBerralShift[u4VldId].i4BitsAvailable=32;
   _rBerralShift[u4VldId].u4Value=u4Value;
}
#endif

#ifdef VDEC_VP8HW_DEBUG

VOID _VDEC_VP8_IO_Log(BOOL fgOnOff)
{
   _fgVdecVPLogSimPat=fgOnOff;
}

BOOL _VDEC_VP8_IO_LogOpen(VOID)
{
  return _fgVdecVPLogSimPat;
}

VOID _VDEC_VP8_IO_Write32(UINT32 u4Base, UINT32 u4Offset,UINT32 u4Value)
{
    if(_fgVdecVPLogSimPat)
    {
        switch(u4Base)
        {
            case VLD_BASE:
                printf("RISCWrite(`VLD + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);
                break;
            case VLD2_BASE:
                printf("RISCWrite(`VLD2 + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);
                break;
            case MC_BASE:
                printf("RISCWrite(`MC + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            case VP8_VLD_BASE:
                printf("RISCWrite(`VP8VLD + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            case VP8_MV_BASE:
                printf("RISCWrite(`VP8MV + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            case VLD_TOP_BASE:
                printf("RISCWrite(`TOP + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            case PP_BASE:
                printf("RISCWrite(`PP + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            case VDEC_MISC_BASE:
                printf("RISCWrite(`MSIC + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
            default:
                printf("RISCWrite(`Unknow + %d*4, 32'h%x);\n",u4Offset>>2,u4Value);            
                break;
        }
        x_thread_delay(1);
    }
    IO_WRITE32(u4Base,u4Offset,u4Value);
}

UINT32 _VDEC_VP8_IO_Read32(UINT32 u4Base, UINT32 u4Offset)
{
    UINT32 u4Value=0;
    u4Value=IO_READ32(u4Base, u4Offset);
    if(_fgVdecVPLogSimPat)
    {
        switch(u4Base)
        {
            case VLD_BASE:
                printf("RISCRead(`VLD + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
            case VLD2_BASE:
                printf("RISCRead(`VLD2 + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
            case MC_BASE:
                printf("RISCRead(`MC + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);            
                break;
            case VP8_VLD_BASE:
                printf("RISCRead(`VP8VLD + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);            
                break;
            case VP8_MV_BASE:
                printf("RISCRead(`VP8MV + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);            
                break;
            case VLD_TOP_BASE:
                printf("RISCRead(`TOP + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);            
                break;
            case PP_BASE:
                printf("RISCRead(`PP + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
            case VDEC_MISC_BASE:
                printf("RISCRead(`MISC + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
			case WEBP_IMGRZ_BASE:
                printf("RISCRead(`IMGRZ + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
            default:
                printf("RISCRead(`Unknow + %d*4)=32'h%x;\n",u4Offset>>2,u4Value);
                break;
        }
        x_thread_delay(1);
    }
    return u4Value;
}
#endif

static VOID _VDEC_VP8_PowerDownSetting(BOOL fgPowerOn)
{
    VDEC_VP8_INFO_T* pVp8Info = VDEC_Vp8GetLockedInstance();
	BOOL fgPowerSetting=FALSE;
    if (!pVp8Info)
    {
        return;
    }

    if (pVp8Info->eDecType == VP8_DEC_TYPE_WEBP)
    {
		fgPowerSetting=TRUE;
    }

#ifdef VP8_ENABLE_POWER_SAVING
    fgPowerSetting=TRUE;
#endif

    if(fgPowerSetting==TRUE)
    {
        if(fgPowerOn)
        {
            #if defined (VDEC_IS_POST_MT5882)
			   if(BSP_GetIcVersion() == IC_VER_5890_AC)
    			{
       				 VDEC_MISC_WRITE32(0xC8, 0xff7f);
					 VDEC_MISC_WRITE32(0x178, 0xfffffffb);
    			}
    			else
    			{
            		VDEC_MISC_WRITE32(0xC8, 0x17F);
					VDEC_MISC_WRITE32(0x178, 0x7B);
    			}
				
			#else
				VDEC_MISC_WRITE32(0xC8, 0x7F);
			#endif
			if(pVp8Info->eDecType == VP8_DEC_TYPE_WEBM)
			{
			        #if defined (VDEC_IS_POST_MT5882)
					VDEC_MISC_WRITE32(0xCC, 0x71201100);
			        #elif defined(VDEC_IS_POST_MT5398)
				VDEC_MISC_WRITE32(0xCC, 0x31A01100);
			        #else
				VDEC_MISC_WRITE32(0xCC, 0x31E01100);
			        #endif
			}
			else if(pVp8Info->eDecType == VP8_DEC_TYPE_WEBP)
			{
			        #if defined (VDEC_IS_POST_MT5882)
					VDEC_MISC_WRITE32(0xCC, 0x7120FD04);
			        #elif defined(VDEC_IS_POST_MT5398)
				VDEC_MISC_WRITE32(0xCC, 0x31A0FD04);
			        #else
				VDEC_MISC_WRITE32(0xCC, 0x31E0FD04);
			        #endif
			}
			else
			{
				ASSERT(0);
			}
        }
		else
		{
			if(pVp8Info->eDecType == VP8_DEC_TYPE_WEBP)
			{
				VDEC_MISC_WRITE32(0xC8, 0xffffffff);
				VDEC_MISC_WRITE32(0xCC, 0xffffffff);
			}
		}
    }
}

static BOOL _VDEC_VP8_WaitVldFetchDone(UINT32 u4VldId)
{
    UINT32 u4Cnt;
 if(u4VldId==VDEC_VLD1)
 {
     if((VDEC_VLD_READ32(RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
     {
         u4Cnt = 0;
         while((VDEC_VLD_READ32(RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
         {
            u4Cnt++;
            if (u4Cnt >= WAIT_THRD)
            {
               LOG(0, "VP8 Vld1 WaitSramStable Fail1\n");
               return (FALSE);
            }
         }
     }
     
#ifdef WAIT_SRAM_STABLE
     u4Cnt = 0;
     if(VDEC_VLD_READ32(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
     {
         while((VDEC_VLD_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
         {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
               LOG(0," VP8 Vld1 WaitSramStable Fail2\n");
               break;
            }
         }
     }
#endif     
   }
   else
   {
     if((VDEC_VLD2_READ32(RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
     {
         u4Cnt = 0;
         while((VDEC_VLD2_READ32(RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
         {
            u4Cnt++;
            if (u4Cnt >= WAIT_THRD)
            {
               LOG(0, "VP8 Vld2 WaitSramStable Fail1\n");
               return (FALSE);
            }
         }
     }
     
#ifdef WAIT_SRAM_STABLE
     u4Cnt = 0;
     if(VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
     {
         while((VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
         {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
               LOG(0," VP8 Vld2 WaitSramStable Fail2\n");
               break;
            }
         }
      }
#endif 
   }
   return (TRUE);
}

BOOL _VDEC_VP8_InitBarrelShifter(UINT32 u4VldId,UINT32 u4ReadPointer, UINT32 u4WritePointer)
{
    BOOL fgFetchOK = FALSE;
    INT32 i;
    UINT32 u4VLDRemainByte;
    u4VLDRemainByte =  u4ReadPointer & 0xf;
    if(u4VldId==VDEC_VLD1)
    {
        for (i = 0; i < 5; i++)
        {
            VDEC_VLD_WRITE32(WO_VLD_WPTR, VDEC_VLD_READ32(WO_VLD_WPTR) | VLD_CLEAR_PROCESS_EN);
            VDEC_VLD_WRITE32(RW_VLD_RPTR, PHYSICAL((UINT32) u4ReadPointer));
            VDEC_VLD_WRITE32(RW_VLD_RPTR, PHYSICAL((UINT32) u4ReadPointer));
            VDEC_VLD_WRITE32(WO_VLD_WPTR, PHYSICAL((UINT32) u4WritePointer));
            VDEC_VLD_WRITE32(WO_VLD_SRST, 0x100);
            VDEC_VLD_WRITE32(WO_VLD_SRST, 0x0);
            VP8_VLD_WRITE32(RW_VP8_STR,0x1);
            VP8_VLD_WRITE32(RW_VP8_STR,0x0);
            // start to fetch data
            VDEC_VLD_WRITE32(RW_VLD_PROC, VLD_INIFET);
        
            if (_VDEC_VP8_WaitVldFetchDone(VDEC_VLD1))
            {
                fgFetchOK = TRUE;
                break;
            }
        }
        
        if (fgFetchOK)
        {
            VDEC_VLD_WRITE32(RW_VLD_PROC, VLD_INIBR); 
            for (i=0;i<u4VLDRemainByte;i++)
            {
#ifndef USE_VP68_SW_VLD
                _VDEC_VP8_VLDShiftBit(VDEC_VLD1,8);
#endif
            }
        }
        
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
          VDEC_VLD2_WRITE32(WO_VLD_WPTR, VDEC_VLD2_READ32(WO_VLD_WPTR) | VLD_CLEAR_PROCESS_EN);
          VDEC_VLD2_WRITE32(RW_VLD_RPTR, u4ReadPointer);
          VDEC_VLD2_WRITE32(RW_VLD_RPTR, u4ReadPointer);
          VDEC_VLD2_WRITE32(WO_VLD_WPTR, u4WritePointer);
          VDEC_VLD2_WRITE32(WO_VLD_SRST, 0x100);
          VDEC_VLD2_WRITE32(WO_VLD_SRST, 0x0);
          VP8_VLD_WRITE32(RW_VP8_STR,0x100);
          VP8_VLD_WRITE32(RW_VP8_STR,0x0);
          // start to fetch data
          VDEC_VLD2_WRITE32(RW_VLD_PROC, VLD_INIFET);
          
          if(_VDEC_VP8_WaitVldFetchDone(VDEC_VLD2))
          {
             fgFetchOK = TRUE;
             break;
          }
       }
           
       if(fgFetchOK)
       {
         VDEC_VLD2_WRITE32(RW_VLD_PROC, VLD_INIBR);           
         for (i=0;i<u4VLDRemainByte;i++)
         {
#ifndef USE_VP68_SW_VLD
           _VDEC_VP8_VLDShiftBit(VDEC_VLD2,8);
#endif
         }
      }
    }

    
    return fgFetchOK;
}

UINT32 _VDEC_VP8_InitBoolDecoder(UINT32 u4VldId)
{
   if(u4VldId==VDEC_VLD1)
   {
      return VP8_VLD_READ32(RO_VP8_BINIT1);
   }
   else
   {
      return VP8_VLD_READ32(RO_VP8_BINIT2);
   }
}

UINT32 _VDEC_VP8_VLDShiftBit(UINT32 u4VldId,UINT32 u4ShiftBits)
{
#ifndef USE_VP68_SW_VLD
   UINT32 u4RetValue=0;
   UINT32 u4RepeatNum=0;
   UINT32 u4VP8_ShiftReg=RO_VP8_VBSR1;
   if(u4VldId==VDEC_VLD2)
   {
      u4VP8_ShiftReg=RO_VP8_VBSR2;
   }
   
   u4RepeatNum=u4ShiftBits>>5;
   while(u4RepeatNum>0)
   {
      VP8_VLD_READ32(u4VP8_ShiftReg+32*4);
      u4RepeatNum--;
   }
   
   u4RepeatNum=u4ShiftBits&0x1f;
   u4RetValue=(VP8_VLD_READ32(u4VP8_ShiftReg+0)>>(32-u4ShiftBits));
   VP8_VLD_READ32(u4VP8_ShiftReg+u4RepeatNum*4);
#if 0//def VDEC_VP8HW_DEBUG
   LOG(2,"_VDEC_VP8_VLDShiftBit(%d,%d)=0x%x\n",u4VldId,u4ShiftBits,u4RetValue);
#endif
   return u4RetValue;
#else
   #if 0//def VDEC_VP8HW_DEBUG
   UINT32 u4RetValue=0;
   u4RetValue=_SwBerrelShift(u4VldId,u4ShiftBits);
   LOG(2,"_VDEC_VP8_VLDShiftBit(%d,%d)=0x%x\n",u4VldId,u4ShiftBits,u4RetValue);
   return u4RetValue;
   #else
   return _SwBerrelShift(u4VldId,u4ShiftBits);
   #endif
#endif
}

UINT32 _VDEC_VP8_VLDReadLiteral(UINT32 u4Bits)
{
#if 1//ndef VDEC_VP8HW_DEBUG
   #ifndef USE_VP68_SW_VLD
      return VP8_VLD_READ32(RO_VP8_VRLR+(u4Bits-1)*4);
   #else
      return _SwBoolReadValue(0,u4Bits);
   #endif
#else
   UINT32 u4Value=0;
#ifndef USE_VP68_SW_VLD
   u4Value= VP8_VLD_READ32(RO_VP8_VRLR+(u4Bits-1)*4);
#else
   u4Value=_SwBoolReadValue(0,u4Bits);
#endif
   LOG(2,"_VDEC_VP8_VLDReadLiteral(%d)=0x%x\n",u4Bits,u4Value);
   return u4Value;
#endif
}


VOID _VDEC_VP8_WraperWrite(UINT32 u4SrcData,UINT32 u4WraperAddr,UINT32 u4DataLen)
{
    UINT32 u4UnitAddr,u4ByteAddr,u4BytesLen;
	UINT8 *puSrcData=(UINT8 *)u4SrcData;
	UINT32 u4WriteData=0,u4WriteIndex=0,u4CtrlVal=0;
	BOOL fgWrite=TRUE,fgAligend=FALSE;
	u4UnitAddr=u4WraperAddr/16;
	u4ByteAddr=3-((u4WraperAddr%16)/4) ;
	//u4BytesLen= u4WraperAddr%4 ? u4WraperAddr%4 : 4;
	u4BytesLen= (u4WraperAddr&0x3) ? (u4WraperAddr&0x3) : 4;
	u4BytesLen=VDEC_MIN(u4BytesLen,u4DataLen);
	u4DataLen-=u4BytesLen;
	
    fgAligend=(((u4SrcData&0x3)==0)&&(u4BytesLen==4)) ? TRUE :FALSE;
	while(fgWrite)
    {
		u4CtrlVal=RW_CC_WRAPPER_ACCESS_WRITE_MARSK|(u4UnitAddr<<2);
		do
        {
            if(fgAligend)
            {
                u4WriteData=*((UINT32 *)(puSrcData+u4WriteIndex));
				u4WriteIndex+=u4BytesLen;
            }
			else
			{
				while(u4BytesLen-- > 0) u4WriteData=(u4WriteData<<8)+puSrcData[u4WriteIndex++];
			}
			
			u4CtrlVal=(u4CtrlVal&~3)|u4ByteAddr;
			VDEC_MC_WRITE32(RW_MC_WRAPPER_ACCES_CTRL,u4CtrlVal);
			VDEC_MC_WRITE32(RW_MC_WRAPPER_DATA,u4WriteData);
			u4ByteAddr--;
			u4BytesLen=VDEC_MIN(4,u4DataLen);
            if(u4BytesLen==0)
            {
               fgWrite=FALSE;
			   break;
            }
			u4DataLen-=u4BytesLen;
        }while(u4ByteAddr<3);
        
        u4ByteAddr=3;
		u4UnitAddr++;
	}

	return;
}

VOID _VDEC_VP8_WraperRead(UINT32 u4TargetBuffer,UINT32 u4WraperAddr,UINT32 u4DataLen)
{
    UINT32 u4UnitAddr,u4ByteAddr,u4BytesLen;
	UINT8 *puSrcData=(UINT8 *)u4TargetBuffer;
	UINT32 u4WriteData=0,u4WriteIndex=0,u4CtrlVal=0;
	BOOL fgRead=TRUE,fgAligend=FALSE;
	u4UnitAddr=u4WraperAddr/16;
	u4ByteAddr=3-((u4WraperAddr%16)/4);
	//u4BytesLen= u4WraperAddr%4 ? u4WraperAddr%4 : 4;
	u4BytesLen= (u4WraperAddr&0x3) ? (u4WraperAddr&0x3) : 4;
	u4BytesLen=VDEC_MIN(u4BytesLen,u4DataLen);
	u4DataLen-=u4BytesLen;
	
    fgAligend=(((u4TargetBuffer&0x3)==0)&&(u4BytesLen==4)) ? TRUE :FALSE;
	while(fgRead)
	{
        u4CtrlVal=u4UnitAddr<<2;
		do
		{
			u4CtrlVal=(u4CtrlVal&~3)|u4ByteAddr;
			VDEC_MC_WRITE32(RW_MC_WRAPPER_ACCES_CTRL,u4CtrlVal);
			u4WriteData=VDEC_MC_READ32(RW_MC_WRAPPER_DATA);
			u4ByteAddr--;
			if(fgAligend)
			{
               *((UINT32 *)(puSrcData+u4WriteIndex))=u4WriteData;
			   u4WriteIndex+=u4BytesLen;
			}
			else
			{
				while(u4BytesLen-- > 0) puSrcData[u4WriteIndex++]=(u4WriteData>>(u4BytesLen*8))&0xff;
			}
			
			u4BytesLen=VDEC_MIN(4,u4DataLen);
			if(u4BytesLen==0)
			{
               fgRead=FALSE;
			   break;
			}
			u4DataLen-=u4BytesLen;
		}while(u4ByteAddr<3);
		
		u4ByteAddr=3;
		u4UnitAddr++;
	}
	
	return;
}

#ifndef VP8_HEADERPARSE_HWACCELATOR
UINT32 _VDEC_VP8_VLDReadBit(UINT32 u4Prob)
{
#ifndef USE_VP68_SW_VLD
   UINT32 u4RetValue=0;
   VP8_VLD_WRITE32(RW_VP8_FWPROB,u4Prob);
   u4RetValue=VP8_VLD_READ32(RO_VP8_VRLR);
   VP8_VLD_WRITE32(RW_VP8_FWPROB,128);
   #if 0//def VDEC_VP8HW_DEBUG
   LOG(2,"_VDEC_VP8_VLDReadBit(%d)=0x%x\n",u4Prob,u4RetValue);
   #endif
   return u4RetValue;
#else
   #if 0//def VDEC_VP8HW_DEBUG
   UINT32 u4RetValue=0;
   u4RetValue=_SwBoolDecode(0,u4Prob);
   LOG(2,"_VDEC_VP8_VLDReadBit(%d)=0x%x\n",u4Prob,u4RetValue);
   return u4RetValue;
   #else
   return _SwBoolDecode(0,u4Prob);
   #endif
#endif
}
#endif

UINT32 _VDEC_VP8_VLDGetReadPtr(UINT32 *pu4Bits)
{
    UINT32 u4DramRptr,u4HalVlaue=0;
    UINT32 u4SramRptr, u4SramWptr,u4SramRcnt;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr=0,u4Bits=0;
    UINT32 u4Cnt = 0;
    UINT32 u4VStart,u4VEnd;
#ifdef WAIT_SRAM_STABLE
    if (VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(5,"WaitSramStable Fail\n");
                break;
            }
        }
    }
#endif

    u4DramRptr = VDEC_VLD2_READ32(RO_VLD_VRPTR);
    u4HalVlaue=VDEC_VLD2_READ32(RO_VLD_VBAR);
    u4SramRptr = u4HalVlaue&0xf;
    u4SramWptr = (u4HalVlaue>>8)&0xf;
    u4SramRcnt = (u4HalVlaue>>24)&0x3;
    
    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
    }
      
    u4Bits=(VP8_VLD_READ32(RW_VP8_CTRL)>>16)&0x3f;
    u4ByteAddr=u4DramRptr+u4SramRcnt*4 +((u4Bits+7)/8)-(u4SramDataSz*16)-20;
    if(pu4Bits)
    {
      *pu4Bits = u4Bits&0x7;
    }
    u4VStart = VDEC_VLD2_READ32(RW_VLD_VSTART) << 6;
    u4VEnd = VDEC_VLD2_READ32(RW_VLD_VEND) << 6;
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


VOID _VDEC_VP8_VLDSetWritePtr(UINT32 u4WritePtr)
{
    UINT32 u4VStart, u4VEnd;

    CRIT_STATE_T crit;
    crit = x_crit_start();    //protect u4RingData
    
    u4VStart = VDEC_VLD2_READ32(RW_VLD_VSTART)<<6;
    u4VEnd = VDEC_VLD2_READ32(RW_VLD_VEND)<<6;    
    ASSERT((u4WritePtr&WPTR_ALIGN_MARK)==0);
    if(u4WritePtr < u4VStart)
    {
        u4WritePtr = u4VEnd - (u4VStart - u4WritePtr);
        ASSERT(0);
    }
    
    if(u4WritePtr >=u4VEnd)
    {
        u4WritePtr = u4VStart + (u4WritePtr - u4VEnd);
        ASSERT(0);
    }
    
    VDEC_VLD2_WRITE32(WO_VLD_WPTR, PHYSICAL(u4WritePtr));
    x_crit_end(crit);
}


VOID _VDEC_VP8_VLD_PowerOff(VOID)
{
   return _VDEC_VP8_PowerDownSetting(FALSE);
}

VOID _VDEC_VP8_VLDReset(UINT32 u4VldId)
{
    if(u4VldId==VDEC_VLD1)
    {     
#if 0
		if(VDEC_VLD_READ32(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
		{
			UINT32 u4Cnt = 0;
			while((VDEC_VLD_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
			{
				u4Cnt++;
				if(u4Cnt >= WAIT_THRD)
				{
					LOG(1, "VP8 VLD1 HWReset WaitSramStable Fail\n");
					break;
				}
			}
		}
#endif   

        VDEC_VLD_WRITE32(WO_VLD_SRST, 0x101);
        _VDEC_CkgenSel(0, VDEC_FMT_VP8, 0);
        _VDEC_VP8_PowerDownSetting(TRUE);
        _VDEC_SysClkSel(0, VDEC_FMT_VP8, 0);
        VDEC_VLD_WRITE32(WO_VLD_SRST, 0);
        VP8_VLD_WRITE32(RW_VP8_CTRL,VDEC_GET_FLAGVAL(1,RW_VP8_VP8FLAG));
    }
    else
    {
#if 0
		if(VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
		{
			UINT32 u4Cnt = 0;
			while((VDEC_VLD2_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
			{
				u4Cnt++;
				if(u4Cnt >= WAIT_THRD)
				{
					LOG(1, "VP8 VLD1 HWReset WaitSramStable Fail\n");
					break;
				}
			}
		}
#endif

        VDEC_VLD2_WRITE32(WO_VLD_SRST, 0x101);
        _VDEC_CkgenSel(0, VDEC_FMT_VP8, 0);
        _VDEC_VP8_PowerDownSetting(TRUE);
        _VDEC_SysClkSel(0, VDEC_FMT_VP8, 0);
        VDEC_VLD2_WRITE32(WO_VLD_SRST, 0);
    }
}


VOID _VDEC_VP8_HwAccCoefProbUpdate(VOID)
{
    UINT32 u4WaitCount=0,u4DivValue=0;
#if 1
   static UINT32 u4MaxCoefProbWaitCount=0;
   VP8_VLD_WRITE32(RW_VP8_CPUT,1);
   while(VP8_VLD_READ32(RW_VP8_CPUT)!=0)
   {
     u4WaitCount++;
     u4DivValue=u4WaitCount/50+u4DivValue;
   }
   if(u4MaxCoefProbWaitCount<u4WaitCount)
   {
     LOG(2,"Get NewCoefProb counter=%d\n",u4WaitCount);
     u4MaxCoefProbWaitCount=u4WaitCount;
   }
#else
   VP8_VLD_WRITE32(RW_VP8_CPUT,1);
   while(VP8_VLD_READ32(RW_VP8_CPUT)==0)
   {
     u4WaitCount++;
     u4DivValue=u4WaitCount/50+u4DivValue;
     if(u4WaitCount>VP8_HWCOEFPROB_ACC_THRESOLD)
     {
       ASSERT(0);
     }
   }
#endif
}

VOID _VDEC_VP8_HwAccMVProbUpdate(VOID)
{
    UINT32 u4WaitCount=0,u4DivValue=0;
#if 1
   static UINT32 u4MaxMvProbWaitCount=0;
   VP8_VLD_WRITE32(RW_VP8_MVPUT,1);
   while(VP8_VLD_READ32(RW_VP8_MVPUT)!=0)
   {
     u4WaitCount++;
     u4DivValue=u4WaitCount/50+u4DivValue;
   }
   if(u4MaxMvProbWaitCount<u4WaitCount)
   {
     LOG(2,"Get NewMVProb counter=%d\n",u4WaitCount);
     u4MaxMvProbWaitCount=u4WaitCount;
   }
#else
   VP8_VLD_WRITE32(RW_VP8_MVPUT,1);
   while(VP8_VLD_READ32(RW_VP8_MVPUT)==0)
   {
     u4WaitCount++;
     u4DivValue=u4WaitCount/50+u4DivValue;
     if(u4WaitCount>VP8_HWMVPROB_ACC_THRESOLD)
     {
       ASSERT(0);
     }
   }
#endif
}

#ifdef VDEC_DEC_REG_QUERY
void VDEC_VP8DumpReg(void)
{
    UINT32 i = 0;
    
    LOG(0, "\n*******Start Dump VLD Registers *******\n");
    LOG(0, "VLD1 ReadPtr: 0x%08x\n", VDEC_VLD_READ32(RW_VLD_RPTR));
    LOG(0, "VLD2 ReadPtr: 0x%08x\n", VDEC_VLD2_READ32(RW_VLD_RPTR));

    /* VP8 VLD */
    LOG(0, "******* VP8 VLD Registers *******\n");
    for (i = 41; i < 68; i++)
    {
        LOG(0, "VP8 VLD %d (0x%04x): 0x%08x\n", i, i * 4, VP8_VLD_READ32(i * 4));
        VDEC_REG_DUMP_DELAY(10);
    }

    for (i = 72; i < 97; i++)
    {
       LOG(0, "VP8 VLD %d (0x%04x): 0x%08x\n", i, i * 4, VP8_VLD_READ32(i * 4));
       VDEC_REG_DUMP_DELAY(10);
    }

    /* TOP VLD */
    LOG(0, "******* TOP VLD Registers *******\n");
    for (i = 0; i < 100; i++)
    {
        LOG(0, "TOP VLD %d (0x%04x): 0x%08x\n", i, i * 4, VDEC_VLD_TOP_READ32(i * 4));
        VDEC_REG_DUMP_DELAY(10);
    }

    /* MC */
    LOG(0, "******* MC Registers *******\n");
    for (i = 0; i < 700; i++)
    {
        LOG(0, "MC %d (0x%04x): 0x%08x\n", i, i * 4, VDEC_MC_READ32(i * 4));
        VDEC_REG_DUMP_DELAY(10);
    }

    /* MV */
    LOG(0, "******* MV Registers *******\n");
    LOG(0, "MV 181 (0x0x02D4): 0x%08x\n", VDEC_MV_READ32(i * 4));


    /* PP */
    LOG(0, "******* PP Registers *******\n");
    for (i = 0; i < 640; i++)
    {
        LOG(0, "PP %d (0x%04x): 0x%08x\n", i, i * 4, VDEC_PP_READ32(i * 4));
        VDEC_REG_DUMP_DELAY(10);
    }

#if defined(VP8_ENABLE_CRC) || defined(WEBP_ENABLE_CRC)
    /* CRC */
    LOG(0, "******* MISC Registers *******\n");
    for (i = 0; i < 10; i++)
    {
        LOG(0, "MISC %d (0x%04x): 0x%08x\n", i, i * 4, VDEC_MISC_READ32(i * 4));
        VDEC_REG_DUMP_DELAY(10);
    }
#endif

    LOG(0, "******* IQ Registers *******\n");
    for(i=0;i<MAX_MB_SEGMENTS;i++)
    {
       VDEC_VLD_WRITE32(RW_VLD_SCL_ADDR,0x100+(i<<4));
       LOG(0, "IQ Y2 %d (0x%04x): 0x%08x\n", i, i<<4,VDEC_VLD_READ32(RW_VLD_SCL_DATA));
       VDEC_REG_DUMP_DELAY(10);

       VDEC_VLD_WRITE32(RW_VLD_SCL_ADDR,0x100+(i<<4)+4);
       LOG(0, "IQ Y1 %d (0x%04x): 0x%08x\n", i, (i<<4)+4, VDEC_VLD_READ32(RW_VLD_SCL_DATA));
       VDEC_REG_DUMP_DELAY(10);
      
       VDEC_VLD_WRITE32(RW_VLD_SCL_ADDR,0x100+(i<<4)+8);
       LOG(0, "IQ UV %d (0x%04x): 0x%08x\n", i, (i<<4)+8, VDEC_VLD_READ32(RW_VLD_SCL_DATA));
       VDEC_REG_DUMP_DELAY(10);
    }
    
    return ;
}

void VDEC_VP8DumpInputWnd()
{
    LOG(0, "*** VLD1 Input Window 0x%X\n", VP8_VLD_READ32(RO_VP8_VBSR1));
    LOG(0, "*** VLD2 Input Window 0x%X\n", VP8_VLD_READ32(RO_VP8_VBSR2));

    return ;
}
#endif //def VDEC_DEC_REG_QUERY

