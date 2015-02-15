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
//#include "x_hal_926.h"
#include "x_hal_ic.h"
#include "x_printf.h"
#include "x_debug.h"
#include "x_stl_lib.h"
#include "x_os.h"
#ifdef __MW_CLI_DEF__
#include "x_mid_cli.h"
#else
#include "x_drv_cli.h"
#endif

#include "x_rand.h"
#include "x_util.h"
#include "drv_common.h"
#include "drv_thread.h"
#include "drv_ir.h"
#include "ir_kp.h"

#include "drv_config.h"
#include "chip_ver.h"

#define DEFINE_IS_LOG   CLI_IsLog
#include "x_debug.h"


#if CONFIG_IRRX_SWITCH
#include "u_ir_switch.h"
#include "x_ir_switch.h"
#include "ir_regs_switch.h"
#else
#include "ir_regs.h"
#endif

#if (CONFIG_DRV_VERIFY_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530))
#define IR_CMD_VRF_ENABLE 1
#else
#define IR_CMD_VRF_ENABLE 0
#endif

#if  IR_CMD_VRF_ENABLE
extern void vIrMain(void);
#endif

/******************************************************************************
* Declare the Rx SubCli command functions.
******************************************************************************/
static INT32 _RxCLI_Stop(INT32 i4Argc, const CHAR ** szArgv)
{
    IRHW_RxStop();
    return 0;
}

static INT32 _RxCLI_SetParam(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i;
    UINT32 u4Val;

    for (i = 1; i < i4Argc; i++)
    {
        if (szArgv[i][0] != '-')
        {
            continue;
        }
        switch (szArgv[i][1])
        {
      				
            case 'r':
                i++;
                u4Val = StrToInt(szArgv[i]);
                IRRX_QuerySetRepeat((INT32 *) & u4Val);
                break;
            case 't':
                i++;
                u4Val = StrToInt(szArgv[i]);
                IRRX_QuerySetRepeatTime(&u4Val);
                break;
            case '1':
                i++;
                u4Val = StrToInt(szArgv[i]);
                IRRX_QuerySet1stPulse(&u4Val);
                break;
            case '2':
                i++;
                u4Val = StrToInt(szArgv[i]);
                IRRX_QuerySet2ndPulse(&u4Val);
                break;
            case '3':
                i++;
                u4Val = StrToInt(szArgv[i]);
                IRRX_QuerySet3rdPulse(&u4Val);
                break;
            default:
                break;
        }
    }
	
    UTIL_Printf("Repeat flag: %s\t",
           IRRX_QuerySetRepeat(NULL) ? "enabled" : "disabled");
    UTIL_Printf("Conflict time: %d ms\n", IRRX_QuerySetRepeatTime(NULL));
    UTIL_Printf("1st Pulse:%d\t2nd Pulse:%d\t3rd Pulse:%d\n",
           IRRX_QuerySet1stPulse(NULL),
           IRRX_QuerySet2ndPulse(NULL), IRRX_QuerySet3rdPulse(NULL));
    return 0;
}

/*! \fn static INT32 _RxCLI_Verify(INT32 i4Argc, const CHAR ** szArgv)
    \brief IR IC verify function, default use NEC protocol
*/
#if   IR_CMD_VRF_ENABLE
static INT32 _RxCLI_Verify(INT32 i4Argc, const CHAR ** szArgv)
{
    vIrMain();

	return 0;
}
#endif

/*! \fn static void _RxCLIReset(INT32 i4Argc, const CHAR ** szArgv)
    \brief reset IR with new config register set, sampling period and threshold 
    count.
    \param  szArgv[0]  config register value
    \param szArgv[1]  sampling period
    \param  szArgv[2] deglitch count and threshold count 
*/
static INT32 _RxCLIReset(INT32 i4Argc, const CHAR ** szArgv)
{
   // INT32 i4Ret;
    UINT32 i4Config, i4Sa, i4Thres;
    UINT32 i4Clock1, i4Clock2;

    if (i4Argc == 1)
    {
        i4Config = 0x0121;
        i4Sa = 0x32;
        i4Thres = 0x201;
    }
    else if (i4Argc == 4)
    {
        i4Config = StrToInt(szArgv[1]);
        i4Sa = StrToInt(szArgv[2]);
        i4Thres = StrToInt(szArgv[3]);
    }
    else
    {
        UTIL_Printf("drv.ir.rx.rs [config] [sample period] [Threshold]\n");
        return 0;
    }

    //stop IR
    //vIrRxSetIsr((BOOL)FALSE);
    IRHW_RxStop();

    IRHW_RxInit(i4Config, i4Sa, i4Thres);


    (*(volatile UINT32 *) (IO_BASE + 0xd068)) |= 0x0800;
    (*(volatile UINT32 *) (IO_BASE + 0xd0d4)) &= (~(UINT32) 0x0800);
    i4Clock1 = (*(volatile UINT32 *) (IO_BASE + 0xd068));
    i4Clock2 = (*(volatile UINT32 *) (IO_BASE + 0xd0d4));



    LOG(1, "%d %d\n", i4Clock1, i4Clock2);

    return 0;

}

