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
 * $RCSfile: irrx_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file irrx_if.c
 *  irrx_if.c provides IRRX export interface functions.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_bim.h"
#include "x_timer.h"
#include "x_os.h"
#include "x_assert.h"
#include "../ir_debug.h"
#include "x_pinmux.h"
#include "x_util.h"
#include "drv_ir.h"
#include "ir_cus.h"
#include "ir_regs.h"
#include "drv_config.h"
#include "chip_ver.h"
#include "sys_config.h"
#include <linux/module.h>

 
extern INT32 _fgRepeat;
extern UINT32 	_u4PrevKey;

#if (IRRX_RC_PROTOCOL == IRRX_RC_NEC)
UINT32 _u4GroupId = MTK_IRRX_GRPID_DVD;
#endif

UINT32 IRRX_QuerySetGroupId(const UINT32 * pu4Data)
{
  #if (IRRX_RC_PROTOCOL == IRRX_RC_NEC)
     if (pu4Data != NULL)
    {
        _u4GroupId = *pu4Data;
    }
    return _u4GroupId;
  #else
    return 0;
  #endif
}
EXPORT_SYMBOL(IRRX_QuerySetGroupId);


#if (IRRX_RC_PROTOCOL == IRRX_RC_RC6 )   
 
UINT32 _IRRX_XferRC6ToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4BitCnt = 0;
    static UINT32 u4TogKey = 0;
    UINT32 u4RC6key = 0;

    u4BitCnt = INFO_TO_BITCNT(u4Info);

    /* Check data. */
    if ((u4BitCnt != IRRX_RC6_BITCNT) 
		|| (pu1Data == NULL)
		|| (IRRX_RC6_GET_CUSTOM(pu1Data[0], pu1Data[1]) != IRRX_RC6_CUSTOM)
		|| (IRRX_RC6_GET_LEADER(pu1Data[0]) != IRRX_RC6_LEADER))
    {
        LOG( 9 , "Bitcnt: 0x%02x, Leader: 0x%02x, Custom: 0x%02x\n ", 
			 u4BitCnt, IRRX_RC6_GET_LEADER(pu1Data[0]), IRRX_RC6_GET_CUSTOM(pu1Data[0], pu1Data[1]));
        return BTN_NONE;
    }

    if (u4TogKey != IRRX_RC6_GET_TOGGLE(pu1Data[0]))
    {
        //a new key
        u4RC6key = IRRX_RC6_GET_KEYCODE(pu1Data[1], pu1Data[2]);
		u4TogKey = IRRX_RC6_GET_TOGGLE(pu1Data[0]);
        LOG( 9 , "a RC6 key down: 0x%02x  toggle: 0x%02x\n ", u4RC6key, u4TogKey);

	    if(u4RC6key < IRRX_RC6_MAX_MAP_ENTRY) 
	    {
	      if(TRUE == u1NeedRelease)
	      {
		    _u4PrevKey = _au4RC6CrystalKeyMap[u4RC6key];  
	        return _u4PrevKey;
	      }
		  else
		  {
		    return (_au4RC6CrystalKeyMap[u4RC6key]);
		  }
	    }
		else
		{
		  return BTN_NONE;
		}
    }
    else
    {                           //key hold
       LOG(9, " a RC6 key hold : 0x%02x\n", IRRX_RC6_GET_KEYCODE(pu1Data[1],
                                                              pu1Data[2]));
       if (_fgRepeat)
       {
         return BTN_KEY_REPEAT;
       }
       else
       {
           return _u4PrevKey;
       }
    }
   
}

 INT32 _IRRX_XferCrystalToRC6(UINT32 u4CrystalKey, UINT32* pu4RC6IrM, UINT32* pu4RC6IrL)
{
  UINT32 u4Cnt;

  *pu4RC6IrM = 0x00000000;
  *pu4RC6IrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_RC6_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4RC6CrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(IRRX_RC6_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4RC6IrM = (*pu4RC6IrM << 0x06) | (u4Cnt & 0x3F);
	  *pu4RC6IrM = (*pu4RC6IrM << 0x08) | (IRRX_RC6_CUSTOM & 0x3F);
	  *pu4RC6IrM = (*pu4RC6IrM << 0x02) | ((u4Cnt & 0xC0) >> 0x06);
	  *pu4RC6IrM = (*pu4RC6IrM << 0x04) | (IRRX_RC6_LEADER & 0x0F);
	  *pu4RC6IrM = (*pu4RC6IrM << 0x02) | (IRRX_RC6_TOGGLE0 & 0x03);
	  *pu4RC6IrM = (*pu4RC6IrM << 0x02) | ((IRRX_RC6_CUSTOM & 0xC0) >> 0x06);
	}
  }

  return (IR_SUCC);
}

