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
 * $RCSfile: uart_drv.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file uart_drv.c
 *  Brief of file uart_drv.c.
 *  Details of file uart_drv.c (optional).
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "uart_debug.h"
#include "uart_hw_drvif.h"
#include "uart_drv.h"
#include "drvcust_if.h"

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_serial.h"
LINT_EXT_HEADER_END
//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

/* Ctrl + q */
#define MAGIC_QUERY         (UINT8)(17)
/* Ctrl + o */
#define MAGIC_OUTPUT_CHANGE (UINT8)(15)
#define MAGIC_OUT_NUMBER    (UINT8)(3)
#define UART_OUTPUT_CLI     (UINT8)(3)
#define UART_OUTPUT_MW      (UINT8)(2)
#define UART_OUTPUT_ALL     (UINT8)(1)

/* Ctrl + i */
#define MAGIC_INPUT_CHANGE  (UINT8)(9)
#define MAGIC_INPUT_NUMBER  (UINT8)(2)
#define UART_INPUT_FACTORY  (UINT8)(3)
#define UART_INPUT_CLI      (UINT8)(2)
#define UART_INPUT_MW       (UINT8)(1)

/* Force switching to driver CLI and pass char to driver */
// #define MAGIC_INPUT_FORCE_FACTORY  (UINT8)(0xc0)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
typedef struct _SWTXPARAM_T
{
    UINT32 u4Port;
    const UINT8 *pData;
    UINT32 u4TotalLength;  ///< user's total data length.
    UINT32 u4ActualLength; ///< actual transfer data length.
    UINT32 u4FifoLength;   ///< write to tx fifo length when fifo write.
    BOOL fgBlockedMode;
    BOOL fgDMAEnable;
} SWTXPARAM_T;

typedef struct __BUFFER_INFO
{
    UINT32 Read;        /* @field Current Read index. */
    UINT32 Write;       /* @field Current Write index. */
    UINT32 Length;      /* @field Length of buffer */
    UINT8 *CharBuffer;  /* @field Start of buffer */
} BUFFER_INFO;

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

#define CHECK_PORT_NUMBER(port) ASSERT((port) < (UART_PORT_NUM))

//#define ResetFifo(Buffer)           Buffer.Write = Buffer.Read = 0
#define BWrite(Buffer)          (Buffer->Write)
#define BRead(Buffer)           (Buffer->Read)
#define BLength(Buffer)         (Buffer->Length)
#define BuffWrite(Buffer)       (Buffer->CharBuffer+Buffer->Write)
#define BuffRead(Buffer)        (Buffer->CharBuffer+Buffer->Read)

#define BWrite_addr(Buffer)     (Buffer.Write)
#define BRead_addr(Buffer)      (Buffer.Read)
#define BLength_addr(Buffer)    (Buffer.Length)
#define BuffWrite_addr(Buffer)  (Buffer.CharBuffer+Buffer.Write)
#define BuffRead_addr(Buffer)   (Buffer.CharBuffer+Buffer.Read)
#define Buff_EndAddr(Buffer)    (Buffer.CharBuffer+Buffer.Length-1)
#define Buff_StartAddr(Buffer)  (Buffer.CharBuffer)

#define Buff_isEmpty            1
#define Buff_notEmpty           0
#define Buff_isFull             1
#define Buff_notFull            0
#define Buff_PushOK             0
#define Buff_PushErr            1
#define Buff_PopOK              0
#define Buff_PopErr             1

#define Buf_init(_Buffer,_Buffaddr,_uTotalSize)                   \
{                                                                 \
    BUFFER_INFO *_Buf=_Buffer;                                    \
    _Buf->Read = 0;                                               \
    _Buf->Write = 0;                                              \
    _Buf->Length = _uTotalSize;                                   \
    _Buf->CharBuffer = _Buffaddr;                                 \
}                                                                 \

#define Buf_IsFull(_Buffer,_result)                               \
{                                                                 \
    BUFFER_INFO *_Buf=_Buffer;                                    \
    kal_uint16 _tmp = BRead(_Buf);                                \
    if (_tmp == 0)                                                \
        _tmp = BLength(_Buf);                                     \
    if ((_tmp-BWrite(_Buf)) == 1)                                 \
    {                                                             \
        _result = Buff_isFull;                                    \
    }                                                             \
    else                                                          \
    {                                                             \
        _result = Buff_notFull;                                   \
    }                                                             \
}                                                                 \

#define Buf_GetRoomLeft(_Buffer,_RoomLeft)                        \
{                                                                 \
    BUFFER_INFO *_Buf=_Buffer;                                    \
    if (BRead(_Buf) <= BWrite(_Buf))                              \
    {                                                             \
        _RoomLeft = (BLength(_Buf) - BWrite(_Buf)) + (BRead(_Buf) - 1); \
    }                                                             \
    else                                                          \
    {                                                             \
        _RoomLeft = (BRead(_Buf) - BWrite(_Buf)) - 1;             \
    }                                                             \
}                                                                 \

#define Buf_Push(_Buffer,_pushData)                               \
{                                                                 \
    BUFFER_INFO *_Buf=_Buffer;                                    \
    *BuffWrite(_Buf) = _pushData;                                 \
    if(BWrite(_Buf) >= (BLength(_Buf) - 1))                       \
    {                                                             \
        BWrite(_Buf) = 0;                                         \
    }                                                             \
    else                                                          \
    {                                                             \
        BWrite(_Buf)++;                                           \
    }                                                             \
}                                                                 \