static INT32 _RxCLI_MtkInit(INT32 i4Argc, const CHAR ** szArgv)
{
    IRRX_ResetMtkIr();
    return 0;
}

static volatile INT32 _i4Loop = 1;
static HANDLE_T _hMtkThread;
static void _RxCLI_IrThread(void *pvArgv)
{
    UINT32 u4Key, u4PrevKey = BTN_NONE;
    INT32 i4Ret;

    UTIL_Printf("MtkIrThread Start... Loop:%d\n", _i4Loop);
    while (_i4Loop)
    {
        i4Ret = IRRX_PollMtkIr(&u4Key);
        if (i4Ret == IR_SUCC)
        {
            if (u4Key == BTN_NONE)
            {                   // polling timeout. 
                if (u4PrevKey != BTN_NONE)
                {               //a key up. 
                    UTIL_Printf("u4Key is 0x%08x up1.\n", u4PrevKey);
                    u4PrevKey = BTN_NONE;
                }
            }
            else
            {
                if ((u4PrevKey != u4Key) && (u4PrevKey != BTN_NONE))
                {
                    UTIL_Printf("u4Key is 0x%08x up3.\n", u4PrevKey);
                }
                if (u4Key != u4PrevKey)
                {
                    UTIL_Printf("u4Key is 0x%08x down.\n", u4Key);
                    u4PrevKey = u4Key;
                }
            }
        }
        else
        {
            if (u4PrevKey != BTN_NONE)
            {
                UTIL_Printf("u4Key is 0x%08x up2.\n", u4PrevKey);
                u4PrevKey = BTN_NONE;
            }
        }
        x_thread_delay(110);
    }
    UTIL_Printf("MtkIrThread Exit... Loop:%d\n", _i4Loop);
}

static INT32 _RxCLI_MtkIr(INT32 i4Argc, const CHAR ** szArgv)
{
    const CHAR *szStr;
    INT32 i, i4Ret;
    UINT32 u4GrpId;

    for (i = 0; i < i4Argc; i++)
    {
        szStr = szArgv[i];
        if (szStr[0] == '-')
        {
            switch (szStr[1])
            {
                case 'E':
                case 'e':
                    _i4Loop = 0;
                    return 0;
                case 'G':
                case 'g':
                    i++;
                    szStr = szArgv[i];
                    if (szStr[0] == '0' || StrToInt(szStr) != 0)
                    {
                        u4GrpId = StrToInt(szStr);
                    }
                    break;
                default:
                    break;
            }
        }
    }

     IRRX_ResetMtkIr();
    _i4Loop = 1;
    i4Ret =
        x_thread_create(&_hMtkThread, MTK_CLIIR_NAME, MTK_CLIIR_STACK_SZ,
                        MTK_CLIIR_PRIORITY, _RxCLI_IrThread, 0, NULL);
    if (i4Ret != OSR_OK)
    {
        UTIL_Printf("Create thread to poll MTK-IR failed.\n");
        return 1;
    }
    UTIL_Printf("Create thread to poll MTK-IR successfully.\n");
    return 0;
}

typedef struct __string2key
{
    CHAR *szString;
    UINT32 u4Key;
} STR2KEY_T;

static STR2KEY_T _aStr2KeyArray[] = {
    {"0", BTN_DIGIT_0},
    {"1", BTN_DIGIT_1},
    {"2", BTN_DIGIT_2},
    {"3", BTN_DIGIT_3},
    {"4", BTN_DIGIT_4},
    {"5", BTN_DIGIT_5},
    {"6", BTN_DIGIT_6},
    {"7", BTN_DIGIT_7},
    {"8", BTN_DIGIT_8},
    {"9", BTN_DIGIT_9},
    {"hundred", BTN_DIGIT_PLUS_100},
    {"dot", BTN_DIGIT_DOT},
    {"left", BTN_CURSOR_LEFT},
    {"right", BTN_CURSOR_RIGHT},
    {"up", BTN_CURSOR_UP},
    {"down", BTN_CURSOR_DOWN},
    {"exit", BTN_EXIT},         // exit
    {"ce", BTN_CE},             // exit
    {"select", BTN_SELECT},
    {"enter", BTN_SELECT},
    {"ch_up", BTN_PRG_UP},
    {"ch_down", BTN_PRG_DOWN},
    {"prevch", BTN_PREV_PRG},
    {"pop", BTN_POP},
    {"zoom", BTN_ZOOM},
    {"picsize", BTN_PICSIZE},   // swap
    {"favorite", BTN_FAVORITE},
    {"fav_ch", BTN_FAV_CH},
    {"osd", BTN_OSD},           // prg_info
    {"add_erase", BTN_ADD_ERASE},
    {"cc", BTN_CC},
    {"input", BTN_INPUT},       // input_src
    {"vol_up", BTN_VOL_UP},
    {"vol_down", BTN_VOL_DOWN},
    {"mute", BTN_MUTE},
    {"mts", BTN_MTS},
    {"power", BTN_POWER},
    {"menu", BTN_MENU},
    {"clock", BTN_CLOCK},       // epg
    {"update", BTN_UPDATE},     // mem_card
    {"red", BTN_RED},
    {"green", BTN_GREEN},
    {"yellow", BTN_YELLOW},
    {"blue", BTN_BLUE},
    {"actctrl", BTN_ACTCTRL},
    {"smartpic", BTN_SMARTPIC},
    {"tttv", BTN_TTTV},
    {"pippos", BTN_PIPPOS},
    {"colorsys", BTN_COLORSYS},
    {"capture", BTN_CAPTURE},
    {"sleep", BTN_SLEEP},
    {"index", BTN_INDEX},
    {"timer", BTN_TIMER},
    {"freeze", BTN_FREEZE},
    {"vchip", BTN_VCHIP},
    {"SNDEFCT", BTN_SNDEFCT},
    {NULL, 0},
};