#elif (IRRX_RC_PROTOCOL == IRRX_RC_SIRC )   

static BYTE Reverse1Byte(BYTE ucSrc)
{
  BYTE ucRet=0;
  int i;
  BYTE ucTemp;
  for(i=0;i<8;i++)
  {
 	 ucTemp=1<<i;
 	 if(ucSrc&ucTemp)
 	 {
 		 ucRet+=1<<(7-i);
 	 }
  }
 
  return ucRet;
}


 UINT32 _IRRX_XferSIRCToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4BitCnt;   
    UINT32 u1Command, u1Device, u1Extended;
    static HAL_TIME_T rPrevTime;
    HAL_TIME_T rTime, rDelta;
    BOOL fgRepeatTime;

    HAL_GetTime(&rTime);
    HAL_GetDeltaTime(&rDelta, &rPrevTime, &rTime);
    if ((rDelta.u4Seconds == 0)
         && (rDelta.u4Micros < (1000 * 50)))
    {
       fgRepeatTime = TRUE; 
    }
    else
    {
       fgRepeatTime = FALSE;  
    }
	HAL_GetTime(&rPrevTime);
	
    u4BitCnt = INFO_TO_BITCNT(u4Info);

    /* Check empty data. */
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
        // V_IR_FAILED
        return BTN_NONE;
    }

    switch (u4BitCnt)
    {
        case IRRX_SIRC_BITCNT12:
			LOG(2, "Sony BDP device is 20B,when receive 12B need return BTN_NONE\n");
			return BTN_NONE;
            u1Command = (pu1Data[0] >> 1);
			u1Command = Reverse1Byte(u1Command<<1);
			
			u1Device = ((pu1Data[0] & 0x01) << 4) | ((pu1Data[1] & 0xF0) >> 4);
			u1Device = Reverse1Byte(u1Device<<3);
			
            LOG(2, "Received 12B Key: 0x%02x, Device = 0x%02x\n", u1Command, u1Device);

            if((u1Command >= IRRX_SIRC_MAX_MAP_ENTRY) 
				|| (u1Device != IRRX_SIRC_12B_DEVICE))  
            {
                return BTN_NONE;
            }

            if ((TRUE == u1NeedRelease) && fgRepeatTime && (_u4PrevKey == _au4SIRC12BCrystalKeyMap[u1Command]))
            {
                if(_fgRepeat) 
                {
                    return BTN_KEY_REPEAT;
                }
                else
                {
                    return _u4PrevKey;
                }
            }
			
            if(TRUE == u1NeedRelease)
            {
              _u4PrevKey = _au4SIRC12BCrystalKeyMap[u1Command];
        	  return _u4PrevKey;
            }
            else
            {
              return _au4SIRC12BCrystalKeyMap[u1Command];
            }
				
        case IRRX_SIRC_BITCNT15:
			
            u1Command = (pu1Data[0] >> 1);
			u1Command = Reverse1Byte(u1Command<<1);
			
            u1Extended = ((pu1Data[0] & 0x01) << 7) | ((pu1Data[1] & 0xFE) >> 1);
            u1Extended = Reverse1Byte(u1Extended);
			
            LOG(2, "Received 15B Key: 0x%02x, u1Extended = 0x%02x\n", u1Command,  u1Extended);

            if(u1Command >= IRRX_SIRC_MAX_MAP_ENTRY)  
            {
                return BTN_NONE;
            }

            if((SONY_STR_IR_EXTEND_CODE != u1Extended) || (IR_BTN_SYSTEM_OFF != u1Command))
            {
                if(SONY_KEYCON_IR_EXTEND_CODE == u1Extended)
                {
                    switch (u1Command)
                    {
                        case 0x14:
                        case 0x16:
                        case 0x17:
                            break;
                        default:
                            return BTN_NONE;
                    }                      
                }
                else if(SONY_ECHO_IR_EXTEND_CODE == u1Extended)
                {
                    switch (u1Command)
                    {
                       case 0x5f:
                           break;
                       default:
                           return BTN_NONE;
                    }                      
                }
                else
                {
                    LOG(2, "Sony BDP device is 20B,when receive 15B need return BTN_NONE\n");
                    return BTN_NONE;
                }
            }
            
            if(b_g_in_standby)
            { //Quick 
                if(IR_BTN_SYSTEM_OFF == u1Command)//Just support Power off operate.
                {
                    return BTN_NONE;
                }
            }


            if ((TRUE == u1NeedRelease) && fgRepeatTime && (_u4PrevKey == _au4SIRC15BCrystalKeyMap[u1Command]))
            {
                if(_fgRepeat) 
                {
                    return BTN_KEY_REPEAT;
                }
                else
                {
                    return _u4PrevKey;
                }
            }
			
            if(TRUE == u1NeedRelease)
            {
              _u4PrevKey = _au4SIRC15BCrystalKeyMap[u1Command];
        	  return _u4PrevKey;
            }
            else
            {
              return _au4SIRC15BCrystalKeyMap[u1Command];
            }
            
        case IRRX_SIRC_BITCNT20:
			
            u1Command = (pu1Data[0] >> 1);
			u1Command = Reverse1Byte(u1Command<<1);
			
			u1Device = ((pu1Data[0] & 0x01) << 4) | ((pu1Data[1] & 0xF0) >> 4);
			u1Device = Reverse1Byte(u1Device<<3);

			u1Extended = ((pu1Data[1] & 0x0F) << 4) | ((pu1Data[2] & 0xF0) >> 4);
			u1Extended = Reverse1Byte(u1Extended);

            LOG(2, "Received 20B Key: 0x%x, Device = 0x%x, u1Extended = 0x%x\n", u1Command, u1Device, u1Extended);

			if((u1Command >= IRRX_SIRC_MAX_MAP_ENTRY) 
				|| (u1Device != IRRX_SIRC_20B_DEVICE))  
            {
                return BTN_NONE;
            }

            if ((TRUE == u1NeedRelease) && fgRepeatTime && (_u4PrevKey == _au4SIRC20BCrystalKeyMap[u1Command]))
            {
                if(_fgRepeat) 
                {
                    return BTN_KEY_REPEAT;
                }
                else
                {
                    return _u4PrevKey;
                }
            }
			
            if(TRUE == u1NeedRelease)
            {
              _u4PrevKey = _au4SIRC20BCrystalKeyMap[u1Command];
        	  return _u4PrevKey;
            }
            else
            {
              return _au4SIRC20BCrystalKeyMap[u1Command];
            }
			
        default:
            return BTN_NONE;
    }

}

