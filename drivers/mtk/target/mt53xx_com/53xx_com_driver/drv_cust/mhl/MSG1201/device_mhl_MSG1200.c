//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _MST_C_
#define _MST_C_

//#include "device_mhl_MSG1200.h"
//#include "Enum_Device.h"
//#include "Board.h"

#ifdef MSOS_TYPE_LINUX_KERNEL
//#include <linux/string.h>
#else
//#include <string.h>
#endif

#if 1//((MHL_TYPE == MHL_TYPE_ELAND) || (MHL_TYPE == MHL_TYPE_SI) || (MHL_TYPE == MHL_TYPE_SiI1292))
//#include "drvIIC.h"
//#include "msAPI_Timer.h"
#endif
//#include "MApp_Key.h"

#include "hdmi_mhl_if_msg1200.h"
#include "Eland/drvMHL_ELAND_st.h"
#include "device_mhl_MSG1200.h"

#include "Eland/halMHL_ELAND.h"
#include "Eland/drvMHL_ELAND.h"
#include "Eland/apiMHL_ELAND.h"
//#include "Eland/halMHL_ELAND.c"
//#include "Eland/drvMHL_ELAND.c"
//#include "Eland/apiMHL_ELAND.c"



static MS_U8 MStar_MHL_EDID[256] =
{
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x18,0x0F,0x01,0x03,0x80,0x73,0x41,0x78,0x0A,0xCF,0x74,0xA3,0x57,0x4C,0xB0,0x23,
    0x09,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0xFF,0x01,0xFF,0xFF,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x20,0x01,0x1D,0x80,0x3E,0x73,0x38,0x2D,0x40,0x7E,0x2C,
    0x45,0x80,0xC4,0x8E,0x21,0x00,0x00,0x1E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,
    0x6E,0x28,0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3B,0x3C,0x1F,0x2D,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x64,
    0x02,0x03,0x1F,0xF2,0x4C,0x05,0x04,0x03,0x02,0x01,0x12,0x13,0x14,0x16,0x07,0xA2,
    0xA0,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,0x00,0x8C,
    0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,
    0x18,0x8C,0x0A,0xA0,0x14,0x51,0xF0,0x16,0x00,0x26,0x7C,0x43,0x00,0xC4,0x8E,0x21,
    0x00,0x00,0x99,0x01,0x1D,0x00,0xBC,0x52,0xD0,0x1E,0x20,0xB8,0x28,0x55,0x40,0xC4,
    0x8E,0x21,0x00,0x00,0x1F,0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,0x2C,0x25,
    0x80,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,
};

MS_U8 tMHL_DEVICE_CAPABILITY_TABLE[MHL_DEVICE_CAPABILITY_SIZE] =
{
    MHL_DEV_STATE_0x00,    //DEV_STATE
    MHL_VERSION_0x01,    //MHL_VERSION
    MHL_DEV_CAT_0x02,    //DEV_CAT, [4]:POW, [3:0]:DEV_TYPE
    MHL_ADOPTER_ID_H_0x03,    // ADOPTER_ID_H, Mstar adopter ID 715(0x2CB)
    MHL_ADOPTER_ID_L_0x04,    // ADOPTER_ID_L
    MHL_VID_LINK_MODE_0x05,    // VID_LINK_MODE, [5]:SUPP_VGA, [4]:ISLANDS, [3]:PPIXEL, [2]:YUV422, [1]:YUV444, [0]:RGB444
    MHL_AUD_LINK_MODE_0x06,    // AUD_LINK_MODE, [1]:AUD_8CH, [0]:AUD_2CH
    MHL_VIDEO_TYPE_0x07,    // VIDEO_TYPE, [7]: SUPP_VT, [3]:GAME, [2]:CINEMA, [1]: PHOTO, [0]:GRAPHICS
    MHL_LOG_DEV_MAP_0x08,    // LOG_DEV_MAP, [0]:LD_DISPLAY
    MHL_BANDWIDTH_0x09,    // BANDWIDTH, 15*5MHz = 75MHz
    MHL_FEATURE_FLAG_0x0A,    // FEATURE_FLAG, [2]:Scratchpad, [1]:RAP, [0]:RCP
    MHL_DEVICE_ID_H_0x0B,    // DEVICE_ID_H, ? CHIP_ID
    MHL_DEVICE_ID_L_0x0C,    // DEVICE_ID_L, ? CHIP_ID
    MHL_SCRATCHPAD_SIZE_0x0D,    // SCRATCHPAD_SIZE, 16 bytes
    MHL_INT_STAT_0x0E,    // INT_STAT_SIZE, [7:4]:Status 4 bytes, [3:0]:Interrupt 4 bytes
    0x00,    // Reserved
};