/*! \fn static INT32 _RXCLI_SendQueue(INT32 i4Argc, const CHAR ** szArgv)
    \brief  simulate remote controller's buttons is pressed.
    \param  szArgv[1]  continual times(ms)
    \param szArgv[2]   space time between last button released and next button 
    pressed.Default is 110ms.
    \param  szArgv[3]  how many times the key is pressed. Default is 1 time.
*/
static INT32 _RXCLI_SendQueue(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Key = BTN_NONE;
    INT32 u4Itvl = 1;
    UINT32 u4Rep = 1;
    UINT32 u4Space = 10;
    const UINT32 u4TimeSlice = 105;
//    UINT32 u4RemTime;
    INT32 i4Ind, i4Jnd;
  //  INT32 i4Ret;

    if (i4Argc != 2 && i4Argc != 3 && i4Argc != 5)
    {
        UTIL_Printf
            ("ir.rx.sq KeyID [continual time(ms)]  [repeat space ][repeat times]" \
            "-- send keys to queue\n");
        return 0;
    }

    if (i4Argc >= 2)
    {
        u4Key = StrToInt(szArgv[1]);

        if (i4Argc >= 3)
        {
            u4Itvl = StrToInt(szArgv[2]);

            if (i4Argc == 5)
            {
                u4Space = StrToInt(szArgv[3]);
                u4Rep = StrToInt(szArgv[4]);

            }
        }
    }

    LOG(1, "Key code : %x  continuance time: %dms \n "
           "Space time :%d  repeat     times: %x\n",
           u4Key, u4Itvl, u4Space, u4Rep);

    for (i4Jnd = 1; i4Jnd <= u4Rep; i4Jnd++)
    {

        for (i4Ind = 1; i4Ind <= u4Itvl; i4Ind += u4TimeSlice)
        {

            if (i4Ind == 1)
            {
                IRRX_SendMtkIr(u4Key);
            }
            else
            {
                IRRX_SendMtkIr(BTN_KEY_REPEAT);
            }
            x_thread_delay(u4TimeSlice);

        }
        x_thread_delay(u4Space);
    }

    return 0;
}

static INT32 _RxCLI_SendKey(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i4Ret, i;
    UINT32 u4Key;

    if (i4Argc != 2)
    {
        UTIL_Printf("ir.rx.mtk.send [KeyId] - send key id into ir key queue\n");
        return 1;
    }

    if ((szArgv[1][0] == '0') && (szArgv[1][1] == 'x'))
    {
        u4Key = StrToInt(szArgv[1]);
        // i4Ret = IRRX_SendMtkIr(BTN_POWER);
        i4Ret = IRRX_SendMtkIr(u4Key);
        if (i4Ret != OSR_OK)
        {
            UTIL_Printf
                ("ir.rx.mtk.send send key:0x%08x failed and get return %d\n",
                 u4Key, i4Ret);
            return 1;
        }
        return 0;
    }

    for (i = 0;
         ((_aStr2KeyArray[i].szString != NULL) &&
          (x_strcmp(szArgv[1], _aStr2KeyArray[i].szString) != 0)); i++)
    {
    }

    u4Key = _aStr2KeyArray[i].u4Key;
    i4Ret = IRRX_SendMtkIr(u4Key);
    if (i4Ret != OSR_OK)
    {
        UTIL_Printf("ir.rx.mtk.send send key:0x%08x failed and get return %d\n",
               u4Key, i4Ret);
        return 1;
    }

    // UTIL_Printf("ir.rx.mtk.send send key:0x%08x successfully\n", u4Key);
    return 0;
}