INT32 _IRRX_XferCrystalToSIRC(UINT32 u4CrystalKey, UINT32* pu4SIRCIrM, UINT32* pu4SIRCIrL)
{
  UINT32 u4Cnt;
  UINT8  u1BitCnt = 0x0; 
  
  *pu4SIRCIrM = 0x00000000;
  *pu4SIRCIrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_SIRC_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4SIRC12BCrystalKeyMap[u4Cnt])
      {
        u1BitCnt = IRRX_SIRC_BITCNT12;
        break;
      }

	  if(u4CrystalKey == _au4SIRC15BCrystalKeyMap[u4Cnt])
      {
        u1BitCnt = IRRX_SIRC_BITCNT15;
        break;
      }

	  if(u4CrystalKey == _au4SIRC20BCrystalKeyMap[u4Cnt])
      {
        u1BitCnt = IRRX_SIRC_BITCNT20;
        break;
      }
    }

	if(IRRX_SIRC_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  if(u1BitCnt == IRRX_SIRC_BITCNT20)
	  { 
	      *pu4SIRCIrM = ((Reverse1Byte(u4Cnt) & 0xFE)) 
		  	         | ((Reverse1Byte(IRRX_SIRC_20B_EXTENDED) & 0xF0) << 4)
		  	         | ((Reverse1Byte(IRRX_SIRC_20B_EXTENDED) & 0x0F) << 20)
        			 | ((Reverse1Byte(IRRX_SIRC_20B_DEVICE) & 0x78) << 9)
        			 | ((Reverse1Byte(IRRX_SIRC_20B_DEVICE) & 0x80) >> 7);
	  }
	  else if(u1BitCnt == IRRX_SIRC_BITCNT15)
	  {
	  	*pu4SIRCIrM = ((Reverse1Byte(u4Cnt) & 0xFE)) 
			         | ((Reverse1Byte(IRRX_SIRC_15B_DEVICE) & 0x80) >> 7)
			         | ((Reverse1Byte(IRRX_SIRC_15B_DEVICE) & 0x7F) << 9);
	  }
	  else if(u1BitCnt == IRRX_SIRC_BITCNT12)
	  {
	  	*pu4SIRCIrM = ((Reverse1Byte(u4Cnt) & 0xFE)) 
			         | ((Reverse1Byte(IRRX_SIRC_12B_DEVICE) & 0x80) >> 7)
			         | ((Reverse1Byte(IRRX_SIRC_12B_DEVICE) & 0x78) << 9);
	  }
    }
  }
  return (IR_SUCC);
}

