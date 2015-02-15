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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: imgrz_coeff.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
 
/** @file imgrz_coeff.c
 *  coefficient interfaces of image resizer driver
 *
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "imgrz_dif.h"
#include "imgrz_hw.h"
#include "imgrz_cmdque.h"
#include "imgrz_debug.h"
#include "imgrz_coeff.h"


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

// if (tgW / srcW) >= 1.0   (scaling up)
static UINT32 _u4H8TapFilterTable_16[D_RZ_H8TAP_COEF_NUM] = 
{
    0x00000000,
    0xfef40300,
    0xf9ea0500,
    0xf0e30700,
    0xe5de0800,
    0xd6db0800,
    0xc5da0800,
    0xb1db0700,
    0x9ddd0600,
    0x87e10500,
    0x70e50400,
    0x5aea0300,
    0x44ef0200,
    0x31f40100,
    0x20f80000,
    0x0ffc0000,
    0x44444448,
    0x44444444
};

// if 0.5 <= (tgW / srcW) < 1.0     (scaling down)
static UINT32 _u4H8TapFilterTable_8[D_RZ_H8TAP_COEF_NUM] = 
{
    0x845000ee,
    0x844bfdef,
    0x8345faf0,
    0x8340f7f1,
    0x813af5f2,
    0x7f35f3f3,
    0x7d2ff1f4,
    0x7a29eff6,
    0x7724eef7,
    0x741fedf8,
    0x6e1aedfa,
    0x6915edfb,
    0x6610ecfc,
    0x5f0cedfd,
    0x5c07edfe,
    0x5404eeff,
    0x33333331,
    0x33333333
};

// if 0.25 <= (tgW / srcW) < 0.5    (scaling down)
static UINT32 _u4H8TapFilterTable_4[D_RZ_H8TAP_COEF_NUM] = 
{
    0x3833220f,
    0x3831210e,
    0x3830200d,
    0x382f1f0c,
    0x382f1d0b,
    0x372e1c0a,
    0x372d1b09,
    0x372c1a08,
    0x362b1807,
    0x342a1706,
    0x34291605,
    0x34281504,
    0x33271403,
    0x34251302,
    0x34241102,
    0x34231001,
    0x00000000,
    0x00000000
};

// if 0.125 <= (tgW / srcW) < 0.25  (scaling down)
static UINT32 _u4H8TapFilterTable_2[D_RZ_H8TAP_COEF_NUM] = 
{
    0x3c24211d,
    0x2524211c,
    0x2524211c,
    0x2524201b,
    0x2524201b,
    0x2523201b,
    0x25231f1a,
    0x25231f1a,
    0x24231f1a,
    0x24231f19,
    0x26221e19,
    0x25221e18,
    0x24221e18,
    0x25221d18,
    0x24221d17,
    0x25211d17,
    0x00000000,
    0x00000000
};

// if 0.0625 <= (tgW / srcW) < 0.125    (scaling down)
static UINT32 _u4H8TapFilterTable_1[D_RZ_H8TAP_COEF_NUM] = 
{
    0x3f21201f,
    0x2121201f,
    0x2121201f,
    0x2121201f,
    0x2121201f,
    0x2121201f,
    0x2121201f,
    0x2121201e,
    0x2121201e,
    0x2121201e,
    0x2021201e,
    0x2021201e,
    0x21211f1e,
    0x22201f1e,
    0x22201f1e,
    0x22201f1e,
    0x00000000,
    0x00000000 
};

// if (tgW / srcW) < 0.0625     (scaling down)
static UINT32 _u4H8TapFilterTable_0[D_RZ_H8TAP_COEF_NUM] = 
{
    0x40202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x20202020,
    0x00000000,
    0x00000000
};

// UI scaling up
static UINT32 _u4H8TapFilterTable_Bilinear[D_RZ_H8TAP_COEF_NUM] = 
{
	0x00000000,
	0xf0000000,
	0xe0000000,
	0xd0000000,
	0xc0000000,
	0xb0000000,
	0xa0000000,
	0x90000000,
	0x80000000,
	0x70000000,
	0x60000000,
	0x50000000,
	0x40000000,
	0x30000000,
	0x20000000,
	0x10000000,
	0x00000008,
	0x00000000
};

static UINT32 _u4H8TapFilterTable_Rm[D_RZ_H8TAP_COEF_NUM] = 
{
    0x0,
    0xf0000000,
    0xe0000000,
    0xd0000000,
    0xc0000000,
    0xb0000000,
    0xa0000000,
    0x90000000,
    0x80000000,
    0x70000000,
    0x60000000,
    0x50000000,
    0x40000000,
    0x30000000,
    0x20000000,
    0x10000000,
    0x00000008,
    0x00000000
};

#if 0
static UINT32 _u4H8TapFilterTable_Repeat[D_RZ_H8TAP_COEF_NUM] = 
{
	0x00000000,
	0xff000000,
	0xff000000,
	0xff000000,
	0xff000000,
	0xff000000,
	0xff000000,
	0xff000000,
	0x80000000,
	0x01000000,
	0x01000000,
	0x01000000,
	0x01000000,
	0x01000000,
	0x01000000,
	0x01000000,
	0x00000008,
	0x00000000
};
#endif

// if (tgH / srcH) >= 1.0   (scaling up)
static UINT32 _u4V4TapFilterTable_16[D_RZ_V4TAP_COEF_NUM] = 
{
    0x01f40000,
    0xf8e2ffea,
    0xdfd9eddd,
    0xbad9ced8,
    0x8edfa5db,
    0x5fe976e4,
    0x33f348ee,
    0x0ffc1ff8,
    0x00000005 
};

// if 0.5 <= (tgH / srcH) < 1.0     (scaling down)
static UINT32 _u4V4TapFilterTable_8[D_RZ_V4TAP_COEF_NUM] = 
{
    0x72417446,
    0x6f36703b,
    0x6a2c6c31,
    0x65226827,
    0x6019621e,
    0x59115c15,
    0x510a560d,
    0x4a034f06,
    0x00000000 
};

// if 0.25 <= (tgH / srcH) < 0.5    (scaling down)
static UINT32 _u4V4TapFilterTable_4[D_RZ_V4TAP_COEF_NUM] = 
{
    0x4b427a43,
    0x4a3f4b40,
    0x493c4a3e,
    0x483a493b,
    0x47374838,
    0x47344636,
    0x45324533,
    0x442f4431,
    0x00000000 
};

// if 0.125 <= (tgW / srcW) < 0.25  (scaling down)
static UINT32 _u4V4TapFilterTable_2[D_RZ_V4TAP_COEF_NUM] = 
{
    0x43417e41,
    0x42404340,
    0x423f4240,
    0x423e423f,
    0x423e423e,
    0x423d423d,
    0x423c413d,
    0x403c413c,
    0x00000000 
};

// if 0.0625 <= (tgW / srcW) < 0.125    (scaling down)
static UINT32 _u4V4TapFilterTable_1[D_RZ_V4TAP_COEF_NUM] = 
{
    0x41408040,
    0x41404140,
    0x41404140,
    0x40404140,
    0x413f4040,
    0x403f403f,
    0x403f403f,
    0x403f403f,
    0x00000000 
};

// if (tgW / srcW) < 0.0625     (scaling down)
static UINT32 _u4V4TapFilterTable_0[D_RZ_V4TAP_COEF_NUM] = 
{
    0x40408040,
    0x40404040,
    0x40404040,
    0x40404040,
    0x40404040,
    0x40404040,
    0x40404040,
    0x40404040,
    0x00000000 
};

// UI scaling down
static UINT32 _u4V4TapFilterTable_Bilinear[D_RZ_V4TAP_COEF_NUM] = 
{
	0xf0000000,
	0xd000e000,
	0xb000c000,
	0x9000a000,
	0x70008000,
	0x50006000,
	0x30004000,
	0x10002000,
	0x00000001
};

static UINT32 _u4V4TapFilterTable_Rm[]=
{
    0xf0000000,
    0xd000e000,
    0xb000c000,
    0x9000a000,
    0x70008000,
    0x50006000,
    0x30004000,
    0x10002000,
    0x1
};

#if 0
static UINT32 _u4V4TapFilterTable_Repeat[D_RZ_V4TAP_COEF_NUM] = 
{
	0xff000000,
	0xff00ff00,
	0xff00ff00,
	0xff00ff00,
	0x01008000,
	0x01000100,
	0x01000100,
	0x01000100,
	0x00000001
};
#endif


#ifdef IMGRZ_AUTO_TEST
extern INT32 RzEmuGetParam(INT32 i4ParamType,INT32 i4Param);
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


void _IMGRZ_CalcHVFactorForRm(UINT32 u4HorV,RZ_SCL_FAC_OFT_T *prFacOft,UINT32 u4InWH,UINT32 u4OutWH)
{
    UINT32 hfactor;
    UINT32 hoffset;
    UINT32 vfactor;
    UINT32 voffset;
    INT32  iUxR, iUyLB;
    INT32  Hprime, D;
    INT32  m, n;
    INT32  ax_initial, ax_increment;
    INT32  iUyL_inc;
    INT32  uTemp;

   
    if(u4HorV) // H
    {
      m = 0;
      uTemp = u4InWH;
      while (uTemp > 0)
      {
        m=m+1;
        uTemp = uTemp>>1;
      }
     
      /* check for case when uInWidth is power of two */
      if (u4InWH == (1<<(m-1))) m=m-1;
      
      Hprime = 1<<m;
      D = (64*Hprime)/16;
    
      /* initial x displacement and the x increment are independent of row */
      /* so compute once only */
      /* iUxL and iUxR are independent of row, so compute once only */
      iUxR = ((((u4InWH - u4OutWH)<<1))<<(4+m));    /* numerator part */
      /* complete iUxR init by dividing by H with rounding to nearest integer, */
      /* half-integers away from 0 */
      if (iUxR >= 0)
        iUxR = (iUxR + (u4OutWH>>1))/u4OutWH;
      else
        iUxR = (iUxR - (u4OutWH>>1))/u4OutWH;
      
      ax_initial = iUxR + (D>>1);
      ax_increment = (Hprime<<6) + (iUxR<<1);

      hfactor = ax_increment<<(18-(m+6));
      hoffset = ax_initial<<(18-(m+6));
      prFacOft->u4YFactor=hfactor;
      prFacOft->u4CrFactor=hfactor;
      prFacOft->u4CbFactor=hfactor;
      prFacOft->u4YOffset=hoffset;
      prFacOft->u4CbOffset=hoffset;
      prFacOft->u4CrOffset=hoffset;
    }
    else  //V
    {

      n = 0;
      uTemp = u4InWH;
      while (uTemp > 0)
      {
        n=n+1;
        uTemp = uTemp>>1;
      }
      /* check for case when uInHeight is power of two */
      if (u4InWH == (1<<(n-1))) n=n-1;
      
      iUyLB = ((u4InWH - u4OutWH)<<(n+5)); /* numerator */
      /* complete iUyLB by dividing by V with rounding to nearest integer, */
      /* half-integers away from 0 */
      if (iUyLB >= 0)
        iUyLB = (iUyLB + (u4OutWH>>1))/u4OutWH;
      else
        iUyLB = (iUyLB - (u4OutWH>>1))/u4OutWH;
      iUyL_inc = (iUyLB<<1);

      vfactor = ((1<<(6+n)) + iUyL_inc) << (18-(n+6));
      voffset = (iUyLB + (1<<(1+n))) << (18-(n+6));
   
      prFacOft->u4YFactor=vfactor;
      prFacOft->u4CrFactor=vfactor;
      prFacOft->u4CbFactor=vfactor;
      prFacOft->u4YOffset=voffset;
      prFacOft->u4CbOffset=voffset;
      prFacOft->u4CrOffset=voffset;
    }
 
    return;
}