static INT32 _RxCLI_PollKey(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i4Ret;
    UINT32 u4Key;

    if (i4Argc != 1)
    {
        UTIL_Printf("ir.rx.mtk.poll - Print poll key value from ir key queue.\n");
        return 1;
    }

    i4Ret = IRRX_PollMtkIr(&u4Key);
    if (i4Ret != IR_SUCC)
    {
        UTIL_Printf("ir.rx.mtk.poll poll MTKIR key failed. return %d\n", i4Ret);
        return 1;
    }
    UTIL_Printf("ir.rx.mtk.poll get key value:0x%08x\n", u4Key);
    return 0;
}

static INT32 _RxTestUint(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i4Init;
    if (i4Argc != 2)
    {
        UTIL_Printf("Usgae : cmd 1/0  1: init   0 : uinit\n");
    }

    i4Init = StrToInt(szArgv[1]);

    if (i4Init == 0)
    { 
     #if(! CONFIG_DRV_LINUX )
        i4IrUninit();
	 #else
	    i4Ir_Uninit(0);
	 #endif
    }
    else
    {
        IRRX_InitMtkIr();
    }
    return 0;
}

static INT32 _i4DelayMs = 400;
static INT32 _i4PatternNumber = 0;
static volatile INT32 _i4SendLoop = 1;
static BOOL _fgRandom = FALSE;
static void _RxCLI_SendIrPattern(void *pvArgv)
{

    INT32 i4Ret, i4Idx = 0;
    UINT32 u4Key;

    UTIL_Printf("MtkThread Send Ir Pattern... Loop:%d\n", _i4SendLoop);

    while (_i4SendLoop)
    {
        u4Key = _au4MtkKeyPattern[_i4PatternNumber][i4Idx];

        if (_fgRandom)
        {
        	#if 0
            i4Idx = random((_au4TotalPattern[_i4PatternNumber] - 1));
          #else
            i4Idx = 100;
          #endif
        }
        else
        {
            i4Idx++;
        }
        if (i4Idx >= _au4TotalPattern[_i4PatternNumber])
        {
            i4Idx = 0;
        }

        if (u4Key == BTN_NONE)
            continue;
        else if (u4Key == BTN_DELAY_1S)
        {
            x_thread_delay(1000);
            continue;
        }
        else if (u4Key == BTN_DELAY_2S)
        {
            x_thread_delay(2000);
            continue;
        }
        else if (u4Key == BTN_DELAY_5S)
        {
            x_thread_delay(5000);
            continue;
        }
        else if (u4Key == BTN_DELAY_10S)
        {
            x_thread_delay(10000);
            continue;
        }
        i4Ret = IRRX_SendMtkIr(u4Key);
        if (i4Ret != 0)
        {
            UTIL_Printf("Send u4Key:%d failed return %d\n", u4Key, i4Ret);
        }
        x_thread_delay(_i4DelayMs);
    }

    UTIL_Printf("MtkThread Send Ir Pattern Exist... Loop:%d\n", _i4SendLoop);

}

static INT32 _RxCLI_Pattern(INT32 i4Argc, const CHAR ** szArgv)
{
    const CHAR *szStr;
    INT32 i4Ret;

    szStr = szArgv[1];
    _fgRandom = FALSE;
    if (szStr[0] == '-')
    {
        switch (szStr[1])
        {
            case 'E':
            case 'e':
                _i4SendLoop = 0;
                return 0;
            case 'R':
            case 'r':
                _fgRandom = TRUE;
                /* fall through */
            case 'I':
            case 'i':
                if (i4Argc < 2)
                {
                    UTIL_Printf
                        ("ir.rx.mtk.kp [-e] [-ri [delay ms] [pattern#]] - Have a thread to send keys to queue.\n");
                    return 1;
                }
                szStr = szArgv[2];
                if (i4Argc > 2 && StrToInt(szStr) != 0)
                {
                    _i4DelayMs = StrToInt(szStr);
                    UTIL_Printf("_i4DelayMs is setting as %d\n", _i4DelayMs);
                }
                szStr = szArgv[3];
                if (i4Argc > 3)
                {
                    _i4PatternNumber = StrToInt(szStr);
                }
                break;
            default:
                UTIL_Printf
                    ("ir.rx.mtk.kp [-e] [-ri [delay ms]] - Have a thread to send keys to queue.\n");
                return 1;
        }
    }
    _i4SendLoop = 1;
    i4Ret =
        x_thread_create(&_hMtkThread, MTK_CLIIRSPTN_NAME, MTK_CLIIR_STACK_SZ,
                        MTK_CLIIR_PRIORITY, _RxCLI_SendIrPattern, 0, NULL);
    if (i4Ret != OSR_OK)
    {
        UTIL_Printf("Create thread to send MTK-IR stress pattern failed.\n");
        return 1;
    }
    UTIL_Printf("Create thread to send MTK-IR stress pattern successfully.\n");
    return 0;
}