#define Buf_GetBytesAvail(_Buffer,_BytesAvail)                    \
{                                                                 \
    BUFFER_INFO *_Buf = _Buffer;                                  \
    _BytesAvail = 0;                                              \
    if (BWrite(_Buf) >= BRead(_Buf))                              \
    {                                                             \
        _BytesAvail = BWrite(_Buf) - BRead(_Buf);                 \
    }                                                             \
    else                                                          \
    {                                                             \
        _BytesAvail = (BLength(_Buf) - BRead(_Buf)) + BWrite(_Buf); \
    }                                                             \
}                                                                 \

#define Buf_Pop(_Buffer,_popData)                                 \
{                                                                 \
    BUFFER_INFO *_Buf = _Buffer;                                  \
    _popData= *BuffRead(_Buf);                                    \
    BRead(_Buf)++;                                                \
    if (BRead(_Buf) >= BLength(_Buf))                             \
    {                                                             \
        BRead(_Buf) -= BLength(_Buf);                             \
    }                                                             \
}                                                                 \

/* should be deleted */
#define Buf_IsEmpty(_Buffer,_result)   \
{                                      \
    BUFFER_INFO *_Buf = _Buffer;       \
    if ( BRead(_Buf) == BWrite(_Buf) ) \
    {                                  \
        _result = Buff_isEmpty;        \
    }                                  \
    else                               \
    {                                  \
        _result = Buff_notEmpty;       \
    }                                  \
}                                      \

/* void Get32FromBuff(BUFFER_INFO *Buf,kal_uint32 DATA) */
#define Get32FromBuf(_Buffer,_DATA)        \
{                                          \
    BUFFER_INFO *_Buf = _Buffer;           \
    kal_uint8    _tmp,_index;              \
    kal_uint32   _tmp32;                   \
    _DATA =0;                              \
    for (_index =0;_index < 4;_index++)    \
    {                                      \
        Buff_Pop(_Buf,&_tmp);              \
        _tmp32 = (kal_uint32)_tmp;         \
        (_DATA) |= (_tmp32 << (8*_index)); \
    }                                      \
}                                          \

/*void Put32toBuff(BUFFER_INFO *Buf,kal_uint32 *DATA)*/
#define Put32toBuf(_Buffer,_DATA)          \
{                                          \
    BUFFER_INFO *_Buf = _Buffer;           \
    kal_uint8   _tmp,_index;               \
    kal_uint32  _tmp32;                    \
    for (_index =0;_index < 4;_index++)    \
    {                                      \
        _tmp32 = ((*_DATA) >> (8*_index)); \
        _tmp = (kal_uint8)_tmp32;          \
        Buff_Push(_Buf,&_tmp);             \
    }                                      \
}                                          \

#define Buf_Flush(_Buffer)                 \
{                                          \
    BUFFER_INFO *_Buf = _Buffer;           \
    _Buf->Write = 0; \
    _Buf->Read = 0; \
}

#define Buf_look(_Buffer,_popData,_num)    \
{                                          \
   BUFFER_INFO *_Buf = _Buffer;            \
   kal_uint8 _index;                       \
   kal_uint16 _tmp;                        \
   _tmp = BRead(Buf);                      \
   for(_index=0;_index<_num;_index++)      \
   {                                       \
       *_popData= *(Buf->CharBuffer+_tmp); \
       _tmp++;                             \
       if (_tmp >= BLength(Buf))           \
       {                                   \
           _tmp -= BLength(Buf);           \
       }                                   \
    }                                      \
}

#define CHK_UART_RX_INT(u1Port, value)  (value &                                                          \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              (INT_SER0_RX_TOUT|INT_SER0_RX_FULL|INT_SER0_RX_RISC_DATA) : \
                                              (INT_SER1_RX_TOUT|INT_SER1_RX_FULL)))

#define CHK_UART_DMA_RX_INT(u1Port, value)  (value &                                                      \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              (INT_SER0_RX_TOUT|INT_SER0_RX_FULL|INT_SER0_RX_RISC_DATA) : \
                                              (INT_SER1_DMAW_TOUT |INT_SER1_DMAW_OVERFLOW | INT_SER1_DMAW_FULL)))

#define CHK_UART_RX_LAST_INT(u1Port, value)  ((value &                                                    \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              INT_SER0_RX_TOUT : INT_SER1_RX_TOUT)) && ((value &          \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              INT_SER0_RX_FULL : INT_SER1_RX_FULL))==0))

#define CHK_UART_TX_INT(u1Port, value)  (value &                                                          \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              INT_SER0_TX_EMPTY : (INT_SER1_TX_EMPTY | INT_SER1_DMAR_EMPTY)))

#define CHK_UART_RX_OVERFLOW(u1Port, value)  (value &                                                     \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              INT_SER0_RX_OVERFLOW : INT_SER1_RX_OVERFLOW))

#define CHK_UART_RX_ERROR(u1Port, value)  (value &                                                        \
                                             ((u1Port == UART_PORT_0) ?                                   \
                                              INT_SER0_RX_ERR : INT_SER1_RX_ERR))

#define UART_VECTOR(u1Port)  ((u1Port == (UART_PORT_2)) ? (VECTOR_PDWNC) : (VECTOR_RS232))

#define IRQ_ENABLE(u1Port) \
    if (_u4UartIrqEnable[u1Port]) \
    { \
    UNUSED(BIM_EnableIrq(UART_VECTOR(u1Port)));\
    }

#define IRQ_DISABLE(u1Port) \
    _u4UartIrqEnable[u1Port] = BIM_IsIrqEnabled(UART_VECTOR(u1Port)); \
    if (_u4UartIrqEnable[u1Port]) \
    { \
        UNUSED(BIM_DisableIrq(UART_VECTOR(u1Port)));\
    }

