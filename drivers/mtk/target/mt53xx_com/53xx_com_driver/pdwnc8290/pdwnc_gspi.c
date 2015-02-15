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
 * $RCSfile: pdwnc_gspi.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pdwnc_gspi.c
 *  pdwnc_gspi.c The PDWNC (Power Down Controller) GSPI functions
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_pinmux.h"
#include "x_pdwnc.h"
#include "x_bim.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_ckgen.h"
#include "x_timer.h"
#include "x_hal_arm.h"
#include "x_gpio.h"
#include "drvcust_if.h"
#include "pdwnc_config.h"
#include "pdwnc_debug.h"
#include "pdwnc_hw.h"
#include "pdwnc_t8032.h"
#include "pdwnc_srvad.h"
#include "pdwnc_if.h"
#include "sif_if.h"

#define CC_SUPORT_PDWNC_GSPI 1


#if defined(CC_SUPORT_PDWNC_GSPI)

//-----------------------------------------------------------------------------
/** _PdwncGspiClock() set Clock freq. (Clock rate: 24M/(2*u4ClockDiv + 2))
 *  @param u4ClockDiv, 0x000 ~ 0xFFF.
 *  Clock Min = 2929.6875Hz, Max = 12000000Hz.
 */
//-----------------------------------------------------------------------------
void _PdwncGspiClockFreq(UINT32 u4ClockDiv)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CKCFG,u4ClockDiv,FLD_CLK_DIV);
}

//CPOL = 0, default is low; CPOL = 1, default is high.
void _PdwncGspiClockPol(UINT32 u4ClockPol)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CKCFG,u4ClockPol,FLD_CPOL);
}
//CPHA = 0 or 1.
void _PdwncGspiClockPha(UINT32 u4ClockPha)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CKCFG,u4ClockPha,FLD_CPHA);
}

void _PdwncGspiCsLeadLatency(UINT32 u4CsLead)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CSACC,u4CsLead,FLD_CS_LEAD_LATENCY);
}

void _PdwncGspiCsLagLatency(UINT32 u4CsLag)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CSACC,u4CsLag,FLD_CS_LAG_LATENCY);
}

void _PdwncGspiCsHoldLatency(UINT32 u4CsHold)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CSACC2,u4CsHold,FLD_CS_HOLD_LATENCY);
}
void _PdwncGspiLsbMode(UINT32 u4LsbMode)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CON,u4LsbMode,FLD_LSB_MODE);
}
void _PdwncGspiSetEnable(UINT32 u4Enable)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CON,u4Enable,FLD_GSPI_EN);
}

#if 0
void _PdwncGspiNdataLen(UINT32 u4NdataLen)
{
	vIO32WriteFldAlign(PDWNC_GSPI_NOB,u4NdataLen,FLD_NDATA_LEN);
}

void _PdwncGspiMdataLen(UINT32 u4MdataLen)
{
	vIO32WriteFldAlign(PDWNC_GSPI_NOB,u4MdataLen,FLD_MDATA_LEN);
}
void _PdwncGspiSetIntEnable(UINT32 u4Enable)
{
	vIO32WriteFldAlign(PDWNC_GSPI_INTR_EN,0x1,FLD_GSPI_DONE_INTR_EN); //The data transfer completion interrupt enable.
}

void _PdwncGspiTriger(UINT32 u4Trig)
{
	vIO32WriteFldAlign(PDWNC_GSPI_CON,u4Trig,FLD_GSPI_TRIG);
}

//defined(CC_SUPORT_PDWNC_GSPI_INT)
static void _GspiDoneIsr(UINT16 u2VctId)
{
	//...
	Printf("_GspiDoneIsr .\n");
	
}
#endif