static INT32 _RxCLI_MemKeys(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i, i4KeyIdx;
    UINT32 u4KeyVal;

    if ((i4Argc & 1) == 0)
    {
        UTIL_Printf
            ("ir.rx.mtk.mk [[keyidx1] [keyval1] [keyidx2] [keyval2] ...] - List/Modify key pattern\n");
        UTIL_Printf("\t\t[keyidx] and [keyval] must be pair\n");
    }
    for (i = 1; i < i4Argc; i += 2)
    {
        i4KeyIdx = StrToInt(szArgv[i]);
        u4KeyVal = StrToInt(szArgv[i + 1]);

        if (i4KeyIdx >= 0 && i4KeyIdx < _au4TotalPattern[_i4PatternNumber])
        {
            _au4MtkKeyPattern[_i4PatternNumber][i4KeyIdx] = u4KeyVal;
        }
        else
        {
            UTIL_Printf("Input KeyIdx:%d is not valid, must >= 0 and < %d\n",
                   i4KeyIdx, _au4TotalPattern[_i4PatternNumber]);
        }
    }

    UTIL_Printf("\nIndex |  Key 0     |  Key 1     |  Key 2     |  Key 3     |");
    UTIL_Printf("\n------+------------+------------+------------+------------+");
    for (i = 0; i < _au4TotalPattern[_i4PatternNumber]; i++)
    {
        if (i % 4 == 0)
        {
            UTIL_Printf("\n%4d  | 0x%08x |", i,
                   _au4MtkKeyPattern[_i4PatternNumber][i]);
        }
        else
        {
            UTIL_Printf(" 0x%08x |", _au4MtkKeyPattern[_i4PatternNumber][i]);
        }
    }
    return 0;
}

#if 0

/*! \fn static INT32 _RxCLI_Status(void);
    \brief Print registers' value and settings
*/
static INT32 _RxCLI_Status(INT32 i4Argc, const CHAR ** szArgv)
{
  IR_Status_WD();
  return 0;
}


static INT32 _IRCLI_Diag(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i4Ret;

    if (i4Argc > 1 && szArgv[1][0] == 's')
    {
        i4Ret = IR_Diag();
        UTIL_Printf("\n==================\n"
               " IR Diag %s!"
               "\n==================\n", ((i4Ret == 0) ? "Success" : "Fail"));
        return i4Ret;
    }

#if 1
    /* NOR */
    i4Ret = x_cli_parser("nor.diag");
    UTIL_Printf("=====================> nor.diag return %d\n", i4Ret);
    if (i4Ret != 0)
        return i4Ret;
    /* NAND */
//  i4Ret = x_cli_parser("nand.diag");
//  UTIL_Printf("=====================> nand.diag return %d\n", i4Ret);
//  if (i4Ret!=0) return i4Ret;
    /* DMX */
    i4Ret = x_cli_parser("dmx.diag");
    UTIL_Printf("=====================> dmx.diag return %d\n", i4Ret);
    if (i4Ret != 0)
        return i4Ret;
    /* PSR */
    x_cli_parser("mpv.d_l 0");
    i4Ret = x_cli_parser("mpv.init");
    UTIL_Printf("=====================> mpv.init return %d\n", i4Ret);
    if (i4Ret != 0)
        return i4Ret;
    i4Ret = x_cli_parser("aud.init");
    UTIL_Printf("=====================> aud.init return %d\n", i4Ret);
    if (i4Ret != 1)
        return 1;
    i4Ret = x_cli_parser("psr.init");
    UTIL_Printf("=====================> psr.init return %d\n", i4Ret);
    if (i4Ret != 0)
        return i4Ret;
    i4Ret = x_cli_parser("psr.diag");
    UTIL_Printf("=====================> psr.diag return %d\n", i4Ret);
    if (i4Ret != 0)
        return i4Ret;
    x_cli_parser("mpv.d_l 5");
//  /* IDE */
//  i4Ret = x_cli_parser("ide.diag");
//  UTIL_Printf("=====================> ide.diag return %d\n", i4Ret);
//  if (i4Ret!=0) return i4Ret;
    /* FCI */
//  i4Ret = x_cli_parser("fci.diag");
//  UTIL_Printf("=====================> fci.diag return %d\n", i4Ret);
//  if (i4Ret!=0) return i4Ret;
#endif

    i4Ret = IR_Diag();
    UTIL_Printf("\n==================\n"
           " IR Diag %s!"
           "\n==================\n", ((i4Ret == 0) ? "Success" : "Fail"));
    return i4Ret;
}
#endif


#if 0
static void _RxCLI_Callback(UINT32 u4Info, const UINT8 * pu1Data)
{
    INT32 i, i4BitCnt;
    UINT32 *pu4Data;

    i4BitCnt = INFO_TO_BITCNT(u4Info);
    UTIL_Printf("%d(%d)(%d)(%d) - ", i4BitCnt, INFO_TO_1STPULSE(u4Info),
           INFO_TO_2NDPULSE(u4Info), INFO_TO_3RDPULSE(u4Info));
    pu4Data = (UINT32 *) pu1Data;
    for (i = 0; i * 8 < (i4BitCnt - 1); i++)
    {
        if (i % 4 == 0)
        {
            UTIL_Printf("(0x%08x)", pu4Data[i >> 2]);
        }
        UTIL_Printf("0x%02x ", pu1Data[i]);
    }
    UTIL_Printf("\n");
}

