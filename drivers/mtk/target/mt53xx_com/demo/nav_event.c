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
 * $RCSfile: nav_event.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_event.c
 *  
 */


#include "x_lint.h"
LINT_EXT_HEADER_BEGIN

#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
//#include "x_hal_5371.h"
#include "x_hal_926.h"
#include "x_pdwnc.h"

#include "osd_if.h"
#include "gfx_if.h"
#include "ir_if.h"
#include "drv_fnt.h"
#include "drv_common.h"
#include "vdp_if.h"
#include "nptv_if.h"
#include "drvcust_if.h"

LINT_EXT_HEADER_END

#include "widget_if.h"
#include "psipsr_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_event.h"
#include "nav_aquality.h"
#include "nav_vquality.h"
#include "nav_freqtbl.h"
#include "nav_i1394.h"
//#include "i1394_if.h"

#include "x_drv_if.h"

#include "nav_debug.h"

static BOOL _fgMenuSelected = FALSE;

EXTERN UINT32 u4NumATVProg;
EXTERN NAV_ATV_TBL_PROG_T arATVTbl[NTSC_CABLE_CH_NS];
EXTERN UINT32 u4NumDTVProg;
EXTERN PSIPSR_TBL_VCT_T arVctTbl[NAV_DTV_PROG_MAX_NUM];
EXTERN PSIPSR_TBL_TS2FREQ_T arTsTbl[NAV_DTV_PROG_MAX_NUM];

BOOL NAV_EVENT_GetMenuStatus(void);
void NAV_EVENT_SetMenuStatus(BOOL fgSelected);
BOOL NAV_IRHandler_MenuItem_RangedValue(UINT32 u4Key, void* pvArg);
BOOL NAV_IRHandler_MenuItem_BinaryValue(UINT32 u4Key, void* pvArg);
BOOL NAV_IRHandler_MenuItem_Action(UINT32 u4Key, void* pvArg);
BOOL NAV_IRHandler_MenuItem_Option(UINT32 u4Key, void* pvArg);
BOOL NAV_IRHandler_1KeyAction(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key, VOID* pvArg);
BOOL NAV_IRHandler_SetProg2Key(WIDGET_OSDINFO_T arOSD[], UINT32 u4KeyArray[], VOID* pvArg);
BOOL NAV_IRHandler_SetProg3Key(WIDGET_OSDINFO_T arOSD[], UINT32 u4KeyArray[], VOID* pvArg);


static BOOL _IRHandler_ChangeVol(UINT32 u4Key, VOID* pvArg);
static BOOL _IRHandler_ChangeProg(UINT32 u4Key, VOID* pvArg);
#ifdef CC_YES_1394
static BOOL _IRHandler_I1394(UINT32 u4Key);
#endif
static BOOL _IRHandler_Input(WIDGET_OSDINFO_T arOSD[],  UINT32 u4Key);
static BOOL _IRHandler_Menu(UINT32 u4Key, WIDGET_MENU_T* prMenu);
static INT32 _IRHandler_PIPPOP(UINT32 u4Key);

EXTERN void ADAC_SpeakerEnable(BOOL fgEnable);

BOOL NAV_EVENT_GetMenuStatus(void)
{
	return _fgMenuSelected;
}


void NAV_EVENT_SetMenuStatus(BOOL fgSelected)
{
	_fgMenuSelected = fgSelected;
	
}

