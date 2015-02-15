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
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_dvbt_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pd_dvbt_if.h
 *  Add your description here.
 */

#ifndef PD_DVBS_IF_H
#define PD_DVBS_IF_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "u_handle.h"
#include "u_tuner.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct _DVBS_PD_CTX_T
{
    void*                  pDemodCtx;
    x_break_fct     isBreak;
    HANDLE_T                hHalLock;
    HANDLE_T    t_escape_mon_suspend;
    BOOL        fgDisStatus;

    HANDLE_T        t_monitor_thread;
    BOOL            fgBreakMonitor;
    BOOL            fgPIMoniStatus; //1: doing monitor 0:not doing monitor
    BOOL            fgMonitorThreadRunning;
//d120718_Haibo:single cable info
	BOOL       	    fgUniCab;/*To indicate whether current antenna type is single cable(TRUE) or others(FALSE) */
    TUNER_UNICABLE_CC_T t_UniCab_info;/*single cable info for ODU_CHANNEL_CHANGE*/
    
} DVBS_PD_CTX_T, *PDVBS_PD_CTX_T;

typedef struct CurrnetChannel
{
		UINT32  m_uiFrequency_kHz;		///< The channel carrier frequency in units of kHz. 
		UINT32  m_uiSymbolRate_Hz;
		TUNER_FEC_INNER_T m_fec;
}DVBS2_CHANNEL_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------- 
/** DVBT_SetDmxExt
 *  switch Demux input to external MT8295 and make sure Demod clock is on while switching
 *  @param  void.
 *  @retval void.   
 */
//-----------------------------------------------------------------------------
void  DVBS_SetDmxExt(void);

#endif /* PD_DVBS_IF_H */