//-------------------------------------------------------------------------
/** _IMGRZ_LoadH8TapCoeffTbl
 *  1. select H8Tap filter coefficient table
 *  2. set H8Tap filter coefficients
 */
//-------------------------------------------------------------------------
void _IMGRZ_LoadH8TapCoeffTbl(UINT32 u4SrcW, UINT32 u4TgW)
{
    UINT32 u4Ratio;
    
    u4Ratio = ((u4TgW * D_RZ_H8TAP_RANGE_SCALE_UP) / (u4SrcW));

	if (_IMGRZ_GetScaleOpt() == E_RZ_INOUT_TYPE_OSDUI)
    {
    	_IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_Bilinear);    	
		return;
	}

    if(_IMGRZ_GetScaleOpt()==E_RZ_INOUT_TYPE_VDORMRPR)
    {
      _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_Rm);
      return;
    }
    
    if (u4Ratio >= D_RZ_H8TAP_RANGE_SCALE_UP)
    {
        // scaling up
        _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_16);
    }
    else 
    {
        // scaling down
        if ((u4Ratio >= (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 1)) &&
            (u4Ratio <  (UINT32)D_RZ_H8TAP_RANGE_SCALE_UP))
        {
            _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_8);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 2)) &&
                 (u4Ratio <  (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 1)))
        {
            _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_4);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 3)) &&
                 (u4Ratio <  (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 2)))
        {
            _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_2);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 4)) &&
                 (u4Ratio <  (UINT32)(D_RZ_H8TAP_RANGE_SCALE_UP >> 3)))
        {
            _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_1);
        }
        else
        {
            _IMGRZ_SetH8TapFilterCoeff(_u4H8TapFilterTable_0);
        }
    }
}