static INT32 _RxCLI_Init(INT32 i4Argc, const CHAR ** szArgv)
{
    const CHAR *szStr;
    INT32 i, i4Config, i4SaPeriod, i4Threshold, i4Down, i4PBit;
    UINT32 u4Key1, u4Key2;

    IRHW_RxSetCallback(_RxCLI_Callback, NULL);
    IRHW_RxRdConf(&i4Config, &i4SaPeriod, &i4Threshold);
    i4Down = IRHW_Down(NULL);
    i4PBit = IRHW_PowerBitNum(NULL);
    u4Key1 = IRHW_PowerUpKey1(NULL);
    u4Key2 = IRHW_PowerUpKey2(NULL);
    for (i=0; i<i4Argc; i++) {
        szStr = szArgv[i];
        if (szStr[0]=='-') {
            switch(szStr[1]) {
            case 'C': case 'c':                   
                i++; szStr = szArgv[i];
                if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                    i4Config = StrToInt(szStr);
                }
                break;
            case 'S': case 's':                     // SaPeriod 
                i++; szStr = szArgv[i];
                if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                    i4SaPeriod = StrToInt(szStr);
                }
                break;
            case 'T': case 't':                     //Threshold 
                i++; szStr = szArgv[i];
                if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                    i4Threshold = StrToInt(szStr);
                }
                break;
            case 'D': case 'd':
                i++; szStr = szArgv[i];
                if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                    i4Down = StrToInt(szStr);
                }
                break;
            case 'B': case 'b':
                i++; szStr = szArgv[i];
                if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                    i4PBit = StrToInt(szStr);
                }
                break;
            case 'K': case 'k':
                if (szStr[2]=='1') {
                    i++; szStr = szArgv[i];
                    if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                        u4Key1 = StrToInt(szStr);
                    }
                } else if (szStr[2]=='2') {
                    i++; szStr = szArgv[i];
                    if (szStr[0]=='0' || StrToInt(szStr)!=0) {
                        u4Key2 = StrToInt(szStr);
                    }
                }
                break;
            default: continue;
            }
        }
    }

    IRHW_Down(&i4Down);
    IRHW_PowerBitNum((UINT32*)&i4PBit);
    IRHW_PowerUpKey1(&u4Key1);
    IRHW_PowerUpKey2(&u4Key2);
    IRHW_RxInit(i4Config, i4SaPeriod, i4Threshold);

    IRHW_RxRdConf(&i4Config, &i4SaPeriod, &i4Threshold);
    UTIL_Printf("%s %s\n", __DATE__, __TIME__);
    UTIL_Printf("IrRx Conf:0x%08x\tSP:0x%08x\tTd:0x%08x\tDown:%d\n"
           "     PowerKeyBit:%d\tPowerKey1:0x%08x\tPowerKey2:0x%08x\n",
            i4Config, i4SaPeriod, i4Threshold, IRHW_Down(NULL),
            IRHW_PowerBitNum(NULL), IRHW_PowerUpKey1(NULL), IRHW_PowerUpKey2(NULL));
    return 0;
}

#ifdef CC_SBOX_HWCTRL
extern void Tuner_Kill(void);
#endif
static INT32 _RxCLI_PowerDown(INT32 i4Argc, const CHAR ** szArgv)
{
#ifdef CC_SBOX_HWCTRL
    Tuner_Kill();
#endif //CC_SBOX_HWCTRL 
    IRHW_PowerDown();
    return 0;
}

static volatile INT32 _i4DOGLoop = 1;
static HANDLE_T _hDogThrd;
static void _RxCLI_WatchThread(void *pvDummy)
{
    UINT32 u4Val;

    _i4DOGLoop = 1;
    UTIL_Printf("Watch Thread Create\n");
    while (_i4DOGLoop) {
        BIM_WatchDog(0);
        do {
            u4Val = BIM_WatchCounter();
            // UTIL_Printf("Counter:0x%08x\n", u4Val);
        } while (u4Val < 0x80000000);
    }
    UTIL_Printf("Watch Thread Exit\n");
}

static INT32 _RxCLI_WatchDog(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i4Ret;

    if (i4Argc == 2 && szArgv[1][0]=='-') {
        _RxCLI_WatchThread(NULL);
        return 0;
    }
    if (i4Argc == 2 && szArgv[1][0]=='x') {
        _i4DOGLoop = 0;
        return 0;
    }

    i4Ret = x_thread_create(&_hDogThrd, MTK_CLIWATCHDOG_NAME, 512, MTK_CLIIR_PRIORITY, _RxCLI_WatchThread, 0, NULL);
    if (i4Ret != OSR_OK) {
        UTIL_Printf("Create thread to watchdog failed.\n");
        return 1;
    }
    UTIL_Printf("Create thread to watchdog successfully.\n");
    return 0;
}

