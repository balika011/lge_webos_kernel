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
/*****************************************************************************
*  Uart : Diagnostic command
*****************************************************************************/
//#define CC_CLI
#include "x_lint.h"

LINT_SAVE_AND_DISABLE

#ifdef __MW_CLI_DEF__
#include "x_mid_cli.h"
#else
#include "x_drv_cli.h"
#endif

#include "x_os.h"
#include "x_printf.h"
#include "x_stl_lib.h"

#define DEFINE_IS_LOG	CLI_IsLog
#include "x_debug.h"
#include "x_util.h"
#include "x_hal_5381.h"

#include "uart_hw_drvif.h"

#ifdef CC_CLI

/******************************************************************************
* Declare the macros and reglist for RegTest functions
******************************************************************************/

#define UART_BASE			RS232_BASE 
#define UART_REG_LENGTH		0

/*
REG_TEST_T arUARTRgtList[] = {
	{ REG_SER_PORT,	            eRD_ONLY,	4,	0xffffffff,	0,	0 },
	{ REG_SER01_PORT,	        eRD_ONLY,	1,	0x000000ff,	0,	0 },
	{ REG_SER01_PORT_4BYTES,	eRD_ONLY,	4,	0xffffffff,	0,	0 },
	{ REG_SER_CTR,	            eRD_WR,	    2,	0x00000ff7,	1,	0 },
	{ REG_SER01_CTR,	        eRD_WR,	    2,	0x00000ff7,	1,	0 },
	{ REG_SER_STATUS,	        eRD_ONLY,	2,	0x0000ffff,	0,	0 },
	{ REG_SER01_STATUS,	        eRD_ONLY,	2,	0x0000ffff,	0,	0 },
	{ REG_SER_BUF_CTL,	        eRD_WR,	    2,	0x0000ff0f,	1,	0x1 },
	{ REG_SER_BUF_CTL,	        eWR_ONLY,	1,	0x000000c0,	0,	0 },
	{ REG_SER01_BUF_CTL,	    eRD_WR,	    2,	0x0000ff0f,	1,	0x1 },
	{ REG_SER01_BUF_CTL,	    eWR_ONLY,   1,	0x000000c0,	0,	0 },
	{ REG_SER_INT_EN,	        eRD_WR,	    4,	0x002f002f,	1,	0 },
	{ REG_SER_INT_STATUS,	    eRD_ONLY,   4,	0x006f006f,	0,	0 },

    // End.
    { 0xffff, eNO_TYPE, -1 },
};
*/
#define BUFFER_SIZE 1024

/* Declare the debug on/off/level and RegTest functions */
CLIMOD_DEBUG_FUNCTIONS(UART)

/*CLIMOD_REGTST_FUNCTIONS(UART)*/

/* End of Declare */

extern void SerEnableMergeMode(BOOL fgEnable);
/****************************************************************************
** Audio Command
****************************************************************************/

extern INT32 UART_Test(UINT8 u1Port);

static INT32 _UartCmdTest(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UNUSED(i4Argc);
    UNUSED(szArgv);

    SerSetoutbyte(FALSE);

    UART_Test(UART_PORT_1);

    SerSetoutbyte(TRUE);

    return 1;
}

static INT32 _UartCmdSetBaud(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Port;
    UINT32 u4Baud;
    
    if (i4Argc < 2)
    {
        Printf("Arg: u4Port baudrate.\n");
        return 0;
    }
    
    u4Port = StrToInt(szArgv[1]);    

    if (u4Port > UART_PORT_1)
    {
        Printf("Arg: wrong port number.\n");
    }

    u4Baud = StrToInt(szArgv[2]);    

    Printf("u4Port = %d, baudrate = %d.\n", u4Port, u4Baud);

    UART_HwSetCustomBaudRate(u4Port, u4Baud);

    return 1;
}

static INT32 _UartCmdWriteByte(INT32 i4Argc, const CHAR **szArgv)
{
    UINT32 u4Ret;
    UINT32 u4MemLen, i;
    UINT8 *pu1Buf = (UINT8 *)0x02000000;
    UINT32 u4Port;
    
    if (i4Argc < 3)
    {
        Printf("uart.wb u4Port [byte1] [byte2] ...\n");
        return 1;
    }

    u4Port = StrToInt(szArgv[1]);    

    if (u4Port > UART_PORT_1)
    {
        Printf("Arg: wrong port number.\n");
    }

    u4MemLen = (i4Argc-2);
    if ((u4MemLen > BUFFER_SIZE) ||(u4MemLen == 0))
    {
        Printf("Error: Max data length = %d.\n", BUFFER_SIZE);
        return 1;
    }
    Printf("Data length = %d.\n", u4MemLen);

    
    for (i = 0; i < u4MemLen; i ++)
    {
        pu1Buf[i] = StrToInt(szArgv[2+i]);
    }

    // send data.   
    u4Ret = UART_Write(u4Port, (const UINT8 *)pu1Buf, u4MemLen, TRUE);
    if (u4Ret != u4MemLen)
    {
        Printf("Uart write failed, write length = %d.\n", u4Ret);
        return 1;
    }

    Printf("Uart write successfully.\n");
    return 0;
}