BOOL NAV_IRHandler_MenuItem_RangedValue(UINT32 u4Key, VOID* pvArg)
{
	WIDGET_MENUITEM_RANGEDVALUE_T* prItem;
	UINT32 u4StepSize;
	VERIFY(pvArg != NULL);
	prItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)pvArg;
	u4StepSize = (UINT32)((prItem->i4Max - prItem->i4Min) / 100);
	if(u4StepSize == 0)
	{
		u4StepSize = 1;
	}
	if(prItem->pfOp == NULL)
	{
		return FALSE;
	}
	if(rNAVOption.fgEnableOSD32)
	{
		switch(u4Key)
		{
			case BTN_VOL_DOWN:
				if(prItem->pfOp((void*)(prItem->i4Value - (INT32)u4StepSize)) == (INT32)E_NAV_MSG_OK)
				{
					prItem->i4Value -= (INT32)u4StepSize;
				}
				break;
			case BTN_VOL_UP:
				if(prItem->pfOp((void*)(prItem->i4Value + (INT32)u4StepSize)) == (INT32)E_NAV_MSG_OK)
				{
					prItem->i4Value += (INT32)u4StepSize;
				}
				break;
			default:
				return FALSE;
		}
	}
	else
	{
		switch(u4Key)
		{
			case BTN_CURSOR_LEFT:
				if(prItem->pfOp((void*)(prItem->i4Value - (INT32)u4StepSize)) == (INT32)E_NAV_MSG_OK)
				{
					prItem->i4Value -= (INT32)u4StepSize;
				}
				break;
			case BTN_CURSOR_RIGHT:
				if(prItem->pfOp((void*)(prItem->i4Value + (INT32)u4StepSize)) == (INT32)E_NAV_MSG_OK)
				{
					prItem->i4Value += (INT32)u4StepSize;
				}
				break;
			default:
				return FALSE;
		}
	}
	VERIFY(prItem->pfActivate((const WIDGET_MENUITEM_T*)(void*)prItem, (const WIDGET_OSDINFO_T*)prItem->prParentMenu->prParentList->prParentOSD, 1) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(WIDGET_Flush(prItem->prParentMenu->prParentList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);

	return TRUE;
}
BOOL NAV_IRHandler_MenuItem_BinaryValue(UINT32 u4Key, VOID* pvArg)
{
	WIDGET_MENUITEM_BINARYVALUE_T* prItem;
	VERIFY(pvArg != NULL);
	prItem = (WIDGET_MENUITEM_BINARYVALUE_T*)pvArg;
	if(prItem->pfOp == NULL)
	{
		return FALSE;
	}
	if(rNAVOption.fgEnableOSD32)
	{
		switch(u4Key) 
		{
			case BTN_VOL_DOWN:
			case BTN_VOL_UP:
				prItem->fgOn = !(prItem->fgOn);
				break;
			default:
				return FALSE;
		}
	}
	else
	{
		switch(u4Key) 
		{
			case BTN_CURSOR_LEFT:
			case BTN_CURSOR_RIGHT:
				prItem->fgOn = !(prItem->fgOn);
				break;
			default:
				return FALSE;
		}
	}
	VERIFY(prItem->pfActivate((WIDGET_MENUITEM_T*)(void*)prItem, prItem->prParentMenu->prParentList->prParentOSD, 1) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(prItem->pfOp((void*)(prItem->fgOn)) != (INT32)E_WIDGET_MSG_ERROR);
	VERIFY(WIDGET_Flush(prItem->prParentMenu->prParentList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
	return TRUE;
}

BOOL NAV_IRHandler_MenuItem_Action(UINT32 u4Key, VOID* pvArg)
{
	WIDGET_MENUITEM_ACTION_T* prItem;
	VERIFY(pvArg != NULL);
	prItem = (WIDGET_MENUITEM_ACTION_T*)pvArg;	
	if(u4Key == BTN_SELECT) 
	{
		if(prItem->pfOp == NULL)
		{
			return FALSE;
		}
		//VERIFY(prItem->pfOp((void*)prItem->pvCallbackTag) != (INT32)E_WIDGET_MSG_ERROR);
        if( prItem->pfOp((void*)prItem->pvCallbackTag) == (INT32)E_NAV_MSG_OK ) 
        {
            return TRUE;
        }
        else
        {
    		return FALSE;
        }
	}
	return FALSE;
}

BOOL NAV_IRHandler_MenuItem_Option(UINT32 u4Key, VOID* pvArg)
{
	WIDGET_MENUITEM_OPTION_T* prItem;
	WIDGET_STRINGLIST_T* prList;
	VERIFY(pvArg != NULL);
	prItem = (WIDGET_MENUITEM_OPTION_T*)pvArg;
	prList = prItem->prStrList;
	VERIFY(prList != NULL);
	if(prItem->pfOp == NULL)
	{
		return FALSE;
	}
	if(rNAVOption.fgEnableOSD32)
	{
		switch(u4Key)
		{
			case BTN_VOL_DOWN:
				prList->prCurr = prList->prCurr->prNext;
				break;
			case BTN_VOL_UP:
				prList->prCurr = prList->prCurr->prPrev;
				break;
			case BTN_SELECT:
				VERIFY(prItem->pfOp((void*)(prList->prCurr->szStr)) != (INT32)E_WIDGET_MSG_ERROR);
				return TRUE;
			default:
				return FALSE;
		}
	}
	else
	{
		switch(u4Key)
		{
			case BTN_CURSOR_RIGHT:
				prList->prCurr = prList->prCurr->prNext;
				break;
			case BTN_CURSOR_LEFT:
				prList->prCurr = prList->prCurr->prPrev;
				break;
			case BTN_SELECT:
				VERIFY(prItem->pfOp((void*)(prList->prCurr->szStr)) != (INT32)E_WIDGET_MSG_ERROR);
				return TRUE;
			default:
				return FALSE;
		}
	}
	VERIFY(prItem->pfActivate((WIDGET_MENUITEM_T*)(void*)prItem, prItem->prParentMenu->prParentList->prParentOSD, 1) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(WIDGET_Flush(prItem->prParentMenu->prParentList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
	return TRUE;
}

static BOOL _IRHandler_Menu(UINT32 u4Key, WIDGET_MENU_T* prMenu)
{
	WIDGET_MENUITEM_T* prDeselected;
	VERIFY(prMenu != NULL);
	prDeselected = prMenu->prCurr;
	VERIFY(prDeselected != NULL);
	LOG(6, "Handle item %s\n", prMenu->prCurr->szTitle);
	switch(u4Key)
	{
		case BTN_CURSOR_UP:
		{
			UINT32 u4StartIdx = prMenu->prPageStart->u4Index;
			if(prMenu->prCurr->u4Index == 0)
			{
			    break;	
			}
			prMenu->prCurr = prMenu->prCurr->prPrev;		
			if(prMenu->prCurr->u4Index < u4StartIdx)
			{
              	    do 
              	    {
              	        prMenu->prPageStart = prMenu->prPageStart->prPrev;
              	    } while(prMenu->prPageStart->u4Index + prMenu->u4NumOfItemPage > u4StartIdx);
              	    WIDGET_ShowMenuList(prMenu->prParentList);
              	    return TRUE;                 	    
			}
			break;
		}
		case BTN_CURSOR_DOWN:
		{
			UINT32 u4StartIdx = prMenu->prPageStart->u4Index;			
			if(prMenu->prCurr->u4Index  == prMenu->u4NumOfItem - 1)
			{
			    break;	
			}
			
			prMenu->prCurr = prMenu->prCurr->prNext;
			if(prMenu->prCurr->u4Index  >= u4StartIdx + prMenu->u4NumOfItemPage)
			{
              	    do 
              	    {
              	        prMenu->prPageStart = prMenu->prPageStart->prNext;              	        
              	    } while(prMenu->prPageStart->u4Index < u4StartIdx + prMenu->u4NumOfItemPage);
              	    WIDGET_ShowMenuList(prMenu->prParentList);
              	    return TRUE;              	    
			}			
			break;
		}			
		case BTN_EXIT:
		case BTN_MENU:
			_fgMenuSelected = FALSE;
			VERIFY(prMenu->prCurr->pfDeactivate(prMenu->prCurr, prMenu->prParentList->prParentOSD, 0) == (INT32)E_WIDGET_MSG_OK);
			prMenu->prCurr = NULL;
			
			prMenu->prPageStart = prMenu->prHead;
			
			VERIFY(WIDGET_Flush(prMenu->prParentList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
			return TRUE;

              case BTN_PRG_DOWN:
              	{
               	    UINT32 u4StartIdx = prMenu->prPageStart->u4Index;
               	    if(u4StartIdx + prMenu->u4NumOfItemPage >= prMenu->u4NumOfItem)
               	    {
               	        break;
               	    }               	    
              	    do 
              	    {
              	        prMenu->prPageStart = prMenu->prPageStart->prNext;              	        
              	    } while(prMenu->prPageStart->u4Index < u4StartIdx + prMenu->u4NumOfItemPage);
              	    prMenu->prCurr = prMenu->prPageStart;
              	    WIDGET_ShowMenuList(prMenu->prParentList);
              	    return TRUE;
              	}
              	//break;
	       case BTN_PRG_UP:
              	{
               	    UINT32 u4StartIdx = prMenu->prPageStart->u4Index;
               	    if(u4StartIdx < prMenu->u4NumOfItemPage)
               	    {
               	        break;
               	    }
              	    do 
              	    {
              	        prMenu->prPageStart = prMenu->prPageStart->prPrev;
              	    } while(prMenu->prPageStart->u4Index + prMenu->u4NumOfItemPage > u4StartIdx);
              	    prMenu->prCurr = prMenu->prPageStart;              	    
              	    WIDGET_ShowMenuList(prMenu->prParentList);
              	    return TRUE;
              	}
              	//break;
              	
		default:
			return prMenu->prCurr->pfHandler(u4Key, prMenu->prCurr);		
	}
	// Re-draw the newly activated item and newly de-activated item
	//_WIDGET_DrawMenuItem(prDeselected, prMenu->prParentList->prParentOSD, 0);
	//_WIDGET_DrawMenuItem(prMenu->prCurr, prMenu->prParentList->prParentOSD, 1);
	VERIFY(prDeselected->pfDeactivate(prDeselected, prMenu->prParentList->prParentOSD, 0) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(prMenu->prCurr->pfActivate(prMenu->prCurr, prMenu->prParentList->prParentOSD, 1) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(WIDGET_Flush(prMenu->prParentList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);

	return TRUE;
}

static BOOL _IRHandler_MenuList(UINT32 u4Key, VOID* pvArg)
{
	WIDGET_MENULIST_T *prList;

	//UINT32 u4Idx;    
	
	VERIFY(pvArg != NULL);
	
	prList = (WIDGET_MENULIST_T*)pvArg;

	
	if(rNAVOption.fgEnableOSD32)
	{
		switch(u4Key)
		{
			case BTN_CURSOR_RIGHT:
				// Skip inactive menu while navigating
				do
				{
					prList->prCurr = prList->prCurr->prNext;
				} while(!prList->prCurr->fgEnable); 		
				break;		
			case BTN_CURSOR_LEFT:
				// Skip inactive menu while navigating
				do
				{
					prList->prCurr = prList->prCurr->prPrev;
				} while(!prList->prCurr->fgEnable); 	
				break;
			default:	
				return _IRHandler_Menu(u4Key, prList->prCurr);
		}
	}
	else
	{
		if(!_fgMenuSelected)
		{
			//WIDGET_MENUITEM_T* prItem;
			switch(u4Key)
			{
				case BTN_CURSOR_DOWN:
					// Skip inactive menu while navigating
					do
					{
						prList->prCurr = prList->prCurr->prNext;
					} while(!prList->prCurr->fgEnable); 		
					break;		
				case BTN_CURSOR_UP:
					// Skip inactive menu while navigating
					do
					{
						prList->prCurr = prList->prCurr->prPrev;
					} while(!prList->prCurr->fgEnable); 	
					break;
				case BTN_SELECT:
				case BTN_CURSOR_RIGHT:
					_fgMenuSelected = TRUE;
					prList->prCurr->prCurr = prList->prCurr->prHead;
					VERIFY(WIDGET_DrawMenuList(prList, prList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
					VERIFY(WIDGET_Flush(prList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
					break;
				default:	
					return FALSE;
			}
		}
		else
		{
			return _IRHandler_Menu(u4Key, prList->prCurr);
		}
	}
	// Draw the newly active menu
	//_WIDGET_DrawMenu(prList->prCurr, prList->prParentOSD, 1);

	// Re-draw all
	VERIFY(WIDGET_ShowMenuList(prList) == (INT32)E_WIDGET_MSG_OK);

	return TRUE;
}


static BOOL _IRHandler_Input(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key)
{
	UINT32 u4ConfIdx;


	
	NAV_MODECHANGE_SEMA_LOCK
	
	if(u4Key == BTN_INPUT_SRC)
	{
#if NAV_PIP_DTV_CVBS1	
		if(ePIPMode != E_NAV_MODE_SINGLE)
		{
			NAV_MODECHANGE_SEMA_UNLOCK
			return TRUE;
		}	
#endif		


		if(u4ActiveVDP == NAV_VDP_MASTER)
		{
			
			do
			{
				prInputSrcList->prCurr = prInputSrcList->prCurr->prNext;
			} while ( (ePIPMode != (UINT32)E_NAV_MODE_SINGLE) && (!NAV_IsSourcePairValid((UINT32)prInputSrcList->prCurr->pvContent, arVideoPath[NAV_VDP_SLAVE].u4Src)) );
			
			u4ConfIdx = (UINT32)prInputSrcList->prCurr->pvContent;
			
		}	
		else
		{
			do
			{
				prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
			} while (!NAV_IsSourcePairValid(arVideoPath[NAV_VDP_MASTER].u4Src, (UINT32)prInputSrcListPIP->prCurr->pvContent));
			
			u4ConfIdx = (UINT32)prInputSrcListPIP->prCurr->pvContent;
		}

		VERIFY(NAV_SetSrc(u4ActiveVDP, u4ConfIdx) == (INT32)E_NAV_MSG_OK);
		VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
		fgInfoOSDOn = TRUE;
		VERIFY(NAV_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);
		VERIFY(WIDGET_DrawMenuList(arOSD[0].prList, &arOSD[0]) == (INT32)E_WIDGET_MSG_OK);
		VERIFY(WIDGET_Flush(&arOSD[0]) == (INT32)E_WIDGET_MSG_OK);

		NAV_MODECHANGE_SEMA_UNLOCK
		
		return TRUE;
	}

	NAV_MODECHANGE_SEMA_UNLOCK
	
	return FALSE;
}
	
static BOOL _IRHandler_ChangeVol(UINT32 u4Key, VOID* pvArg)
{
	INT32 i4Vol = (INT32)NAV_AQ_GetVol();
	UNUSED(pvArg);
	if(u4Key == BTN_VOL_UP)
	{
		if( (i4Vol + 1) <= 100)
		{
			VERIFY(NAV_AQ_SetVol((void*)(i4Vol + 1)) == (INT32)E_NAV_MSG_OK);
		}
		VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
		return TRUE;
	}
	if(u4Key == BTN_VOL_DOWN)
	{
		if( (i4Vol - 1) >= 0)
		{
			VERIFY(NAV_AQ_SetVol((void*)(i4Vol - 1)) == (INT32)E_NAV_MSG_OK);
		}
		VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
		return TRUE;
	}

	return FALSE;
}
static BOOL _IRHandler_ChangeProg(UINT32 u4Key, VOID* pvArg)
{
	UINT32 u4ProgNum;
	UNUSED(pvArg);
	switch(u4Key)
	{
		case BTN_PRG_UP:
			if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT1)
			{
				u4ProgNum = (rCurrDTVProg.u4VctTblIdx + 1) % NAV_GetNumDTVProg();
				LOG(6, "Change DTV program to %d\n", u4ProgNum);
				VERIFY(NAV_SetProg(u4ActiveVDP, u4ProgNum) == (INT32)E_NAV_MSG_OK);
			}
			else if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_TUNER1)
			{
				u4ProgNum = (rCurrATVProg.u4ATVTblIdx + 1) % NAV_GetNumATVProg();
				VERIFY(NAV_SetProg(u4ActiveVDP, u4ProgNum) == (INT32)E_NAV_MSG_OK);
			}
			break;
		case BTN_PRG_DOWN:
			if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT1)
			{
				u4ProgNum = (rCurrDTVProg.u4VctTblIdx == 0)? (NAV_GetNumDTVProg() - 1):(rCurrDTVProg.u4VctTblIdx - 1);
				VERIFY(NAV_SetProg(u4ActiveVDP, u4ProgNum) == (INT32)E_NAV_MSG_OK);
				LOG(6, "Change DTV program to %d\n", u4ProgNum);
			}
			else if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_TUNER1)
			{
				u4ProgNum =(rCurrATVProg.u4ATVTblIdx== 0)? (NAV_GetNumATVProg() - 1):(rCurrATVProg.u4ATVTblIdx - 1);
				VERIFY(NAV_SetProg(u4ActiveVDP, u4ProgNum) == (INT32)E_NAV_MSG_OK);
				LOG(6, "Change DTV program to %d\n", u4ProgNum);
			}
			break;
		default:
			return FALSE;

	}	
	return TRUE;
}

BOOL NAV_IRHandler_SetProg2Key(WIDGET_OSDINFO_T arOSD[], UINT32 u4KeyArray[], VOID* pvArg)
{
	UINT32 u4ProgNum;
	UINT32 i;
	UNUSED(pvArg);
	VERIFY(u4KeyArray != NULL);
       t_crit_scanchannel = x_crit_start();
       if(NAV_IsDTVScanning())
       {
              x_crit_end(t_crit_scanchannel);
       	return FALSE;
       }	
       x_crit_end(t_crit_scanchannel);
       if( (u4KeyArray[0] >= BTN_DIGIT_0) && (u4KeyArray[0] <= BTN_DIGIT_9) && (u4KeyArray[1] >= BTN_DIGIT_0) && (u4KeyArray[1] <= BTN_DIGIT_9) )
       {
		if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_TUNER1)
		{
			u4ProgNum = ((u4KeyArray[0] - BTN_DIGIT_0) * 10) + (u4KeyArray[1] - BTN_DIGIT_0);
			for(i = 0; i < NTSC_CABLE_CH_NS; i++)
			{
				if( NAV_GetATVProgNum(i) == u4ProgNum)
				{
					rCurrATVProg.u4ATVTblIdx = i;
					LOG(6, "Change Analog TV program to %d\n", rCurrATVProg.u4ATVTblIdx);
					VERIFY(NAV_SetProg(u4ActiveVDP, rCurrATVProg.u4ATVTblIdx) == (INT32)E_NAV_MSG_OK);
					return TRUE;
				}
			}	
		}
		else if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT1)
		{
			u4ProgNum = ((u4KeyArray[0] - BTN_DIGIT_0) * 10) + (u4KeyArray[1] - BTN_DIGIT_0);
			if(u4ProgNum < NAV_GetNumDTVProg())
			{
				rCurrDTVProg.u4VctTblIdx = u4ProgNum;
				LOG(6, "Change Digital TV program to %d\n", rCurrDTVProg.u4VctTblIdx);
				VERIFY(NAV_SetProg(u4ActiveVDP, rCurrDTVProg.u4VctTblIdx) == (INT32)E_NAV_MSG_OK);
				return TRUE;
			}	
		}
       }
	return FALSE;
}

BOOL NAV_IRHandler_SetProg3Key(WIDGET_OSDINFO_T arOSD[], UINT32 u4KeyArray[], VOID* pvArg)
{
	UINT32 u4ProgNum;
	UINT32 i;
	UNUSED(pvArg);
	VERIFY(u4KeyArray != NULL);
       t_crit_scanchannel = x_crit_start();
       if(NAV_IsDTVScanning())
       {
              x_crit_end(t_crit_scanchannel);
       	return FALSE;
       }	
       x_crit_end(t_crit_scanchannel);
	if( (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_TUNER1) && (u4KeyArray[0] == BTN_TTTV) && (u4KeyArray[1] >= BTN_DIGIT_0) && (u4KeyArray[1] <= BTN_DIGIT_9) && (u4KeyArray[2] >= BTN_DIGIT_0) && (u4KeyArray[2] <= BTN_DIGIT_9) )
	{
		u4ProgNum = (100 + ((u4KeyArray[1] - BTN_DIGIT_0) * 10)) + (u4KeyArray[2] - BTN_DIGIT_0);
		for(i = 0; i < NTSC_CABLE_CH_NS; i++)
		{
			if( NAV_GetATVProgNum(i)  == u4ProgNum)
			{
				rCurrATVProg.u4ATVTblIdx = i;
				LOG(6, "Change DTV program to %d\n", rCurrATVProg.u4ATVTblIdx);
				VERIFY(NAV_SetProg(u4ActiveVDP, rCurrATVProg.u4ATVTblIdx) == (INT32)E_NAV_MSG_OK);
				//NAV_DrawInfoOSD();
				return TRUE;
			}
		}	
	}
	return FALSE;
}

#ifdef CC_YES_1394
static BOOL _IRHandler_I1394(UINT32 u4Key)
{
	UINT32 u4Mode;
	switch(u4Key)
	{
		case BTN_CURSOR_LEFT:
			if(NAV_I1394_GetMode() != (INT32)TP_REC_PLAY)
			{
				VERIFY(NAV_I1394_SetMode((UINT32)TP_PLAY_FAST_REVERSE) == (INT32)E_NAV_MSG_OK);
			}
			break;
		case BTN_CURSOR_RIGHT:
			if(NAV_I1394_GetMode() != (INT32)TP_REC_PLAY)
			{
				VERIFY(NAV_I1394_SetMode((UINT32)TP_PLAY_FAST_FORWARD) == (INT32)E_NAV_MSG_OK);		
			}
			break;
		case BTN_SELECT:
			u4Mode = (UINT32)NAV_I1394_GetMode();
			if(u4Mode == (UINT32)TP_ERROR)
			{
				break;
			}
			else if(u4Mode == (UINT32)TP_STOP)
			{
				VERIFY(NAV_I1394_SetMode((UINT32)TP_PLAY) == (INT32)E_NAV_MSG_OK);		
			}
			else
			{
				VERIFY(NAV_I1394_SetMode((UINT32)TP_STOP) == (INT32)E_NAV_MSG_OK);						
			}
			break;
		default: 
			return FALSE;
	}
	VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
	return TRUE;
}
#endif

BOOL NAV_EventHandler(WIDGET_OSDINFO_T arOSD[], WIDGET_EVENT_T rEvent)
{
    if(rEvent.rIrEvent.rType != DRV_IR_EVENT_EXT)
    {
        return FALSE;
    }
#ifdef CC_NAV_SCART       
    if(rEvent.rIrEvent.u4Key == BTN_SCART_1)
    {
        if(NAV_SetSrcActive("SCART") == 0)
        {
            return TRUE;
        }
        return FALSE;
    }
    else if(rEvent.rIrEvent.u4Key == BTN_SCART_2)
    {    
        if(NAV_SetSrcActive("SCART2") == 0)
        {
            return TRUE;
        }
        return FALSE;
    } 
#endif
	return FALSE;
}


BOOL NAV_IRHandler_1KeyAction(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key, VOID* pvArg)
{
    //INT32 i;

    static UINT32 u4DispFmt = 0;
    
    t_crit_scanchannel = x_crit_start();
    if(NAV_IsDTVScanning() || NAV_IsATVScanning())
    {
      if(u4Key ==BTN_EXIT)
      	{
        VERIFY(NAV_StopScan() == (INT32)E_NAV_MSG_OK);
      	}
		x_crit_end(t_crit_scanchannel);
        return TRUE;
    }	
    x_crit_end(t_crit_scanchannel);
    
    if(arOSD[0].fgEnable && _IRHandler_MenuList(u4Key, pvArg))
    {
        return TRUE;
    }	
    
    if(u4Key == BTN_POWER)
    {          
        DRV_SYS_WAKEUP_T rWakeupSetting;  
#if 0		
        if(0)
        {
            UINT32 u4HeaderOff;    
            UINT32 u4DTVChSize = NAV_DTV_PROG_MAX_NUM * sizeof(PSIPSR_TBL_VCT_T);
            UINT32 u4ATVChSize = NAV_ATV_PROG_MAX_NUM * sizeof(NAV_ATV_TBL_PROG_T);

            VERIFY(DRV_SYS_EraseNor() == DRV_E_OK);
            VERIFY(DRV_SYS_WriteNor(0, (UINT32)NAV_NOR_MAGIC, x_strlen(NAV_NOR_MAGIC) + 1) == DRV_E_OK);
            u4HeaderOff = 12;        
            VERIFY(DRV_SYS_WriteNor(u4HeaderOff, (UINT32)&u4NumATVProg, sizeof(UINT32)) == DRV_E_OK);                
            u4HeaderOff += sizeof(UINT32);
            VERIFY(DRV_SYS_WriteNor(u4HeaderOff, (UINT32)&u4NumDTVProg, sizeof(UINT32)) == DRV_E_OK);                
            if(u4NumATVProg != 0)
            {          
                u4HeaderOff += sizeof(UINT32);        
                VERIFY(DRV_SYS_WriteNor(u4HeaderOff, (UINT32)&rCurrATVProg.u4ATVTblIdx, sizeof(UINT32)) == DRV_E_OK);                
                VERIFY(DRV_SYS_WriteNor(0x100, (UINT32)arATVTbl, u4ATVChSize) == DRV_E_OK);            
            }
            if(u4NumDTVProg != 0)
            {
                u4HeaderOff += sizeof(UINT32);        
                VERIFY(DRV_SYS_WriteNor(u4HeaderOff, (UINT32)&rCurrDTVProg.u4VctTblIdx, sizeof(UINT32)) == DRV_E_OK);                      
                VERIFY(DRV_SYS_WriteNor(0x100 + u4ATVChSize, (UINT32)arVctTbl, u4DTVChSize) == DRV_E_OK);
                VERIFY(DRV_SYS_WriteNor(0x100 + u4ATVChSize + u4DTVChSize, (UINT32)arTsTbl, NAV_DTV_PROG_MAX_NUM * sizeof(PSIPSR_TBL_TS2FREQ_T)) == DRV_E_OK);
            }

        }
#endif		
        rWakeupSetting.fgIRWakeup = TRUE;
        rWakeupSetting.fgRS232Wakeup = TRUE;
        rWakeupSetting.fgRTCWakeup = TRUE;
        rWakeupSetting.fgVGAWakeup = TRUE;
        VERIFY(DRV_SYS_SetWakeup(&rWakeupSetting) == DRV_E_OK);
        DRV_SYS_Down();
        return TRUE;
    }
    
    if( (u4Key == BTN_BLUE) && ( (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_VGA) || (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_YPbPr1) || (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_YPbPr2)) )
    {
        VERIFY(NAV_VQ_SetVGAAuto(NULL) == (INT32)E_NAV_MSG_OK);
        VERIFY(NAV_VQ_SetVGAAutoColor(NULL) == (INT32)E_NAV_MSG_OK);
        return TRUE;
    }
    /*
    if( u4Key == BTN_RED || u4Key == BTN_SWAP)
    {
        NAV_MODECHANGE_SEMA_LOCK
        if(ePIPMode != (UINT32)E_NAV_MODE_SINGLE)
        {
            VERIFY(NAV_SwapSrc() == (INT32)E_NAV_MSG_OK);
        }
        NAV_MODECHANGE_SEMA_UNLOCK
        return TRUE;
    }
    */
    #if 0
    if(DRVCUST_OptGet(eFbmMemSize) != FBM_MEM_CFG_MT5371 && u4Key == BTN_FREEZE)
    {
    
        VERIFY(NAV_SetFreeze(u4ActiveVDP, !NAV_GetFreeze(u4ActiveVDP)) == (INT32)E_NAV_MSG_OK);
        
        return TRUE;
    } 
    #endif
    if(u4Key == BTN_MUTE)
    {
        VERIFY(NAV_AQ_SetMute(!NAV_AQ_GetMute()) == (INT32)E_NAV_MSG_OK);
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
        return TRUE;
    }
    if(u4Key == BTN_MENU)
    {
        if(arOSD[0].fgEnable)
        {
            //DRV_OSD_SetColorkey(arWidgetOSD[0].u4Region, TRUE, 254);
            DRV_OSD_SetRegionAlpha(arOSD[0].u4Region, 0x0);                        
            arOSD[0].fgEnable = FALSE;
            WIDGET_ClearScreen(&arOSD[0]);  
            VERIFY(WIDGET_Flush(&(arOSD[0])) == (INT32)E_WIDGET_MSG_OK);             
        }
        else
        {        
            arOSD[0].fgEnable = TRUE;
			DRV_OSD_SetRegionAlpha(arOSD[0].u4Region, 0xD0);
            WIDGET_ShowMenuList(arOSD[0].prList);
            //DRV_OSD_SetColorkey(arWidgetOSD[0].u4Region, FALSE, 254);
        }
        return TRUE;
    }
    if(u4Key == BTN_EXIT ||u4Key == BTN_CE)
    {
        if(arOSD[0].fgEnable)
        {
            //DRV_OSD_SetColorkey(arWidgetOSD[0].u4Region, TRUE, 254);
            DRV_OSD_SetRegionAlpha(arOSD[0].u4Region, 0x0);                        
            arOSD[0].fgEnable = FALSE;
            WIDGET_ClearScreen(&arOSD[0]);                
            VERIFY(WIDGET_Flush(&(arOSD[0])) == (INT32)E_WIDGET_MSG_OK);             
            return TRUE;
        }        
    }
    else if(u4Key == BTN_OSD)
    {
        fgInfoOSDOn = !fgInfoOSDOn;
        if(fgInfoOSDOn)
        {
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            //DRV_OSD_SetColorkey(arWidgetOSD[1].u4Region, FALSE, 254);
            DRV_OSD_SetRegionAlpha(arOSD[1].u4Region, 0xD0);               
        }
        else
        {
            //DRV_OSD_SetColorkey(arWidgetOSD[1].u4Region, TRUE, 254);
            DRV_OSD_SetRegionAlpha(arOSD[1].u4Region, 0x0);               
            VERIFY(WIDGET_ClearScreen(&(arOSD[1])) == (INT32)E_WIDGET_MSG_OK);
            VERIFY(WIDGET_Flush(&(arOSD[1])) == (INT32)E_WIDGET_MSG_OK);
        }		
        return TRUE;
    }
    if(u4Key == BTN_MTS)
    {
        if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT1)
        {
            SwitchDTVMTS(u4ActiveVDP);
        }
        return TRUE;
    }
    
    if(_IRHandler_PIPPOP(u4Key))
    {
        return TRUE;
    }
        
    if(_IRHandler_Input(arOSD, u4Key))
    {
        return TRUE;
    }
    if( ( (NAV_GetNumDTVProg() > 0) && (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT1) ) ||
    ( (NAV_GetNumATVProg() > 0) && (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_TUNER1) ) ) 
    {
        if(_IRHandler_ChangeProg(u4Key, pvArg))
        {
            return TRUE;
        }
    }		
    if(_IRHandler_ChangeVol(u4Key, pvArg))
    {
        return TRUE;
    }
#ifdef CC_YES_1394    
    if( (arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT2) && (!_fgMenuSelected && _IRHandler_I1394(u4Key)) )
    {
        return TRUE;
    }
#endif    
    if(u4Key == BTN_GREEN)
    {
        u4DispFmt = (u4DispFmt + 1) % (DRV_VDO_DISP_FMT_AUTO + 1);
        DRV_VDO_SetDispFmt(u4ActiveVDP, (DRV_VDO_DISP_FMT_T)u4DispFmt);
        return TRUE;
    }

    return FALSE;
}

static INT32 _IRHandler_PIPPOP(UINT32 u4Key)
{
    if(u4Key == BTN_YELLOW)
    {
        if(NAV_GetBorderStatus())
        {
            VERIFY(NAV_ClearBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
        }
        else
        {
            VERIFY(NAV_ShowBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
        }	
        return TRUE;
    }
    else if(u4Key == BTN_POP || u4Key == BTN_PIP_POP)
    {
        // 1394 is currently forbidden in POP mode
        if(arVideoPath[u4ActiveVDP].u4Src == (UINT32)E_NAV_VS_DT2)
        {
            return TRUE;
        }
        if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
        {	
            if(NAV_SetMode((UINT32)E_NAV_MODE_POP) != (INT32)E_NAV_MSG_OK)
            {
                return TRUE;
            }
            ePIPMode = (UINT32)E_NAV_MODE_POP;	
        }	
        else if(ePIPMode == (UINT32)E_NAV_MODE_POP)
        {
            VERIFY(NAV_SetMode((UINT32)E_NAV_MODE_PIP) == (INT32)E_NAV_MSG_OK);
            ePIPMode = (UINT32)E_NAV_MODE_PIP;	
        }		
        else if(ePIPMode == (UINT32)E_NAV_MODE_PIP)
        {
            VERIFY(NAV_SetMode((UINT32)E_NAV_MODE_SINGLE) == (INT32)E_NAV_MSG_OK);
            ePIPMode = (UINT32)E_NAV_MODE_SINGLE;	
        }		        
        return TRUE;
    }   
    
    NAV_MODECHANGE_SEMA_LOCK
    if( (!arWidgetOSD[0].fgEnable) && (ePIPMode != (UINT32)E_NAV_MODE_SINGLE) )
    {		
        if( (u4Key == BTN_CURSOR_LEFT) || (u4Key == BTN_CURSOR_RIGHT) )
        {
            WIDGET_MENUITEM_RANGEDVALUE_T* prHScaleItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "H-Scale");
            WIDGET_MENUITEM_RANGEDVALUE_T* prVScaleItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "V-Scale");
            WIDGET_MENUITEM_RANGEDVALUE_T* prHPosItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "H-Pos");
            WIDGET_MENUITEM_RANGEDVALUE_T* prVPosItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "V-Pos");
            
            VERIFY(NAV_ClearBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
            u4ActiveVDP = (UINT32)(!(BOOL)u4ActiveVDP);
            VERIFY(NAV_ShowBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            
            prHPosItem->i4Value = (INT32)arVideoPath[u4ActiveVDP].rVDPRegion.u4Left;
            prVPosItem->i4Value = (INT32)arVideoPath[u4ActiveVDP].rVDPRegion.u4Top;
            prHScaleItem->i4Value = (INT32)arVideoPath[u4ActiveVDP].rVDPRegion.u4Width;
            prVScaleItem->i4Value = (INT32)arVideoPath[u4ActiveVDP].rVDPRegion.u4Height;
            
            VERIFY(NAV_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);
            
            VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
            VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
            
            
            NAV_MODECHANGE_SEMA_UNLOCK	
            return TRUE;
        }
        
    }
    NAV_MODECHANGE_SEMA_UNLOCK	

    return FALSE;
}