#endif

#if CONFIG_IRRX_SWITCH

extern void ir_switch_init_para(UINT32 ui4_irpro);
extern long ir_switch_set_protocol(unsigned long ui4_protocol);
extern long ir_switch_store_protocol(unsigned long ui4_protocol,unsigned long ui4_offset, BOOL b_store_ui4_protocol);
extern long ir_switch_get_store_flash_protocol( unsigned long * pi4_out_protocol, UINT32 ui4offet);
extern UINT32 ir_switch_get_using_protocol(void);


static INT32 _RxCLI_SwitchIr(INT32 i4Argc, const CHAR ** szArgv)
{
    
	UINT32 ui4_value;
	const CHAR * p = NULL;
	
    if (i4Argc == 1 ||  x_strcmp(szArgv[1],"-h") == 0)
    {
       UTIL_Printf("arg[1] = ir_value(0x...)\n");
	   UTIL_Printf(" #define IRRX_RC_NEC  (0x00) \n ");
	   UTIL_Printf(" #define IRRX_RC_RC6  (0x01) \n");
	   UTIL_Printf(" #define IRRX_RC_RC5  (0x02) \n");
	   UTIL_Printf(" #define IRRX_RC_PAN  (0x03) \n");
	   UTIL_Printf(" #define IRRX_RC_NEC  (0x04) \n");
	   UTIL_Printf(" #define IRRX_RC_SIRC (0x05) \n");
	   UTIL_Printf(" #define IRRX_RC_JVC  (0x06) \n");
	   UTIL_Printf(" #define IRRX_RC_SAS  (0x07) \n");	   
    }
	else
	{
	    p = szArgv[1];
		if (p[0] == '0' && ( p[1] == 'x' ||  p[1] == 'X' ) && ( x_strlen(p) > 2) )
		{
		    ui4_value = StrToHex(&p[2],x_strlen(&p[2]));
			if (ui4_value >=IRRX_MAX_VALUE)
			{
		  		 UTIL_Printf("[IR_SWITCH_CMD]: invalid ir value, too big !!! \n");
		  		 return 0;
			}
		    ir_switch_set_protocol(ui4_value);	    
		    
		}
		else
		{
		   UTIL_Printf("[IR_SWITCH_CMD]: invalid arg !!! \n");
		   return 0;
		    
		}
		
	}
    return 0;
}

static INT32 _RxCLI_StoreIr(INT32 i4Argc, const CHAR ** szArgv)
{
    
	
	UINT32 ui4_value;	
	const CHAR * p_ir = NULL;
	int value;
	
 
    if (i4Argc == 1 || x_strcmp(szArgv[1],"-h") == 0)
    {
       UTIL_Printf("arg[1] = ir_value(0x...)\n");	 	     
    }
	else
	{
	    if ( i4Argc !=2 )
	   	{
	   	    UTIL_Printf("arg wrong, use [-h] for help!!!\n");
			return 0;
	   	}
	   
		 p_ir = szArgv[1];
		 value  =( p_ir[0] == '0' && ( p_ir[1] == 'x' ||  p_ir[1] == 'X')   && ( x_strlen(p_ir) > 2) );
		
		if ( value)
		{
		    ui4_value = StrToHex(&p_ir[2], x_strlen(&p_ir[2]));
			
			if (ui4_value >=IRRX_MAX_VALUE)
			{
		  		 UTIL_Printf("[IR_SWITCH_CMD]: invalid ir value, too big !!! \n");
		  		 return 0;
			}
		    ir_switch_store_protocol(ui4_value,0, TRUE);	    		    
		}
		else
		{
		   UTIL_Printf("[IR_SWITCH_CMD]: invalid arg !!! \n");
		}
		    
	}
    return 0;
}

static INT32 _RxCLI_GetIrInfo(INT32 i4Argc, const CHAR ** szArgv)
{
    
    UINT32 ui4_value;
	long i4_ret;
    i4_ret = ir_switch_get_store_flash_protocol(&ui4_value,0 );	
	UTIL_Printf("[IR_SWITCH_CMD]: ir store partition is %s \n",FAST_INIT_PARAM);
	UTIL_Printf("[IR_SWITCH_CMD]: ir store offset is 0x%08x \n",0);
	if (i4_ret < 0)
	{
	    ui4_value = ir_switch_get_using_protocol();
	    UTIL_Printf("[IR_SWITCH_CMD]: default ir protocol is 0x%08x \n",ui4_value );
	}
	else
	{
	    UTIL_Printf("[IR_SWITCH_CMD]: ir store data is 0x%08x \n",ui4_value);
	}
	 	   
    return 0;
}