typedef struct
{
    MS_U8 u8RCPKeyCode;
    MS_U8 u8IRKeyCode;

} ST_RCP_KEY_TRANSLATE;

#if 0
ST_RCP_KEY_TRANSLATE const astRCPKeyTranslate[] =
{
    //  Note: except the null IR key, all other IR keys should appear once.

    {RCP_KEYID_SELECT,          KEY_SELECT},
    {RCP_KEYID_UP,              KEY_UP},
    {RCP_KEYID_DOWN,            KEY_DOWN},
    {RCP_KEYID_LEFT,            KEY_LEFT},
    {RCP_KEYID_RIGHT,           KEY_RIGHT},

    //Sink does not support...
    /*
    {RCP_KEYID_RIGHT_UP, },
    {RCP_KEYID_RIGHT_DOWN, },
    {RCP_KEYID_LEFT_UP, },
    {RCP_KEYID_LEFT_DOWN, },
    */

    {RCP_KEYID_ROOT_MENU,       KEY_MENU},

    //Sink does not support...
    /*
    {RCP_KEYID_SETUP_MENU, },
    {RCP_KEYID_CONTENTS_MENU, },
    {RCP_KEYID_FAVORITE_MENU, },
    */

    {RCP_KEYID_EXIT,            KEY_EXIT},
    //0x0E - 0x1F are reserved
    {RCP_KEYID_NUM_0,           KEY_0},
    {RCP_KEYID_NUM_1,           KEY_1},
    {RCP_KEYID_NUM_2,           KEY_2},
    {RCP_KEYID_NUM_3,           KEY_3},
    {RCP_KEYID_NUM_4,           KEY_4},
    {RCP_KEYID_NUM_5,           KEY_5},
    {RCP_KEYID_NUM_6,           KEY_6},
    {RCP_KEYID_NUM_7,           KEY_7},
    {RCP_KEYID_NUM_8,           KEY_8},
    {RCP_KEYID_NUM_9,           KEY_9},

    //Sink does not support...
    /*
    {RCP_KEYID_DOT, },
    */

    {RCP_KEYID_ENTER,           KEY_NULL},
    {RCP_KEYID_CLEAR,           KEY_NULL},
    //0x2D - 0x2F are reserved
    {RCP_KEYID_CH_UP,           KEY_CHANNEL_PLUS},
    {RCP_KEYID_CH_DOWN,         KEY_CHANNEL_MINUS},
    {RCP_KEYID_PRE_CH,          KEY_CHANNEL_RETURN},
    {RCP_KEYID_SOUND_SELECT,    KEY_AUDIO},

    //Sink does not support...
    /*
    {RCP_KEYID_INPUT_SELECT, },
    {RCP_KEYID_SHOW_INFO, },
    {RCP_KEYID_HELP, },
    {RCP_KEYID_PAGE_UP, },
    {RCP_KEYID_PAGE_DOWN, },
    //0x39 - 0x40 are reserved
    {RCP_KEYID_VOL_UP, },
    {RCP_KEYID_VOL_DOWN, },
    {RCP_KEYID_MUTE, },
    */

    {RCP_KEYID_PLAY,            KEY_PLAY},
    {RCP_KEYID_STOP,            KEY_STOP},
    {RCP_KEYID_PAUSE,           KEY_PAUSE},
    {RCP_KEYID_RECORD,          KEY_RECORD},
    {RCP_KEYID_REWIND,          KEY_REWIND},
    {RCP_KEYID_FAST_FWD,        KEY_FF},
    {RCP_KEYID_EJECT,           KEY_NULL},
    {RCP_KEYID_FORWARD,         KEY_NEXT},
    {RCP_KEYID_BACKWARD,        KEY_PREVIOUS},
    //0x4D - 0x4F are reserved

    //Sink does not support...
    /*
    {RCP_KEYID_ANGLE, },
    {RCP_KEYID_SUBPICTURE, },
    */

    //0x52 - 0x5F are reserved
    {RCP_KEYID_PLAY_FUNC,       KEY_NULL},
    {RCP_KEYID_PAUSE_PLAY_FUNC, KEY_NULL},
    {RCP_KEYID_RECORD_FUNC,     KEY_NULL},
    {RCP_KEYID_PAUSE_REC_FUNC,  KEY_NULL},
    {RCP_KEYID_STOP_FUNC,       KEY_NULL},

    //Sink does not support...
    /*
    {RCP_KEYID_MUTE_FUNC, },
    {RCP_KEYID_UN_MUTE_FUNC, },
    {RCP_KEYID_TUNE_FUNC, },
    {RCP_KEYID_MEDIA_FUNC, },
    //0x69 - 0x70 are reserved
    {RCP_KEYID_F1, },
    {RCP_KEYID_F2, },
    {RCP_KEYID_F3, },
    {RCP_KEYID_F4, },
    {RCP_KEYID_F5, },
    //0x76 - 0x7D are reserved
    {RCP_KEYID_VENDOR_SPECIFIC, },
    {RCP_KEYID_RESERVED, }
    */
};