//-------------------------------------------------------------------------
/** _IMGRZ_CalcHsaScaleFacOft
 *  calculate HSA scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_CalcHsaScaleFacOft(RZ_SCL_FAC_OFT_T *prFacOft, 
    UINT32 u4YSrcW, UINT32 u4CbSrcW, UINT32 u4CrSrcW, 
    UINT32 u4YTgW, UINT32 u4CbTgW, UINT32 u4CrTgW)
{
    IMGRZ_ASSERT(prFacOft != NULL);

    // Y
    prFacOft->u4YFactor = (((D_RZ_SCALE_FAC_ONE * u4YTgW) + (u4YSrcW >> 1)) / (u4YSrcW));
    prFacOft->u4YOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4YFactor);

    // Cb
    prFacOft->u4CbFactor = (((D_RZ_SCALE_FAC_ONE * u4CbTgW) + (u4CbSrcW >> 1)) / (u4CbSrcW));
    prFacOft->u4CbOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4CbFactor);

    // Cr
    prFacOft->u4CrFactor = (((D_RZ_SCALE_FAC_ONE * u4CrTgW) + (u4CrSrcW >> 1)) / (u4CrSrcW));
    prFacOft->u4CrOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4CrFactor);
}


//-------------------------------------------------------------------------
/** _IMGRZ_CalcH8TapScaleFacOft
 *  calculate H8Tap scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_CalcH8TapScaleFacOft(RZ_SCL_FAC_OFT_T *prFacOft, 
    UINT32 u4YSrcW, UINT32 u4CbSrcW, UINT32 u4CrSrcW, 
    UINT32 u4YTgW, UINT32 u4CbTgW, UINT32 u4CrTgW)
{
    IMGRZ_ASSERT(prFacOft != NULL);

   	// Y
   	prFacOft->u4YFactor = (((D_RZ_SCALE_FAC_H8V4* u4YSrcW) + (u4YTgW >> 1)) / (u4YTgW)) * D_RZ_SCALE_OLD_TRANSFER;
   
    // Cb
   	prFacOft->u4CbFactor = (((D_RZ_SCALE_FAC_H8V4* u4CbSrcW) + (u4CbTgW >> 1)) / (u4CbTgW)) * D_RZ_SCALE_OLD_TRANSFER;
    
    // Cr
   	prFacOft->u4CrFactor = (((D_RZ_SCALE_FAC_H8V4* u4CrSrcW) + (u4CrTgW >> 1)) / (u4CrTgW)) * D_RZ_SCALE_OLD_TRANSFER;

	prFacOft->u4YOffset = 0;
	prFacOft->u4CbOffset = 0;
    prFacOft->u4CrOffset = 0;    
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetHoriFilterOpt
 *  set hori-filter option
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetHoriFilterOpt(
    UINT32 u4YSrcW, UINT32 u4CbSrcW, UINT32 u4CrSrcW, 
    UINT32 u4YTgW, UINT32 u4CbTgW, UINT32 u4CrTgW,
    UINT32 u4FilterType,UINT32 u4IsPartialUpdate)
{
    UINT32 u4H8TapValidRatio = D_RZ_H8TAP_VALID_RATIO;
    BOOL fgH8TapFilter = FALSE;
    RZ_SCL_FAC_OFT_T rFacOft;
    UINT32 u4InOutType;
    if ((u4YSrcW  == u4YTgW) && 
        (u4CbSrcW == u4CbTgW) && 
        (u4CrSrcW == u4CrTgW))
    {
        // bypass H8Tap & Hsa filter
        _IMGRZ_BypassH8TapFilter();
        _IMGRZ_BypassHsaFilter();
		// select & set H8TapFilter coefficients
        _IMGRZ_LoadH8TapCoeffTbl(u4YSrcW, u4YTgW);
        return;
    }

    u4InOutType=_IMGRZ_GetScaleOpt();
	if (u4InOutType== E_RZ_INOUT_TYPE_OSDUI)
    {
    	if (u4YSrcW <= u4YTgW)
    	{
        	fgH8TapFilter = TRUE;
    	}
    }
    else if(u4InOutType==E_RZ_INOUT_TYPE_VDOENCODE)
    {
        fgH8TapFilter = FALSE;
    }
    else if(u4InOutType==E_RZ_INOUT_TYPE_VDORMRPR)
    {
        fgH8TapFilter=TRUE;
    }
	else
	{
    	// for hw limitation & quality
    	// use H8Tap filter if horizontal down scaling factor >= 1/8
    	if (u4YSrcW >= u4YTgW)
    	{
        	if (u4YSrcW <= (u4YTgW * u4H8TapValidRatio))
     	    {
        	    fgH8TapFilter = TRUE;
        	}
    	}
    	else    // hori scaling up
    	{
        	fgH8TapFilter = TRUE;
    	}
	}	
#ifdef IMGRZ_AUTO_TEST
		{
		  INT32 i4FilterType=0;
		  i4FilterType=RzEmuGetParam(3,0);
	
		  if(i4FilterType==1)
		  {
			  fgH8TapFilter=FALSE;
		  }
		  else if(i4FilterType==2)
		  {
			  fgH8TapFilter=TRUE;
		  }
		  else
		  {
			 // 0--do defautl
		  }
		}
#else
        if(u4FilterType==1)
        {
			fgH8TapFilter=FALSE;
        }
		else if(u4FilterType==2)
		{
		    fgH8TapFilter=TRUE;
		}
#endif
    
	//#ifdef CC_53XX_RESIZER_PATIAL_UPDATE
	if(u4IsPartialUpdate)
		fgH8TapFilter = TRUE;
    
    if (fgH8TapFilter)  // H8Tap
    {
        // bypass Hori-Src-Accumulator (HSA)
        _IMGRZ_BypassHsaFilter();

        // calculate H8Tap factor & offset
        if(u4InOutType==E_RZ_INOUT_TYPE_VDORMRPR)
        {
            _IMGRZ_CalcHVFactorForRm(1,&rFacOft,u4YSrcW,u4YTgW);
        }
        else
        {
            _IMGRZ_CalcH8TapScaleFacOft(&rFacOft, 
                u4YSrcW, u4CbSrcW, u4CrSrcW, 
                u4YTgW, u4CbTgW, u4CrTgW);
        }
    
        // set H8Tap scaling factor & offset
        _IMGRZ_SetH8TapScaleFacOft(&rFacOft);
        
        // select & set H8TapFilter coefficients
        _IMGRZ_LoadH8TapCoeffTbl(u4YSrcW, u4YTgW);
    }
    else                // Hsa
    {
        // bypass H8Tap filter
        _IMGRZ_BypassH8TapFilter();

        if(_IMGRZ_GetScaleOpt()==E_RZ_INOUT_TYPE_VDOENCODE)
        {
           u4YTgW=u4YSrcW/4;
        }

        // calculate Hsa factor & offset
        _IMGRZ_CalcHsaScaleFacOft(&rFacOft, 
            u4YSrcW, u4CbSrcW, u4CrSrcW, 
            u4YTgW, u4CbTgW, u4CrTgW);

        // set Hsa scaling factor & offset
        _IMGRZ_SetHsaScaleFacOft(&rFacOft);
    }
}


//-------------------------------------------------------------------------
/** _IMGRZ_LoadV4TapCoeffTbl
 *  1. select V4Tap filter coefficient table
 *  2. set V4Tap filter coefficients
 */