static INT32 _UartCmdWrite(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i;
    UINT8 *pu1Buf = (UINT8 *)0x02000000;
    UINT32 u4Ret;
    
    UART_WRITE32(0x14, 1);              //switch output pin 
    
    for (i=0 ;i<BUFFER_SIZE ; i++)
    {
        pu1Buf[i] = i%10 + 48;	
    }

    // send data.
    u4Ret = UART_Write(UART_PORT_1, pu1Buf, BUFFER_SIZE, TRUE);
    if (u4Ret != BUFFER_SIZE)
    {
        Printf("Uart write failed, write length = %d.\n", u4Ret);
        return 1;
    }

    // wait uart 0 to send out all character before switch output pin.
    //x_thread_delay(1000);
    UART_WRITE32(0x14, 0);              //switch output pin 
    return 1;
}

static INT32 _UartCmdRead(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i;
    UINT8 *pu1Buf = (UINT8 *)0x02000000;
    UINT32 u4Ret;
    UINT32 u4Port;
    
    if (i4Argc < 2)
    {
        Printf("uart.r u4Port\n");
        return 1;
    }

    u4Port = StrToInt(szArgv[1]);    

    if (u4Port > UART_PORT_1)
    {
        Printf("Arg: wrong port number.\n");
    }
    
    UART_WRITE32(0x14, 1);              //switch output pin    

    // read data by non-blocking mode.
    u4Ret = UART_Read(u4Port, pu1Buf, BUFFER_SIZE, FALSE);
    Printf("Uart read length = %d.\n", u4Ret);

    // print sif read data content.
    for (i = 0; i<u4Ret; i ++)
    {
        if (!(i % 8))
        {
            Printf("\n0x%08X :", i);                    
        }
        
        Printf(" 0x%02X,", pu1Buf[i]);
    }
    Printf("\n");

    // wait uart 0 to send out all character before switch output pin.
    //x_thread_delay(1000);
    UART_WRITE32(0x14, 0);              //switch output pin 
    return 1;
}

static INT32 _UartCmdLoop(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i, j;
    UINT8 *pu1Buf = (UINT8 *)0x02000000;
    RS_232_SETUP_INFO_T rUartSetupInfo;

    UART_WRITE32(0x14, 1);              //switch output pin     

    // wait uart 0 to send out all character before switch output pin.
    x_thread_delay(1000);

    rUartSetupInfo.e_speed = (RS_232_SPEED_T)RS_232_SPEED_921600;
    rUartSetupInfo.e_stop_bit = (RS_232_STOP_BIT_T)RS_232_STOP_BIT_1;
    rUartSetupInfo.e_parity = (RS_232_PARITY_T)RS_232_PARITY_NONE;
    rUartSetupInfo.e_data_len = (RS_232_DATA_LEN_T)RS_232_DATA_LEN_8;                    
    
    // set port parameter.
    UART_SetParameter(UART_PORT_1, (const RS_232_SETUP_INFO_T *)&rUartSetupInfo);

    while(1)
    {
        for (i=1 ;i<=65536; i++)
        {
            pu1Buf = (UINT8 *)0x02000000;
            pu1Buf += (i % 128);
            
            UART_Read(UART_PORT_1, pu1Buf, i, TRUE);

            for(j=0; j<i; j++)
            {
                if (pu1Buf[j] != (UINT8)(j & 0xFF))
                {
                    Printf("Compare fai.\n");

                    // wait uart 0 to send out all character before switch output pin.
                    x_thread_delay(1000);
                    UART_WRITE32(0x14, 0);              //switch output pin                     
                    return 1;
                }
            }

            UART_Write(UART_PORT_1, pu1Buf, i, TRUE);
        }
    }    
    
    UART_WRITE32(0x14, 0);              //switch output pin                     
}