#elif (IRRX_RC_PROTOCOL == IRRX_RC_JVC)

UINT32 _IRRX_XferJVCToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4CusId, u4BitCnt, u4Code;
	static UINT32 u4PreCode = 0xFF;

    /* Check empty data. */
    u4BitCnt = INFO_TO_BITCNT(u4Info);
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
         return BTN_NONE;
    }

    u4Code = pu1Data[2];
	u4Code = (u4Code << 8) + pu1Data[1];
	u4Code = (u4Code << 8) + pu1Data[0];
	
    /* Check repeat key. */
    if ((u4BitCnt == IRRX_JVC_BITCNT_REPEAT) && (TRUE == u1NeedRelease))
    {
        u4CusId = u4Code & 0xFF;
        u4Code = (u4Code >> 8) & 0xFF;
		
        if ((u4CusId == IRRX_JVC_CUSTOM) && 
			(u4Code == u4PreCode) &&
			(INFO_TO_1STPULSE(u4Info) <= IRRX_JVC_1ST_PULSE_REPEAT)&&
            (INFO_TO_2NDPULSE(u4Info) <= 2) &&
            (INFO_TO_3RDPULSE(u4Info) <= 2))
        {
            LOG(6, "KeyCode is 0x%02x - repeat\n", u4Code);
            if (_fgRepeat)
            {
                return BTN_KEY_REPEAT;
            }
            else
            {
                return _u4PrevKey;
            }
        }
        else
        {
            LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
            return BTN_NONE;
        }
    }

    u4Code >>= 1;
    u4CusId = u4Code & 0xFF;
    u4Code = (u4Code >> 8) & 0xFF;
		
    /* Check invalid pulse. */
    if ((u4CusId != IRRX_JVC_CUSTOM)
		|| (u4BitCnt != IRRX_JVC_BITCNT_NORMAL)
        || (INFO_TO_1STPULSE(u4Info) != IRRX_JVC_1ST_PULSE_NORMAL)
        || (INFO_TO_2NDPULSE(u4Info) > (2))
        || (INFO_TO_3RDPULSE(u4Info) > (2)))
    {
        LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }

    u4PreCode = u4Code;
	
    LOG(6, "KeyCode is 0x%02x\n", u4Code);
    if (u4Code >= IRRX_JVC_MAX_MAP_ENTRY)
    {
        LOG(9, "%s(%d) BTN_NONE\n", __FILE__, __LINE__);
        return BTN_NONE;
    }

    if(TRUE == u1NeedRelease)
    {
      _u4PrevKey = _au4JVCCrystalKeyMap[u4Code];
	  return _u4PrevKey;
    }
    else
    {
      return _au4JVCCrystalKeyMap[u4Code];
    }
	
}

 INT32 _IRRX_XferCrystalToJVC(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL)
{

  UINT32 u4Cnt;

  *pu4MtkIrM = 0x00000000;
  *pu4MtkIrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_JVC_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4JVCCrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(IRRX_JVC_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4MtkIrM = (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 8) | (IRRX_JVC_CUSTOM & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 1) | (1);
	}
  }

  return (IR_SUCC);
}