// Uart only handle visible character. 
// When receive character > UART_MIN_FACTORY_KEY and character = _u4MagicMaxInputForceFactory or _u4MagicMinInputForceFactory
// it will enter factory mode. Uart driver will notify MW by _pfMwFactoryInput() when receive new character.
// MW will receive data by calling  UART_Read().
#define UART_MIN_FACTORY_KEY    (0x20)
#define UART_MAX_FACTORY_KEY    (0x80)

#define UART_TX_DMA_BLOCKSIZE    (256)
//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static BOOL _fgUartInitialed = FALSE;

static HANDLE_T _ahTxNotifySema[UART_PORT_NUM];
static HANDLE_T _ahRxNotifySema[UART_PORT_NUM];
static HANDLE_T _ahTxProtectSema[UART_PORT_NUM];
static HANDLE_T _ahRxProtectSema[UART_PORT_NUM];

#if defined(CLI_SUPPORT) || defined(CLI_STRESS_MODE) //EXT_DBG_DEV

static UINT32 _u4InputMode = UART_INPUT_MW;    // 0: cli  1: mw
#else

static UINT32 _u4InputMode = UART_INPUT_CLI;   // 0: cli  1: mw
#endif

static UINT32 _u4FactoryFlag = 0;
static UINT32 _u4OutputMode = UART_OUTPUT_ALL; // 1: all  2: cli 3:mw

static SWTXPARAM_T _arTxParam[UART_PORT_NUM];
static UINT8 _au1RxBuf[UART_PORT_NUM][SW_FIFO_SIZE];
static BUFFER_INFO _arRxFIFO[UART_PORT_NUM];
static RS_232_NFY_INFO_T _arRs232NfyInfo[UART_PORT_NUM];
static PF_DBG_INPUT_T _pfMwCliDbgInput = NULL;
static PF_MODEM_NOTIFY_T _pfModemNfy = NULL;

/// Tx private tag
static UINT32 _arTxPrivateTag[UART_PORT_NUM];

/// factory mode variable.
static PF_DBG_INPUT_T _pfMwFactoryInput = NULL;
static UINT32 _u4MagicMaxInputForceFactory = 0xC0;
static UINT32 _u4MagicMinInputForceFactory = 0x10;

/// debug port number, normally = COM0, define in serial.c.
static UINT32 _u4DebugPort;