CLI_EXEC_T  arIRSWITCHCmdTbl[] = {

    DECLARE_G_CMD(_RxCLI_SwitchIr, switch_ir, swi,
				  "switch ir protocol [-h] for help"),
    DECLARE_G_CMD(_RxCLI_StoreIr, store_ir, sti,
						  "store ir protocol to nand [-h] for help "),
    DECLARE_G_CMD(_RxCLI_GetIrInfo, get_ir_info, gii,
						  "get ir protocol info "),	
	
	DECLARE_END_ITEM(),

};
#endif




CLIMOD_DEBUG_FUNCTIONS(IR) 
//CLIMOD_REGTST_FUNCTIONS(IR)

/******************************************************************************
* Declare the SubCli command lists.
******************************************************************************/



CLI_EXEC_T arMTKIRCmdTbl[] = {
	DECLARE_G_CMD(_RxCLI_MtkInit, init, init,
				  "ir.rx.mtk.init - Initial mtk IR."),
	DECLARE_G_CMD(_RxCLI_MtkIr, thread, td,
				  "ir.rx.mtk.td [-e] [-g [GrpId]] - Have a polling thread to get IR key to print."),
	DECLARE_G_CMD(_RxCLI_SendKey, send, send,
				  "ir.rx.mtk.send [KeyId] - send key id into ir key queue"),
	DECLARE_G_CMD(_RxCLI_PollKey, poll, poll,
				  "ir.rx.mtk.poll - Print poll key value from ir key queue."),
	DECLARE_G_CMD(_RxCLI_Pattern, keypattern, kp,
				  "ir.rx.mtk.kp [-e] [-ri [delay ms] [pattern#]] - Have a thread to send keys to queue."),
	DECLARE_G_CMD(_RxCLI_MemKeys, memkeys, mk,
				  "ir.rx.mtk.mk [[keyidx1] [keyval1] [keyidx2] [keyval2]...] - List/Modify key pattern\n"),

	DECLARE_END_ITEM(),
};

CLI_EXEC_T arRXCmdTbl[] = {
	
	DECLARE_G_CMD(_RxCLI_Stop, stop, stop, "ir.rx.stop"),
	DECLARE_G_CMD(_RxCLI_SetParam, setparam, sp,
				  "ir.rx.setparam -g [id] -[1|2|3] [pulse]"),
	//DECLARE_G_CMD(_RxCLI_Status, status, stt, "ir.rx.stt	-Print Register Values and settings"),
#if  IR_CMD_VRF_ENABLE
	DECLARE_G_CMD(_RxCLI_Verify, verify, vrf, "ir.rx.vrf  ic verify"),
#endif
    DECLARE_G_CMD(_RxCLIReset, reset, ret, "ir.rx.ret config sa threshold"),
	DECLARE_G_CMD(_RXCLI_SendQueue, sendQ, sq,
				  "ir.rx.sq  KeyID [continual time(ms)] "
				  " [repeat space	   ] [repeat times] -- send keys to  queue"),
	DECLARE_G_CMD(_RxTestUint, testUnit, tui, "tui 1/0  1:init  0:uint"), 

    //DECLARE_G_CMD(_RxCLI_Init, init, init, "ir.rx.init -c [conf] -s [sap] -t [thld]"),
    // DECLARE_G_CMD(_RxCLI_PowerDown, down, down, "ir.rx.down"),
	//DECLARE_G_CMD(_RxCLI_WatchDog, dog, dog, "ir.rx.dog"),
	
	DECLARE_G_SUBLIST(MTKIR, mtk, mtk, "MTK TV IR control functions"),

	DECLARE_END_ITEM(),
};

CLI_EXEC_T arIrCmdTbl[] = {
//    CLIMOD_REGTST_CLIENTRY(IR),
    CLIMOD_DEBUG_CLIENTRY(IR),

    //DECLARE_G_CMD(_IRCLI_Diag, diag, diag, "ir.diag"),

    DECLARE_G_SUBLIST(RX, rx, rx, "RX commands"),

    DECLARE_G_SUBLIST(MTKIR, mtkir, mtkir, "MTKIR commands"),
    //add by ji.wang
    #if CONFIG_IRRX_SWITCH
    DECLARE_G_SUBLIST(IRSWITCH, ir_switch, ir_switch, "ir_switch commands"),
    #endif
	
    DECLARE_END_ITEM(),
};

/*****************************************************************************/

static CLI_EXEC_T _rIrModCmdTbl = {
    "ir",
    "ir",
    NULL,
    arIrCmdTbl,
    "Ir command",
    CLI_GUEST
};

/******************************************************************************
* Global Function
******************************************************************************/

#ifdef __MW_CLI_DEF__
CLI_EXEC_T *GetIrMwCmdTbl(void)
#else
CLI_EXEC_T *GetIrCmdTbl(void)
#endif
{
    return &_rIrModCmdTbl;
}