//-----------------------------------------------------------------------------
/** PDWNC_GspiInit() set GSPI agrument
 *  @param prGspiMode->u4Mode  GSPI config.
 *  @param prGspiMode->u4Freq  SPI clock, (uint:1hz,Min = 2929.6875Hz, Max = 12000000Hz).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GspiInit(PDWNC_GSPI_MODE_T *prGspiMode)
{
#if 0//defined(CC_SUPORT_PDWNC_GSPI_INT)
	void (* pfnOldIsr)(UINT16);
	vIO32WriteFldAlign(PDWNC_GSPI_INTR_EN,1,FLD_GSPI_DONE_INTR_EN); 
	if (PDWNC_RegIsr((UINT16)PDWNC_INTNO_GSPI, _GspiDoneIsr, &pfnOldIsr) != OSR_OK)
    {
        Printf("GSPI Init failed\n");
		return;
    }
#endif
	UINT32 u4Val;
	u4Val = 12000000/(prGspiMode->u4Freq) - 1;
	_PdwncGspiClockFreq(u4Val); //24M/(2*CLK_DIV+2)
	_PdwncGspiClockPha((prGspiMode->u4Mode & GSPI_CPHA)? 1:0);
	_PdwncGspiClockPol((prGspiMode->u4Mode & GSPI_CPOL)? 1:0);
	_PdwncGspiLsbMode((prGspiMode->u4Mode & GSPI_LSB_FIRST)? 1:0);
	_PdwncGspiCsLeadLatency(prGspiMode->u4CsLead);
	_PdwncGspiCsLagLatency(prGspiMode->u4CsLag);
	_PdwncGspiCsHoldLatency(prGspiMode->u4CsHold);

	_PdwncGspiSetEnable(TRUE);
	return 0;
}

//-----------------------------------------------------------------------------
/** PDWNC_GspiReadWrite() 
 *  @param fgReadWrite,  GSPI read/write flag.
 *  @param u4Nob, MDATA_LEN * NDATA_LEN is 32bytes,read/write buffer size (bits,1~256).
 *  @param u1RdBuf, read buffer.
 *  @param u1WrBuf, write buffer.  
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GspiReadWrite(UINT32 fgReadWrite, UINT32 u4Nob,UINT8 *u1RdBuf, UINT8 *u1WrBuf)
{
	INT32 i=0,j=0;
	INT32 u4Size=0;
	u4Size = (((u4Nob&0xFF) + 1) * (((u4Nob>>16)&0xFF) + 1));
	if(u4Size > 256) 
	{	
		return -1;//The maximum valid value MDATA_LEN*NDATA_LEN is 32 bytes.
	}
	//1st level SPI data,MDATA_LEN,value=0~255.
	//2st level SPI data,NDATA_LEN,value=0~255.
	vIO32Write4B(PDWNC_GSPI_NOB,u4Nob & 0x00FF00FF);

	if((u1WrBuf != NULL) && (fgReadWrite & GSPI_WRITE))//GSPI Write enable.
	{
		if(IO32ReadFldAlign(PDWNC_GSPI_CON,FLD_LSB_MODE))//LSB_MODE,0:MSB first,1:LSB first.
		{	//LSB
			for(i=0;i<u4Size/0x08;i++)
			{
				vIO32Write1B(PDWNC_GSPI_DATA0+i,u1WrBuf[i]);
			}
		}
		else
		{	//MSB
			j = 0;
			for(i=31;i>=(32-(u4Size/0x08));i--)
			{
				vIO32Write1B(PDWNC_GSPI_DATA0+i,u1WrBuf[j++]);
			}
		}
	}
	vIO32WriteFldAlign(PDWNC_GSPI_INTR_EN,0x1,FLD_GSPI_DONE_INTR_EN); //The data transfer completion interrupt enable.
	vIO32WriteFldAlign(PDWNC_GSPI_CON,0x1,FLD_GSPI_TRIG); //Trigger GSPI.
	while((IO32ReadFldAlign(PDWNC_GSPI_INTR,FLD_GSPI_DONE_INTR) == 0) && (i<300)) //wait status.
	{
		HAL_Delay_us(10);
		i++;
	}

	if((u1RdBuf != NULL) && (fgReadWrite & GSPI_READ)) //GSPI Read enable.
	{
		if(IO32ReadFldAlign(PDWNC_GSPI_CON,FLD_LSB_MODE))//LSB_MODE,0:MSB first,1:LSB first.
		{	//LSB
			j=0;
			for(i=(32-(u4Size/0x08));i<32;i++)
			{
				u1RdBuf[j++] = u1IO32Read1B(PDWNC_GSPI_SHREG0+i);
			}
			
		}
		else
		{ 	//MSB
			j=0;
			for(i=((u4Size/0x08) - 1);i>=0;i--)
			{
				u1RdBuf[j++] = u1IO32Read1B(PDWNC_GSPI_SHREG0+i);
			}
		}
	}

	for(i=0;i<0x20;i+=4)
	{
		vIO32Write4B(PDWNC_GSPI_DATA0+i,0x00000000);//Clear write date register.
	}
	vIO32WriteFldAlign(PDWNC_INTCLR,0x01,FLD_GSPI_INTCLR); //clear interrupt status.
	return 0;
}

#endif