//-------------------------------------------------------------------------
void _IMGRZ_LoadV4TapCoeffTbl(UINT32 u4SrcW, UINT32 u4TgW)
{
    UINT32 u4Ratio;
    
    u4Ratio = ((u4TgW * D_RZ_V4TAP_RANGE_SCALE_UP) / (u4SrcW));

	if (_IMGRZ_GetScaleOpt() == E_RZ_INOUT_TYPE_OSDUI)
    {
    	_IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_Bilinear);    	
		return;
	}

    if(_IMGRZ_GetScaleOpt()==E_RZ_INOUT_TYPE_VDORMRPR)
    {
        _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_Rm);
        return ;
    }
    
    if (u4Ratio >= D_RZ_V4TAP_RANGE_SCALE_UP)
    {
        // scaling up
        _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_16);
    }
    else 
    {
        // scaling down
        if ((u4Ratio >= (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 1)) &&
            (u4Ratio <  (UINT32)D_RZ_V4TAP_RANGE_SCALE_UP))
        {
            _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_8);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 2)) &&
                 (u4Ratio <  (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 1)))
        {
            _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_4);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 3)) &&
                 (u4Ratio <  (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 2)))
        {
            _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_2);
        }
        else if ((u4Ratio >= (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 4)) &&
                 (u4Ratio <  (UINT32)(D_RZ_V4TAP_RANGE_SCALE_UP >> 3)))
        {
            _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_1);
        }
        else
        {
            _IMGRZ_SetV4TapFilterCoeff(_u4V4TapFilterTable_0);
        }
    }
}