static UINT32 _u4UartIrqEnable[UART_PORT_NUM] = {TRUE, TRUE};

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
//-------------------------------------------------------------------------
/** _Uart_InOutQuery
 *  query cli input/output mode.
 *  @param  void.
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_InOutQuery(void)
{
    Printf("\n===================\n");

    if (_u4InputMode == UART_INPUT_MW)
    {
        Printf("Input:  MW\n");
    }
    else
    {
        Printf("Input:  CLI\n");
    }

    if (_u4OutputMode == UART_OUTPUT_MW)
    {
        Printf("Output: MW only\n");
    }
    else if (_u4OutputMode == UART_OUTPUT_CLI)
    {
        Printf("Output: CLI only\n");
    }
    else
    {
        Printf("Output: CLI and MW\n");
    }

    Printf("===================\n");
}

//-------------------------------------------------------------------------
/** _Uart_OutputSwitch
 *  switch cli output mode.
 *  @param  void.
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_OutputSwitch(void)
{
    _u4OutputMode++;

    if (_u4OutputMode > MAGIC_OUT_NUMBER)
    {
        _u4OutputMode = UART_OUTPUT_ALL;
    }

    if (_u4OutputMode == UART_OUTPUT_CLI)
    {
        // enable driver debug log.
        UNUSED(UTIL_SetLogFlag(TRUE));
    }
    else if (_u4OutputMode == UART_OUTPUT_MW)
    {
        // disable driver debug log.
        UNUSED(UTIL_SetLogFlag(FALSE));
    }
    else if (_u4OutputMode == UART_OUTPUT_ALL)
    {
        // enable driver debug log.
        UNUSED(UTIL_SetLogFlag(TRUE));
    }

    // show cli input and output mode status.
    _Uart_InOutQuery();
}

//-------------------------------------------------------------------------
/** _Uart_SendRcvDataEvent
 *  notify user when uart receive data byte.
 *  @param  u1Port      uart port number.
 *  @param  u4DataSZ     uart rx data length.
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_SendRcvDataEvent(UINT8 u1Port, UINT32 u4DataSZ)
{
    UINT32 i;

    UINT8 ucCh;
    UINT32 u4DataAvail;
    BUFFER_INFO *prRxFIFO;

    if (u4DataSZ == 0)
    {
        return;
    }

    if (u1Port == _u4DebugPort)
    {
        // must be transparent mode 
        if ((u1Port != UART_PORT_0) || UART_HwTransMode())
        {
            if ((_u4InputMode == UART_INPUT_FACTORY) && (_pfMwFactoryInput != NULL))
            {
                // only transfer data length.
                _pfMwFactoryInput((INT32)u4DataSZ);
            }
            else if ((_u4InputMode == UART_INPUT_MW) && (_pfMwCliDbgInput != NULL))
            {
                prRxFIFO = &_arRxFIFO[u1Port];
                Buf_GetBytesAvail(prRxFIFO, u4DataAvail);

                ASSERT(u4DataAvail > 0);
                for (i = 0; i < u4DataAvail; i++)
                {
                    Buf_Pop(prRxFIFO, ucCh);
                    _pfMwCliDbgInput((INT32)ucCh);
                }
            }
            else if (_u4InputMode == UART_INPUT_CLI)
            {
                VERIFY(x_sema_unlock(_ahRxNotifySema[u1Port]) == OSR_OK);
            }
            else
            {
                Printf("UART_INPUT_MW: MW RS232 notify function hasn't been set\n");
            }
        }
        else
        {
            VERIFY(x_sema_unlock(_ahRxNotifySema[u1Port]) == OSR_OK);
        }
    }
    else
    {
        if (!_arRs232NfyInfo[u1Port].pf_rs_232_nfy)
        {
            VERIFY(x_sema_unlock(_ahRxNotifySema[u1Port]) == OSR_OK);
        }
        else
        {
            _arRs232NfyInfo[u1Port].pf_rs_232_nfy((VOID *)_arRs232NfyInfo[u1Port].pv_tag,
                                                  RS_232_COND_REC_BUFFER,
                                                  u4DataSZ);
        }
    }
}

//-------------------------------------------------------------------------
/** _Uart_CheckMagicChar
 *  uart HISR handler.
 *  @param  u1Port      uart port number.
 *  @param  ucChar     uart rx data byte.
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_CheckMagicChar(UINT8 u1Port, UINT8 ucChar)
{
    // In (1) not debug port (2) factory mode (3) normal mode, do not check ucChar = magic key !
    if ((_u4DebugPort != u1Port) || (_u4InputMode == UART_INPUT_FACTORY) || (!UART_HwTransMode()))
    {
        return;
    }

    if (ucChar == MAGIC_INPUT_CHANGE)
    {
        UNUSED(UART_InputSwitch());
        _Uart_InOutQuery();
    }
    else if (ucChar == MAGIC_OUTPUT_CHANGE)
    {
        _Uart_OutputSwitch();
    }
    else if (ucChar == MAGIC_QUERY)
    {
        _Uart_InOutQuery();
    }
    else if (((_u4MagicMaxInputForceFactory > UART_MAX_FACTORY_KEY) &&
              ((ucChar == (UINT8)(_u4MagicMaxInputForceFactory & 0xff)) ||
               (ucChar == (UINT8)((_u4MagicMaxInputForceFactory + 2) & 0xff)) ||
               (ucChar == (UINT8)((_u4MagicMaxInputForceFactory + 4) & 0xff)))) ||
             ((_u4MagicMinInputForceFactory < UART_MIN_FACTORY_KEY) &&
              (ucChar == (UINT8)(_u4MagicMinInputForceFactory & 0xff))))
             
    {
        // switch to factory mode.
        _u4InputMode = UART_INPUT_FACTORY;
    }
    else if (_u4FactoryFlag == 1)
    {
        _u4InputMode = UART_INPUT_FACTORY;
        _u4FactoryFlag = 0;
    }
}

//-------------------------------------------------------------------------
/** _Uart_Isr
 *  uart HISR handler.
 *  @param  u2Vector  uart interrupt vector number.
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_Isr(UINT16 u2Vector)
{
    UINT32 i;

    UINT8 u1Port = UART_PORT_0;
    UINT32 u4RxCnt;
    UINT32 u4TxCnt;
    UINT32 u4RomeLeft;
    UINT32 u4IntStatus;
    UINT8 u1Data;

    UINT8 *pData;
    SWTXPARAM_T *prTxParam;
    BUFFER_INFO *prRxFIFO;

    if ((u2Vector != VECTOR_RS232) & (u2Vector != VECTOR_PDWNC))
    {
        return;
    }
    
    u1Port = (u2Vector == VECTOR_RS232) ? UART_PORT_0 : UART_PORT_PD;

    u4IntStatus = UART_HwGetIntStatus(u1Port);

    // Clear BIM uart status
    VERIFY(BIM_ClearIrq(u2Vector));

    UART_HwClearIntStatus(u1Port);

    for (u1Port = UART_PORT_0; u1Port <= UART_PORT_1; u1Port++)
    {
        // Case1: Overflow/Error 
        if (CHK_UART_RX_OVERFLOW(u1Port, u4IntStatus) ||
            CHK_UART_RX_ERROR(u1Port, u4IntStatus))
        {
            printf("_Uart_Isr() overflow, error status = 0x%08X !!!\n",
                   u4IntStatus);
            ASSERT(0);
        }

        // Case2: Rx interrupt
        // Check if Rx interrupt, 
        // if yes a. read data to software fifo 
        //        b. call notify function
        if (CHK_UART_DMA_RX_INT(u1Port, u4IntStatus))
        {
            if (u1Port == UART_PORT_1)
            {
                u4RxCnt = UART_HwGetRxDataCnt(u1Port);
                if (u4RxCnt > 0)
                {
                    _Uart_SendRcvDataEvent(u1Port, u4RxCnt);
                }                        
            }
            else
            {
                prRxFIFO = &_arRxFIFO[u1Port];
            
                Buf_GetRoomLeft(prRxFIFO, u4RomeLeft);

                u4RxCnt = UART_HwGetRxDataCnt(u1Port);
                //ASSERT(u4RxCnt);            
                u4RxCnt = MIN(u4RomeLeft, u4RxCnt);

                if (u4RxCnt > 0)
                {
                    // Read data into software fifo
                    for (i = 0; i < u4RxCnt; i++)
                    {
                        u1Data = UART_READ_DATA(u1Port);

                        _Uart_CheckMagicChar(u1Port, u1Data);

                        // push to software fifo.
                        Buf_Push(prRxFIFO, u1Data);
                    }
                    _Uart_SendRcvDataEvent(u1Port, u4RxCnt);
                }            
            }        
        }

        // Case3: Tx interrupt
        // if yes a. read data from software fifo and write to uart hardware
        //        b. call notify function

        if (CHK_UART_TX_INT(u1Port, u4IntStatus))
        {
            prTxParam = &_arTxParam[u1Port];

            ASSERT(prTxParam->u4Port == u1Port);
            ASSERT(prTxParam->pData);

            if (!prTxParam->fgDMAEnable)
            {
                u4TxCnt = UART_HwGetTxEmptyCnt(u1Port);
                ASSERT(prTxParam->u4FifoLength <= u4TxCnt);
                UNUSED(u4TxCnt);
            }
            else
            {
                ASSERT(prTxParam->u4Port == UART_PORT_1);
                ASSERT(u4IntStatus & INT_SER1_DMAR_EMPTY);
                UART_HwDmaReadDisable();
            }
        
            prTxParam->u4ActualLength += prTxParam->u4FifoLength;
        
            if (prTxParam->u4TotalLength > prTxParam->u4ActualLength)
            {
                pData = (UINT8 *)((UINT32)prTxParam->pData + prTxParam->u4ActualLength);

                if (!prTxParam->fgDMAEnable)
                {   
                    prTxParam->u4FifoLength = UART_HwGetTxEmptyCnt(u1Port);
                    prTxParam->u4FifoLength = 
                        MIN((prTxParam->u4TotalLength - prTxParam->u4ActualLength),
                            prTxParam->u4FifoLength);
                    UART_HwDiableTxIntr(u1Port);                                          
                    UART_HwWrite(u1Port, pData, prTxParam->u4FifoLength);
                    UART_HwEnableTxIntr(u1Port);            
                }
                else
                {
                    prTxParam->u4FifoLength = 
                        MIN((prTxParam->u4TotalLength - prTxParam->u4ActualLength),                
                            UART_TX_DMA_BLOCKSIZE);
                    HalFlushInvalidateDCache();
                    UART_HwDmaReadDram((UINT32)pData, prTxParam->u4FifoLength,
                                       FALSE);                            
                }
            }
            else
            {
                ASSERT(prTxParam->u4TotalLength == prTxParam->u4ActualLength);

                // disable tx interrupt.
                UART_HwDiableTxIntr(u1Port);
                // notify user.
                if (prTxParam->fgBlockedMode)
                {
                    VERIFY(x_sema_unlock(_ahTxNotifySema[u1Port]) == OSR_OK);
                }
                else
                {
                    ASSERT(_arRs232NfyInfo[u1Port].pf_rs_232_nfy);
                    _arRs232NfyInfo[u1Port].pf_rs_232_nfy(
                        (VOID *)_arRs232NfyInfo[u1Port].pv_tag, 
                        RS_232_COND_XMT_EMPTY, _arTxPrivateTag[u1Port]);
                }
            }
        }

        if (u1Port == UART_PORT_1)
        {
            // Check UART2 Modem interrupt
            if (UART_READ32(REG_SER_INT_STATUS) & INT_SER1_MODEM)
            {
                if (_pfModemNfy)
                {
                    _pfModemNfy();
                }
                LOG(5, "INT_UART1_MRIE\n");
            }
        }
    }
// Clear Local UART interrupt
//UART_WRITE32(REG_SER_INT_STATUS, (u1Port == UART_PORT_0) ? /*0xff0000*/0xff : 0xff0000);  //FIXME