#define LENGTH_TRANSLATION_ARRAY sizeof(astRCPKeyTranslate)/sizeof(ST_RCP_KEY_TRANSLATE)
#endif

BOOL devMHL_IsCbusConnected(void)
{
    return mapi_mhl_CableDetect();
}

void devMHL_CbusIsolate(void)
{
    mapi_mhl_CbusIsolate();
}

void devMHL_Initialization(void)
{
    //printf("\r\nMHL Init, MHL_TYPE = 0x%x\r\n", MHL_TYPE);

    // Reset
    MHL_Reset_High();
    msAPI_Timer_Delayms(5);
    MHL_Reset_Low();
    msAPI_Timer_Delayms(5);
    MHL_Reset_High();

    // I2C Init
    //mapi_swi2c_init(g_MhlI2CBus);

    mapi_mhl_RegisterIOCallBackFunctions(mhl_IIC_ReadBytes, mhl_IIC_WriteBytes);
    //mapi_mhl_I2C_Channel_Settings(E_I2C_BUS_DDCD0);
    mapi_mhl_I2C_Channel_Settings(MHL_IIC_BUS, MHL_ELAND_COMMON, MHL_ELAND_CONFIG);
    mapi_mhl_TxSwap_Settings(0);

    // MHL Init
    mapi_mhl_init(MStar_MHL_EDID, tMHL_DEVICE_CAPABILITY_TABLE);

    //mapi_mhl_RegisterCallBackFunctions();
}

void devMHL_Handler(void)
{
    mapi_mhl_handler();
}

#if 0
MS_BOOL devMHL_IRKeyProcess(MS_U8 u8Keycode, MS_BOOL bIsRelease)
{
    MS_U8 u8RcpKey = 0xFF;
    MS_U8 u8Idx;// u8Len;

    //printf("IR Key: 0x%x\r\n", u8Keycode);
    //u8Len = sizeof(astRCPKeyTranslate)/sizeof(ST_RCP_KEY_TRANSLATE);

    for(u8Idx = 0; u8Idx < LENGTH_TRANSLATION_ARRAY; u8Idx++)
    {
        if (astRCPKeyTranslate[u8Idx].u8IRKeyCode == u8Keycode)
        {
            u8RcpKey = astRCPKeyTranslate[u8Idx].u8RCPKeyCode;
            //printf("RCP Key: 0x%x\r\n", u8RcpKey);
            break;
        }
    }

    if (u8Idx == LENGTH_TRANSLATION_ARRAY)
    {
        printf("IR key is not supported by RCP key.\r\n");
        return FALSE;
    }
    else
    {
        return mapi_mhl_SendRCP_PressRelease_Cmd(u8RcpKey, bIsRelease);
    }

}
#endif
MS_BOOL devMHL_SendRAPCmd(MS_U8 databuf)
{
    return mapi_mhl_SendRAPCmd(databuf);
}

#endif