static INT32 _UartCmdSelfLoop(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i, j;
    UINT8 u1Port = UART_PORT_1;
    UINT8 *pu1TxBuf = (UINT8 *)0x02000000;
    UINT8 *pu1RxBuf = (UINT8 *)0x02020000;
    RS_232_SPEED_T eSpeed = RS_232_SPEED_115200;

    RS_232_SETUP_INFO_T rUartSetupInfo;

    if (u1Port == UART_PORT_1)
    {
        //  MERGE_MODE	UART Pin Swap Configuration 
        //  0b  UART pin swap is disabled
        //  1b  UART pin swap is enabled 
        UART_WRITE32(0x14, 1);      // switch output pin     

        // wait uart 0 to send out all character before switch output pin.
        x_thread_delay(1000);

        // 1:0 LOOPBACK_MODE	UART Internal Loopback Control Configuration
        //
        // 00b UART0 loopback is disabled. UART1 loopback is disabled.
        // 01b UART0 self loopback is enabled, ie UART0's input pins are 
        //     connected to UART0's output pins inside chip.
        //     UART1 loopback is disabled.
        // 10b UART0 loopback is disabled and UART1 self loopback is enabled, 
        //     ie UART1's input pins are connected to UART1's output pins
        //     inside chip.
        // 11b UART cross loopback is enabled, ie UART0's input pins are 
        //     connected to UART1's output pins inside chip,
        //     and UART1's input pins are connected to UART0's output pins.
        UART_WRITE32(0x24, 0x5AF00002);
    }
    else
    {
        UART_WRITE32(0x24, 0x5AF00001);
    }

    // wait uart 0 to send out all character before switch output pin.
    x_thread_delay(1000);

    rUartSetupInfo.e_speed = eSpeed;
    rUartSetupInfo.e_stop_bit = (RS_232_STOP_BIT_T)RS_232_STOP_BIT_1;
    rUartSetupInfo.e_parity = (RS_232_PARITY_T)RS_232_PARITY_NONE;
    rUartSetupInfo.e_data_len = (RS_232_DATA_LEN_T)RS_232_DATA_LEN_8;                    
    
    // set port parameter.
    UART_SetParameter(u1Port, (const RS_232_SETUP_INFO_T *)&rUartSetupInfo);

//    while(1)
    {
        for (i=1 ;i<=65536; i++)
        {
            pu1TxBuf = (UINT8 *)0x02000000;
            pu1TxBuf += (i % 128);

            UART_Write(u1Port, pu1TxBuf, i, TRUE);
            
            UART_Read(u1Port, pu1RxBuf, i, TRUE);

            for(j=0; j<i; j++)
            {
                if (pu1RxBuf[j] != pu1TxBuf[j])
                {
                    Printf("Compare fai.\n");
                    goto End;
                }
            }
            
            x_thread_delay(100);
        }
    }

End:    
    UART_WRITE32(0x24, 0x5AF00000);

    // wait uart 0 to send out all character before switch output pin.
    x_thread_delay(1000);

    if (u1Port == UART_PORT_1)
    {
        UART_WRITE32(0x14, 0);          // switch output pin
    }
    
    return 1;
}

/******************************************************************************
* Variable		: cli "aud" command table
******************************************************************************/
#endif

static CLI_EXEC_T _arUartCmdTbl[] =
{
#ifdef CC_CLI
 	CLIMOD_DEBUG_CLIENTRY(UART),
 	/*CLIMOD_REGTST_CLIENTRY(UART),*/
 	{"test",		"t",	_UartCmdTest, NULL,	        "Uart port 2 testing", CLI_GUEST},
        {"baud",		"b",	_UartCmdSetBaud, NULL,	"Uart Set Baud test", CLI_SUPERVISOR},
        {"writebyte",    "wb", _UartCmdWriteByte, NULL,	"Uart write byte test", CLI_SUPERVISOR},
        {"write",		"w",	_UartCmdWrite, NULL,	"Uart write test", CLI_SUPERVISOR},
        {"read",		"r",	_UartCmdRead, NULL,	"Uart read test", CLI_SUPERVISOR},
        {"loop",		"l",	_UartCmdLoop, NULL,	"Uart loop test", CLI_SUPERVISOR},
        {"selfloop",		"sl",	_UartCmdSelfLoop, NULL,	"Uart self loop test", CLI_SUPERVISOR},
#endif 	
 	{NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

CLI_MAIN_COMMAND_ITEM(Uart)
{
	"uart",
	NULL,
	NULL,
	_arUartCmdTbl,
	"Uart command",
	CLI_GUEST
};

LINT_RESTORE