#elif (IRRX_RC_PROTOCOL == IRRX_RC_PAN)

 UINT32 _IRRX_XferPANToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT16 u2CusId = 0;
    UINT8 u1SysBit = 0;
    UINT8 u1DeviceBit = 0;
    UINT8 u1CommandBit = 0;
    UINT8 u1ParityBit = 0;

    UINT32 u4BitCnt = 0;
///    static UINT32 u4PreCode = 0xFF;

    /* Check empty data. */
    u4BitCnt = INFO_TO_BITCNT(u4Info);
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
         return BTN_NONE;
    }

    u2CusId = pu1Data[1];
    u2CusId = (u2CusId << 8) + pu1Data[0];
    u1SysBit = pu1Data[2];
    u1DeviceBit = pu1Data[3];
    u1CommandBit = pu1Data[4];
    u1ParityBit = pu1Data[5];
	
    /* Check invalid pulse. */
    if((IRRX_PAN_CUSTOM != u2CusId) || 				///customer ID should be fixed as 0x2002
		(0x00 != (u1SysBit & 0x0F)) ||				/// customer parity ID should be fixed as 0
		(u1ParityBit != (u1SysBit ^ u1DeviceBit ^ u1CommandBit)))	///verify value
    {
        LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }

    if((0x80 != (u1SysBit & 0xF0)) && (0x90 != (u1SysBit & 0xF0)) && (0xB0 != (u1SysBit & 0xF0)))		///System ID should be reasonable value
    {
        LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	 return BTN_NONE;
    }

    if(IRRX_PAN_BITCNT_NORMAL != u4BitCnt)		///not normal bit count..
    {
        LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	 return BTN_NONE;
    }

    LOG(6, "KeyCode is 0x%02x %02x %02x \n", (u1SysBit & 0xF0), (0x0F & u1DeviceBit) , (u1CommandBit));

    if(0x90 == (u1SysBit & 0xF0))
    {
	    switch (u1CommandBit)
	    {
	        case 0xB1 :
			return BTN_CUSTOM_1;		///(Amplifier region) Volum Up
		 	break;
		 case 0xB2 :
			return BTN_CUSTOM_2;		///(Amplifier region)Volum Down
			break;
	        default:
			return BTN_NONE;
			break;
	    }
    }
    else if(0x80 == (u1SysBit & 0xF0))
    {
	    switch (u1CommandBit)
	    {
	        case 0x3D :
			return BTN_CUSTOM_3;		///(TV region)Power 
		 	break;
		 case 0x05 :
			return BTN_CUSTOM_4;		///(TV region)Input
			break;
	        case 0x20 :
			return BTN_CUSTOM_5;		///(TV region)Volum Up 
		 	break;
		 case 0x21 :
			return BTN_CUSTOM_6;		///(TV region)Volum Down
			break;
	        case 0x34 :
			return BTN_CUSTOM_7;		///(TV region)Channel Up 
		 	break;
		 case 0x35 :
			return BTN_CUSTOM_8;		///(TV region)Channel Down
			break;
	        default:
			return BTN_NONE;
			break;
	    }
    }
    else
    {
	    if(0x01 == (0x0F & u1DeviceBit))
	    {
		    switch (u1CommandBit)
		    {
		        case 0x56 :
				return BTN_PIP;
			 	break;
			 case 0x51 :
				return BTN_EXIT;
				break;
		        case 0x50 :
				return BTN_POP;
			 	break;
			 case 0x41 :
				return BTN_RED;
				break;
		        case 0x42 :
				return BTN_GREEN;
			 	break;
			 case 0x43 :
				return BTN_YELLOW;
				break;
			 case 0x40 :
				return BTN_BLUE;
				break;
		        case 0x57 :
				return BTN_PIP_AUDIO;
			 	break;
			 case 0x82 :
				return BTN_CUSTOM_9;
				break;
		        default:
				return BTN_NONE;
				break;
		    }		
	    }
	    else if(0x00 == (0x0F & u1DeviceBit))
	    {
      		    return _au4PANCrystalKeyMap[u1CommandBit];
	    }
	    else
	    {
	     	    return BTN_NONE;
	    }
    }
}

 INT32 _IRRX_XferCrystalToPAN(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL)
{

  UINT32 u4Cnt;

  *pu4MtkIrM = 0x00000000;
  *pu4MtkIrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_PAN_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4PANCrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(IRRX_PAN_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4MtkIrM = (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 8) | (IRRX_PAN_CUSTOM & 0xFFFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 1) | (1);
	}
  }

  return (IR_SUCC);
}