//-------------------------------------------------------------------------
/** _IMGRZ_CalcVSclScaleFacOft
 *  calculate VScl scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_CalcVSclScaleFacOft(RZ_SCL_FAC_OFT_T *prFacOft, 
    UINT32 u4YSrcH, UINT32 u4CbSrcH, UINT32 u4CrSrcH, 
    UINT32 u4YTgH, UINT32 u4CbTgH, UINT32 u4CrTgH)
{
    IMGRZ_ASSERT(prFacOft != NULL);

    // Y
    if (u4YTgH < u4YSrcH)
    {
        prFacOft->u4YFactor = (((D_RZ_SCALE_FAC_ONE * u4YTgH) + (u4YSrcH >> 1)) / (u4YSrcH));
        if(prFacOft->u4YFactor&0x3)
        {
          prFacOft->u4YFactor = prFacOft->u4YFactor + 4 - (prFacOft->u4YFactor&0x3);
        }
        prFacOft->u4YOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4YFactor);
        if(prFacOft->u4YOffset == 0)
        {
            prFacOft->u4VupY = IMGRZ_ENABLE;
        }
        else
        {   
            prFacOft->u4VupY = IMGRZ_DISABLE;
        }
    }
    else
    {
        prFacOft->u4YFactor = (((D_RZ_SCALE_FAC_ONE * u4YSrcH) + (u4YTgH >> 1)) / (u4YTgH));
		prFacOft->u4YOffset = 0;
        prFacOft->u4VupY    = IMGRZ_ENABLE;
    }

    // Cb
    if (u4CbTgH < u4CbSrcH)
    {
        prFacOft->u4CbFactor = (((D_RZ_SCALE_FAC_ONE * u4CbTgH) + (u4CbSrcH >> 1)) / (u4CbSrcH));
        if(prFacOft->u4CbFactor&0x3)
        {
          prFacOft->u4CbFactor = prFacOft->u4CbFactor + 4 - (prFacOft->u4CbFactor&0x3);
        }
	prFacOft->u4CbOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4CbFactor);
        if(prFacOft->u4CbOffset == 0)
        {
            prFacOft->u4VupCb    = IMGRZ_ENABLE;
        }
        else
        {   
            prFacOft->u4VupCb    = IMGRZ_DISABLE;
        }
    }
    else
    {
        prFacOft->u4CbFactor = (((D_RZ_SCALE_FAC_ONE * u4CbSrcH) + (u4CbTgH >> 1)) / (u4CbTgH));
        prFacOft->u4CbOffset = 0;
        prFacOft->u4VupCb    = IMGRZ_ENABLE;
    }

    // Cr
    if (u4CrTgH < u4CrSrcH)
    {
        prFacOft->u4CrFactor = (((D_RZ_SCALE_FAC_ONE * u4CrTgH) + (u4CrSrcH >> 1)) / (u4CrSrcH));
        if(prFacOft->u4CrFactor&0x3)
        {
          prFacOft->u4CrFactor = prFacOft->u4CrFactor + 4 - (prFacOft->u4CrFactor&0x3);
        }
        prFacOft->u4CrOffset = (D_RZ_SCALE_FAC_ONE - prFacOft->u4CrFactor);
        if(prFacOft->u4CrOffset == 0)
        {
            prFacOft->u4VupCr  = IMGRZ_ENABLE;
        }
        else
        {   
            prFacOft->u4VupCr = IMGRZ_DISABLE;
        }
    }
    else
    {
        prFacOft->u4CrFactor = (((D_RZ_SCALE_FAC_ONE * u4CrSrcH) + (u4CrTgH >> 1)) / (u4CrTgH));
        prFacOft->u4CrOffset = 0;
        prFacOft->u4VupCr    = IMGRZ_ENABLE;
    }
}


//-------------------------------------------------------------------------
/** _IMGRZ_CalcV4TapScaleFacOft
 *  calculate V4Tap scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_CalcV4TapScaleFacOft(RZ_SCL_FAC_OFT_T *prFacOft, 
    UINT32 u4YSrcH, UINT32 u4CbSrcH, UINT32 u4CrSrcH, 
    UINT32 u4YTgH, UINT32 u4CbTgH, UINT32 u4CrTgH)
{
    IMGRZ_ASSERT(prFacOft != NULL);

        // Y
    prFacOft->u4YFactor = (((D_RZ_SCALE_FAC_H8V4* u4YSrcH) + (u4YTgH >> 1)) / (u4YTgH)) * D_RZ_SCALE_OLD_TRANSFER;
    prFacOft->u4YOffset = 0;
        
        // Cb
    prFacOft->u4CbFactor = (((D_RZ_SCALE_FAC_H8V4* u4CbSrcH) + (u4CbTgH >> 1)) / (u4CbTgH)) * D_RZ_SCALE_OLD_TRANSFER;
    prFacOft->u4CbOffset = 0;
        
        // Cr
    prFacOft->u4CrFactor = (((D_RZ_SCALE_FAC_H8V4* u4CrSrcH) + (u4CrTgH >> 1)) / (u4CrTgH)) * D_RZ_SCALE_OLD_TRANSFER;
    prFacOft->u4CrOffset = 0;       
 
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetVertFilterOpt
 *  set vert-filter option
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetVertFilterOpt(
    UINT32 u4YSrcH, UINT32 u4CbSrcH, UINT32 u4CrSrcH, 
    UINT32 u4YTgH, UINT32 u4CbTgH, UINT32 u4CrTgH, UINT32 u4YTgW, 
    UINT32 u4FilterType,E_RZ_INOUT_TYPE_T eType)
{
    BOOL fgV4TapFilter = TRUE;
    RZ_SCL_FAC_OFT_T rFacOft;

    if ((u4YSrcH  == u4YTgH) && 
        (u4CbSrcH == u4CbTgH) && 
        (u4CrSrcH == u4CrTgH))
    {
        // enable VScl filter
        _IMGRZ_SwitchToVSclFilter(u4YSrcH, u4YTgH, u4YTgW, eType);

        // bypass VScl & V4Tap filter
        _IMGRZ_BypassVSclFilter();
        _IMGRZ_BypassV4TapFilter();
        _IMGRZ_LoadV4TapCoeffTbl(u4YSrcH, u4YTgH);
        return;
    }

    // hw limitation
    if(eType == E_RZ_INOUT_TYPE_JPGMD)
    {
        fgV4TapFilter = FALSE;
    }

	if(eType == E_RZ_INOUT_TYPE_OSDUI)
    {
    	if (u4YSrcH > u4YTgH)
    	{
        	fgV4TapFilter = FALSE;
    	}
    }
    else if(eType==E_RZ_INOUT_TYPE_VDOENCODE)
    {
       fgV4TapFilter=FALSE;
    }
    else if(eType==E_RZ_INOUT_TYPE_VDORMRPR)
    {
        fgV4TapFilter=TRUE;
    }
#ifdef IMGRZ_AUTO_TEST
    {
      INT32 i4FilterType=0;
	  
	  i4FilterType=RzEmuGetParam(4,0);
	 // (0,&i4FilterType);

	  if(i4FilterType==1)
	  {
          fgV4TapFilter=FALSE;
	  }
	  else if(i4FilterType==2)
	  {
          fgV4TapFilter=TRUE;
	  }
	  else
	  {
         // 0--do defautl
	  }
	  

    }
#else
    if(u4FilterType==1)
    {
		fgV4TapFilter=FALSE;
    }
	else if(u4FilterType==2)
	{
		fgV4TapFilter=TRUE;
	}
#endif
    
    if (fgV4TapFilter)  // V4Tap
    {
        // enable V4Tap filter
        _IMGRZ_SwitchToV4TapFilter();

        // bypass VScl filter
        _IMGRZ_BypassVSclFilter();
        
        // calculate V4Tap factor & offset
        if(eType==E_RZ_INOUT_TYPE_VDORMRPR)
        {
            _IMGRZ_CalcHVFactorForRm(0,&rFacOft,u4YSrcH,u4YTgH);
        }
        else
        {
            _IMGRZ_CalcV4TapScaleFacOft(&rFacOft, 
                u4YSrcH, u4CbSrcH, u4CrSrcH, 
                u4YTgH, u4CbTgH, u4CrTgH);
        }

        // set V4Tap scaling factor & offset
        _IMGRZ_SetV4TapScaleFacOft(&rFacOft);
        
        // select & set V4Tap coefficients
        _IMGRZ_LoadV4TapCoeffTbl(u4YSrcH, u4YTgH);
    }
    else                // VScl
    {
        // enable VScl filter
        _IMGRZ_SwitchToVSclFilter(u4YSrcH, u4YTgH, u4YTgW, eType);

        // bypass V4Tap filter
        _IMGRZ_BypassV4TapFilter();

        if(eType==E_RZ_INOUT_TYPE_VDOENCODE)
        {
           u4YTgH=u4YSrcH/4;
        }

        // calculate VScl factor & offset
        _IMGRZ_CalcVSclScaleFacOft(&rFacOft, 
            u4YSrcH, u4CbSrcH, u4CrSrcH, 
            u4YTgH, u4CbTgH, u4CrTgH);
        // set VScl scaling factor & offset
        _IMGRZ_SetVSclScaleFacOft(&rFacOft);
    }
}
//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------






