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

/** @file dmx_ide.c
 *  Demux driver - IDE test port
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "vdec_ide.h"
#include "x_lint.h"

#include "x_printf.h"
LINT_EXT_HEADER_BEGIN

//#include "x_hal_5381.h"
//#include "x_hal_926.h"
#include "x_assert.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "vdec_verify_mm_map.h"

LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// IDE read file to memory
#define WAIT_IDE_ACK()  while (IDE_RX_MSG & 0x80)
#define IDE_MSG_TXFILE     0x02
#define IDE_MSG_FILENAME   0x03
#define IDE_MSG_ITEMNO     0x04
#define IDE_MSG_TXBST      0x05
#define IDE_MSG_TXGY       0x06
#define IDE_MSG_TXGU       0x07
#define IDE_MSG_TXGV       0x08
#define IDE_MSG_TXMY       0x09
#define IDE_MSG_TXMC       0x0A
#define IDE_MSG_TXGALL     0x0B
#define IDE_MSG_TXFRMINFO  0x0C
#define IDE_MSG_CHECK      0x0D
#define IDE_MSG_RXFB       0x0E
#define IDE_MSG_CMP_RET    0x0F
#define IDE_MSG_TXRATE     0x10
#define IDE_MSG_RXRATE     0x11
#define IDE_MSG_OPENFILE   0x12
#define IDE_MSG_RXWB       0x13
#define IDE_MSG_RXWBA      0x14
#define IDE_MSG_SEEKOFF    0x15
#define IDE_MSG_SEEKPINT   0x16
#define IDE_MSG_READLEN    0x17

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

/// Buffer of IDETPOUT. Be allocated at first time use
//static UINT8* _pu1IdeBuf = (UINT8 *)IDE_BUFFER_SA;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _DmxIdeSendMsg(UINT8 u1Msg)
{
    ASSERT((IDE_RX_MSG & 0x80) == 0);
    
    u1Msg &= 0x7F;
    IDE_RX_MSG = u1Msg;
    u1Msg |= 0x80;
    IDE_RX_MSG = u1Msg;
    
    WAIT_IDE_ACK();
}

static void _DmxIdeGetNextCmd(UINT8 *pu1Msg)
{
    while (1)
    {
        *pu1Msg = IDE_OUT_MSG1;
        if ((*pu1Msg) & 0x80)
        {
            break;
        }
    }
    IDE_OUT_MSG1 = 0;  // ACK
    (*pu1Msg) &= 0x7F;
}

static UINT32 _DmxIdeGetFileLen(void)
{
    UINT8 u1Msg;
    UINT32 u4Len;
    
    // get length encoded in 5 messages
    _DmxIdeGetNextCmd(&u1Msg);

    u4Len = u1Msg << 25;
    _DmxIdeGetNextCmd(&u1Msg);
    u4Len |= (u1Msg << 18);
    _DmxIdeGetNextCmd(&u1Msg);
    u4Len |= (u1Msg << 11);
    _DmxIdeGetNextCmd(&u1Msg);
    u4Len |= (u1Msg << 4);
    _DmxIdeGetNextCmd(&u1Msg);
    u4Len |= (u1Msg & 0x0F);

    return (u4Len);
}

static BOOL _DmxIdeRead(UINT32 u4Addr, UINT32 u4Size)
{
    UINT8 u1Msg;
    CRIT_STATE_T rState;

    rState = x_crit_start();

    while (u4Size > 0)
    {
        // Flush cache
        //HalFlushInvalidateDCache();

        // Trigger IDE
        IDE_DMA_SOURCE = u4Addr;
        IDE_DMA_LEN = u4Size;
        IDE_DMA_CONTROL |= 0x08;
	    
        u1Msg = IDE_MSG_TXFILE;
        u1Msg &= 0x7F;
        IDE_RX_MSG = u1Msg; //*(UINT8 *)0x70031410 = u1Msg;
        
        u1Msg |= 0x80;
        IDE_RX_MSG = u1Msg; //*(UINT8 *)0x70031410 = u1Msg;

        // Flush cache
        //HalFlushInvalidateDCache();

        // Wait for completion
        while ((IDE_DMA_CONTROL & 0x08) != 0)
        {
        }

        // Flush cache
        //HalFlushInvalidateDCache();

        u4Size -= u4Size;
    }

    x_crit_end(rState);

    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
void _DmxIdeReset(void)
{
    // disable
    IDE_OUT_DISABLE
    
    IDE_DMA_CONTROL = 0;
    IDE_OUT_ENABLE
    IDE_SELECT_ENABLE
    
    // Clear out-of-band messages
    IDE_IN_MSG = 0;
    IDE_OUT_MSG1 = 0;
    IDE_OUT_MSG2 = 0;

    IDE_SELECT_OUT
    IDE_OUT_ENABLE

    #if 0
    if (_pu1IdeBuf == NULL)
    {
        _pu1IdeBuf = (UINT8*)BSP_AllocAlignedDmaMemory(IDE_BUFFER_SIZE,
        32);
        ASSERT(_pu1IdeBuf != NULL);
        _pu1IdeBuf = VIRTUAL(_pu1IdeBuf);
    }
    #endif
}

BOOL _DmxIdeSendName(char *pcItem)
{
    int i;
    
    _DmxIdeSendMsg(IDE_MSG_FILENAME);  // begin
    for (i = 0; pcItem[i] != 0; i++)
    {
      _DmxIdeSendMsg(pcItem[i]);
    }
    _DmxIdeSendMsg(IDE_MSG_FILENAME);  // end
    
    return (TRUE);
}

BOOL _DmxIdeGetFile(UINT32 u4Addr, UINT32 u4Type, UINT32 *u4FileLength, UINT32 u4TargSize)
{
    UINT8 u1Msg;
    UINT32 u4InputFileLen;
    
    switch (u4Type)
    {
    case 0:
      u1Msg = IDE_MSG_TXBST;
      break;
    case 1:
      u1Msg = IDE_MSG_TXGY;
      break;
    case 2:
      u1Msg = IDE_MSG_TXGU;
      break;
    case 3:
      u1Msg = IDE_MSG_TXGV;
      break;
    case 4:
      u1Msg = IDE_MSG_TXMY;
      break;
    case 5:
      u1Msg = IDE_MSG_TXMC;
      break;
    case 6:
      u1Msg = IDE_MSG_TXGALL;
      break;
    case 7:
      u1Msg = IDE_MSG_TXFRMINFO;
      break;
    }
    
    _DmxIdeSendMsg(u1Msg);
    _DmxIdeGetNextCmd(&u1Msg);  // read file length from PC
 
    ASSERT(u1Msg == IDE_MSG_TXFILE || u1Msg == 0x00);
    u4InputFileLen = _DmxIdeGetFileLen();
    if(u4InputFileLen == 0)	//christie 080128
    {
    	return (FALSE);
    }

	#ifdef SEMI_RING_FIFO
	*u4FileLength = u4InputFileLen; //xiaolei.li's
	#endif
    if ( u4InputFileLen > u4TargSize)
    	   u4InputFileLen = u4TargSize - 1;
    #ifndef SEMI_RING_FIFO
    *u4FileLength = u4InputFileLen;
	#endif
    _DmxIdeRead(u4Addr, u4InputFileLen);
    
    return (TRUE);
}


BOOL _DmxIdeOpenFile(void)
{
    UINT8 u1Msg;
    UINT32 u4InputFileLen;
    
    _DmxIdeReset();
    
    u1Msg = IDE_MSG_OPENFILE;
    _DmxIdeSendMsg(u1Msg);
    _DmxIdeGetNextCmd(&u1Msg);  // read file length from PC
 
    ASSERT(u1Msg == IDE_MSG_TXFILE || u1Msg == 0x00);
    u4InputFileLen = _DmxIdeGetFileLen();
    _DmxIdeReset();
    if(u4InputFileLen == 0)	//christie 080128
    {
        return (FALSE);
    }
    return (TRUE);
}

BOOL _DmxIdeWrite(UINT32 u4Addr, UINT32 u4Size)
{
	CRIT_STATE_T rState;

	rState = x_crit_start();

    //while (u4Size > 0)
    {
        // Flush cache
        //HalFlushDCache();

        // Trigger IDE
	    //IDE_DMA_SOURCE = PHYSICAL((UINT32)u4Addr);
	    IDE_DMA_SOURCE = (UINT32)u4Addr;
	    IDE_DMA_LEN = u4Size;
	    IDE_DMA_CONTROL |= 1;

        // Wait for completion
	    while ((IDE_DMA_CONTROL & 0x1) != 0)
	    {
	    }

        // Update left size and data pointer
        //u4Size -= u4Size;
    }

    x_crit_end(rState);

	return TRUE;
}


void _DmxIdeSendData(UINT32 u4Addr, UINT32 u4Mode, UINT32 u4Len)
{
  UINT8 u1Msg;

  if(u4Mode == 7)
  {
    u1Msg = IDE_MSG_RXWB;
  }
  else
  {
    u1Msg = IDE_MSG_RXWBA;
  }
  _DmxIdeSendMsg(u1Msg);
  // send the length
  u1Msg = u4Len >> 25;
  _DmxIdeSendMsg(u1Msg);
  u1Msg = (u4Len >> 18) & 0x7F;
  _DmxIdeSendMsg(u1Msg);
  u1Msg = (u4Len >> 11) & 0x7F;
  _DmxIdeSendMsg(u1Msg);
  u1Msg = (u4Len >> 4) & 0x7F;
  _DmxIdeSendMsg(u1Msg);
  u1Msg = u4Len & 0x0F;
  _DmxIdeSendMsg(u1Msg);  
  
  // wait for PC ready
  _DmxIdeGetNextCmd(&u1Msg);
  if(u4Mode == 7)
  {
    ASSERT(u1Msg == IDE_MSG_RXWB);
  }
  else
  {
    ASSERT(u1Msg == IDE_MSG_RXWBA);
  }

  _DmxIdeWrite(u4Addr, u4Len);
  
  // wait for ACK
  //_DmxIdeGetNextCmd(&u1Msg);
  //ASSERT(u1Msg == IDE_MSG_RXFB);
}

#ifdef SEMI_RING_FIFO
void _DmxIdeSeek(UINT32 u4SeekOffset, UINT8 u1SeekPoint)
{
	CHAR strSeekOffset[100];
	CHAR strSeekPoint[2];
	UINT32 i;
	sprintf(strSeekOffset, "%d", u4SeekOffset);
	sprintf(strSeekPoint, "%d", u1SeekPoint);
	_DmxIdeSendMsg(IDE_MSG_SEEKOFF); 
	for (i = 0; strSeekOffset[i] != 0; i++)
    {
    	_DmxIdeSendMsg(strSeekOffset[i]);
	}
	_DmxIdeSendMsg(IDE_MSG_SEEKOFF);

	_DmxIdeSendMsg(IDE_MSG_SEEKPINT); 
	for (i = 0; strSeekPoint[i] != 0; i++)
	{
		_DmxIdeSendMsg(strSeekPoint[i]);
	}
	_DmxIdeSendMsg(IDE_MSG_SEEKPINT);
}
void _DmxIdeSendReadLen(UINT32 u4ReadLength)
{
	CHAR strSendReadLength[100];
	UINT32 i;
	sprintf(strSendReadLength, "%d", u4ReadLength);
	_DmxIdeSendMsg(IDE_MSG_READLEN); 
	for (i = 0; strSendReadLength[i] != 0; i++)
	{
		_DmxIdeSendMsg(strSendReadLength[i]);
	}
	_DmxIdeSendMsg(IDE_MSG_READLEN); 
}
#endif