#elif (IRRX_RC_PROTOCOL == IRRX_RC_RC5 )   

 UINT32 _IRRX_XferRC5ToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4BitCnt = 0;
    static UINT32 u4TogKey = 0xFF;
    static UINT32 u4RC5key = 0xFF;

    u4BitCnt = INFO_TO_BITCNT(u4Info);

    /* Check data. */
    if ((u4BitCnt != IRRX_RC5_BITCNT) 
		|| (pu1Data == NULL)
		|| (IRRX_RC5_GET_CUSTOM(pu1Data[0]) != IRRX_RC5_CUSTOM))
    {
        return BTN_NONE;
    }

    if (u4TogKey != IRRX_RC5_GET_TOGGLE(pu1Data[0]))
    {
        //a new key
        u4RC5key = IRRX_RC5_GET_KEYCODE(pu1Data[0], pu1Data[1]);
		u4TogKey = IRRX_RC5_GET_TOGGLE(pu1Data[0]);
        LOG(2, "a RC5 key down: 0x%02x  toggle: 0x%02x\n ", u4RC5key, u4TogKey);

	    if(u4RC5key < IRRX_RC5_MAX_MAP_ENTRY) 
	    {
	      if(TRUE == u1NeedRelease)
	      {
		    _u4PrevKey = _au4RC5CrystalKeyMap[u4RC5key];  
	        return _u4PrevKey;
	      }
		  else
		  {
		    return (_au4RC5CrystalKeyMap[u4RC5key]);
		  }
	    }
		else
		{
		  return BTN_NONE;
		}
    }
    else if(u4RC5key == IRRX_RC5_GET_KEYCODE(pu1Data[0], pu1Data[1]))
    {//key hold
       LOG(2, " a RC5 key hold : 0x%02x\n", u4RC5key);
       if (_fgRepeat)
       {
         return BTN_KEY_REPEAT;
       }
       else
       {
           return _u4PrevKey;
       }
    }

	return BTN_NONE;
}

 INT32 _IRRX_XferCrystalToRC5(UINT32 u4CrystalKey, UINT32* pu4RC5IrM, UINT32* pu4RC5IrL)
{
  UINT32 u4Cnt;

  *pu4RC5IrM = 0x00000E00;
  *pu4RC5IrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_RC5_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4RC5CrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(IRRX_RC5_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4RC5IrM |= (((~IRRX_RC5_CUSTOM) & 0x1F) << 2);
	  *pu4RC5IrM |= (((~u4Cnt) & 0x30) >> 4);
	  *pu4RC5IrM |= (((~u4Cnt) & 0x0F) << 12);
	}
  }

  return (IR_SUCC);
}


#elif (IRRX_RC_PROTOCOL == IRRX_RC_RCA)

