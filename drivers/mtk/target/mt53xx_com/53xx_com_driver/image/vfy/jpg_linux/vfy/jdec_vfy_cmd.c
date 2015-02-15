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

/*-----------------------------------------------------------------------------
 *
 * Description:
 *         The CLI commands sample
 *
 *---------------------------------------------------------------------------*/

//#include "vdec_verify_vdec.h"
#include "x_drv_cli.h"
#include "x_serial.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_util.h"
#include "x_stl_lib.h"
//---------------------------------------------------------------------
// LOG macro related

#define DEFINE_IS_LOG	CLI_IsLog
#include "x_debug.h"


extern void vJpegVerify(void);
extern void vJpegVerifyCLI(UINT32 u4TestCase, BOOL fgBasliePic, UINT32 ui4Delay);

extern UINT32 _ui4StartClock;
extern UINT32 _ui4EndClock;
extern UINT32  BURN_IN_TEST_COUNT;
extern BOOL _fgForceDisableInterrupt;
extern BOOL   _debug_decode_done_log;
extern UINT32  _ui4TestDelay;
INT32 _Cmd_JdecVfyAutoTest(INT32 i4Argc, const CHAR** aszArgv)
{
  UINT32 u4TestCase;
  BOOL fgBasliePic= FALSE; UINT32 ui4Delay =0;
  UINT32 u4Basline =0;
   BURN_IN_TEST_COUNT =1;
   _ui4StartClock = 5;
  _ui4EndClock = _ui4StartClock;
  _debug_decode_done_log = FALSE;
  _ui4TestDelay =0;
  if(i4Argc < 2)
  {
     Printf("CLI failed ====\n");
     return -1;//u4TestCase = 0;
  } 
  if(i4Argc >= 2)
  {
    u4TestCase = (UINT32)StrToInt(aszArgv[1]);
    if(i4Argc >= 3)
    {
      u4Basline = (UINT32)StrToInt(aszArgv[2]);
	  if(u4Basline) 
	  	fgBasliePic = TRUE;
	  else 
	  	fgBasliePic = FALSE;
    }
	if(i4Argc >= 5)
    {
      _ui4StartClock = (UINT32)StrToInt(aszArgv[3]);
      _ui4EndClock = (UINT32)StrToInt(aszArgv[4]);
      
    }
	if(i4Argc >= 6)
	{
	   BURN_IN_TEST_COUNT =(UINT32)StrToInt(aszArgv[5]);
	}
	if(i4Argc >= 7)
	{
	  _fgForceDisableInterrupt =(UINT32)StrToInt(aszArgv[6]);
	}
	if(i4Argc >= 8)
	{
	  _debug_decode_done_log =(UINT32)StrToInt(aszArgv[7]);
	}
	if(i4Argc >= 9)
	{
	  _ui4TestDelay =(UINT32)StrToInt(aszArgv[8]);
	}
  }
  Printf("_ui4StartClock[%d] _ui4EndClock[%d]====\n",_ui4StartClock, _ui4EndClock);      
  vJpegVerifyCLI(u4TestCase,fgBasliePic, ui4Delay);
  return 0;
}

extern UINT32 _u4_debug_loglevel;
extern BOOL _fgForceLogEnable;
extern BOOL _fgEnableSimLog;

INT32 _Cmd_JdecLog(INT32 i4Argc, const CHAR** aszArgv)
{
  UINT32 u4TestCase;
  BOOL fgBasliePic= FALSE; UINT32 ui4Delay =0;
  UINT32 u4Basline =0;
  UINT32 u4LogLevel =0;
  if(i4Argc < 2)
  {
     //UTIL_Printf("CLI failed ====\n");
     return -1;//u4TestCase = 0;
  } 
  _fgForceLogEnable = FALSE;
  if(i4Argc >= 2)
  {
    u4LogLevel = (UINT32)StrToInt(aszArgv[1]);   
    Printf("CLI _u4_debug_loglevel[%d]\n",u4LogLevel);

	if(u4LogLevel==9)
	{
	  _fgEnableSimLog = TRUE;
	  _u4_debug_loglevel = 0;
	  return 0;
	}

	_u4_debug_loglevel = u4LogLevel;
	if(u4LogLevel==0)
	{
	  _fgEnableSimLog = FALSE;
	  _fgForceLogEnable =FALSE;
	  return 0;
	}			
	if(_u4_debug_loglevel ==6)
	{
	  _fgForceLogEnable =TRUE;
	  _u4_debug_loglevel =0;
	}
	else if(_u4_debug_loglevel >6)
	{
	  _fgForceLogEnable =TRUE;
	}
	if(u4LogLevel>=7)
	{
	  _fgEnableSimLog = TRUE;	 
	}
  }
 
  return 0;
}



/*
INT32 _Cmd_JdecSimLog(INT32 i4Argc, const CHAR** aszArgv)
{
  UINT32 u4Basline =0;
  if(i4Argc < 2)
  {
     //UTIL_Printf("CLI failed ====\n");
     return -1;//u4TestCase = 0;
  } 
  _fgEnableSimLog = FALSE;
  if(i4Argc >= 2)
  {
    _fgEnableSimLog = (UINT32)StrToInt(aszArgv[1]);   
    Printf("CLI _fgEnableSimLog[%d]\n",_fgEnableSimLog);
  }
 
  return 0;
}
*/
//===========================================================================

CLI_EXEC_T _arJDecVfyCmdTbl[] =
{
  //Start to decode
  {"verify",     "vfy", 	_Cmd_JdecVfyAutoTest,     NULL, "verify", CLI_GUEST},

  //CLIMOD_DEBUG_CLIENTRY(Video Decoder),
  //Last CLI command record, NULL
  {NULL, NULL, NULL, NULL, NULL, CLI_GUEST}
};

CLI_EXEC_T _rJDecVfyCmdTbl =
{
  "jdecvfy",
  NULL,
  NULL,
  _arJDecVfyCmdTbl,
  "Jpeg decoder verify command",
  CLI_GUEST
};

CLI_EXEC_T* GetJDecVfyCmdTbl(void)
{
  return &_rJDecVfyCmdTbl;
}