// Clear UART interrupt
//VERIFY(BIM_ClearIrq(u2Vector));
}

//-------------------------------------------------------------------------
/** _Uart_SwInit
 *  Initialize software component.
 *  @param  void
 *  @retval   void
 */
//-------------------------------------------------------------------------
static void _Uart_SwInit(void)
{
    INT32 i4Idx;
    x_os_isr_fct pfnOldIsr;

    _u4DebugPort = SerGetDebugPortNum();

    // Create message queue
    for (i4Idx = 0; i4Idx < UART_PORT_NUM; i4Idx++)
    {
        VERIFY(x_sema_create(&_ahTxNotifySema[i4Idx], X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_LOCK) == OSR_OK);

        VERIFY(x_sema_create(&_ahRxNotifySema[i4Idx], X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_LOCK) == OSR_OK);
        VERIFY(x_sema_create(&_ahTxProtectSema[i4Idx], X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY(x_sema_create(&_ahRxProtectSema[i4Idx], X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_UNLOCK) == OSR_OK);

        Buf_init(&_arRxFIFO[i4Idx], &_au1RxBuf[i4Idx][0], SW_FIFO_SIZE);
        _arTxPrivateTag[i4Idx] = 0;
    }
    
    VERIFY(x_reg_isr(VECTOR_RS232, _Uart_Isr, &pfnOldIsr) == OSR_OK);
    VERIFY(x_reg_isr(VECTOR_PDWNC, _Uart_Isr, &pfnOldIsr) == OSR_OK);

#ifdef UART_MODEM

    UART_ModemInit(); // FIXME
#endif
}

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
//-------------------------------------------------------------------------
/** DBG_FactorySetCallback
 *  set factory cli input mode notify function.
 *  @param  fgEnable    True = set notify function, False = clear notify function.
 *  @param  pfInputCb   factory cli notify function.
 *  @retval   TRUE.
 */
//-------------------------------------------------------------------------
INT32 DBG_FactorySetCallback(BOOL fgEnable, const PF_DBG_INPUT_T pfInputCb)
{
    _pfMwFactoryInput = fgEnable ? pfInputCb : NULL;

    return TRUE;
}

//-------------------------------------------------------------------------
/** DBG_SetCallback
 *  set mw cli input mode notify function.
 *  @param  fgEnable    True = set notify function, False = clear notify function.
 *  @param  pfInputCb   mw cli notify function.
 *  @retval   TRUE.
 */
//-------------------------------------------------------------------------
INT32 DBG_SetCallback(BOOL fgEnable, PF_DBG_INPUT_T pfInputCb)
{
    _pfMwCliDbgInput = fgEnable ? pfInputCb : NULL;

    return TRUE;
}

//-------------------------------------------------------------------------
/** UART_InputSwitch
 *  switch cli input mode.
 *  @param  void
 *  @retval   cli input mode.
 */
//-------------------------------------------------------------------------
INT32 UART_InputSwitch(void)
{
    _u4InputMode++;

    if (_u4InputMode > MAGIC_INPUT_NUMBER)
    {
        _u4InputMode = UART_INPUT_MW;
    }

    return (INT32)_u4InputMode;
}

//-------------------------------------------------------------------------
/** UART_IsOutputMwMode
 *  query if out mode is include mw mode.
 *  @param  void
 *  @retval   TRUE: include UART_OUTPUT_MW
 *  @retval   FALSE: not include UART_OUTPUT_MW
 */
//-------------------------------------------------------------------------
INT32 UART_IsOutputMwMode(void)
{
    return (_u4OutputMode <= UART_OUTPUT_MW);
}

//-------------------------------------------------------------------------
/** UART_SetModemStatusChgNotify
 *  set modem notify function.
 *  @param  pfModemNotify   modem notify function pointer.
 *  @retval   TRUE
 */
//-------------------------------------------------------------------------
#ifdef UART_MODEM
INT32 UART_SetModemStatusChgNotify(PF_MODEM_NOTIFY_T pfModemNotify)
{
    _pfModemNfy = pfModemNotify;

    return TRUE;
}
#endif
//-------------------------------------------------------------------------
/** UART_SetDebugPortFactory
 *  set debug port to factory mode or MW mode.
 *  @param  fgSet 	  TRUE: facotry mode  FALSE: enter normal mode.
 *  @retval   void
 */
//-------------------------------------------------------------------------
void UART_SetDebugPortFactory(BOOL fgSet)
{
    if (fgSet)
    {
//        _u4InputMode = UART_INPUT_FACTORY;
        _u4FactoryFlag = 1;
        SerTransparent();
    }
    else
    {
        _u4InputMode = UART_INPUT_MW;
        // enter normal mode.
        SerEnd();
    }
}

//-------------------------------------------------------------------------
/** UART_SetTxPrivateTag
 *  set uart Tx private tag.
 *  @param  u1Port 	        uart port number.
 *  @param  u4PrivateTag    Tx private tag.
 *  @retval void.
 */
//-------------------------------------------------------------------------
void UART_SetTxPrivateTag(UINT8 u1Port, const UINT32 u4PrivateTag)
{
    CHECK_PORT_NUMBER(u1Port);
    
    _arTxPrivateTag[u1Port] = u4PrivateTag;
}

//-------------------------------------------------------------------------
/** UART_SetNotifyFuc
 *  set uart notify function.
 *  @param  u1Port 	        uart port number.
 *  @param  pData 		Pointer to user's buffer.
 *  @param  u4NumToRead user's data length.
 *  @param  fgBlockedMode blocking or nonblocking call.
 *  @retval   >= 0		Actual number of bytes to be read.
 */
//-------------------------------------------------------------------------
void UART_SetNotifyFuc(UINT8 u1Port, const RS_232_NFY_INFO_T *prRs232NfyInfo)
{
    CHECK_PORT_NUMBER(u1Port);

    ASSERT(prRs232NfyInfo);

    _arRs232NfyInfo[u1Port].pv_tag = prRs232NfyInfo->pv_tag;
    _arRs232NfyInfo[u1Port].pf_rs_232_nfy = prRs232NfyInfo->pf_rs_232_nfy;
}

//-------------------------------------------------------------------------
/** UART_Read
 *  read data from specific uart port.
 *  @param  u1Port 	        uart port number.
 *  @param  pData 		Pointer to user's buffer.
 *  @param  u4NumToRead user's data length.
 *  @param  fgBlockedMode blocking or nonblocking call.
 *  @retval   >= 0		Actual number of bytes to be read.
 */
//-------------------------------------------------------------------------
UINT32 UART_Read(UINT8 u1Port, UINT8 *pBuffer, UINT32 u4NumToRead, BOOL fgBlockedMode)
{
    BUFFER_INFO *prRxFIFO;

    UINT32 u4NumRead = 0;
    UINT32 u4DataAvail;

    ASSERT(pBuffer);
    CHECK_PORT_NUMBER(u1Port);

    prRxFIFO = &_arRxFIFO[u1Port];

    if ((u1Port == _u4DebugPort) && (_u4InputMode == UART_INPUT_FACTORY))
    {
        return UART_ReadRxFifo(u1Port, pBuffer, u4NumToRead);
    }

    // Enter critical section
    VERIFY(x_sema_lock(_ahRxProtectSema[u1Port], X_SEMA_OPTION_WAIT) == OSR_OK);

    if (fgBlockedMode)
    {
        while (u4NumRead < u4NumToRead)
        {
            if (u1Port ==  UART_PORT_1)
            {
                u4DataAvail = UART_HwGetRxDataCnt(u1Port);
            }
            else
            {
                Buf_GetBytesAvail(prRxFIFO, u4DataAvail);
            }
            if (u4DataAvail == 0)
            {
                VERIFY(x_sema_lock(_ahRxNotifySema[u1Port], X_SEMA_OPTION_WAIT) == OSR_OK);
            }
            u4DataAvail = UART_ReadRxFifo(u1Port, pBuffer, (u4NumToRead - u4NumRead));

            pBuffer += u4DataAvail;
            u4NumRead += u4DataAvail;            
        }
    }
    else
    {
        u4NumRead = UART_ReadRxFifo(u1Port, pBuffer, u4NumToRead);
    }

    // Exit critical section
    VERIFY(x_sema_unlock(_ahRxProtectSema[u1Port]) == OSR_OK);

    return u4NumRead;
}

//-------------------------------------------------------------------------
/** UART_ReadRxFifo
 *  read data from rx sw fifo.
 *  @param  u1Port 	        uart port number.
 *  @param  pBuffer 		Pointer to user's buffer.
 *  @param  u4NumToRead user's dedicated data length.
 *  @retval   >= 0		Actual number read bytes.
 */
//-------------------------------------------------------------------------
UINT32 UART_ReadRxFifo(UINT8 u1Port, UINT8 *pBuffer, UINT32 u4NumToRead)
{
    BUFFER_INFO *prRxFIFO;

    UINT32 u4DataAvail;
    UINT32 i;
    
    ASSERT(pBuffer);
    CHECK_PORT_NUMBER(u1Port);

    if (u1Port ==  UART_PORT_1)
    {
        HalFlushInvalidateDCache();
        u4DataAvail = UART_GetRxDMABuf(u1Port, pBuffer, u4NumToRead);
    }
    else
    {
        prRxFIFO = &_arRxFIFO[u1Port];

        Buf_GetBytesAvail(prRxFIFO, u4DataAvail);

        u4DataAvail = MIN(u4DataAvail, u4NumToRead);

        for (i = 0; i < u4DataAvail; i++)
        {
            Buf_Pop(prRxFIFO, pBuffer[i]);
        }
    }

    return u4DataAvail;
}

//-------------------------------------------------------------------------
/** UART_Write
 *  write data to specific uart port.
 *  @param  u1Port 	        uart port number.
 *  @param  pData 		Pointer to user's buffer.
 *  @param  u4NumberToWrite user's data length.
 *  @param  fgBlockedMode blocking or nonblocking call.
 *  @retval   >= 0		Actual number of bytes to be written.
 */
//-------------------------------------------------------------------------
UINT32 UART_Write(UINT8 u1Port, const UINT8 *pData, UINT32 u4NumberToWrite, BOOL fgBlockedMode)
{
    UINT32 u4Ret = 0;

    SWTXPARAM_T *prTxParam;

    ASSERT(pData);
    CHECK_PORT_NUMBER(u1Port);

    if (u4NumberToWrite == 0)
    {
        return 0;
    }

    VERIFY(x_sema_lock(_ahTxProtectSema[u1Port], X_SEMA_OPTION_WAIT) == OSR_OK);

    if (UART_HwGetTxEmptyCnt(u1Port) >= u4NumberToWrite)
    {
        UART_HwDiableTxIntr(u1Port);
        UART_HwWrite(u1Port, pData, u4NumberToWrite);
        u4Ret = u4NumberToWrite;

        if (!fgBlockedMode)
        {
            VERIFY(x_sema_unlock(_ahTxProtectSema[u1Port]) == OSR_OK);
        
            // call back user's notify.
            ASSERT(_arRs232NfyInfo[u1Port].pf_rs_232_nfy);
            _arRs232NfyInfo[u1Port].pf_rs_232_nfy(
                (VOID *)_arRs232NfyInfo[u1Port].pv_tag, RS_232_COND_XMT_EMPTY,
                _arTxPrivateTag[u1Port]);
            return u4Ret;
        }
    }
    else
    {
        prTxParam = &_arTxParam[u1Port];

        prTxParam->u4Port = u1Port;
        prTxParam->pData = pData;
        prTxParam->u4TotalLength = u4NumberToWrite;
        prTxParam->u4ActualLength = 0;
        prTxParam->fgBlockedMode = fgBlockedMode;
        
        if (u1Port == UART_PORT_1)
        {
            prTxParam->fgDMAEnable = TRUE;
            HalFlushInvalidateDCache();
            prTxParam->u4FifoLength = MIN(u4NumberToWrite, UART_TX_DMA_BLOCKSIZE);
            UART_HwDmaReadDram((UINT32)pData, prTxParam->u4FifoLength, FALSE);        
        }
        else
        {
            prTxParam->u4FifoLength = UART_HwGetTxEmptyCnt(u1Port);
            prTxParam->fgDMAEnable = FALSE;
            UART_HwDiableTxIntr(u1Port);
            UART_HwWrite(u1Port, pData, prTxParam->u4FifoLength);
            UART_HwEnableTxIntr(u1Port);
        }
        
        if (fgBlockedMode)
        {
            VERIFY(x_sema_lock(_ahTxNotifySema[u1Port], X_SEMA_OPTION_WAIT) == OSR_OK);
            u4Ret = prTxParam->u4ActualLength;
        }
    }

    VERIFY(x_sema_unlock(_ahTxProtectSema[u1Port]) == OSR_OK);

    return u4Ret;
}

//-------------------------------------------------------------------------
/** UART_Init
 *  uart init.
 *  @param  void.
 *  @retval   UART_SUCC  init ok.
 */
//-------------------------------------------------------------------------
INT32 UART_Init(void)
{
    if (!_fgUartInitialed)
    {
        UART_HwInit();

        _Uart_SwInit();
        
        // get factory magic key value from customer definition. this value should > 0xa0.    
        UNUSED(DRVCUST_OptQuery(eMagicMaxInputForceFactory, &_u4MagicMaxInputForceFactory));
        UNUSED(DRVCUST_OptQuery(eMagicMinInputForceFactory, &_u4MagicMinInputForceFactory));

        UART_HwDmaReadDisable();
        // dma only use in UART_PORT_1.     
        HalFlushInvalidateDCache();
        UART_HwDmaWriteDram((UINT32)&_au1RxBuf[UART_PORT_1][0], SW_FIFO_SIZE, 1);
       
        _fgUartInitialed = TRUE;
    }

    return UART_SUCC;
}

//-------------------------------------------------------------------------
/** UART_SetParameter
 *  uart set port parameter.
 *  @param  u1Port      uart port number
 *  @param  prSetupInfo     uart parameter pointer. 
 *  @retval   UART_SUCC set ok.
 */
//-------------------------------------------------------------------------
INT32 UART_SetParameter(UINT8 u1Port, const RS_232_SETUP_INFO_T *prSetupInfo)
{
    CHECK_PORT_NUMBER(u1Port);

    ASSERT(prSetupInfo);

    UART_HwSetBaudRate(u1Port, (UINT8)prSetupInfo->e_speed);
    UART_HwSetParity(u1Port, (UINT8)prSetupInfo->e_parity);
    UART_HwSetStopBit(u1Port, (UINT8)prSetupInfo->e_stop_bit);
    UART_HwSetDataLen(u1Port, (UINT8)prSetupInfo->e_data_len);

    return UART_SUCC;
}

//-------------------------------------------------------------------------
/** UART_GetParameter
 *  uart get port parameter.
 *  @param  u1Port      uart port number
 *  @param  prSetupInfo    uart parameter pointer. 
 *  @retval   UART_SUCC get ok.
 */
//-------------------------------------------------------------------------
INT32 UART_GetParameter(UINT8 u1Port, RS_232_SETUP_INFO_T *prSetupInfo)
{
    CHECK_PORT_NUMBER(u1Port);

    ASSERT(prSetupInfo);

    UART_HwGetBaudRate(u1Port, prSetupInfo);
    UART_HwGetParity(u1Port, prSetupInfo);
    UART_HwGetStopBit(u1Port, prSetupInfo);
    UART_HwGetDataLen(u1Port, prSetupInfo);

    return UART_SUCC;
}

//-------------------------------------------------------------------------
/** UART_FlushBuffer
 *  uart flush buffer.
 *  @param  u1Port      uart port number
 *  @retval   void.
 */
//-------------------------------------------------------------------------
void UART_FlushBuffer(UINT8 u1Port)
{
    CHECK_PORT_NUMBER(u1Port);

    // Flush hardware buffer
    UART_HwClearIntStatus(u1Port);
    UART_HwWaitTxBufClear(u1Port);
    UART_HwClearFifo(u1Port);
    
    IRQ_DISABLE(u1Port);
    
    Buf_Flush(&_arRxFIFO[u1Port]);
    
    IRQ_ENABLE(u1Port);
}

//-------------------------------------------------------------------------
/** UART_SetControlLine
 *  uart set port control line status.
 *  @param  u1Port      uart port number
 *  @param  ucSet       uart control line status
 *  @retval   void.
 */
//-------------------------------------------------------------------------
void UART_SetControlLine(UINT8 u1Port, UINT8 ucSet)
{
    CHECK_PORT_NUMBER(u1Port);

    // FIXME
    UNUSED(ucSet);
}

//-------------------------------------------------------------------------
/** UART_GetControlLine
 *  uart get port control line status.
 *  @param  u1Port      uart port number
 *  @retval   uart control line status.
 */
//-------------------------------------------------------------------------
INT32 UART_GetControlLine(UINT8 u1Port)
{
    CHECK_PORT_NUMBER(u1Port);

    return 1;
}

//-------------------------------------------------------------------------
/** UART_GetRcvDataLen
 *  uart get the data length of rx fifo .
 *  @param  u1Port      uart port number
 *  @retval   .
 */
//-------------------------------------------------------------------------
UINT32 UART_GetRcvDataLen(UINT8 u1Port)
{
    UINT32 u4Ret;

    CHECK_PORT_NUMBER(u1Port);
    
    IRQ_DISABLE(u1Port);

    if (u1Port ==  UART_PORT_1)
    {
        u4Ret = UART_HwGetRxDataCnt(u1Port);
    }
    else
    {
        Buf_GetBytesAvail(&_arRxFIFO[u1Port], u4Ret);
    }

    IRQ_ENABLE(u1Port);

    return u4Ret;
}

//-------------------------------------------------------------------------
/** UART_GetTxFreeLen
 *  uart get port empty byte count of tx buffer.
 *  @param  u1Port      uart port number
 *  @retval   Empty byte count of tx buffer .
 */
//-------------------------------------------------------------------------
UINT32 UART_GetTxFreeLen(UINT8 u1Port)
{
    CHECK_PORT_NUMBER(u1Port);

    return UART_HwGetTxEmptyCnt(u1Port);
}

//-------------------------------------------------------------------------
/** UART_GetCapability
 *  uart get port capability.
 *  @param  u1Port      uart port number
 *  @param  prCapability      capability buffer pointer.
 *  @retval   void.
 */
//-------------------------------------------------------------------------
void UART_GetCapability(UINT8 u1Port, RS_232_CAPABILITY_INFO_T *prCapability)
{
    CHECK_PORT_NUMBER(u1Port);

    ASSERT(prCapability);

    prCapability->ui4_speed = (RS_232_CAP_SPEED_110 | RS_232_CAP_SPEED_300 | RS_232_CAP_SPEED_1200
                                   | RS_232_CAP_SPEED_2400 | RS_232_CAP_SPEED_4800
                                   | RS_232_CAP_SPEED_9600 | RS_232_CAP_SPEED_19200
                                   | RS_232_CAP_SPEED_38400 | RS_232_CAP_SPEED_57600
                                   | RS_232_CAP_SPEED_115200 | RS_232_CAP_SPEED_230400
                                   | RS_232_CAP_SPEED_460800 | RS_232_CAP_SPEED_921600);

    prCapability->ui4_data_len = (RS_232_CAP_DATA_LEN_5 | RS_232_CAP_DATA_LEN_6
                                      | RS_232_CAP_DATA_LEN_7 | RS_232_CAP_DATA_LEN_8);

    prCapability->ui4_stop_bit = (RS_232_CAP_STOP_BIT_1);

    prCapability->ui4_parity = (RS_232_CAP_PARITY_NONE | RS_232_CAP_PARITY_EVEN
                                    | RS_232_CAP_PARITY_ODD);
}