static BYTE Reverse1Byte(BYTE ucSrc)
{
  BYTE ucRet=0;
  int i;
  BYTE ucTemp;
  for(i=0;i<8;i++)
  {
 	 ucTemp=1<<i;
 	 if(ucSrc&ucTemp)
 	 {
 		 ucRet+=1<<(7-i);
 	 }
  }
 
  return ucRet;
}

 UINT32 _IRRX_XferRCAToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32  u4BitCnt;
    BYTE u1DataCode, u1CustomCode, u1DataCodeVerf, u1CustomCodeVerf;

    u4BitCnt = INFO_TO_BITCNT(u4Info);

    /* Check empty data. */
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
           return BTN_NONE;
    }

    /* Check invalid pulse. */
    if (u4BitCnt != IRRX_RCA_BITCNT_NORMAL)
    {
        LOG(11, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }

    u1CustomCode = pu1Data[0] & 0x0f;
    u1CustomCodeVerf = (pu1Data[1] & 0xf0) >>4;

    if((0xf != (u1CustomCode + u1CustomCodeVerf)) 
		|| (IRRX_RCA_CUSTOM != u1CustomCode))
    {
        LOG(11, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }
	
    u1DataCode = ((pu1Data[1] & 0x0f) << 4) + ((pu1Data[0] & 0xf0) >>4);
    u1DataCodeVerf = pu1Data[2];

    if(0xff != (u1DataCode + u1DataCodeVerf)) 
    {
        LOG(11, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }
	
    u1DataCode = Reverse1Byte(u1DataCode);
    u1DataCode = ~u1DataCode;
    LOG(7, "KeyCode is 0x%02x\n", u1DataCode);

    if(TRUE == u1NeedRelease)
    {
      _u4PrevKey = _au4RCACrystalKeyMap[u1DataCode];
	  return _u4PrevKey;
    }
    else
    {
      return _au4RCACrystalKeyMap[u1DataCode];
    }
	
}

 INT32 _IRRX_XferCrystalToRCA(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL)
{ 
  UINT32 u4Cnt;

  *pu4MtkIrM = 0x00000000;
  *pu4MtkIrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < 0XFF; u4Cnt ++)
    {
      if(u4CrystalKey == _au4RCACrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(0XFF == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4MtkIrM = 0XFF - (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 8) | (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 16) | (IRRX_RCA_CUSTOM & 0xFFFF);
	}
  }
  
  return (IR_SUCC);
}

#elif (IRRX_RC_PROTOCOL == IRRX_RC_SAS)


 UINT32 _IRRX_XferSASToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4VenderID, u4BitCnt;
    UINT8 u1ProductCode, u1DataCode,u1DataCodeVerf;

    u4BitCnt = INFO_TO_BITCNT(u4Info);

    /* Check empty data. */
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
           return BTN_NONE;
    }


    /* Check invalid pulse. */
    if (u4BitCnt != IRRX_BITCNT_NORMAL_SAS)
    {
        LOG(10, "BitCount Err, BTN_NONE\n" );
        return BTN_NONE;
    }
	
    /* Check Vender Id. */
    u4VenderID= pu1Data[0];
    u4VenderID = (u4VenderID << 8) + pu1Data[1];

    if(u4VenderID != IRRX_CUSTOM_SAS)
    {
        LOG(10, "Vender ID Err, BTN_NONE\n" );
        return BTN_NONE;
    }
	
    /* Check Product Id. */
    u1ProductCode = (pu1Data[2] >> 1) & 0xF;

    if(u1ProductCode != IRRX_GRPID_BD_SAS)
    {
        LOG(10, "Product ID Err, BTN_NONE\n" );
        return BTN_NONE;
    }

    /* Check Data Bit */
    u1DataCode = (pu1Data[2] >> 5) + (pu1Data[3] << 3);
    u1DataCodeVerf = (pu1Data[3] >> 5) + (pu1Data[4] << 3);

    if ((u1DataCode + u1DataCodeVerf) != IRRX_BIT8_VERIFY_SAS)
    {
        LOG(10, "DataBit Err, BTN_NONE\n" );
        return BTN_NONE;
    }

    LOG(7, "KeyCode is 0x%x\n", u1DataCode);

    if(TRUE == u1NeedRelease)
    {
      _u4PrevKey = _au4SASCrystalKeyMap[u1DataCode];
	  return _u4PrevKey;
    }
    else
    {
      return _au4SASCrystalKeyMap[u1DataCode];
    }
}

 INT32 _IRRX_XferCrystalToSAS(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL)
{
  UINT32 u4Cnt;

  *pu4MtkIrM = 0x00000000;
  *pu4MtkIrL = 0x00000000;
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < IRRX_MAX_MAP_ENTRY_SAS; u4Cnt ++)
    {
      if(u4CrystalKey == _au4SASCrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(IRRX_MAX_MAP_ENTRY_SAS == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4MtkIrM = IRRX_BIT8_VERIFY_SAS - (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 8) | (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 16) | (IRRX_CUSTOM_SAS & 0xFFFF);
	}
  }

  return (IR_SUCC);
}

#else      


 UINT32 _IRRX_XferMtkToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
{
    UINT32 u4GrpId, u4BitCnt;

    u4BitCnt = INFO_TO_BITCNT(u4Info);
   
	
    /* Check empty data. */
    if ((u4BitCnt == 0) || (pu1Data == NULL))
    {
           return BTN_NONE;
    }

    /* Check repeat key. */
    if ((u4BitCnt == MTK_IRRX_BITCNT_REPEAT) && (TRUE == u1NeedRelease))
    {
        if (((INFO_TO_1STPULSE(u4Info) == MTK_IRRX_1st_Plus_REPEAT) ||
        	   (INFO_TO_1STPULSE(u4Info) == MTK_IRRX_1st_Plus_REPEAT - 1) ||
             (INFO_TO_1STPULSE(u4Info) == MTK_IRRX_1st_Plus_REPEAT + 1)) &&
            (INFO_TO_2NDPULSE(u4Info) == 0) &&
            (INFO_TO_3RDPULSE(u4Info) == 0))
        {
            if (_fgRepeat)
            {
                //modify by msz00420 07-09-10 for resolving BTN_REPEAT multi-defined in
                //drv_ir.h and u_irrc_btn_def.h
                /*last code
                   return BTN_REPEAT;
                 */
                return BTN_KEY_REPEAT;
                //modify end
            }
            else
            {
                return _u4PrevKey;
            }
        }
        else
        {
            LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
            return BTN_NONE;
        }
    }

    /* Check invalid pulse. */
    if (u4BitCnt != MTK_IRRX_BITCNT_NORMAL)
    {
        LOG(10, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }


    u4GrpId = pu1Data[1];
    u4GrpId = (u4GrpId << 8) + pu1Data[0];

    /* Check GroupId. */
    if (u4GrpId != _u4GroupId)
    {
        LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }

    /* Check invalid key. */
    if ((pu1Data[2] + pu1Data[3]) != MTK_IRRX_BIT8_VERIFY)
    {
        LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
        return BTN_NONE;
    }

    /* Here, pu1Data[2] is the key of MTKDVD remote controller. */
    LOG(7, "KeyCode is 0x%02x\n", pu1Data[2]);
    if (pu1Data[2] >= MTK_NEC_MAX_MAP_ENTRY)
    {
        LOG(9, "%s(%d) BTN_NONE\n", __FILE__, __LINE__);
        return BTN_NONE;
    }

    if(TRUE == u1NeedRelease)
    {
      _u4PrevKey = _au4MtkCrystalKeyMap[pu1Data[2]];
	  return _u4PrevKey;
    }
    else
    {
      return _au4MtkCrystalKeyMap[pu1Data[2]];
    }
	
}

 INT32 _IRRX_XferCrystalToMtk(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL)
{
  UINT32 u4Cnt;

  *pu4MtkIrM = 0x00000000;
  *pu4MtkIrL = 0x00000000;
 
  
  if((BTN_NONE == u4CrystalKey)||(BTN_NO_DEF == u4CrystalKey))
  {
    LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	return (IR_FAIL);
  }
  else
  {
    for(u4Cnt = 0; u4Cnt < MTK_NEC_MAX_MAP_ENTRY; u4Cnt ++)
    {
      if(u4CrystalKey == _au4MtkCrystalKeyMap[u4Cnt])
      {
        break;
      }
    }

	if(MTK_NEC_MAX_MAP_ENTRY == u4Cnt)
	{
	  LOG(9, "%s(%d) BTN_NONE\n", __FUNCTION__, __LINE__);
	  return (IR_FAIL);
	}
	else
	{
	  *pu4MtkIrM = MTK_IRRX_BIT8_VERIFY - (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 8) | (u4Cnt & 0xFF);
	  *pu4MtkIrM = (*pu4MtkIrM << 16) | (_u4GroupId & 0xFFFF);
	}
  }

  return (IR_SUCC);
}
#endif
 
 
