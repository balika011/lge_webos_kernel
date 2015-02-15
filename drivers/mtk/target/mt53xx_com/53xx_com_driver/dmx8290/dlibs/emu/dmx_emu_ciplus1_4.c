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
 * $RCSfile: dmx_emu_ciplus1_4.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_cmd.c
 *  CI+1.4  Emulation CLI commands
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_mm_if.h"
#include "dmx_emu_if.h"
#include "dmx_drm_drvif.h"
#include "dmx_drm_if.h"
#include "fvr_if.h"
#include "x_lint.h"
#include "fvr.h"
#include "dmx_emu.h"
#include "dmx_emu_mm_multi2_dec.h"
#include "dmx_emu_mm_multi2_enc.h"


LINT_EXT_HEADER_BEGIN


#include "x_drv_cli.h"
#include "x_stl_lib.h"
#include "x_printf.h"
#include "x_assert.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"
#include "x_rand.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "x_bim.h"
#include "dmx_mm.h"
#include <math.h>

//---------------------------------------------------------------------
// LOG macro related

// #define DEFINE_IS_LOG    CLI_IsLog
#define DEFINE_IS_LOG    DMX_IsLog
#include "x_debug.h"

LINT_EXT_HEADER_END

#if defined(CC_MT5890)
#define CACHE(addr)                                   (addr < 0xA0000000)?addr:(addr - 0xA0000000)
#define NONCACHE(addr)                                (addr >= 0xA0000000)?addr:(addr + 0xA0000000)
#else  //MT5882
#define CACHE(addr)                                   ((addr) & 0x7fffffff)
#define NONCACHE(addr)                                (CACHE(addr) | 0x80000000)
#endif

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//LINT_SAVE_AND_DISABLE
//CLIMOD_DEBUG_FUNCTIONS(DMX)
//LINT_RESTORE

//LINT_SUPPRESS_SYMBOL(818, aszArgv)

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
/*define some global address for _DMX_EMU_TS_Parameter_Init function using*/
#define FVR_PES_PID                 0x10;
#define DMX_TS_TOTAL_INPUT_SOURCE  4 //add 4 ts input source 
#define DMX_TEST_PID_TOTAL_NUM     5
#define DDI_TS_TO_FRAMER_TOTAL_NUM  2 //just use this to pass frame0 and framer1.
#define DMX_EMU_FRAMER_COUNT       4 //4
#define PES_RESULT_ADDR            0x5A00000
#define PES_RESULT_PERPID_SIZE     0x180000 //1.5MB
#define PES_PACKET_PREFIX_LEN      6
#define RATE_N                     0x2000
#define RATE_M                     0x1C0000
#define PCR_PID0                   0x987
#define PCR_PID1                   0xd83
#define PCR_TS_SIZE                0x118FD7
#define MULTI_STREAM_BUFFER_START  (FVR_GBL_ARY_W(4, 0))
#define MULTI_STREAM_BUFFER_END    (FVR_GBL_ARY_W(4, 1))
#define MULTI_STREAM_BUFFER_WP     (FVR_GBL_ARY_W(4, 3))
#define MULTI_STREAM_BUFFER_RP     (FVR_GBL_ARY_W(4, 4))
#define Dmx_Pvr3_Wp_Thread_NAME    "PVR3"
#define PVR_MOVE_CHUNK_DATA_UNIT_SIZE  (188*30)
#define PVR_MM_ALIGN_SIZE              (188)
/*for encryption/decryption test ,define key index*/
#define TV_PLAY_KEY_IDX                (0)
#define TV_RECORD_KEY_IDX              (1)
#define TIMESHIFT_PLAY_KEY_IDX         (2)
#define MM_PLAY_KEY_IDX                (3)

//-----------------------------------------------------------------------------
// Type definitions &&Static variables&& Macro definitions
//-----------------------------------------------------------------------------
/*define the structure to defined while use the ddi_ts to enter signal*/
BOOL DDI_FRONTEND_CONFIG_T[DMX_EMU_FRAMER_COUNT]={TRUE,TRUE,FALSE,FALSE};

//construct the dst buffer for every pid.
typedef struct
{
  UINT32 u4BufAddr ;
  UINT32 u4BufSize;
}PID_DATA_STRUCTURE_BUFFER_T;

PID_DATA_STRUCTURE_BUFFER_T _arPidBuffer[DMX_TEST_PID_TOTAL_NUM+1];
/*define the different pvr type*/
typedef enum
{
    TIMESHIFT_PVR=0,
    IP_PVR=1,
    CI_REMUX_PVR=2,
    MM_PVR=3,
    TS_PVR=4
}PVR_TYPE_T;

typedef enum
{
    DMX_TS_FROM_DDI = 0,
    DMX_TS_FROM_PVR1 = 1,
    DMX_TS_FROM_PVR2= 2,
    DMX_TS_FROM_PVR3= 3
}DMX_TS_INPUT_SOURCE;


typedef struct
{
    UINT32 Es_Addr_Start;
    UINT32 ES_Length; // init ,for total size 
    UINT32 ES_Size;  //used size for es datas.
    UINT32 Pes_Addr_Start;
    UINT32 Pes_Addr_Offset;
    UINT32 Pes_Size;
    UINT32 TS_Addr_Start;
    UINT32 TS_Addr_Offset;
    UINT32 TS_Pktcount;
    BOOL   fg_used;
}DMX_GOLDEN_FILE_T;

typedef struct
{
    UINT32 u4_RecordAddr;
    UINT32 u4_RecordSize;
}Record_Address_Config_T;

typedef enum
{
    DDI_TS_TO_FRAMER0=0,
    DDI_TS_TO_FRAMER1=1,
    PVR_TS_TO_FRAMER_DBM=2
}DMX_TS_PASS_FRAMER_T;

typedef struct
{
    UINT32 DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    UINT32 DDI_TS_TO_FRAMER_GOLDEN_PES_Offset;
    UINT32 DDI_TS_TO_FRAMER_GOLDEN_PES_Len;
}DDI_TS_TO_FRAMER_T;

static DMX_GOLDEN_FILE_T _arTsGoldenFile[DMX_TS_TOTAL_INPUT_SOURCE] =
{/*Es_Addr_Start,ES_Length,ES_Size,Pes_Addr_Start,Pes_Addr_Offset,Pes_Size,
    *TS_Addr_Start,TS_Addr_Offset,TS_Pktcount,used*/
     {0x3000000,0x80000,0,0x3200000,0x3200000,0,0x3400000,0x3400000,0,1}, /*DDI_TS,80000*/
     {0x3800000,0x80000,0,0x3A00000,0x3A00000,0,0x3C00000,0x3C00000,0,0},  /*PVR1_TS*/
     {0x4000000,0x80000,0,0x4200000,0x4200000,0,0x4400000,0x4400000,0,1},  /*PVR2_TS*/
     {0x4800000,0x80000,0,0x4A00000,0x4A00000,0,0x4C00000,0x4C00000,0,0}   /*PVR3_TS*/
};

/*check the ts to framer type*/
static DMX_TS_PASS_FRAMER_T PID_FRAMER_TABLE[DMX_TEST_PID_TOTAL_NUM];

static Record_Address_Config_T Record_Addr_T[FVR_GBL_COUNT]=
{/*record_address,record_size*/
      {0x2000000,0x180000},
    {0x2180000,0x180000},
    {0x2300000,0x180000},
    {0x2480000,0x180000},
    {0x2600000,0x800000}
};

/*used for set pid num*/
static UINT16 PID_TABLE[DMX_TEST_PID_TOTAL_NUM];
/*for every pid ,set the cc number*/
static UINT8  PID_CC_TABLE[DMX_TEST_PID_TOTAL_NUM];

/*the data structure used for save the golden pes datas for framer0 and framer1.*/
static DDI_TS_TO_FRAMER_T DDI_TS_TO_FRAMER_GOLDEN_FILES[DDI_TS_TO_FRAMER_TOTAL_NUM] =
{/*pes_start_addr,pes_offset_addr,pes_len*/
   {0x5200000,0x5200000,0},
   {0x5600000,0x5600000,0}
};


static UINT32 _au4StartCode[] =
{
    0x01000008,
    0x01000010,
    0x01000018
};



static UINT8 _au1FVRPesHeader[] = { 0x00,0x00,0x01,0xe0,0x00,0x00, 0x85,0xc0,0x11,0x39,0x85,0xdb,
                                    0xe2,0x85,0xdb,0x9c,0x67,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff
                                  };
static UINT32 _u4PesHeaderLen = sizeof(_au1FVRPesHeader) / sizeof(UINT8);


static HANDLE_T _hFvrEmuQueue = NULL_HANDLE;
static HANDLE_T _hFvrEmuThread = NULL_HANDLE;
static HANDLE_T _hMMThread = NULL_HANDLE;
static HANDLE_T _hMMQueue = NULL_HANDLE;
static BOOL fg_pvr3_complete = FALSE;
static BOOL fg_init_first = FALSE;
static BOOL fg_mm_move_stop = TRUE; 
static UINT32 _mmCounter = 0;


typedef struct
{
    UINT8 u1TsIdx;
    UINT32 u4Addr;
    UINT32 u4Size;
} FVR_MSG_EMU_T;

typedef struct
{
  UINT32 u4Addr;
  UINT32 u4Size;
}MM_MSG_T;
/*used for key type used.*/
typedef enum
{
  TV_PLAY_KEY=0,
  TV_RECORD_KEY=1,
  PVR_PLAY_KEY = 2,
  MM_PLAY_KEY=3,
  DMX_KEY_TYPE_TOTAL=4
}DMX_KEY_TYPE_T;

/* for every key type ,init the des param*/
static DES_PARAM_T _arParam_Table[DMX_KEY_TYPE_TOTAL]; 

/*define  function prototype*/
static VOID _FvrThread(VOID* pvArg);
static void FVR_GBLCallback(FVR_GBL_NOTIFY_CODE_T eCode,
                                       UINT32 u4Data, const void* pvTag);
static BOOL _SetPVRGBLEx5(UINT8 u1Index, BOOL fgTimestamp,UINT32 Pattern,UINT8 mask,
                                UINT32 u4TimeToNotify);


static VOID _MMThread(VOID* pvArg);

static VOID _InitDesParam(DMX_KEY_TYPE_T e_type,UINT16 u2pid1,UINT16 u2pid2);




// Static functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** function name:_DMX_EmuSetPVRPlay
 *    description: this function is used for set the  timestamp for pvr.
 *    parameter:fgOn:pvr handle the timestamp or not. 
 *                    fgContainTimeStamp:set whether the ts contains the timestamp
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuSetTimestamp(UINT8 u1TsOutIdx,BOOL fgContainTimeStamp,BOOL fgOn)
{
    DMX_PVR_PLAY_T rPlay;

    if (fgOn)
    {
        rPlay.eMode = DMX_PVR_PLAY_STREAM;
    }
    else
    {
        rPlay.eMode = DMX_PVR_PLAY_SINGLEMOVE;
    }

    rPlay.fgContainTimeStamp = fgContainTimeStamp;
    rPlay.fgIgnoreTimeStamp = !fgOn;
    rPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    if (!DMX_PVRPlay_SetEx(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_MODE | PVRPLAY_FLAGS_TIMESTAMP), &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EmuSetPVRPlayBuffer
 *    description: this function is used for set the mini-pvr buffer.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_EmuSetPVRPlayBuffer(PVR_TYPE_T e_type)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize;
    DMX_TS_INPUT_SOURCE e_source_mode;
    UINT8 u1TsOutIdx=0;
    UINT32 u4Addr;

#if defined(CC_MT5890)
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {/*for pvr2, IP/MM path*/
            e_source_mode = DMX_TS_FROM_PVR2;
            u1TsOutIdx =1;
        }break;
        case TIMESHIFT_PVR:
        {
            e_source_mode = DMX_TS_FROM_PVR1;
            u1TsOutIdx = 0;
        }break;
        case CI_REMUX_PVR:
        {
            e_source_mode = DMX_TS_FROM_PVR3;
            u1TsOutIdx = 2;
        }break;
    }
#else  //MT5882
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {/*for pvr2, IP/MM path*/
            e_source_mode = DMX_TS_FROM_PVR2;
            u1TsOutIdx =1;
        }break;
        case TIMESHIFT_PVR:
        {
            e_source_mode = DMX_TS_FROM_PVR2;
            u1TsOutIdx = 1;
        }break;
        case CI_REMUX_PVR:
        {
            e_source_mode = DMX_TS_FROM_PVR1;
            u1TsOutIdx = 0;
        }break;
    }
#endif

    if((e_type==MM_PVR) || (e_type==TS_PVR))
    {
        u4DataSize=_arTsGoldenFile[e_source_mode].ES_Length;
        u4Addr = (_arTsGoldenFile[e_source_mode].Es_Addr_Start+u4DataSize);
        rPlay.u4BufStart = _arTsGoldenFile[e_source_mode].Es_Addr_Start;
    }
    else
    {
        u4DataSize=_arTsGoldenFile[e_source_mode].TS_Addr_Offset-  \
              _arTsGoldenFile[e_source_mode].TS_Addr_Start;
        u4Addr = _arTsGoldenFile[e_source_mode].TS_Addr_Offset;
        rPlay.u4BufStart = _arTsGoldenFile[e_source_mode].TS_Addr_Start;
    }
    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufSize = (u4DataSize+0x10);
    rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    rPlay.pfnPVRNotify = NULL;

    if (!_DMX_PVRPlay_Set(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER |
                                   PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }
    /*set wp pointer*/
    if(u1TsOutIdx == 0)
    {
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP,u4Addr); 
    }
    else if(u1TsOutIdx == 1)
    {
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP2,u4Addr); 
    }
    else
    {
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP3,u4Addr); 
    }

    return TRUE;
}
//-----------------------------------------------------------------------------
/** function name:_DMX_EmuSetPVRPlay
 *    description: this function is used for set the  timestamp and buffer for pvr according to
 *                      different source type.
 *    parameter:fgOn:pvr handle the timestamp or not. 
 *                    e_type:set whether the ts contains the timestamp
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuSetPVRPlay(PVR_TYPE_T e_type,BOOL fgOn)
{
    BOOL fgcontaintimestamp = TRUE;
    UINT8 u1TsOutIdx=0;
    /*set buffer according to the pvr type:ip,timeshfit,ci_cam out*/
    if(_DMX_EmuSetPVRPlayBuffer(e_type) == FALSE)
    {
        Printf("Pvr buffer allocate error.\r\n");

        return FALSE;
    }

#if defined(CC_MT5890)
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {/*for pvr2, IP/MM path*/
            fgcontaintimestamp=FALSE; 
            u1TsOutIdx = 1; 
        }break;
        case TIMESHIFT_PVR:
        {
            fgcontaintimestamp = TRUE;
            u1TsOutIdx = 0; 
        }break;
        case CI_REMUX_PVR:
        {
            fgcontaintimestamp = TRUE;
            u1TsOutIdx = 2 ; 
        }break;
    }
#else  //MT5882
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {/*for pvr2, IP/MM path*/
            fgcontaintimestamp=FALSE; 
            u1TsOutIdx = 1; 
        }break;
        case TIMESHIFT_PVR:
        {
            fgcontaintimestamp = TRUE;
            u1TsOutIdx = 1; 
        }break;
        case CI_REMUX_PVR:
        {
            fgcontaintimestamp = TRUE;
            u1TsOutIdx = 0 ; 
        }break;
    }
#endif
    /*set timestamp*/
    _DmxEmuSetTimestamp(u1TsOutIdx,fgcontaintimestamp,fgOn);
    return TRUE;
}


static BOOL _DmxEmuSetPVRPlayExt(PVR_TYPE_T e_type,BOOL fgOn,BOOL fgcontaintimestamp)
{
    UINT8 u1TsOutIdx=0;
    /*set buffer according to the pvr type:ip,timeshfit,ci_cam out*/
    if(_DMX_EmuSetPVRPlayBuffer(e_type) == FALSE)
    {
      Printf("Pvr buffer allocate error.\r\n");

      return FALSE;
    }

#if defined(CC_MT5890)
    if(e_type == IP_PVR)
    {/*for IP source ,the ts can't contain timestamp*/
       fgcontaintimestamp=FALSE; 
       u1TsOutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       fgcontaintimestamp = TRUE;
       u1TsOutIdx = 0;
    }
    else
    {/*for debug used.*/
       u1TsOutIdx = 2;
    }
#else
    if(e_type == IP_PVR)
    {/*for IP source ,the ts can't contain timestamp*/
       fgcontaintimestamp=FALSE; 
       u1TsOutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       fgcontaintimestamp = TRUE;
       u1TsOutIdx = 1;
    }
    else
    {/*for debug used.*/
       u1TsOutIdx = 0 ;
    }
#endif

    /*set timestamp*/
    _DmxEmuSetTimestamp(u1TsOutIdx,fgcontaintimestamp,fgOn);
    return TRUE;
}


static BOOL _DMX_EmuSetPVRPlayBufferExt(PVR_TYPE_T e_type)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize;
    DMX_TS_INPUT_SOURCE e_source_mode;
    UINT8 u1TsOutIdx=0;
    UINT32 u4Addr;

#if defined(CC_MT5890)
    switch(e_type)
   {
     case IP_PVR:
     case MM_PVR:
     case TS_PVR:
     {/*for pvr2, IP/MM path*/
           e_source_mode = DMX_TS_FROM_PVR2;
        u1TsOutIdx =1;
     }break;
     case TIMESHIFT_PVR:
     {
        e_source_mode = DMX_TS_FROM_PVR1;
        u1TsOutIdx = 0;
     }break;
     case CI_REMUX_PVR:
     {
           e_source_mode = DMX_TS_FROM_PVR3;
        u1TsOutIdx = 2;
     }break;

   }
#else  //MT5882
    switch(e_type)
   {
     case IP_PVR:
     case MM_PVR:
     case TS_PVR:
     {/*for pvr2, IP/MM path*/
           e_source_mode = DMX_TS_FROM_PVR2;
        u1TsOutIdx =1;
     }break;
     case TIMESHIFT_PVR:
     {
        e_source_mode = DMX_TS_FROM_PVR2;
        u1TsOutIdx = 1;
     }break;
     case CI_REMUX_PVR:
     {
           e_source_mode = DMX_TS_FROM_PVR1;
        u1TsOutIdx = 0;
     }break;

   }
#endif

   if((e_type==MM_PVR) || (e_type==TS_PVR))
   {
     u4DataSize=_arTsGoldenFile[e_source_mode].ES_Length;
     u4Addr = _arTsGoldenFile[e_source_mode].Es_Addr_Start;
     rPlay.u4BufStart = _arTsGoldenFile[e_source_mode].Es_Addr_Start;
   }
   else if(e_type==CI_REMUX_PVR)
   {/*use the record ciplus regioin pointer*/
     u4DataSize=Record_Addr_T[4].u4_RecordSize; 
     u4Addr = Record_Addr_T[4].u4_RecordAddr;
     rPlay.u4BufStart = Record_Addr_T[4].u4_RecordAddr;
   }
    else if(e_type==TIMESHIFT_PVR)
   {/*now just record for ts_idx0,use the record ciplus regioin pointer*/
     u4DataSize=Record_Addr_T[0].u4_RecordSize; 
     u4Addr = Record_Addr_T[0].u4_RecordAddr;
     rPlay.u4BufStart = Record_Addr_T[0].u4_RecordAddr;
   }
   else
   {
     u4DataSize=_arTsGoldenFile[e_source_mode].TS_Addr_Offset-  \
              _arTsGoldenFile[e_source_mode].TS_Addr_Start;
     u4Addr = _arTsGoldenFile[e_source_mode].TS_Addr_Offset;
     rPlay.u4BufStart = _arTsGoldenFile[e_source_mode].TS_Addr_Start;
   }


    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufSize = (u4DataSize+0x10);
    rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    rPlay.pfnPVRNotify = NULL;

    if (!_DMX_PVRPlay_Set(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER |
                                   PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }

    if((e_type==CI_REMUX_PVR) || (e_type==TIMESHIFT_PVR) ||(e_type==MM_PVR))
    {
      _DMX_PVRPlay_FlushBuf(u1TsOutIdx);
    }

#if defined(CC_MT5882)
    if (e_type == IP_PVR)
    {
        /*set wp pointer*/
        if(u1TsOutIdx == 0)
        {
          DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP,u4Addr); 
        }
        else if(u1TsOutIdx == 1)
        {
          DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP2,u4Addr); 
        }
        else
        {
        //    DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP3,u4Addr); 
          ASSERT(0);
        }
    }
#endif

    return TRUE;
}

static BOOL _DmxEmuSetPVRPlayExt1(PVR_TYPE_T e_type,BOOL fgOn,BOOL fgcontaintimestamp)
{
    UINT8 u1TsOutIdx=0;
    /*set buffer according to the pvr type:ip,timeshfit,ci_cam out*/
    if(_DMX_EmuSetPVRPlayBufferExt(e_type) == FALSE)
    {
      Printf("Pvr buffer allocate error.\r\n");

      return FALSE;
    }

#if defined(CC_MT5890)
    if(e_type == IP_PVR || e_type==MM_PVR || e_type==TS_PVR)
    {/*for IP source ,the ts can't contain timestamp*/
       fgcontaintimestamp=FALSE; 
       u1TsOutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsOutIdx = 0;
    }
    else 
    {/*for debug used.*/
       u1TsOutIdx = 2 ;
    }
#else  //MT5882
    if(e_type == IP_PVR || e_type==MM_PVR || e_type==TS_PVR)
    {/*for IP source ,the ts can't contain timestamp*/
       fgcontaintimestamp=FALSE; 
       u1TsOutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsOutIdx = 1;
    }
    else 
    {/*for debug used.*/
       u1TsOutIdx = 0 ;
    }

#endif

    /*set timestamp*/
    _DmxEmuSetTimestamp(u1TsOutIdx,fgcontaintimestamp,fgOn);
    return TRUE;
}


//-----------------------------------------------------------------------------
/** function name:_DmxEmuStartPVRPlay
 *    description: this function is used for start pvr according to
 *                      different source type.
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuStartPVRPlay(PVR_TYPE_T e_type)
{
    UINT8 u1TsoutIdx=0;

#if defined(CC_MT5890)
    if(e_type == IP_PVR ||e_type == MM_PVR ||e_type == TS_PVR )
    {
       u1TsoutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsoutIdx = 0;
    }
    else
    {
       u1TsoutIdx = 2;
    }
#else  //MT5882
    if(e_type == IP_PVR ||e_type == MM_PVR ||e_type == TS_PVR )
    {
       u1TsoutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsoutIdx = 1;
    }
    else
    {
       u1TsoutIdx = 0 ;
    }
#endif

    if (!_DMX_PVRPlay_Start(u1TsoutIdx))
    {
       Printf("Start PVR playback(src=%d) Fail.\r\n",e_type);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DmxEmuStopPVRPlay
 *    description: this function is used for start pvr according to
 *                      different source type.
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuStopPVRPlay(PVR_TYPE_T e_type)
{
    UINT8 u1TsoutIdx=0;

#if defined(CC_MT5890)
    if(e_type == IP_PVR ||e_type == MM_PVR || e_type == TS_PVR )
    {
       u1TsoutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsoutIdx = 0;
    }
    else
    {
       u1TsoutIdx = 2;
    }
#else  //MT5882
    if(e_type == IP_PVR ||e_type == MM_PVR || e_type == TS_PVR )
    {
       u1TsoutIdx = 1;
    }
    else if(e_type == TIMESHIFT_PVR)
    {
       u1TsoutIdx = 1;
    }
    else
    {
       u1TsoutIdx = 0 ;
    }
#endif

    if (!_DMX_PVRPlay_Stop(u1TsoutIdx,TRUE))
    {
       Printf("Stop PVR playback(src=%d) Fail.\r\n",e_type);
        return FALSE;
    }
    return TRUE;
}

static BOOL _DMX_EmuPVRPlay_FreeBuf(PVR_TYPE_T e_type)
{
    UINT8 u1TsoutIdx=0;

#if defined(CC_MT5890)
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {
            u1TsoutIdx = 1; 
        }
        break;
        case TIMESHIFT_PVR:
            u1TsoutIdx = 0;
        break;
        case CI_REMUX_PVR:
            u1TsoutIdx = 2;
        break;
        default:Printf("Type error.");break;
    }
#else  //MT5882
    switch(e_type)
    {
        case IP_PVR:
        case MM_PVR:
        case TS_PVR:
        {
            u1TsoutIdx = 1; 
        }break;
        case TIMESHIFT_PVR:
            u1TsoutIdx = 1;break;
        case CI_REMUX_PVR:
            u1TsoutIdx = 0 ;break;
        default:Printf("Type error.");break; 
    }
#endif

    if (!_DMX_PVRPlay_FreeBuf(u1TsoutIdx))
    {
        Printf("Free PVR playback(src=%d) Fail.\r\n",e_type);
        return FALSE;
    }
    return TRUE;
}

static BOOL _DMX_EMU_SetDBMBypassPid(UINT8 u1TsIdx,UINT8 u1pidx, BOOL fg_disable_filter)
{
    UINT32 u4Reg = 0;
    if(u1TsIdx > MAX_TS_INDEX)
    {
        Printf("Error TS index(%d) .\r\n",u1TsIdx);
        return FALSE;
    }
    if(u1TsIdx = 0)
    {
        u4Reg = DMX_REG_DBM_BYPASS_PID;
    }
    else if(u1TsIdx = 1)
    {
        u4Reg = DMX_REG_DBM_BYPASS_PID_2;
    }
    else if(u1TsIdx = 2)
    {
        u4Reg = DMX_REG_DBM_BYPASS_PID_3;
    }
    else if(u1TsIdx = 3)
    {
        u4Reg = DMX_REG_DBM_BYPASS_PID_4;
    }
    else if(u1TsIdx = 4)
    {
        u4Reg = DMX_REG_DBM_BYPASS_PID_5;
    }

    if(u4Reg == 0)
    {
        Printf("Error TS index(%d) .\r\n",u1TsIdx);
        return FALSE;
    }
    
    if(fg_disable_filter)
    {
        DMXCMD_WRITE32(u4Reg, u1pidx << 16 | 0x20);
    }
    else
    {
        DMXCMD_WRITE32(u4Reg, 0);
    }

    return TRUE;
}
//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_SetPlayPid
 *    description: this function is used for set the playback pid for clear TS.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_EMU_SetPlayPid(UINT8 u1TsIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,DMX_PID_TYPE_T e_type)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2pidnum;
    rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
    rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = e_type;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;

    if (!_DMX_SetPid(u1pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return    TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_SetPlayPidEx
 *    description: this function is used for set the playback pid for clear TS.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_EMU_SetPlayPidEx(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,DMX_PID_TYPE_T e_type)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2pidnum;
    rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
    rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = e_type;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;

    if (!_DMX_SetPid(u1pidx, DMX_PID_FLAG_ALL&~DMX_PID_FLAG_BUFFER&~DMX_PID_FLAG_STEER, 
         &rPid, FALSE))
    {
        return FALSE;
    }

    return    TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_SetPlayPCRPid
 *    description: this function is used for set the playback pid for clear TS.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_EMU_SetPlayPcrPid(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,DMX_PID_TYPE_T e_type)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2pidnum;
    rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
    rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = e_type;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;
    rPid.ePcrMode = DMX_PCR_MODE_NEW;
    if (!_DMX_SetPid(u1pidx, DMX_PID_FLAG_ALL&~DMX_PID_FLAG_BUFFER&~DMX_PID_FLAG_STEER, 
         &rPid, FALSE))
    {
        return FALSE;
    }

    return    TRUE;
}


static BOOL _DMX_EMU_SetPlayPidEx1(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,DMX_PID_TYPE_T e_type)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2pidnum;
    rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
    rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = e_type;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;

    if (!_DMX_SetPid(u1pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return    TRUE;
}


static BOOL _DMX_EMU_SetPlayPidEx2(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,DMX_PID_TYPE_T e_type)
{
    DMX_PID_T rPid;
    UINT32 u4Ctrl;
    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = TV_PLAY_KEY;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2pidnum;
    rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
    rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = e_type;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    
    if (!_DMX_SetPid(u1pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

     //after set descmode none,dmx_setpid would't set descrambling scheme.
    //so we should set this after dmx_setpid
    _DMX_Lock();
    u4Ctrl = PID_INDEX_TABLE(u1pidx);
    u4Ctrl |= 0x400; //set ca descramble enable
    PID_INDEX_TABLE(u1pidx) = u4Ctrl;
    _DMX_Unlock();
    
    return    TRUE;
}


//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_SetRecordPid
 *    description: this function is used for set the record pid for TS Record.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_EMU_SetRecordPid(UINT8 u1TsIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,BOOL  fgPicSearch,
                                       BOOL fgScramble,BOOL fgdecrypt)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIdx;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = fgScramble;
    rFVRPid.ePidType = DMX_PID_TYPE_PES;
    rFVRPid.fgPicSearch = fgPicSearch;
    rFVRPid.u2Pid = u2pidnum;

    if (!FVR_SetPid(u1pidx, FVR_PID_FLAG_ALL & (~FVR_PID_FLAG_KEY_LEN), &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}


static BOOL _DMX_EMU_SetRecordPidExt(UINT8 u1TsIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,BOOL  fgPicSearch,
                                       BOOL fgScramble,BOOL fgdecrypt,DMX_PID_TYPE_T e_type)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIdx;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = fgScramble;
    rFVRPid.ePidType = e_type;
    rFVRPid.fgPicSearch = fgPicSearch;
    rFVRPid.u2Pid = u2pidnum;

    if (!FVR_SetPid(u1pidx, FVR_PID_FLAG_ALL & (~FVR_PID_FLAG_KEY_LEN), &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _DMX_EMU_SetMultiRecordPid(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,BOOL  fgPicSearch,
                                       BOOL fgScramble,BOOL fgdecrypt,DMX_PID_TYPE_T e_type)

{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIdx;
    rFVRPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = fgScramble;
    rFVRPid.ePidType = e_type;
    rFVRPid.fgPicSearch = fgPicSearch;
    rFVRPid.u2Pid = u2pidnum;

    if (!FVR_SetPid(u1pidx, FVR_PID_FLAG_ALL & (~FVR_PID_FLAG_KEY_LEN), &rFVRPid))
    {
        return FALSE;
    }

    

    return TRUE;
}

static BOOL _DMX_EMU_SetMultiRecordPid1(UINT8 u1TsIdx,UINT8 u1Ts_LocalIdx,UINT8 u1pidx,
                                       UINT16 u2pidnum,BOOL  fgPicSearch,
                                       BOOL fgScramble,BOOL fgdecrypt,DMX_PID_TYPE_T e_type)

{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIdx;
    rFVRPid.u1LocalTsIndex = u1Ts_LocalIdx;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = fgScramble;
    rFVRPid.ePidType = e_type;
    rFVRPid.fgPicSearch = fgPicSearch;
    rFVRPid.u2Pid = u2pidnum;
    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = TV_PLAY_KEY;
    
    if (!FVR_SetPid(u1pidx, FVR_PID_FLAG_ALL & (~FVR_PID_FLAG_KEY_LEN), &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
/** function name:_SetPVRGBL
 *    description: this function is used for set the record global region for remux/normal record.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRGBL(UINT8 u1Index, BOOL fgTimestamp)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=0x40414243;//0x40414243,for debug used.
    rGblPid.u1SyncMask = 0x7;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

static BOOL _SetPVRGBLEx2(UINT8 u1Index, BOOL fgTimestamp,UINT32 Pattern)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=Pattern;//0x40414243,for debug used.
    rGblPid.u1SyncMask = 0x7;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

static BOOL _SetPVRGBLEx4(UINT8 u1Index, BOOL fgTimestamp,UINT32 Pattern,UINT8 mask)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=Pattern;//0x40414243,for debug used.
    rGblPid.u1SyncMask = mask;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

static BOOL _SetPVRGBLEx5(UINT8 u1Index, BOOL fgTimestamp,UINT32 Pattern,UINT8 mask,
                                UINT32 u4TimeToNotify)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=Pattern;
    rGblPid.u1SyncMask = mask;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    rGblPid.pfnNotifyFunc = FVR_GBLCallback;
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    _SetPVRGBLTimerNotify(u1Index, u4TimeToNotify);
    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}


static BOOL _SetPVRGBLEx(UINT8 u1Index, BOOL fgTimestamp,UINT16 ui2_pkt_cnt)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = ui2_pkt_cnt;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=0x40414243;
    rGblPid.u1SyncMask = 0x7;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

static BOOL _SetPVRGBLEx3(UINT8 u1Index, BOOL fgTimestamp,UINT8 remux_flag)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = Record_Addr_T[u1Index].u4_RecordAddr;
    rGblPid.u4BufSize = Record_Addr_T[u1Index].u4_RecordSize;
    rGblPid.u4SyncPattern=0x40414243;//0x40414243,for debug used.
    rGblPid.u1SyncMask = remux_flag;
    rGblPid.u1CiTsidx =0x8; /*ci tsindex=0x3*/
    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_SetPVRGBLTimerNotify
 *    description: this function is used for set the record timer interrupt.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
BOOL _SetPVRGBLTimerNotify(UINT8 u1Index, UINT32 ui4_timer_notify)
{
    FVR_GBL_T rGblPid;
 

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.u4TimeToNotify = ui4_timer_notify;

    if (!_FVR_SetGBL(u1Index, FVR_GBL_FLAG_TIME_INFO, &rGblPid))
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_FreePlaypid
 *    description: freee the pid reasource.
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static void _DMX_EMU_FreePlaypid(UINT8 u1_pidx)
{
    DMX_PID_T rPid;
    x_memset((void*)&rPid,0,sizeof(DMX_PID_T));
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(u1_pidx));
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_CustFrontEnd
 *    description: use two port:framer0 and framer1 to simulate the two tuner signal 
 *    enter demux by ddi_ts.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------

static BOOL _DMX_EMU_CustFrontEnd(VOID)
{
    UINT32 u4Reg, i;
    DMX_DDI_T rDDI;

    //set the ddi rate to 180Mbps
    rDDI.u4RateN = 0x2000;
    rDDI.u4RateM = 0x1C0000;
    if (!DMX_DDI_Set((UINT32)(DMX_DDI_FLAG_RATE), &rDDI))
    {
        LOG(3, "Failed to set DDI properties!\n");
        return FALSE;
    }
    // set ddi_ts->tsidx->framer config
    for(i=0;i<DMX_EMU_FRAMER_COUNT;i++)
    {
      if(DDI_FRONTEND_CONFIG_T[i] == TRUE)
      {
        _DMX_SetFrontEndEx(i, DMX_FE_DDI);
      }
    }
    //set ddi interface to framer.
    u4Reg = DDI_READ32(DDI_REG_DMX_RX_CTRL);
    u4Reg &= ~0x10;
    DMXCMD_WRITE32(DDI_REG_DMX_RX_CTRL,u4Reg);

    return TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_CustPvrPath
 *    description: according to ciplus1.4 used ,allocate the ts-out for use
*
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------

static BOOL _DMX_EMU_CustPvrPath(PVR_TYPE_T e_pvr_type,UINT8 u1_tsidx,BOOL fg_disable_filter)
{
    UINT32 u4Reg;
    UINT8 i=0x0;
    UINT8 u1TsoutIdx = 0;
    switch(e_pvr_type)
    {
        case TIMESHIFT_PVR:
        {
#if defined(CC_MT5890)
            //for timeshift used
            u1TsoutIdx = 0;
            if(u1_tsidx == 2)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
            }
            else if (u1_tsidx == 4)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_DBM);
            }
            else
            {
                Printf("TS_IDX3 is used by CAM.\r\n");
                return FALSE;
            }
#else  //MT5882
            u1TsoutIdx = 1;
            if(u1_tsidx == 2)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
            }
            else if (u1_tsidx == 3)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
            }
#endif
        }break;
        case IP_PVR:
        {// for ip used
#if defined(CC_MT5890)
            u1TsoutIdx = 1;
            u4Reg=DMXCMD_READ32(DMX_REG_DBM_CONTROL);
            if(fg_disable_filter)
            {
                i = 0x1;
            }
            if(u1_tsidx == 2)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
                u4Reg &= ~(0x1<<28);
                u4Reg |= (i<<28);
            }
            else if (u1_tsidx == 4)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_DBM);
                u4Reg &= ~(0x1<<25);
                u4Reg |= (i<<25);
            }
            else
            {
                Printf("TS_IDX3 is used by CAM.\r\n");
                return FALSE;
            }
#else
            u1TsoutIdx = 1;
            u4Reg=DMXCMD_READ32(DMX_REG_DBM_CONTROL);
            if(fg_disable_filter)
            {
                i = 0x1;
            }
            if(u1_tsidx == 2)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
                u4Reg &= ~(0x1<<28);
                u4Reg |= (i<<28);
            }
            else if (u1_tsidx == 3)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
                u4Reg &= ~(0x1<<29);
                u4Reg |= (i<<29);
            }
            else
            {
                Printf("TS_IDX Error.\r\n");
                return FALSE;
            } 
#endif
            DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,u4Reg);
        }break;
        case CI_REMUX_PVR:
        {
#if defined(CC_MT5890)
            u1TsoutIdx = 2;
            //for  cam-out ,the path is :pvr3->ts_idx3->
            if(u1_tsidx == 2)
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2);
            }      
            else
            {
                _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3);
            }
#else  //MT5882
            u1TsoutIdx = 0;
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3);
#endif
        }break;
        default: 
        {
            Printf("Set tsoutidx error .\r\n"); 
            return FALSE;
        }break;        
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_InitHWPath
 *    description: init hardware path for ciplus1.4 setting test. 
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static void  _DMX_EMU_InitHWPath(void)
{
    /*use the ddi_ts to simulate the tuner front-end,t
        *he ddi_ts for mm is also included in this path*/
    _DMX_EMU_CustFrontEnd();

#if defined(CC_MT5890)
    /*init pvr-playback path  path*/
    /*for ip*/
    _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE); /*PB2_TO_DBM->Framer3,pid filter default*/
    /*for timeshift*/
    _DMX_EMU_CustPvrPath(TIMESHIFT_PVR,4,FALSE); /*PB_TO_DBM->TS_IDX4, pid filter default.*/
    /*for ci+1.4 cam output*/
    _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); /*tsout ts3 ->Framer3*/
#else  //MT5882
    /*init pvr-playback path  path*/
    /*for ip*/
    _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE); /*PB2_TO_DBM->Framer3,pid filter default*/
    /*for ci+1.4 cam output*/
    _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); /*tsout ts3 ->Framer3*/
#endif 
 
  return;
}
//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_CIPLUS_Init
 *    description: when you test case ,you need init it before.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static void _DMX_EMU_CIPLUS_Init(VOID)
{
    /*init hardware path*/
    _DMX_EMU_InitHWPath();
}
//-----------------------------------------------------------------------------
/** function name:_DMX_EMU_TS_Parameter_Init
 *    description: init ts parameter for construct the pes datas.
 *    mode :pull mode
 */
//-----------------------------------------------------------------------------
static void _DMX_EMU_TS_Parameter_Init(void)
{
    UINT32 i=0;
    /*init cc to 0*/
    x_memset((VOID*)PID_CC_TABLE,0,sizeof(PID_CC_TABLE));
    /*set the pid num by pid index*/
    for(i=0;i<DMX_TEST_PID_TOTAL_NUM;i++)
    {
        PID_TABLE[i] = i+1;
    }
    /*set the frame type by pid index*/
    for(i=0;i<DMX_TEST_PID_TOTAL_NUM;i++)
    {
        if(i<(DMX_TEST_PID_TOTAL_NUM-3))
        {
            if(i%2 == 0)
            {
                PID_FRAMER_TABLE[i] = DDI_TS_TO_FRAMER0;
            }
            else
            {
                PID_FRAMER_TABLE[i] = DDI_TS_TO_FRAMER1;
            }
        }
        else
        {
            PID_FRAMER_TABLE[i] = PVR_TS_TO_FRAMER_DBM;
        }
    }
    /*init dst buffer*/
    for(i=0;i<(DMX_TEST_PID_TOTAL_NUM+1);i++)
    {
        _arPidBuffer[i].u4BufAddr = PES_RESULT_ADDR+i*PES_RESULT_PERPID_SIZE;
        _arPidBuffer[i].u4BufSize = PES_RESULT_PERPID_SIZE;
        x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
    }
   
    /*reset the offset address to start size*/
    for(i=0;i<DMX_TS_TOTAL_INPUT_SOURCE;i++)
    {
        _arTsGoldenFile[i].Pes_Addr_Offset = _arTsGoldenFile[i].Pes_Addr_Start;
        _arTsGoldenFile[i].TS_Addr_Offset = _arTsGoldenFile[i].TS_Addr_Start;
    }
    /*for pes golden file*/
    for(i=0;i<DDI_TS_TO_FRAMER_TOTAL_NUM;i++)
    {
        DDI_TS_TO_FRAMER_GOLDEN_FILES[i].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset =
        DDI_TS_TO_FRAMER_GOLDEN_FILES[i].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    }

    /*init the record buffer*/
    for(i=0;i<FVR_GBL_COUNT;i++)
    {
        x_memset((VOID*)Record_Addr_T[i].u4_RecordAddr,0,
                Record_Addr_T[i].u4_RecordSize);
    }

    /*init the flag as false*/
    fg_pvr3_complete = FALSE;
    return; 
}

//-----------------------------------------------------------------------------
/** _CreateThread to update the pvr3
 */
//-----------------------------------------------------------------------------
static VOID _FvrThread(VOID* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
        static FVR_MSG_EMU_T rMsg;
        INT32 i4Ret;
        SIZE_T zMsgSize;
        UINT16 u2QueueIndex;
        DMX_MM_DATA_T rData;

        zMsgSize = sizeof (rMsg);
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rMsg, &zMsgSize,
            &_hFvrEmuQueue, 1, X_MSGQ_OPTION_WAIT);
        Printf("receive msg ci1.4\n");
        //move for pvr3
        if(rMsg.u1TsIdx == 4)
        {/*for remux record buffer*/
#if defined(CC_MT5890)
            _DMX_PVRPlay_UpdateWp(2, rMsg.u4Addr, rMsg.u4Size);
#else
            _DMX_PVRPlay_UpdateWp(0, rMsg.u4Addr, rMsg.u4Size);
#endif
        }
        else if((rMsg.u1TsIdx == 0) ||(rMsg.u1TsIdx == 1) )
        {/*ts_idx0 or ts_idx1 for timeshift record buffer*/
#if defined(CC_MT5890)
            _DMX_PVRPlay_UpdateWp(0, rMsg.u4Addr, rMsg.u4Size);
#else  //MT5882 ???
            _DMX_PVRPlay_UpdateWp(1, rMsg.u4Addr, rMsg.u4Size);
#endif
        }

        // unlock record buffer
        FVR_UnlockBuffer(rMsg.u1TsIdx, rMsg.u4Addr, rMsg.u4Size);
    }  
}

static  UINT32 u4end =0;
static  UINT32 u4start = 0;

static VOID _MMThread(VOID* pvArg)
{
    UNUSED(pvArg);
    while (1)
    {//every time write 188*4 byte
        static MM_MSG_T rMsg;
        INT32 iRet;
        SIZE_T zMsgSize;
        UINT16 u2QueueIndex;
        UINT32 u4wp1;
        zMsgSize = sizeof (rMsg);
        iRet = x_msg_q_receive(&u2QueueIndex, (VOID*)&rMsg, &zMsgSize,
            &_hMMQueue, 1, X_MSGQ_OPTION_WAIT);
        if(iRet == OSR_OK)
        {
            _mmCounter++;
        }
        else
        {
            Printf("receive mm msg fail!\n");
        }
        if(_mmCounter<10)
        {
            Printf("Go to move mm data(Addr=0x%08x).\r\n",rMsg.u4Addr);
        }  
        u4end =DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_END2);
        u4start = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_START2);
           u4wp1 = rMsg.u4Addr;
        if(u4wp1 > u4end)
        {
            u4wp1 = u4wp1 - (u4end-u4start);
        }
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP2,u4wp1); 
        x_thread_delay(200);
    }
}

static void FVR_GBLCallback(FVR_GBL_NOTIFY_CODE_T eCode,
                                       UINT32 u4Data, const void* pvTag)
{
    FVR_GBL_TIMENOTIFY_INFO_T *prInfo;
    FVR_MSG_EMU_T rPes;

    switch(eCode)
    {
    case FVR_GBL_NFY_TIME:
        prInfo = (FVR_GBL_TIMENOTIFY_INFO_T*)u4Data;
        LOG(3, "TsIdx(%d),recorded addr: 0x%x size: 0x%x\n",prInfo->u1TsIndex, prInfo->u4Addr, prInfo->u4Size);
        if( prInfo->u4Size > 0)
        {
         rPes.u4Addr = prInfo->u4Addr;
         rPes.u4Size = prInfo->u4Size;
         rPes.u1TsIdx = prInfo->u1TsIndex;
         if (x_msg_q_send(_hFvrEmuQueue, (VOID*)&rPes, sizeof (FVR_MSG_EMU_T),
            128) != OSR_OK)
         {
            LOG(1, "Emu Demux queue full!\n");
         }
        }
        break;

    case FVR_GBL_NFY_PKTCOUNT:
        // Don't use
        ASSERT(0);
        break;

    case FVR_GBL_NFY_OVERFLOW:
        LOG(0, "overflow...\n");
        break;

    default:
        // Unexpected
        ASSERT(0);
        break;
    }
}

//-----------------------------------------------------------------------------
/** _CreateTSHeader
 */
//-----------------------------------------------------------------------------
static void _CreateTSHeader(UINT8 pididx,UINT32 u4Addr, UINT8 *pu1Offset, BOOL fgPUSI)
{
    UINT8 *pu1Ptr;
    UINT8 u1AFLen, u1Offset, u1PUSI;
    ASSERT(pididx < DMX_TEST_PID_TOTAL_NUM);
    pu1Ptr = (UINT8*)u4Addr;
    pu1Ptr[0] = 0x47;

    u1PUSI = 0x0;
    if (fgPUSI)
    {
        u1PUSI = 0x40;
    }
    pu1Ptr[1] = u1PUSI|((PID_TABLE[pididx]&0x1F00)>>8);
    pu1Ptr[2] = PID_TABLE[pididx]&0xFF;
    pu1Ptr[3] = PID_CC_TABLE[pididx] & 0xF;

    PID_CC_TABLE[pididx]=PID_CC_TABLE[pididx]+1;

    u1AFLen = 0x0;

    pu1Ptr[3] |= 0x10;
 
    if (fgPUSI)
    {
        pu1Ptr[4 + u1AFLen] = 0x0;  // Fixme,not adaption field
        u1Offset = u1AFLen;         // PES need not ptr field
    }
    else
    {
        u1Offset = 0x0;
    }

    *pu1Offset = u1Offset;
}

//-----------------------------------------------------------------------------
/**function name: _CreateTSPacket
 *  reuturan value:true /false
 *  description:based on  pes datas to construct the ts  packets.
 */
//-----------------------------------------------------------------------------

static UINT32 _CreateTSPacket(INT8  u1pididx,DMX_TS_INPUT_SOURCE e_source_mode,UINT32 u4PesAddr, UINT32 u4PesSize)
{
    BOOL fgFirst = TRUE;
    UINT8 *pu1Ptr, u1Offset;
    UINT32 u4CpSize, u4PktCount;

    u4PktCount = 0x0;

    while (u4PesSize > 0)
    {
        pu1Ptr = (UINT8*)_arTsGoldenFile[e_source_mode].TS_Addr_Offset;
        x_memset((VOID*)pu1Ptr, 0xFF, 188);
        
        _CreateTSHeader(u1pididx,_arTsGoldenFile[e_source_mode].TS_Addr_Offset, &u1Offset, fgFirst);
        if (fgFirst)
        {
            fgFirst = FALSE;
        }

        u4CpSize = 188 - 4 - u1Offset;
        if (u4CpSize > u4PesSize)
        {
            u4CpSize = u4PesSize;
        }
        x_memcpy((VOID*)(_arTsGoldenFile[e_source_mode].TS_Addr_Offset + 4 + u1Offset), (VOID*)u4PesAddr, u4CpSize);

        _arTsGoldenFile[e_source_mode].TS_Addr_Offset += 188;
        u4PesAddr += u4CpSize;
        u4PesSize -= u4CpSize;
        u4PktCount++;
    }

    return u4PktCount;
}

//-----------------------------------------------------------------------------
/**function name: _FormatPES
 *  parameter:construct the pes datas from ts input source.
 *                  fgPicpat=true ,insert picture information ,else don't insert pic information.
 *  reuturan value:pu4PesAddr,pu4PesSize
 *  description:based on  es datas to construct the pes  packets.
 */
//-----------------------------------------------------------------------------

static BOOL _FormatPES(DMX_TS_INPUT_SOURCE e_source_mode,UINT32 u4ESAddr, UINT32 u4ESSize, UINT32 u4PktCount,
                       UINT32 *pu4PesAddr, UINT32 *pu4PesSize, BOOL fgPicpat)
{
    static UINT32 _u4StartOffset = 0;
    UINT8 *pu1Ptr;
    UINT32 i;
    UINT32 u4StartCode;
    UINT16 u2PesLen = 0;
    
    u4StartCode = _au4StartCode[random(3)];
    pu1Ptr = (UINT8*)_arTsGoldenFile[e_source_mode].Pes_Addr_Offset;
   
    for (i=0; i<_u4PesHeaderLen; i++)
    {
        pu1Ptr[i] = _au1FVRPesHeader[i];
    }
    /*construct the pes  length for every pes packet*/
    u2PesLen = _u4PesHeaderLen+u4ESSize-PES_PACKET_PREFIX_LEN;
    pu1Ptr[4]= (u2PesLen>>8)&0xFF;
    pu1Ptr[5]= (u2PesLen&0xFF);

    x_memcpy((void*)(_arTsGoldenFile[e_source_mode].Pes_Addr_Offset + _u4PesHeaderLen), 
             (void*)u4ESAddr, u4ESSize);

    *pu4PesAddr = _arTsGoldenFile[e_source_mode].Pes_Addr_Offset;
    *pu4PesSize = _u4PesHeaderLen + u4ESSize;

    if (fgPicpat)
    {
        _u4StartOffset++;
        _u4StartOffset = _u4StartOffset % 256;
        // 9 means pes start code to pes header pkt length
        pu1Ptr += (_u4PesHeaderLen + 9 + _u4StartOffset);
        pu1Ptr[0] = 0x00;
        pu1Ptr[1] = 0x00;
        pu1Ptr[2] = (UINT8)((u4StartCode >> 24) & 0xFF);
        pu1Ptr[3] = (UINT8)((u4StartCode >> 16) & 0xFF);
        pu1Ptr[4] = (UINT8)((u4StartCode >> 8) & 0xFF);
        pu1Ptr[5] = (UINT8)(u4StartCode & 0xFF);
    }

    _arTsGoldenFile[e_source_mode].Pes_Addr_Offset += (_u4PesHeaderLen + u4ESSize);

    return TRUE;
}


//-----------------------------------------------------------------------------
/**function name: _GetPididx_By_InputSource
 *  description:use this function to set the pidx to get mapped pid num.for pvr1,pvr2,pvr3 ,just 
 *  allocate one pid for them.
 */
//-----------------------------------------------------------------------------
static INT8  _AllocatePididx_By_InputSource(DMX_TS_INPUT_SOURCE e_source_mode)
{
   INT8 u1pidx=0;
   if(e_source_mode==DMX_TS_FROM_DDI)
   {
     u1pidx = random(DMX_TEST_PID_TOTAL_NUM-3);   
   }
   else if(e_source_mode==DMX_TS_FROM_PVR1)
   {
     u1pidx = (DMX_TEST_PID_TOTAL_NUM-3);
   }
   else if(e_source_mode==DMX_TS_FROM_PVR2)
   {
    u1pidx = (DMX_TEST_PID_TOTAL_NUM-2);
   }
   else if(e_source_mode==DMX_TS_FROM_PVR3)
   {
     u1pidx = (DMX_TEST_PID_TOTAL_NUM-1);
   }
   else
   {
     Printf("Set Fails.\r\n");
   }
   return u1pidx;
}

//-----------------------------------------------------------------------------
/**function name: _DmxEmuGenPES
 *  parameter:e_source_mode:construct the golden files from different ts  input source.
 *                  gPicpat=true ,insert picture information ,else don't insert pic information.
 *  reuturan value:true /false
 *  description:based on the fixed  length es datas to construct the pes/ts packets.the flow
 *  is like this:es->pes ->ts packet.for every pes packet ,allocate the same pid for it.
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuGenPES(DMX_TS_INPUT_SOURCE e_source_mode,BOOL fgPicpat)
{
    UINT32 *pu4Ptr, i;
    UINT32 u4ESAddr, u4ESLen, u4Len;
    UINT32 u4PesAddr, u4PesSize;
    UINT32 u4PktCount;
    UINT32 u4_EsSize = _arTsGoldenFile[e_source_mode].ES_Length/4;
    INT8 u1pidx = 0;
    srand(BIM_READ32(REG_RW_TIMER0_LOW));

    //------------------------------------------
    // Generate ES
    //------------------------------------------
    pu4Ptr = (UINT32*)_arTsGoldenFile[e_source_mode].Es_Addr_Start;
    for (i=0; i<u4_EsSize; i++)
    {
         pu4Ptr[i] = random(256); //generate the random datas.
    }

    //------------------------------------------
    // Generate PES and packets
    //------------------------------------------
    u4ESAddr = _arTsGoldenFile[e_source_mode].Es_Addr_Start;
    u4ESLen = _arTsGoldenFile[e_source_mode].ES_Length;
    u4PktCount = 1;     // start from 1 for fvr compare
    while (u4ESLen > 0)
    {
        if (u4ESLen < (((184 * 3) - _u4PesHeaderLen) * 2))
        {
          break;
        }
        u4Len = (184 * 3) - _u4PesHeaderLen;       // Fixme

        // Format PES packet
        if (!_FormatPES(e_source_mode,u4ESAddr, u4Len, u4PktCount, &u4PesAddr, &u4PesSize, fgPicpat))
        {
            return FALSE;
        }
        //allocate the index by ts input source.
        u1pidx=_AllocatePididx_By_InputSource(e_source_mode);
       // Printf("DMX_TS_INPUT_SOURCE=%d, pididx=%d.\r\n",e_source_mode,u1pidx);
        if(PID_FRAMER_TABLE[u1pidx] == DDI_TS_TO_FRAMER0)
        {/*this pes packet is used for frameo ,save this pes as golden files.*/
         /*copy file to the pes datas*/
           x_memcpy((void*)( DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset), 
             (void*)u4PesAddr, u4PesSize);   
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset +=u4PesSize; 
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Len +=u4PesSize;
        }
        else if(PID_FRAMER_TABLE[u1pidx] == DDI_TS_TO_FRAMER1)
        {/*this pes packet is used for frame1 ,save this pes as golden files.*/
           /*copy file to the pes datas*/
           x_memcpy((void*)( DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset), 
             (void*)u4PesAddr, u4PesSize);   
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset +=u4PesSize; 
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Len +=u4PesSize;
        }
        u4PktCount += _CreateTSPacket(u1pidx,e_source_mode,u4PesAddr, u4PesSize);

        u4ESLen -= u4Len;
        u4ESAddr += u4Len;
        _arTsGoldenFile[e_source_mode].ES_Size+= u4Len;
    }
    _arTsGoldenFile[e_source_mode].TS_Pktcount = u4PktCount;
    _arTsGoldenFile[e_source_mode].ES_Size -= ((184 * 3) - _u4PesHeaderLen);   // Last PES won't be golden, due to no next PUSI = 1

    return TRUE;
}

//----------------------------------------------------------------
 /*   function description: generate the remux data.
 *     parameter:fgon:true,the ts packet contains timestamp.
 */
//-----------------------------------------------------------------


static BOOL _DmxEmuGenRemuxData(BOOL fgon,UINT32 pattern)
{
      UINT8 u1pidx=0;
      
      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
      /*generate the pes datas for ddi_ts*/
      _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
      /*generate the pes datas for IP*/
      _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();
       /*customize the IP path    */
       _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE);
      /*set mini-pvr*/
      _DmxEmuSetPVRPlay(IP_PVR,FALSE);
    
      /*set reocrd    pid for u1_tsidx*/
      u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
      if(!_DMX_EMU_SetRecordPid(2,u1pidx,PID_TABLE[u1pidx],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx2 error.\r\n");
        return -1;
      }
      /*for pvr2 ,if you need to do record ,you just  need set bit[9] to 1, dbm->Pid filter*/
      _DMX_Set_PB_STEER_BIT_9(1,TRUE);
    
       /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,0,PID_TABLE[0],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
      /*set record pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,1,PID_TABLE[1],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }
    
      /*set remux record global region.*/
      _SetPVRGBLEx2(4,fgon,pattern);
      _FVR_Start(4);
      /*start ddi_ts move */
        /*get src address and src size*/
      UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
      UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
       /*enable DDI engine*/
      if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
      {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
      }
      /*start pvr engine*/
       /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(IP_PVR);
      
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status
    
                // Complete
                fgRet = TRUE;
                //Printf("     Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
       x_thread_delay(250);

     
       _DmxEmuStopPVRPlay(IP_PVR);
       _DMX_EmuPVRPlay_FreeBuf(IP_PVR);
       /*get ciplus1.4 record buffer.*/
       #if defined(CC_MT5890)
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start= FVR_GBL_ARY_W(4, 0);
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset= FVR_GBL_ARY_W(4,3);
       #else 
       _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Start= FVR_GBL_ARY_W(4, 0);
       _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Offset= FVR_GBL_ARY_W(4,3);
       #endif
       FVR_Stop(4);
  
       FVR_FreePid(u1pidx);
       FVR_FreePid(0);
       FVR_FreePid(1);
       FVR_FreeGBL(4);

       HalFlushInvalidateDCache();
       return TRUE;
        
}
//----------------------------------------------------------------
 /*   function description: generate MM chunk data which can't contain startcode
 */
//-----------------------------------------------------------------
static BOOL _DmxEmuGenMMData(UINT32 u4Src_Addr, UINT32 u4_size)
{
   _GenerateRandomPattern(u4Src_Addr, u4_size);

   return TRUE;
}
//----------------------------------------------------------------
 /*   function description: generate the remux data for pcr 
 *     use:load the file to memory as the ts input source .
 */
//-----------------------------------------------------------------
static BOOL _DmxEmuGenRemuxDataEx(UINT32 u4_Addr_Start,UINT32 u4_size,
                                             BOOL fgon,UINT32 pattern)
{

      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
     
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();

       /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,0,PCR_PID0,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
      /*set record pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,1,PCR_PID1,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }
    
      /*set remux record global region.*/
      _SetPVRGBLEx4(4,fgon,pattern,0x3);
      _FVR_Start(4);
      /*start ddi_ts move */
        /*get src address and src size*/
      _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start = u4_Addr_Start;
      u4_size=_DMX_Align(u4_size, DDI_BUF_ALIGNMENT); 
      _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset= (u4_Addr_Start+u4_size);
      UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
      UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
       /*enable DDI engine*/
      if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
      {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
      }
      
       x_thread_delay(250);
       HalFlushInvalidateDCache();

#if defined(CC_MT5890)
       /*get ciplus1.4 record buffer.*/
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start= FVR_GBL_ARY_W(4, 0);
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset= FVR_GBL_ARY_W(4,3);
#else  //MT5882
       _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Start= FVR_GBL_ARY_W(4, 0);
       _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Offset= FVR_GBL_ARY_W(4,3);
#endif
       FVR_Stop(4);
  
       FVR_FreePid(0);
       FVR_FreePid(1);
       FVR_FreeGBL(4);
      
       return TRUE;
        
}


static BOOL _DmxEmuSetMMPid(UINT8 u1TsIdx,UINT8 u1pidx,UINT32 u4DataSize)
{/*for mm,set output mode.*/
  UINT8 i;
  UINT32 u4Flags, u4Reg;
  DMX_PID_T rPid;
  UINT32 u4Ctrl;

  // Setup PID
  x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
  rPid.fgEnable = TRUE;
  rPid.fgAllocateBuffer = FALSE;
  rPid.fgPrimary = TRUE;
  rPid.u1TsIndex = u1TsIdx;
  rPid.u1DeviceId = 0;
  rPid.u1KeyIndex = 0;
  rPid.u1SteerMode = DMX_STEER_TO_FTUP;
  rPid.u2Pid = 0xFF;
  rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
  rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
  rPid.ePcrMode = DMX_PCR_MODE_NONE;
  rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;
  rPid.eDescMode = DMX_DESC_MODE_NONE;
  rPid.pfnNotify = NULL;
  rPid.pvNotifyTag = NULL;
  rPid.pfnScramble = NULL;
  rPid.pvScrambleTag = NULL;
  rPid.u1ChannelId = 0;
  u4Flags = DMX_PID_FLAG_ALL;
  if (!_DMX_SetPid(u1pidx, u4Flags, &rPid, FALSE))
  {
      return FALSE;
  }

  PID_S_W(u1pidx, 1) = u4DataSize;
  PID_S_W(u1pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger
  
}

static BOOL _DmxEmuSetMMPid1(UINT8 u1TsIdx,UINT8 u1pidx,UINT32 u4DataSize)
{/*for mm,set output mode.*/
  UINT8 i;
  UINT32 u4Flags, u4Reg;
  DMX_PID_T rPid;
  UINT32 u4Ctrl;

  // Setup PID
  x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
  rPid.fgEnable = TRUE;
  rPid.fgAllocateBuffer = FALSE;
  rPid.fgPrimary = TRUE;
  rPid.u1TsIndex = u1TsIdx;
  rPid.u1DeviceId = 0;
  rPid.u1KeyIndex = MM_PLAY_KEY;
  rPid.u1SteerMode = DMX_STEER_TO_FTUP;
  rPid.u2Pid = 0xFF;
  rPid.u4BufAddr = _arPidBuffer[u1pidx].u4BufAddr;
  rPid.u4BufSize = _arPidBuffer[u1pidx].u4BufSize;
  rPid.ePcrMode = DMX_PCR_MODE_NONE;
  rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;
  rPid.eDescMode = DMX_DESC_MODE_NONE;
  rPid.pfnNotify = NULL;
  rPid.pvNotifyTag = NULL;
  rPid.pfnScramble = NULL;
  rPid.pvScrambleTag = NULL;
  rPid.u1ChannelId = 0;
  u4Flags = DMX_PID_FLAG_ALL;
  if (!_DMX_SetPid(u1pidx, u4Flags, &rPid, FALSE))
  {
      return FALSE;
  }

  PID_S_W(u1pidx, 1) = u4DataSize;
  PID_S_W(u1pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

  
  //after set descmode none,dmx_setpid would't set descrambling scheme.
  //so we should set this after dmx_setpid
  _DMX_Lock();
  u4Ctrl = PID_INDEX_TABLE(u1pidx);
  u4Ctrl |= 0x400; //set ca descramble enable
  PID_INDEX_TABLE(u1pidx) = u4Ctrl;
  _DMX_Unlock();
  
}


static BOOL _DMX_EMU_CustMMPath()
{/*pb2->DBM->bypass pid filter*/
  _DMX_EMU_CustPvrPath(IP_PVR,2,TRUE);
}
//--------------------------------------------------------------
//------------------cli command region----------------------------
//--------------------------------------------------------------
//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_MultiStream_CaOut_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE;
      UINT32 SrcpesAddr,DstpesAddr,u4size;
      UINT32 u4SrcDiff,u4DstDiff,u4Reg;
      BOOL  fg_bypass_pidfilter = FALSE;
      UINT32 u4Ctrl;
      if(i4Argc != 2)
      {
        Printf("%s,Test command:multica bypass pidfilter[0|1].\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        fg_bypass_pidfilter = StrToInt(aszArgv[1]);
      }

      /*generate the remux datas*/
      _DmxEmuGenRemuxData(FALSE,0x40414243);//0x40414243

      /*set hardware path*/
      //Set DBM bypass mode
      if(fg_bypass_pidfilter)
      {
        DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, DMXCMD_READ32(DMX_REG_DBM_CONTROL) | (1 << 29));
        _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
      }
      else
      { /*init hardaware path*/
          DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
        _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
        /*enable remux pid filter*/
          DmxSetLocalTSEnable(TRUE,FALSE);
        /*set ts_idx3 for multi-stream playback settings*/
         DmxSetTSStream(3,MULTI_STREAM);
         DMXSetLocalTSID(3,0x40414243,0x7); /*ts3,ts2,ts1,ts0*/
      }
          // Set Full pull mode
      u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
      u4Reg |= 0xC0000000;
      DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

#if 1
    /*for FPGA, you need config it ts-out two-bit.*/
#if defined(CC_MT5890)
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
#else  //MT5882
    if (!DMX_TsOut_SetDataWidthEx(0, DMX_TSOUT_DATAWIDTH_1))
#endif
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    /*after set the framer mode ,then do errohandle [very important,must add]*/
    if(fg_bypass_pidfilter)
    {
      // Disable framer packet error handling
      _DMX_SetFramerPacketErrorHandling(3, FALSE, 0);
    }
    else
    {
       //the framer error talbe is very useful,if the ts matched with its condition,it will drop.
       _DMX_SetFramerPacketErrorHandling(3, FALSE, 0);
    }
#endif
    if(fg_bypass_pidfilter == FALSE)
    {/*set pid*/

        _DMX_EMU_SetPlayPidEx(3,0,0,PID_TABLE[0],DMX_PID_TYPE_PES);

        _DMX_EMU_SetPlayPidEx(3,1,1,PID_TABLE[1],DMX_PID_TYPE_PES);

        u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
        _DMX_EMU_SetPlayPidEx(3,2,u1pidx,PID_TABLE[u1pidx],DMX_PID_TYPE_PES);

    }
       /*set mini-pvr buffer and whether timestamp is used.*/
      if(_DmxEmuSetPVRPlayExt(CI_REMUX_PVR,FALSE,FALSE) == FALSE)
      {
        Printf("%s ,set pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
      }

      /*set ca output*/
#if defined(CC_MT5890)
        UINT32 SrcSize = _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset - \
                         _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start+0x100;
#else
        UINT32 SrcSize = _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Offset - \
                         _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Start+0x100;
#endif
         UINT32 DstAddr = _DMX_Align(CACHE(0x2900000),DMX_CA_BUFFER_ALIGNMENT);
        //UINT32 DstAddr = _DMX_Align((0x2900000),DMX_CA_BUFFER_ALIGNMENT);
        UINT32 DstEnd =_DMX_Align(DstAddr + SrcSize + 1, DMX_CA_BUFFER_ALIGNMENT);
        x_memset((VOID*)DstAddr,0,(SrcSize+1));
        // Set CA output buffer
        if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, DstAddr, DstEnd,
                                DstEnd - DstAddr))
        {
           Printf("set ca output error.\r\n");
           return -1;
        }
        HalFlushInvalidateDCache();
      /*start  pvr engine*/
        /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(CI_REMUX_PVR);
      /*wait for finish*/
#if defined(CC_MT5890)
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
           HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
#else  //MT5882
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
           HAL_Delay_us(10); //because data is very large ,dely hardware.
        }

#endif
     /*compare golden pes files*/
     /*ts_idx=0*/

#if defined(MT_5890)
    SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start;
    u4size=(_arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset - \
            _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start);
#else
    SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Start;
    u4size=(_arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Offset - \
            _arTsGoldenFile[DMX_TS_FROM_PVR1].TS_Addr_Start);
#endif

    if (!_MemCmp(SrcpesAddr, DstAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);   
        Printf("Dismatch!.\r\n");

    }
   if(b_match0)
   {
     Printf("Match!.\r\n");
   }
    _DmxEmuStopPVRPlay(CI_REMUX_PVR);
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);

      
    return TRUE;
        
}
//----------------------------------------------------------------
 /*  description:test  pvr playback timestamp handle
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_TimeStamp_CaOut_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL fg_contain_timestamp = FALSE, fg_timestamp_on =FALSE;
      BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE;
      UINT32 SrcpesAddr,DstpesAddr,u4size;
      UINT32 u4SrcDiff,u4DstDiff,u4Reg;
      if(i4Argc != 3)
      {
        Printf("%s,Test command:multitime [0|1],[0|1].\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        fg_contain_timestamp = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
        fg_timestamp_on = StrToInt(aszArgv[2]);
      }

      /*generate the remux datas*/
      _DmxEmuGenRemuxData(fg_contain_timestamp,0x47474747);
      /*set hardware path*/
      //Set DBM bypass mode
         if (!_DMX_SetBypassMode(3, 188, FALSE, TRUE))
      {
        Printf("Failed to set bypass mode\n");
        return FALSE;
      }
      _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 

          // Set Full pull mode
      u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
      u4Reg |= 0xC0000000;
      DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

#if 1
    /*for FPGA, you need config it ts-out two-bit.*/
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }    
#endif
      
       /*set mini-pvr buffer and whether timestamp is used.*/
      if(_DmxEmuSetPVRPlayExt(CI_REMUX_PVR,fg_timestamp_on,fg_contain_timestamp) == FALSE)
      {
        Printf("%s ,set pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
      }

      /*set ca output*/
        UINT32 SrcSize = _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset - \
                         _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start+0x100;
        UINT32 DstAddr = _DMX_Align(0x2900000,DMX_CA_BUFFER_ALIGNMENT);
        UINT32 DstEnd =_DMX_Align(DstAddr + SrcSize + 1, DMX_CA_BUFFER_ALIGNMENT);
        x_memset((VOID*)DstAddr,0,(SrcSize+1));
        // Set CA output buffer
        if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, DstAddr, DstEnd,
                                DstEnd - DstAddr))
        {
           Printf("set ca output error.\r\n");
           return -1;
        }
      /*start  pvr engine*/
        /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(CI_REMUX_PVR);
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
     /*compare golden pes files*/
     /*ts_idx=0*/

    SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start;
    u4size=(_arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset - \
            _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start);

    if (!_MemCmp(SrcpesAddr, DstAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);   
        Printf("Dismatch!.\r\n");

    }
   if(b_match0)
   {
     Printf("Match!.\r\n");
   }
    _DmxEmuStopPVRPlay(CI_REMUX_PVR);
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);

      
    return TRUE;
        
}
//----------------------------------------------------------------
 /*  description:construct two pcr_pid data ,and then  send it by pcr
 *    test path:ddi_ts(framer0),ddi_ts(framer1),->Remux Record Buffer.
 *                  playback by ts_idx3.
 *   the command use :firstly ,load the binary to memory[0x3400000,pcr_d86],
 *   [34ca5db,pcr_987]then
 *   input this command.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_MultiStream_PCR_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT32 u4Reg;
      if(i4Argc != 1)
      {
        Printf("%s,Test command:pcr.\r\n",aszArgv[0]);
        return    -1;
      }

      /*generate the remux datas*/
      _DmxEmuGenRemuxDataEx(_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start,
                            PCR_TS_SIZE,
                            FALSE,0x40414243);
      /*set hardware path*/
      DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
      _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
      /*enable remux pid filter*/
      DmxSetLocalTSEnable(TRUE,FALSE);
      /*set ts_idx3 for multi-stream playback settings*/
      DmxSetTSStream(3,MULTI_STREAM);
      DMXSetLocalTSID(3,0x40414243,0xE);

          // Set Full pull mode
      u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
      u4Reg |= 0xC0000000;
      DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

#if 1
    /*for FPGA, you need config it ts-out two-bit.*/
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_2))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_2))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    /*after set the framer mode ,then do errohandle [very important,must add]*/
    //the framer error talbe is very useful,if the ts matched with its condition,it will drop.
    _DMX_SetFramerPacketErrorHandling(3, FALSE, 0);
    
#endif
   /*set  playback pid*/

     _DMX_EMU_SetPlayPcrPid(3,0,0,PCR_PID0,DMX_PID_TYPE_PES);

     _DMX_EMU_SetPlayPcrPid(3,1,1,PCR_PID1,DMX_PID_TYPE_PES);

       /*set mini-pvr buffer and whether timestamp is used.*/
      if(_DmxEmuSetPVRPlayExt(CI_REMUX_PVR,FALSE,FALSE) == FALSE)
      {
        Printf("%s ,set pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
      }

      /*start  pvr engine*/
        /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(CI_REMUX_PVR);
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }


    _DmxEmuStopPVRPlay(CI_REMUX_PVR);
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);

      
    return TRUE;
        
}

//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_MultiStream_Playback_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL fg_ts_idx0_bypass=FALSE,fg_ts_idx1_bypass=FALSE,fg_ts_idx2_bypass=FALSE;
      BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE;
      UINT32 SrcpesAddr,DstpesAddr,u4size;
      UINT32 u4SrcDiff,u4DstDiff;
      UINT32 u4Reg,i=0;
      if(i4Argc != 4)
      {
        Printf("%s,Test command:multiplay ts_idx0_bypass[0|1],     \
                 ts_idx1_bypass[0|1],ts_idx2_bypass[0|1].\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        fg_ts_idx0_bypass = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
        fg_ts_idx1_bypass = StrToInt(aszArgv[2]);
      }

      if(i4Argc > 3)
      {
        fg_ts_idx2_bypass = StrToInt(aszArgv[3]);
      }

        /*generate the remux datas*/
      _DmxEmuGenRemuxData(FALSE,0x40414243);
       /*set hardware path*/
       /*init hardaware path*/
       DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
       _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
        /*enable remux pid filter*/
       DmxSetLocalTSEnable(TRUE,FALSE);
        /*set ts_idx3 for multi-stream playback settings*/
       DmxSetTSStream(3,MULTI_STREAM);
       DMXSetLocalTSID(3,0x40414243,0x7); /*ts3,ts2,ts1,ts0*/

      // Set Full pull mode
      u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
      u4Reg |= 0xC0000000;
      DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

        /*init dst buffer*/
      for(i=0;i<DMX_TEST_PID_TOTAL_NUM;i++)
      {
        x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
      }
   
#if 1
    /*for FPGA, you need config it ts-out two-bit.*/
#if defined(CC_MT5890)
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
#else  //MT5882
    if (!DMX_TsOut_SetDataWidthEx(0, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
#endif
#endif
    /*set multi-stream playback pid */
      if(!fg_ts_idx0_bypass)
      {
        _DMX_EMU_SetPlayPidEx1(3,0,0,PID_TABLE[0],DMX_PID_TYPE_PES);
      }
      
      if(!fg_ts_idx1_bypass)
      {
        _DMX_EMU_SetPlayPidEx1(3,1,1,PID_TABLE[1],DMX_PID_TYPE_PES);
      }
      
      if(!fg_ts_idx2_bypass)
      {
        u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
        _DMX_EMU_SetPlayPidEx1(3,2,u1pidx,PID_TABLE[u1pidx],DMX_PID_TYPE_PES);
      }
      
       /*set mini-pvr buffer and whether timestamp is used.*/
      if(_DmxEmuSetPVRPlayExt(CI_REMUX_PVR,FALSE,FALSE) == FALSE)
      {
        Printf("%s ,set pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
      }
      
      /*start  pvr engine*/
        /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(CI_REMUX_PVR);
      /*wait for finish*/
#if defined(CC_MT5890)
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
#else  //MT5882
        BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
#endif
     /*compare golden pes files*/
     /*ts_idx=0*/

    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[0].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    if(fg_ts_idx0_bypass == TRUE && b_match0==FALSE)
    {
      b_match0 = TRUE;
    }
   /*ts_idx=1*/
   SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
   DstpesAddr=_arPidBuffer[1].u4BufAddr;
   u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match1 = FALSE;
        Printf("ts_idx1,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    if(fg_ts_idx1_bypass == TRUE && b_match1==FALSE)
    {
      b_match1 = TRUE;
    }
    /*IP*/
   SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Start;
   DstpesAddr=_arPidBuffer[u1pidx].u4BufAddr;
   u4size=(_arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Offset- \
           _arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Start);

   
    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match2 = FALSE;
        Printf("ts_idx2,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }


    if(fg_ts_idx2_bypass == TRUE && b_match2==FALSE)
    {
      b_match2 = TRUE;
    }
    
    if(b_match1&&b_match0&&b_match2)
    {
      Printf("Match!\r\n");
    }
    else
    {
      Printf("DisMatch!\r\n");
    }

    /*free source*/
    _DmxEmuStopPVRPlay(CI_REMUX_PVR);
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);

    _DMX_EMU_FreePlaypid(u1pidx);
    _DMX_EMU_FreePlaypid(0);
    _DMX_EMU_FreePlaypid(1);

          
    return TRUE;
        
}

//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *     test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 *     make sure that:the local ts id is unique for every ts_idx which need be remuxed.
 *    test case:1, ts0,ts1->remux->ts3-> 2, timeshift  3 .mm playback by pvr2.
 */
//-----------------------------------------------------------------
static INT32 _DMXCIPLUS_Multi_Play_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
    UINT8 u1pidx=0;
    UINT32 SrcAddr,SrcSize;
    BOOL b_match0=TRUE,b_match1=TRUE,fg_time_on=TRUE;
    UINT32 SrcpesAddr,DstpesAddr,u4size;
    UINT32 u4SrcDiff,u4DstDiff;
    UINT32 u4Reg,i=0;
    MM_MSG_T  rMsg;
    
    if(i4Argc != 2)
    {
        Printf("%s,Test command:play timestamp[0|1]\n",aszArgv[0]);
        return    -1;
    }
    fg_time_on = StrToInt(aszArgv[1]);

    /*Init the golden file */
    _DMX_EMU_TS_Parameter_Init();
    /*init dst buffer*/
    for(i=0;i<DMX_TEST_PID_TOTAL_NUM;i++)
    {
        x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
    }
    /*creat the thread for update the pvr3 wp*/
    // Create fvr queues
    if(fg_init_first == FALSE)
    {
        if (x_msg_q_create(&_hFvrEmuQueue, "EmuFVR",
                sizeof (FVR_MSG_EMU_T), 1024) != OSR_OK)
        {
            Printf("Fail to create picture queue!\n");
            return FALSE;
        }

        if (x_msg_q_create(&_hMMQueue, "MM",
            sizeof(MM_MSG_T), 1024) != OSR_OK)
        {
            Printf("Fail to create MM queue!\n");
            return FALSE;
        }
        // Create fvr  thread
        if (x_thread_create(&_hFvrEmuThread, "_hFvrEmuThread",
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _FvrThread, 0,
            NULL) != OSR_OK)
        {
            Printf("Fail to create DMX thread!\n");
            return FALSE;
        }

        // Create  MM  thread 
        if (x_thread_create(&_hMMThread, "MMThread",
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _MMThread, 0,
            NULL) != OSR_OK)
        {
            Printf("Fail to create MM thread!\n");
            return FALSE;
        }
        fg_init_first = TRUE;
    }
    _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE); //for tuner1 and tuner2.
    /*because pvr2 just move 188 byte unit .make sure the size is aligne 188*/
    _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length = _DMX_Align(_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length,
                                                      PVR_MM_ALIGN_SIZE);
    _DmxEmuGenMMData(_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start,
                         _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length); // create mm chunk data 
    /*init hardaware path*/
    _DMX_EMU_CIPLUS_Init();

    /*Set  Full pull mode*/
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
    /*set DDI to demuxrate*/
       
    /*part1:for multi-stream record settings*/
    /*1,set record pid for ts_idx0 ,ts_idx1*/
    if(!_DMX_EMU_SetRecordPidExt(0,0,PID_TABLE[0],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
    }
    if(!_DMX_EMU_SetRecordPidExt(1,1,PID_TABLE[1],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
    }
    /*2,set ciplus1.4 record region for micro-code use and start record firstly*/
    #ifdef CC_DMX_FPGA_TEST
    _SetPVRGBLEx5(4,fg_time_on,0x47474647,0x3,500);
    #else
    _SetPVRGBLEx5(4,fg_time_on,0x47474647,0x3,200); //for IC,set 20ms/isr
    #endif
    /*3, set timeshift record buffer for ts_idx3*/
    #ifdef CC_DMX_FPGA_TEST
    _SetPVRGBLEx5(0,fg_time_on,0x47474647,0x3,500);
    #else
    _SetPVRGBLEx5(0,fg_time_on,0x47474647,0x3,200);
    #endif
    _FVR_Start(0);
    _FVR_Start(4);
    /*part2:for multi-stream playback settings*/
    /*1,enable ts_idx3 for support */
    DmxSetLocalTSEnable(TRUE,TRUE); //enable multi-stream playback/record.
    DmxSetTSStream(3,MULTI_STREAM);  //set mulit-stream mode for ts_idx3.
    DMXSetLocalTSID(3,0x47474647,0x3); //set local_ts_id valid.

    Printf("Use pid fitler.\r\n");
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_4,0);
      
    /*2, set the connection between pvr3 and ts_idx3*/
    _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
#if defined(CC_MT5890)
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
#else  //MT5882
    if (!DMX_TsOut_SetDataWidthEx(0, DMX_TSOUT_DATAWIDTH_1))
#endif
    {
        Printf("Failed to set data width!\n");
        return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
        Printf("Failed to set data width!\n");
        return -1;
    }
        
#if defined(CC_MT5890)
    /*3, set the connecton between pvr1 and ts_idx4 for timeshift*/
    _DMX_EMU_CustPvrPath(TIMESHIFT_PVR,4,TRUE); 
#else
    /*3, set the connecton between pvr1 and ts_idx3 for timeshift*/
    _DMX_EMU_CustPvrPath(TIMESHIFT_PVR,2,TRUE); /*PB2_TO_DBM->TS_IDX3, pid filter default.*/
#endif
    /*set record and playback pid for this one*/
    /*record :choose which pid can pass  ts_idx3 */
    _DMX_EMU_SetMultiRecordPid(3,0,2,PID_TABLE[0],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES);

#if defined(CC_MT5890)
    /*timeshift :set the playback for ts_idx4.*/
    _DMX_EMU_SetPlayPidEx1(4,0,2,PID_TABLE[0],DMX_PID_TYPE_PES);
#else
    /*timeshift :set the playback for ts_idx2.*/
    _DMX_EMU_SetPlayPidEx1(2,0,2,PID_TABLE[0],DMX_PID_TYPE_PES);
#endif
        
    /*4, set multi-stream playback pid,use DMX_PID_TYPE_TS_RAW to debug it.*/
    _DMX_EMU_SetPlayPidEx1(3,0,0,PID_TABLE[0],DMX_PID_TYPE_PES);
    
    _DMX_EMU_SetPlayPidEx1(3,1,1,PID_TABLE[1],DMX_PID_TYPE_PES);
    /*5, set mini-pvr to the start buffer.*/ 
    if(_DmxEmuSetPVRPlayExt1(CI_REMUX_PVR,fg_time_on,fg_time_on) == FALSE)
    {
        Printf("%s ,set remux pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }

    /*6,set mini-pvr fro timeshift*/
    if(_DmxEmuSetPVRPlayExt1(TIMESHIFT_PVR,fg_time_on,fg_time_on) == FALSE)
    {
        Printf("%s ,set timeshift pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }

#if defined(CC_MT5890)
    /*part3:set for MM playback*/
    _DMX_EMU_CustMMPath();  //init mm path,bypass pid filter
    /*set mini-pvr*/
    if(_DmxEmuSetPVRPlayExt1(MM_PVR,FALSE,FALSE) == FALSE)
    {
        Printf("%s ,set mm pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }
    /*set playback pid and set bypass pid */ 
    _DmxEmuSetMMPid(2,DMX_TEST_PID_TOTAL_NUM,_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_3, (DMX_TEST_PID_TOTAL_NUM<< 16) | 0x20);
    _DMX_SetFramerPacketErrorHandling(2, FALSE, 0);
#else
    /*timeshift :set the playback for ts_idx2. Use Pidx 2 for bypass TS index2*/
    //DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_3, (2<< 16) | 0x20);
#endif

    /*part4: start  engine*/
    /*1, enable the pvr3 playback engine*/
    _DmxEmuStartPVRPlay(CI_REMUX_PVR);

    /*2, enable the pvr playback engine*/
    _DmxEmuStartPVRPlay(TIMESHIFT_PVR);

#if defined(CC_MT5890)
    /*3, enable the mm playback engine*/
    _DmxEmuStartPVRPlay(MM_PVR);
#endif
    /*4 ,enable dma engine ,step by step*/

    HalFlushInvalidateDCache();
    /*get src address and src size*/
    SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
    SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
    /*enable DDI engine*/
    if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
    {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
    }

#if defined(CC_MT5890)    
    /*move mm data */
    UINT32 u4_move_size = PVR_MOVE_CHUNK_DATA_UNIT_SIZE;
    rMsg.u4Addr =  DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_START2);
    for(i=0 ;i<=_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;i+=PVR_MOVE_CHUNK_DATA_UNIT_SIZE)
    {
        if((i+PVR_MOVE_CHUNK_DATA_UNIT_SIZE)>_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length)
        {
            u4_move_size = _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length-i;
        }
        else
        {
            u4_move_size = PVR_MOVE_CHUNK_DATA_UNIT_SIZE;
        }
        rMsg.u4Addr = rMsg.u4Addr+u4_move_size;
        rMsg.u4Size = u4_move_size;
        if (x_msg_q_send(_hMMQueue,(VOID*)&rMsg, sizeof (MM_MSG_T),
                128) != OSR_OK)
        {
            Printf("Send message error.\r\n");
        }
        else
            LOG(2, "send mm msg, ci1.4\n");

    }
#endif
    i=0;
    while(i<50)
    { 
        x_thread_delay(150); //delay 150ms
        i++;
    }

    Printf("Data Transfer is ok ,input <stop> command  to compare the datas.\r\n ");
    _mmCounter = 0;
    /*compare golden pes files*/  
    return TRUE;    
}

static INT32 _DMXCIPLUS_Multi_Stop_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
    UINT32 SrcpesAddr,DstpesAddr,u4size,u4SrcDiff,u4DstDiff;
    BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE,b_match3=TRUE;
    if(i4Argc != 1)
    {
        Printf("%s,Test command:stop",aszArgv[0]);
        return    -1;
    }

    HalFlushInvalidateDCache();
    /*if you check play finish ,compare with golden files*/
    /*ts_idx=0*/

    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[0].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    /*ts_idx=1*/
    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[1].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match1 = FALSE;
        Printf("ts_idx1,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    /*for ts_idx0 ,do timeshift*/
    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[2].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match2 = FALSE;
        Printf("ts_idx4,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }
#if defined(CC_MT5890)    
    /*for  MM playback from pvr2*/
    SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start;
    DstpesAddr= _arPidBuffer[DMX_TEST_PID_TOTAL_NUM].u4BufAddr;
    u4size=_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match3 = FALSE;
        Printf("ts_idx2,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }
#endif

    if(b_match0 && b_match1&& b_match2&&b_match3)
    {
        Printf("Match!\r\n");
    }
    else
    {
        Printf("Dismatch!\r\n");
    }

    fg_mm_move_stop = TRUE; //stop move mm datas.
    /*for record/playback :stop ,then free pid ,then free record buffer.*/

    FVR_Stop(0); //stop timeshift buffer

    FVR_Stop(4); //stop remux buffer
    _DmxEmuStopPVRPlay(CI_REMUX_PVR); //stop remux pvr playback

    _DmxEmuStopPVRPlay(TIMESHIFT_PVR);

#if defined(CC_MT5890)
    _DmxEmuStopPVRPlay(MM_PVR);
#endif
    /*free playback pid */
    _DMX_EMU_FreePlaypid(0);
    _DMX_EMU_FreePlaypid(1); /*for multi-stream playback*/
    _DMX_EMU_FreePlaypid(2) ; /*for timeshift playbakc pid */
    _DMX_EMU_FreePlaypid(DMX_TEST_PID_TOTAL_NUM); /*free pid for mm playback.*/
    /*for ci+1.4 remux record pid*/
    FVR_FreePid(0);
    FVR_FreePid(1); 
    FVR_FreePid(2); 
    /*free playback or record buffer*/
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);

    _DMX_EmuPVRPlay_FreeBuf(TIMESHIFT_PVR);

#if defined(CC_MT5890)
    _DMX_EmuPVRPlay_FreeBuf(MM_PVR);
#endif

    FVR_FreeGBL(0);
    FVR_FreeGBL(4);
  
}

//----------------------------------------------------------------
 /*   description:init des parameter
 */
//-----------------------------------------------------------------
/******************************************************************************
* TS index 0 and 1 make use of key index 0 ~  7.
* TS index 2 and 3 make use of key index 8 ~ 15.
* This function takes TS index into account when setting keys.
******************************************************************************/
static BOOL _SetKeys(UINT8 u1TsIndex, DES_PARAM_T* prParam)
{
    UINT32 i, j;
    UINT32 au4EvenKey[6], au4OddKey[6];
    UINT8* p;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    for (i = 0; i < MAX_KEY_NO; i++)
    {
        if (prParam->arEnPid[i].fgValid)
        {
            for (j = 0; j < 6; j++)
            {
                p = prParam->arEnPid[i].au1EvenKey + (j * 4);
                au4EvenKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

                p = prParam->arEnPid[i].au1OddKey + (j * 4);
                au4OddKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            }
        }
        else
        {
            // Reset keys
            for (j = 0; j < 6; j++)
            {
                au4EvenKey[j] = 0;
                au4OddKey[j] = 0;
            }
        }

        // Set keys
        VERIFY(_DMX_SetDesKey(i, prParam->fg3Des, TRUE, au4EvenKey));
        VERIFY(_DMX_SetDesKey(i, prParam->fg3Des, FALSE, au4OddKey));
    }

    return TRUE;
}

static BOOL _DMX_EncryptDesPacket(UINT8* pu1Buf, DES_PARAM_T* prParam)
{
    DES3_KS r3DesKs;
    DES_KS rDesKs;
    UINT8* pu1Skip4 = pu1Buf + 4;
    UINT32 u4Offset, u4KeyLen;
    INT32 i4WhichKey = -1;
    UINT16 u2Pid = 0;
    UINT8 au1Key[24], au1Text[8], au1InitVector[8];
    UINT8 u1AdaptField = 0, u1PayloadOffset = 0;
    BOOL fgEven = TRUE;
    UINT8 au1Backup[8];

    ASSERT(pu1Buf != NULL);
    ASSERT(prParam != NULL);

    u2Pid = _GetPidEx(pu1Buf);

    if ((i4WhichKey = _MatchKeyEx(u2Pid, prParam)) >= 0)
    {
        u1AdaptField = (pu1Buf[3] >> 4) & 0x3;
        if (0 == (u1AdaptField & 0x1))  // 00 and 10 => skip
        {
            goto _exit;
        }
        else if (u1AdaptField == 0x3)   // adaptation + payload
        {
            u1PayloadOffset = 1 + pu1Buf[4];
        }
        else                            // 0x1 payload only
        {
            u1PayloadOffset = 0;
        }

        switch (prParam->eKeySelect)
        {
        case KEY_SELECT_EVEN:
            fgEven = TRUE;
            break;

        case KEY_SELECT_ODD:
            fgEven = FALSE;
            break;

        case KEY_SELECT_RANDOM:
            fgEven = ((rand() & 1) == 0) ? TRUE : FALSE;
            break;

        default:
            ASSERT(0);
            break;
        }

        u4KeyLen = (prParam->fg3Des ? 24 : 8);

        if (fgEven) // Even
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1EvenKey, u4KeyLen);
            pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
        }
        else        // Odd
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1OddKey, u4KeyLen);
            pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
        }

        if (prParam->eFeedbackMode == DMX_CA_FB_CTR)
        {
            return _DesEncryptCTREx((UINT32)(pu1Skip4 + u1PayloadOffset),
                                  184 - u1PayloadOffset, au1Key, prParam);
        }

        if (prParam->fg3Des)
        {
            des3key(r3DesKs, au1Key, 0);
        }
        else
        {
            deskey(rDesKs, au1Key, 0);
        }

        // make klocwork happy
        x_memset(au1Backup, 0, 8);
        x_memset(au1InitVector, 0, 8);

        if (prParam->eFeedbackMode != DMX_CA_FB_ECB)
        {
            x_memcpy(au1InitVector, prParam->au1InitVector, 8);
        }

        for (u4Offset = u1PayloadOffset; u4Offset <= (TS_PACKET_SIZE - 12);
                u4Offset += 8)
        {
            x_memcpy(au1Backup, pu1Skip4 + u4Offset, 8);   // Get plain text
            x_memcpy(au1Text, pu1Skip4 + u4Offset, 8);   // Get plain text

            if (prParam->eFeedbackMode == DMX_CA_FB_ECB)
            {
                if (prParam->fg3Des)
                {
                    des3(r3DesKs, au1Text);
                }
                else
                {
                    des(rDesKs, au1Text);
                }
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_CBC)
            {
                _memxorEx(au1Text, au1InitVector, 8);

                if (prParam->fg3Des)
                {
                    des3(r3DesKs, au1Text);
                }
                else
                {
                    des(rDesKs, au1Text);
                }

                x_memcpy(au1InitVector, au1Text, 8);
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_CFB)
            {
                INT32 iv_offset = 0;
                if (prParam->fg3Des)
                {
                    des3_crypt_cfb(r3DesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
                else
                {
                    des_crypt_cfb(rDesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_OFB)
            {
                INT32 iv_offset = 0;
                if (prParam->fg3Des)
                {
                    des3_crypt_ofb(r3DesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
                else
                {
                    des_crypt_ofb(rDesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
            }
            x_memcpy(pu1Skip4 + u4Offset, au1Text, 8);  // Copy back
        }

        if ((prParam->eRtb == DMX_DESC_RTB_MODE_CTS) && (prParam->eFeedbackMode == DMX_CA_FB_ECB) &&
                ((184 - u4Offset) != 0) && ((184 - u1PayloadOffset) > 8))
        {
            UINT8 u1Residual;

            x_memcpy(pu1Skip4 + u4Offset - 8, au1Backup, 8);       // Copy plain back

            u1Residual = 184 - u4Offset;
            x_memcpy(au1Text, (pu1Skip4 + (184 - 8)), 8);
            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1Text);
            }
            else
            {
                des(rDesKs, au1Text);
            }
            x_memcpy((pu1Skip4 + 184 - u1Residual), (au1Text + (8 - u1Residual)), u1Residual);  // Copy back

            x_memcpy(au1Backup, pu1Skip4 + u4Offset - 8, u1Residual);
            x_memcpy(au1Backup + u1Residual, au1Text, 8 - u1Residual);
            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1Backup);
            }
            else
            {
                des(rDesKs, au1Backup);
            }
            x_memcpy(pu1Skip4 + u4Offset - 8, au1Backup, 8);  // Copy back
        }

        if ((prParam->eRtb == DMX_DESC_RTB_MODE_SCTE52) && ((TS_PACKET_SIZE - 4 - u4Offset) != 0))
        {
            UINT8 u1Cnt = TS_PACKET_SIZE - 4 - u4Offset;

            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1InitVector);
            }
            else
            {
                des(rDesKs, au1InitVector);
            }

            x_memcpy(au1Text, pu1Skip4 + u4Offset, u1Cnt);   // Get plain text
            _memxorEx(au1Text, au1InitVector, u1Cnt);
            x_memcpy(pu1Skip4 + u4Offset, au1Text, u1Cnt);  // Copy back
        }

    }

_exit:

    return TRUE;
}

static BOOL _EncryptDesPackets(UINT32 u4Addr, UINT32 u4Size,
                               DES_PARAM_T* prParam)
{
    ASSERT(_DMX_IsAligned(u4Size, 188));

    while (u4Size > 0)
    {
        if (!_DMX_EncryptDesPacket((UINT8*)u4Addr, prParam))
        {
            return FALSE;
        }

        u4Addr += 188;
        u4Size -= 188;
    }

    return TRUE;
}

 BOOL _DesEncryptCTREx(UINT32 u4Addr, UINT32 u4Size, UINT8 *pu1Key, DES_PARAM_T* prParam)
{
    DES3_KS r3DesKs;
    DES_KS rDesKs;
    UINT32 u4BlockId = 0;
    UINT8 au1Text[8], au1Output[8];
    UINT8* pu1Buf = (UINT8*)u4Addr;

    // Setup key scheduling
    if (prParam->fg3Des)
    {
        des3key(r3DesKs, pu1Key, 0);
    }
    else
    {
        deskey(rDesKs, pu1Key, 0);
    }

    // Copy sample id as first half of text
    x_memcpy(au1Text, prParam->au1SampleId, 4);

    while (u4Size > 0)
    {
        UINT32 u4RoundSize;

        if (!prParam->fgCTRResidual)
        {
            if (u4Size < 8)
            {
                break;
            }
        }

        u4RoundSize = MIN(u4Size, 8);

        // Copy block id as second half of text
        _hton32Ex(u4BlockId, au1Text + 4);

        x_memcpy(au1Output, au1Text, 8);
        if (prParam->fg3Des)
        {
            des3(r3DesKs, au1Output);
        }
        else
        {
            des(rDesKs, au1Output);
        }

        _XorEx(pu1Buf, au1Output, u4RoundSize);

        pu1Buf += u4RoundSize;
        u4Size -= u4RoundSize;
        u4BlockId++;
    }

    return TRUE;
}

void _memxorEx(UINT8* pu1Dest, UINT8* pu1Src, INT32 i4Count)
{
    INT32 i;

    for (i = 0; i < i4Count; i++)
    {
        pu1Dest[i] ^= pu1Src[i];
    }
}

UINT16 _GetPidEx(UINT8* pu1Buf)
{
    UINT16 u2Pid;

    ASSERT(pu1Buf != NULL);

    pu1Buf++;
    u2Pid = ((*pu1Buf << 8) | *(pu1Buf + 1)) & 0x1fff;

    return u2Pid;
}

/*let it support two pid*/
INT32 _MatchKeyEx(UINT16 u2Pid, DES_PARAM_T* prParam)
{
    INT32 i4Key;

    ASSERT(prParam != NULL);

    for (i4Key = 0; i4Key < MAX_KEY_NO; i4Key++)
    {
        if (prParam->arEnPid[i4Key].fgValid &&
                ((u2Pid == prParam->arEnPid[i4Key].u2Pid) ||
                (u2Pid == prParam->arEnPid[i4Key].u2PidEx[0])))
        {
            return i4Key;
        }
    }

    return -1;
}

void _hton32Ex(UINT32 u4Src, UINT8* pu1Dst)
{
    UINT32 u4Bytes, i;
    UINT8* pu1Src;

    pu1Src = (UINT8*)&u4Src;
    u4Bytes = sizeof (UINT32);

    for (i = 0; i < u4Bytes; i++)
    {
        pu1Dst[i] = pu1Src[u4Bytes - 1 - i];
    }
}

void _XorEx(UINT8* pu1Dst, UINT8* pu1Src, UINT32 u4Size)
{
    UINT32 i;

    for (i = 0; i < u4Size; i++)
    {
        pu1Dst[i] ^= pu1Src[i];
    }
}

static VOID _InitDesParam(DMX_KEY_TYPE_T e_type,UINT16 u2pid1,UINT16 u2pid2)
{
    DES_PARAM_T rParam;
    UINT32 u4KeyLen, i, j;
    UCHAR* szKey = "EVEN";
    DMX_CA_MODE_T eMode;
    UINT32 u1TsIndex = 3;
    UINT8 _u1KeyIdx = 0; 
    switch(e_type)
    {
      case TV_PLAY_KEY:_u1KeyIdx =TV_PLAY_KEY_IDX; break;
      case TV_RECORD_KEY:_u1KeyIdx =TV_RECORD_KEY_IDX;break;
      case PVR_PLAY_KEY :_u1KeyIdx =TIMESHIFT_PLAY_KEY_IDX;break;
      case MM_PLAY_KEY:_u1KeyIdx =MM_PLAY_KEY_IDX;break;
      default:break;
    }

    // Generate DES parameters
    // ECB or CBC (and init vector)
    rParam.eFeedbackMode = (DMX_CA_FEEDBACK_MODE_T)random(5);
    rParam.eRtb = (DMX_DESC_RTB_MODE_T)random(3);
    rParam.fgCTRResidual = (BOOL)random(2);
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_SCTE52) && (rParam.eFeedbackMode != DMX_CA_FB_CBC))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_CTS) && (rParam.eFeedbackMode != DMX_CA_FB_ECB))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }
    if (rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
        for (i=0; i<8; i++)
        {
            rParam.au1SampleId[i] = random(256);
        }
    }

    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        for (i = 0; i < 8; i++)
        {
            rParam.au1InitVector[i] = random(256);
        }
    }

    // DES or 3DES
    rParam.fg3Des = (random(2) == 0);

    // Key select
    rParam.eKeySelect = (KEY_SELECT_T)random(3);

    // Map to key 0 only, fixme
    for (i = 0; i < MAX_KEY_NO; i++)
    {
        rParam.arEnPid[i].fgValid = FALSE;
    }
    rParam.arEnPid[_u1KeyIdx].fgValid = TRUE;

    // EnPid
    for (i = 0; i < MAX_KEY_NO; i++)
    {
        if (rParam.arEnPid[i].fgValid)
        {
            rParam.arEnPid[i].u2Pid = u2pid1; // set pid
            rParam.arEnPid[i].u2PidEx[0]= u2pid2; 
            u4KeyLen = (rParam.fg3Des ? 24 : 8);

            for (j = 0; j < u4KeyLen; j++)
            {
                rParam.arEnPid[i].au1EvenKey[j] = random(256);
                rParam.arEnPid[i].au1OddKey[j] = random(256);
            }
        }
    }

    // Set keys
    if (!_SetKeys(u1TsIndex, &rParam))
    {
        return FALSE;
    }

    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        // Set initial vector
        UINT32 u4Hi, u4Lo;

        u4Hi = (rParam.au1InitVector[0] << 24) | (rParam.au1InitVector[1] << 16) |
               (rParam.au1InitVector[2] << 8) | rParam.au1InitVector[3];

        u4Lo = (rParam.au1InitVector[4] << 24) | (rParam.au1InitVector[5] << 16) |
               (rParam.au1InitVector[6] << 8) | rParam.au1InitVector[7];

        if (rParam.eKeySelect == KEY_SELECT_RANDOM)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, TRUE);
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
        else if (rParam.eKeySelect == KEY_SELECT_EVEN)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, TRUE);
        }
        else if (rParam.eKeySelect == KEY_SELECT_ODD)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
    }

    if (rParam.fg3Des)
    {
        eMode = DMX_CA_MODE_3DES;
    }
    else
    {
        eMode = DMX_CA_MODE_DES;
    }

    if(!_DMX_SetCA_Mode(_u1KeyIdx, eMode, rParam.eFeedbackMode, FALSE))
    {
        return FALSE;
    }

    _DMX_SetRtbMode(_u1KeyIdx, rParam.eRtb);

    if (rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        UINT32 u4SampleId1;
        UINT64 u8Nonce;

        u4SampleId1 = (rParam.au1SampleId[0] << 24) | (rParam.au1SampleId[1] << 16) |
                      (rParam.au1SampleId[2] << 8) | rParam.au1SampleId[3];

        u8Nonce = ((UINT64)u4SampleId1 << 32);

        _DMX_SetCA_CTR(_u1KeyIdx, u8Nonce, 0, TRUE);
        _DMX_SetCA_CTR(_u1KeyIdx, u8Nonce, 0, FALSE);

        _DMX_SetCTR_Rtb(_u1KeyIdx, rParam.fgCTRResidual);
    }

    // Show DES parameters
    switch (rParam.eKeySelect)
    {
    case KEY_SELECT_EVEN:
        szKey = "EVEN";
        break;

    case KEY_SELECT_ODD:
        szKey = "ODD";
        break;

    case KEY_SELECT_RANDOM:
        szKey = "RANDOM";
        break;

    default:
        ASSERT(0);
        break;
    }

    // Show mode and key select
    Printf("    Mode: %s, key select: %s\n", rParam.fg3Des ? "3DES" : "DES", szKey);
    switch (rParam.eFeedbackMode)
    {
    case DMX_CA_FB_ECB:
        Printf("     mode: ECB\n");
        break;
    case DMX_CA_FB_CBC:
        Printf("     mode: CBC\n");
        break;
    case DMX_CA_FB_CFB:
        Printf("     mode: CFB\n");
        break;
    case DMX_CA_FB_OFB:
        Printf("     mode: OFB\n");
        break;
    case DMX_CA_FB_CTR:
        Printf("     mode: CTR, do residual:%d\n", (INT32)rParam.fgCTRResidual);
        break;
    }

    switch (rParam.eRtb)
    {
    case DMX_DESC_RTB_MODE_CLEAR:
        Printf("     rtb: CLEAR\n");
        break;
    case DMX_DESC_RTB_MODE_CTS:
        Printf("     rtb: CTS\n");
        break;
    case DMX_DESC_RTB_MODE_SCTE52:
        Printf("     rtb: SCTE52\n");
        break;
    }

    // Show keys
    u4KeyLen = (rParam.fg3Des ? 24 : 8);
    if (rParam.eKeySelect != KEY_SELECT_ODD)
    {
        Printf("    Even key: ");
        for (i = 0; i < u4KeyLen; i++)
        {
            Printf("%02x", rParam.arEnPid[_u1KeyIdx].au1EvenKey[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }
    if (rParam.eKeySelect != KEY_SELECT_EVEN)
    {
        Printf("    Odd key:  ");
        for (i = 0; i < u4KeyLen; i++)
        {
            Printf("%02x", rParam.arEnPid[_u1KeyIdx].au1OddKey[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }

    // Show initial vector
    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        Printf("    Init vector: ");
        for (i = 0; i < 8; i++)
        {
            Printf("%02x", rParam.au1InitVector[i]);
        }
        Printf("\n");
    }

    /*assgine to different key type*/
    _arParam_Table[e_type] = rParam; 
}

/*set  multi-key for MM.*/
static BOOL _Dmx_Multi2_SetKeys(void)
{
    UINT32 au4Keys[2];
    UINT32 au4SystemKeys[8];
    UINT8 u1KeyIndex;
    UINT16 u2Iteration;
    BOOL fgEven, fgOFBFreeRun;

    // Set Even keys
    x_memset((void*)au4Keys, 0, sizeof(au4Keys));
    au4Keys[0] = 0xC388E7F3;
    au4Keys[1] = 0xBFAF666D;
    u1KeyIndex = MM_PLAY_KEY;
    fgEven = TRUE;
    VERIFY(_DMX_SetMulti2Key(u1KeyIndex, fgEven, (UINT32*)au4Keys));

    // Set Odd keys
    x_memset((void*)au4Keys, 0, sizeof(au4Keys));
    au4Keys[0] = 0x8001BDBF;
    au4Keys[1] = 0x33D600D3;
    fgEven = FALSE;
    VERIFY(_DMX_SetMulti2Key(u1KeyIndex, fgEven, (UINT32*)au4Keys));

    // Set Multi2 Configuration Register
    fgOFBFreeRun = TRUE;
    u2Iteration = 0x20;
    _DMX_SetMulti2Config(u1KeyIndex, fgOFBFreeRun, u2Iteration);

    // Set CBC Inital Vector
    _DMX_SetMulti2CbcIV(u1KeyIndex, 0xFE271999, 0x19690911);

    // Set System keys
    x_memset((void*)au4SystemKeys, 0, sizeof(au4SystemKeys));
    au4SystemKeys[0] = 0x0D2E7D98;
    au4SystemKeys[1] = 0xB74E4084;
    au4SystemKeys[2] = 0x689D4BB7;
    au4SystemKeys[3] = 0xEC993921;
    au4SystemKeys[4] = 0xC4C36C1B;
    au4SystemKeys[5] = 0x32DF9CF5;
    au4SystemKeys[6] = 0x4B17EA5C;
    au4SystemKeys[7] = 0x36310466;
    _DMX_SetMulti2SysKey(u1KeyIndex, au4SystemKeys);

    if(!_DMX_SetCA_Mode(u1KeyIndex, DMX_CA_MODE_MULTI2_BIG, DMX_CA_FB_ECB, FALSE))
    {
        return FALSE;
    }

    return TRUE;
}


static INT32 _DMXCIPLUS_Multi_Scramble_Play_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
    UINT8 u1pidx=0;
    UINT32 SrcAddr,SrcSize;
    BOOL b_match0=TRUE,b_match1=TRUE,fg_time_on=TRUE;
    UINT32 SrcpesAddr,DstpesAddr,u4size;
    UINT32 u4SrcDiff,u4DstDiff;
    UINT32 u4Reg,i=0;
    UINT16 u2Ctrl = 0;
    MM_MSG_T  rMsg;
    if(i4Argc != 2)
    {
        Printf("%s,Test command:dplay timestamp[0|1]",aszArgv[0]);
        return    -1;
    }
    fg_time_on = StrToInt(aszArgv[1]);

    /*Init the golden file */
    _DMX_EMU_TS_Parameter_Init();
    /*init dst buffer*/
    for(i=0;i<=DMX_TEST_PID_TOTAL_NUM;i++)
    {
        x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
    }
    /*creat the thread for update the pvr3 wp*/
    // Create fvr queues
    if(fg_init_first == FALSE)
    {
        if (x_msg_q_create(&_hFvrEmuQueue, "EmuFVR",
            sizeof (FVR_MSG_EMU_T), 1024) != OSR_OK)
        {
            Printf("Fail to create picture queue!\n");
            return FALSE;
        }
        if (x_msg_q_create(&_hMMQueue, "MM",
            sizeof(MM_MSG_T), 1024) != OSR_OK)
        {
            Printf("Fail to create MM queue!\n");
            return FALSE;
        }
        // Create fvr  thread
        if (x_thread_create(&_hFvrEmuThread, "_hFvrEmuThread",
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _FvrThread, 0,
            NULL) != OSR_OK)
        {
            Printf("Fail to create DMX thread!\n");
            return FALSE;
        }

        // Create  MM  thread 
        if (x_thread_create(&_hMMThread, "MMThread",
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _MMThread, 0,
            NULL) != OSR_OK)
        {
            Printf("Fail to create MM thread!\n");
            return FALSE;
        }
        fg_init_first = TRUE;
    }
    //init des_parameter for encrypt/decrypt
    _InitDesParam(MM_PLAY_KEY,PID_TABLE[0],PID_TABLE[1]);
    //set  mm key
    _Dmx_Multi2_SetKeys();
      
    _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE); //for tuner1 and tuner2.
    /*encrypt ddi_ts data by des*/
    SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
    SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
    if (!_EncryptDesPackets(SrcAddr,SrcSize, &_arParam_Table[0]))
    {
        Printf("encrypt datas error!\r\n");
        return FALSE;
    }
    /*because pvr2 just move 188 byte unit .make sure the size is aligne 188*/
    #if 0
    _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length = _DMX_Align(_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length,
                                                      PVR_MM_ALIGN_SIZE);
    _DmxEmuGenMMData(_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start,
                         _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length); // create mm chunk data 
    #else
    _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length = sizeof(au1mmMulti2Encrypted);
    _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length = _DMX_Align(_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length,
                                                      PVR_MM_ALIGN_SIZE);
    x_memcpy((void*)_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start, 
                   (const void*)au1mmMulti2Encrypted,
                   _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
    #endif
    /*init hardaware path*/
    _DMX_EMU_CIPLUS_Init();
    /*Set  Full pull mode*/
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
    /*set DDI to demuxrate*/
       
    /*part1:for multi-stream record settings*/
    /*1,set record pid for ts_idx0 ,ts_idx1*/
    if(!_DMX_EMU_SetRecordPidExt(0,0,PID_TABLE[0],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
    }
    if(!_DMX_EMU_SetRecordPidExt(1,1,PID_TABLE[1],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
    }
    /*2,set ciplus1.4 record region for micro-code use and start record firstly*/
    #ifdef CC_DMX_FPGA_TEST
    _SetPVRGBLEx5(4,fg_time_on,0x47474647,0x3,500);
    #else
    _SetPVRGBLEx5(4,fg_time_on,0x47474647,0x3,200); //for IC,set 20ms/isr
    #endif
    
    /*3, set timeshift record buffer for ts_idx3*/
    #ifdef CC_DMX_FPGA_TEST
    _SetPVRGBLEx5(0,fg_time_on,0x47474647,0x3,500);
    #else
    _SetPVRGBLEx5(0,fg_time_on,0x47474647,0x3,200); //for IC,set 20ms/isr
    #endif
    _FVR_Start(0);
    _FVR_Start(4);
    /*part2:for multi-stream playback settings*/
    /*1,enable ts_idx3 for support */
    DmxSetLocalTSEnable(TRUE,TRUE); //enable multi-stream playback/record.
    DmxSetTSStream(3,MULTI_STREAM);  //set mulit-stream mode for ts_idx3.
    DMXSetLocalTSID(3,0x47474647,0x3); //set local_ts_id valid.

    Printf("Use pid fitler.\r\n");
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
    //DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_4,0);
      
    /*2, set the connection between pvr3 and ts_idx3*/
    _DMX_EMU_CustPvrPath(CI_REMUX_PVR,3,FALSE); 
#if defined(CC_MT5890)    
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
#else
    if (!DMX_TsOut_SetDataWidthEx(0, DMX_TSOUT_DATAWIDTH_1))
#endif
    {
        Printf("Failed to set data width!\n");
        return -1;
    }
    if (!DMX_TsIn_SetDataWidth(3, DMX_TSOUT_DATAWIDTH_1))
    {
        Printf("Failed to set data width!\n");
        return -1;
    }
#if defined(CC_MT5890)
    /*3, set the connecton between pvr1 and ts_idx4 for timeshift*/
    _DMX_EMU_CustPvrPath(TIMESHIFT_PVR,4,TRUE); 
#else
    /*3, set the connecton between pvr1 and ts_idx3 for timeshift*/
    _DMX_EMU_CustPvrPath(TIMESHIFT_PVR,2,TRUE); /*PB2_TO_DBM->TS_IDX3, pid filter default.*/
#endif

    /*set record and playback pid for this one*/
    /*record :choose which pid can pass  ts_idx3 */
    _DMX_EMU_SetMultiRecordPid1(3,0,2,PID_TABLE[0],FALSE,FALSE,FALSE,DMX_PID_TYPE_PES);

#if defined(CC_MT5890)
    /*timeshift :set the playback for ts_idx4.*/
    _DMX_EMU_SetPlayPidEx1(4,0,2,PID_TABLE[0],DMX_PID_TYPE_PES);
#else
    /*timeshift :set the playback for ts_idx2.*/
    _DMX_EMU_SetPlayPidEx1(2,0,2,PID_TABLE[0],DMX_PID_TYPE_PES);
#endif

    /*4, set multi-stream playback pid,use DMX_PID_TYPE_TS_RAW to debug it.*/
    _DMX_EMU_SetPlayPidEx2(3,0,0,PID_TABLE[0],DMX_PID_TYPE_PES);
    _DMX_EMU_SetPlayPidEx2(3,1,1,PID_TABLE[1],DMX_PID_TYPE_PES);
    /*5, set mini-pvr to the start buffer.*/ 
    if(_DmxEmuSetPVRPlayExt1(CI_REMUX_PVR,fg_time_on,fg_time_on) == FALSE)
    {
        Printf("%s ,set remux pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }
    /*6,set mini-pvr fro timeshift*/
    if(_DmxEmuSetPVRPlayExt1(TIMESHIFT_PVR,fg_time_on,fg_time_on) == FALSE)
    {
        Printf("%s ,set timeshift pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }

#if defined(CC_MT5890)    
    /*part3:set for MM playback*/
    _DMX_EMU_CustMMPath();  //init mm path,bypass pid filter
    /*set mini-pvr*/
    if(_DmxEmuSetPVRPlayExt1(MM_PVR,FALSE,FALSE) == FALSE)
    {
        Printf("%s ,set mm pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
    }
    /*set playback pid and set bypass pid */ 
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)| 1<<28));
    _DmxEmuSetMMPid1(2,DMX_TEST_PID_TOTAL_NUM,_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
    u2Ctrl = PID_INDEX_TABLE(DMX_TEST_PID_TOTAL_NUM)&0xffff;
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_3, (DMX_TEST_PID_TOTAL_NUM<< 16) | u2Ctrl);
#else
    /*timeshift :set the playback for ts_idx2. Use Pidx 2 for bypass TS index2*/
    //DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_3, (2<< 16) | 0x20);
#endif
    
    /*part4: start  engine*/
    /*1, enable the pvr3 playback engine*/
    _DmxEmuStartPVRPlay(CI_REMUX_PVR);
    /*2, enable the pvr playback engine*/
    _DmxEmuStartPVRPlay(TIMESHIFT_PVR);
#if defined(CC_MT5890)    
    /*3, enable the mm playback engine*/
    _DmxEmuStartPVRPlay(MM_PVR);
#endif
    /*4 ,enable dma engine ,step by step*/
    HalFlushInvalidateDCache();
    /*get src address and src size*/
    SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
    SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
    /*enable DDI engine*/
    if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
    {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
    }

#if defined(CC_MT5890)    
    /*move mm data */
    UINT32 u4_move_size = PVR_MOVE_CHUNK_DATA_UNIT_SIZE;
    rMsg.u4Addr =  DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_START2);
    for(i=0 ;i<=_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;i+=PVR_MOVE_CHUNK_DATA_UNIT_SIZE)
    {
        if((i+PVR_MOVE_CHUNK_DATA_UNIT_SIZE)>_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length)
        {
            u4_move_size = _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length-i;
        }
        else
        {
            u4_move_size = PVR_MOVE_CHUNK_DATA_UNIT_SIZE;
        }
        rMsg.u4Addr = rMsg.u4Addr+u4_move_size;
        rMsg.u4Size = u4_move_size;
        if (x_msg_q_send(_hMMQueue,(VOID*)&rMsg, sizeof (MM_MSG_T),
                128) != OSR_OK)
        {
            Printf("Send message error.\r\n");
        }
    }
#endif
    i=0;
    while(i<55)
    { 
        x_thread_delay(150); //delay 150ms
        i++;
    }
     
    Printf("Data Transfer is ok ,input <dstop> command  to compare the datas.\r\n ");
    _mmCounter = 0;
    /*compare golden pes files*/  
    return TRUE;     
}

static INT32 _DMXCIPLUS_Multi_Scramble_Stop_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
    UINT32 SrcpesAddr,DstpesAddr,u4size,u4SrcDiff,u4DstDiff;
    BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE,b_match3=TRUE;
    if(i4Argc != 1)
    {
        Printf("%s,Test command:dstop",aszArgv[0]);
        return    -1;
    }
    /*if you check play finish ,compare with golden files*/
    HalFlushInvalidateDCache();
    /*ts_idx=0*/

    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[0].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    /*ts_idx=1*/
    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[1].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match1 = FALSE;
        Printf("ts_idx1,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    /*for ts_idx0 ,do timeshift*/
    SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
    DstpesAddr=_arPidBuffer[2].u4BufAddr;
    u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match2 = FALSE;
        Printf("ts_idx4,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

#if defined(CC_MT5890)    
    /*for  MM playback from pvr2*/
    /*get the golden files*/
    x_memcpy((void*)_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start, 
                   (const void*)au1mmMulti2Decrypted,
                   _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
    /*compare the dst datas with the golden files.*/
    SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start;
    DstpesAddr= _arPidBuffer[DMX_TEST_PID_TOTAL_NUM].u4BufAddr;
    u4size=_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match3 = FALSE;
        Printf("ts_idx2,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }
#endif    
    if(b_match0 && b_match1&& b_match2&&b_match3)
    {
        Printf("Match!\r\n");
    }
    else
    {
        Printf("Dismatch!\r\n");
    }

    fg_mm_move_stop = TRUE; //stop move mm datas.
    /*for record/playback :stop ,then free pid ,then free record buffer.*/
    FVR_Stop(0); //stop timeshift buffer
    FVR_Stop(4); //stop remux buffer
    _DmxEmuStopPVRPlay(CI_REMUX_PVR); //stop remux pvr playback
    _DmxEmuStopPVRPlay(TIMESHIFT_PVR);
#if defined(CC_MT5890)    
    _DmxEmuStopPVRPlay(MM_PVR);
#endif
    /*free playback pid */
    _DMX_EMU_FreePlaypid(0);
    _DMX_EMU_FreePlaypid(1); /*for multi-stream playback*/
    _DMX_EMU_FreePlaypid(2) ; /*for timeshift playbakc pid */
    _DMX_EMU_FreePlaypid(DMX_TEST_PID_TOTAL_NUM); /*free pid for mm playback.*/
    /*for ci+1.4 remux record pid*/
    FVR_FreePid(0);
    FVR_FreePid(1); 
    FVR_FreePid(2); 
    /*free playback or record buffer*/
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);
    _DMX_EmuPVRPlay_FreeBuf(TIMESHIFT_PVR);
#if defined(CC_MT5890)    
    _DMX_EmuPVRPlay_FreeBuf(MM_PVR);
#endif
    FVR_FreeGBL(0);
    FVR_FreeGBL(4);
    return TRUE;
}

static INT32 _DMXCIPLUS_CFlag_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
  if(i4Argc != 2)
  {
    Printf("%s,Test command:cl [value]",aszArgv[0]);
    return    -1;
  }
  
  fg_pvr3_complete = StrToInt(aszArgv[1]);

  return TRUE;
}

//----------------------------------------------------------------
 /* description:check whetherv  the record interrupt is ok or not.
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_Remux_Interrupt_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      UINT32 ui4_pkt_cnt0=0xFFFF,ui4_timer_notify=0;
      UINT32 i=0;
      if(i4Argc != 3)
      {
        Printf("%s,Test command:remuxint pkt_count timer_notify.\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        ui4_pkt_cnt0 = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
        ui4_timer_notify = StrToInt(aszArgv[2]);
      }
      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
      /*generate the pes datas for ddi_ts*/
      _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
      /*generate the pes datas for IP*/
      _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();
       FVR_Init();
       /*customize the IP path    */
       _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE);
      /*set mini-pvr*/
      _DmxEmuSetPVRPlay(IP_PVR,FALSE);
    
      /*set reocrd    pid for u1_tsidx*/
      u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
      if(!_DMX_EMU_SetRecordPid(2,u1pidx,PID_TABLE[u1pidx],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx2 error.\r\n");
        return -1;
      }
      /*for pvr2 ,if you need to do record ,you just  need set bit[9] to 1, dbm->Pid filter*/
      _DMX_Set_PB_STEER_BIT_9(1,TRUE);
    
       /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,0,PID_TABLE[0],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
      /*set record pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,1,PID_TABLE[1],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }
    
      /*set remux record global region.*/
      _SetPVRGBLEx(4,TRUE,ui4_pkt_cnt0); //set remux record global region
      _SetPVRGBLTimerNotify(4, ui4_timer_notify);
      _FVR_Start(4);

      /*start ddi_ts move */
        /*get src address and src size*/
      UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
      UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
       /*enable DDI engine*/
      if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
      {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
      }
      /*start pvr engine*/
       /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(IP_PVR);
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status
    
                // Complete
                fgRet = TRUE;
                //Printf("     Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
       x_thread_delay(10);
       HalFlushInvalidateDCache();
       Printf("Transfter is ok.\r\n");
              //wait for isr to trigger
       while(i<50)
       {
        x_thread_delay(500);
        i++;
       }
    
       _DmxEmuStopPVRPlay(IP_PVR);
       _DMX_EmuPVRPlay_FreeBuf(IP_PVR);
    
       FVR_Stop(4);

       
       FVR_FreePid(u1pidx);
       FVR_FreePid(0);
       FVR_FreePid(1);
       FVR_FreeGBL(4);

       return TRUE;
        
}


//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_Remux_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL fgon=FALSE;
      UINT8 ui1_remux_mask=0x7;
      UINT32 i;
      if(i4Argc != 3)
      {
        Printf("%s,Test command:Remux timestamp[0|1] remuxflag.\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        fgon = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
       ui1_remux_mask = StrToInt(aszArgv[2]);
      }
      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
      /*generate the pes datas for ddi_ts*/
      _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
      /*generate the pes datas for IP*/
      _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();
       /*customize the IP path    */
       _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE);
      /*set mini-pvr*/
      _DmxEmuSetPVRPlay(IP_PVR,FALSE);
    
      /*set reocrd    pid for u1_tsidx*/
      u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
      if(!_DMX_EMU_SetRecordPid(2,u1pidx,PID_TABLE[u1pidx],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx2 error.\r\n");
        return -1;
      }
      /*for pvr2 ,if you need to do record ,you just  need set bit[9] to 1, dbm->Pid filter*/
      _DMX_Set_PB_STEER_BIT_9(1,TRUE);
    
       /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,0,PID_TABLE[0],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
      /*set record pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,1,PID_TABLE[1],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }

      /*set remux record global region.*/
      //set remux record global region
      //disable buffer
      for (i = 0; i < (FVR_GBL_COUNT-1) ; i++)
      {
          FVR_GBL_ARY_W(i, 2) = 0x100;     // Disable record region 0
      }
      _SetPVRGBLEx3(4,fgon,ui1_remux_mask);
      _FVR_Start(4);

      /*start ddi_ts move */
        /*get src address and src size*/
      UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
      UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                        _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
       /*enable DDI engine*/
      if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
      {
        Printf("DDI move datas error,test Fail.\r\n");
        return -1;    
      }
      /*start pvr engine*/
       /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(IP_PVR);
      
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status
    
                // Complete
                fgRet = TRUE;
                //Printf("     Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
       x_thread_delay(250);
       HalFlushInvalidateDCache();
       Printf("Transfter is ok.\r\n");
    
       _DmxEmuStopPVRPlay(IP_PVR);
       _DMX_EmuPVRPlay_FreeBuf(IP_PVR);
       FVR_Stop(4);
  
       FVR_FreePid(u1pidx);
       FVR_FreePid(0);
       FVR_FreePid(1);
       FVR_FreeGBL(4);
      
       return TRUE;
        
}

//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:ddi_ts(framer0),ddi_ts(framer1),IP(pvr2->2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_Record_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
  UINT8 u1pidx=0;
  if(i4Argc != 1)
  {
    Printf("%s,Test command:Remux.\r\n",aszArgv[0]);
    return    -1;
  }

  /*init ts parameter*/
  _DMX_EMU_TS_Parameter_Init();
  /*generate the pes datas for ddi_ts*/
  _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
  /*generate the pes datas for IP*/
  _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
   /*init hw path */
   _DMX_EMU_CIPLUS_Init();
   /*customize the IP path  */
   _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE);
  /*set mini-pvr*/
  _DmxEmuSetPVRPlay(IP_PVR,FALSE);

  /*set reocrd  pid for u1_tsidx*/
  u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
  if(!_DMX_EMU_SetRecordPid(2,u1pidx,PID_TABLE[u1pidx],FALSE,FALSE,FALSE))
  {
    Printf("set record pid for ts_idx2 error.\r\n");
    return -1;
  }
  /*for pvr2 ,if you need to do record ,you just  need set bit[9] to 1, dbm->Pid filter*/
  _DMX_Set_PB_STEER_BIT_9(1,TRUE);

   /*set record  pid for ts_idx0*/
  if(!_DMX_EMU_SetRecordPid(0,0,PID_TABLE[0],FALSE,FALSE,FALSE))
  {
    Printf("set record pid for ts_idx0 error.\r\n");
    return -1;
  }
  /*set record pid for ts_idx1*/
  if(!_DMX_EMU_SetRecordPid(1,1,PID_TABLE[1],FALSE,FALSE,FALSE))
  {
    Printf("set record pid for ts_idx1 error.\r\n");
    return -1;
  }

  /*set remux record global region.*/
  //_SetPVRGBL(4,FALSE); //set remux record global region
  //_FVR_Start(4);
  _SetPVRGBL(0,TRUE);
  _SetPVRGBL(1,TRUE);
  _SetPVRGBL(2,TRUE);
  _FVR_Start(0);
  _FVR_Start(1);
  _FVR_Start(2);
  /*start ddi_ts move */
    /*get src address and src size*/
  UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
  UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                      _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
   /*enable DDI engine*/
  if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
  {
    Printf("DDI move datas error,test Fail.\r\n");
    return -1;    
  }
  /*start pvr engine*/
   /*enable start pvr engine*/
  _DmxEmuStartPVRPlay(IP_PVR);
  
  /*wait for finish*/
   BOOL fgRet = FALSE;
   UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
    while (1)
    {
        UINT32 u4Rp;
        u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
        if (u4Rp == u4Wp)
        {
            ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

            // Complete
            fgRet = TRUE;
            //Printf("   Wait count: %u\n", i);
            break;
        }
        HAL_Delay_us(10); //because data is very large ,dely hardware.
    }
   x_thread_delay(250);
   HalFlushInvalidateDCache();
   Printf("Transfter is ok.\r\n");

   _DmxEmuStopPVRPlay(IP_PVR);

   FVR_Stop(0);
   FVR_Stop(1);
   FVR_Stop(2);
   
   FVR_FreePid(u1pidx);
   FVR_FreePid(0);
   FVR_FreePid(1);
   FVR_FreeGBL(0);
   FVR_FreeGBL(1);
   FVR_FreeGBL(2);
   return TRUE;
    
}

//----------------------------------------------------------------
 /*   description:check whether hw path is ok
 *    test path:ddi_ts->framer0,framer1->caouput
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_DDI_TS_TO_CA(INT32 i4Argc, const CHAR ** aszArgv)
{
  
  if(i4Argc != 1)
  {
    Printf("%s,Test command:ddiframer.\r\n");
    return    -1;
  }
  UINT32 DstAddr,DstEnd;

  /*init ts parameter*/
  _DMX_EMU_TS_Parameter_Init();
  /*generate the pes datas for ddi_ts*/
  _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
  
  /*get src address and src size*/
  UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
  UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                    _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
  /*init hw path */
  _DMX_EMU_CIPLUS_Init();

   /*set playback pid for ts_idx0*/
  if(!_DMX_EMU_SetPlayPid(0,0,PID_TABLE[0],DMX_PID_TYPE_PES))
  {
    Printf("set playback pid for ts_idx0 error.\r\n");
    return -1;
  }
  /*set playback pid for ts_idx1*/
  if(!_DMX_EMU_SetPlayPid(1,1,PID_TABLE[1],DMX_PID_TYPE_PES))
  {
    Printf("set playback pid for ts_idx1 error.\r\n");
    return -1;
  }
  DstAddr = _DMX_Align(0x6400000,DMX_CA_BUFFER_ALIGNMENT);
  DstEnd =_DMX_Align(DstAddr + SrcSize + 1, DMX_CA_BUFFER_ALIGNMENT);
  x_memset((VOID*)DstAddr,0,(SrcSize+1));
  // Set CA output buffer
  if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, DstAddr, DstEnd,
                                DstEnd - DstAddr))
  {
       Printf("set ca output error.\r\n");
       return -1;
  }
  /*enable DDI engine*/
  if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
  {
    Printf("DDI move datas error,test Fail.\r\n");
    return -1;    
  }
   x_thread_delay(200);

   HalFlushInvalidateDCache();
   /*free the    resource */
   _DMX_EMU_FreePlaypid(0);
   _DMX_EMU_FreePlaypid(1);
    return TRUE;
    
}

//----------------------------------------------------------------
 /*   description:check the ip_ts path is ok or not .
 *      ip ts description:ts don't contain the timestamp .
 *      iP:TS format or MM format
 */  
//-----------------------------------------------------------------
static INT32 _DMXCI_IP_TS_TO_DEMUX(INT32 i4Argc, const CHAR ** aszArgv)
{
    UINT8 u1_tsidx=2;
    BOOL fg_disable_filter=FALSE,fgon=FALSE;
    UINT32 SrcpesAddr,DstpesAddr,u4size;
    UINT32 u4SrcDiff=0,u4DstDiff=0;
    BOOL  b_match0=TRUE,b_match1=TRUE;
    BOOL  b_ts_file = TRUE;
    UINT8 u1pidx = 0;
    BOOL fgRet;
    UINT32 u4Wp;
  
    if(i4Argc != 5)
    {
#if defined(CC_MT5890)
        Printf("%s,Test command:IP ts_idx[2,4] disable_filter[0,1]  \
                timestamp[0,1],b_ts_file[0|1].\r\n",aszArgv[0]);
#else
        Printf("%s,Test command:IP ts_idx[2,3] disable_filter[0,1]  \
                timestamp[0,1],b_ts_file[0|1].\r\n",aszArgv[0]);
#endif
        return  -1;
    }

    if(i4Argc > 1)
    {
        u1_tsidx = StrToInt(aszArgv[1]);
    }
 
    if(i4Argc > 2)
    {
        fg_disable_filter = StrToInt(aszArgv[2]);
    }

    if(i4Argc > 3)
    {
        fgon = StrToInt(aszArgv[3]);
    }

    if(i4Argc > 4)
    {
        b_ts_file = StrToInt(aszArgv[4]);
    }
 
    /*init ts parameter*/
    _DMX_EMU_TS_Parameter_Init();
    /*generate the pes datas for ip(pvr2)*/
    if(b_ts_file)
    {
        _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
    }
    else
    {
       /*because pvr2 just move 188 byte unit .make sure the size is aligne 188*/
        _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length = _DMX_Align(_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length,
                                                      PVR_MM_ALIGN_SIZE);
        _DmxEmuGenMMData(_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start,
        _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
    }
    /*init hw path */
    _DMX_EMU_CIPLUS_Init();
    /*customize the IP path  */
    if(b_ts_file)
    {
        _DMX_EMU_CustPvrPath(IP_PVR,u1_tsidx,fg_disable_filter);
    }
    else
    {
        _DMX_EMU_CustMMPath();
    }
    /*set mini-pvr*/
    if(b_ts_file)
    {
        _DmxEmuSetPVRPlay(IP_PVR,fgon);
    
        /*set playback pid for u1_tsidx*/
        u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
        if(!_DMX_EMU_SetPlayPid(u1_tsidx,u1pidx,PID_TABLE[u1pidx],DMX_PID_TYPE_PES))
        {
            Printf("set playback pid for ts_idx0 error.\r\n");
            return -1;
        }
        _DMX_EMU_SetDBMBypassPid(u1_tsidx, u1pidx, fg_disable_filter);      
    }
    else
    {    
        _DmxEmuSetPVRPlay(MM_PVR,FALSE);
        u1pidx=DMX_TEST_PID_TOTAL_NUM;
        _DmxEmuSetMMPid(u1_tsidx,u1pidx,_arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length);
        DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_3, u1pidx << 16 | 0x20);
    }
  
    UINT32 SrcSize = _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;
    UINT32 DstAddr = _DMX_Align(0x2900000,DMX_CA_BUFFER_ALIGNMENT);
    UINT32 DstEnd =_DMX_Align(DstAddr + SrcSize + 1, DMX_CA_BUFFER_ALIGNMENT);
    x_memset((VOID*)DstAddr,0,(SrcSize+1));
        // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, DstAddr, DstEnd,
                                DstEnd - DstAddr))
    {
        Printf("set ca output error.\r\n");
        return -1;
    }

    HalFlushInvalidateDCache();
    /*enable start pvr engine*/
    _DmxEmuStartPVRPlay(IP_PVR);
    /*wait for finish*/
    fgRet = FALSE;
    u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);

    while (1)
    {
        UINT32 u4Rp;
        u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);

        if (u4Rp == u4Wp)
        {
            ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

            // Complete
            fgRet = TRUE;
            //Printf("   Wait count: %u\n", i);
            break;
        }
        HAL_Delay_us(10); //because data is very large ,dely hardware.
    }
   
    Printf("PVR transfter is ok.\r\n");
    /*compare source and datas */
    /*Get the golden pes file*/
    if(b_ts_file)
    {
        SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Start;
        u4size=(_arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Offset- \
           _arTsGoldenFile[DMX_TS_FROM_PVR2].Pes_Addr_Start);
        DstpesAddr=_arPidBuffer[u1pidx].u4BufAddr;
    }
    else
    {
        SrcpesAddr=_arTsGoldenFile[DMX_TS_FROM_PVR2].Es_Addr_Start;
        u4size = _arTsGoldenFile[DMX_TS_FROM_PVR2].ES_Length;
        DstpesAddr=_arPidBuffer[u1pidx].u4BufAddr;
    }
   
    x_thread_delay(20);
    HalFlushInvalidateDCache();
    
    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }
    if(b_match0)
    {
        Printf("Match!\r\n");
    }
    else
    {
        Printf("DisMatch!\r\n");
    }

    _DmxEmuStopPVRPlay(IP_PVR);
    _DMX_EmuPVRPlay_FreeBuf(IP_PVR);
    /*free the  resource */
    _DMX_EMU_FreePlaypid(u1pidx);
  
    return TRUE; 
}

//----------------------------------------------------------------
 /*   description:check whether the pattern data is available.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_DDI_TS_TO_FramerTest(INT32 i4Argc, const CHAR ** aszArgv)
{
  
  if(i4Argc != 1)
  {
    Printf("%s,Test command:ddiframer.\r\n");
    return  -1;
  }
  UINT32 SrcpesAddr,DstpesAddr,u4size;
  UINT32 u4SrcDiff=0,u4DstDiff=0;
  BOOL  b_match0=TRUE,b_match1=TRUE;
  /*init ts parameter*/
  _DMX_EMU_TS_Parameter_Init();
  /*generate the pes datas for ddi_ts*/
  _DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);
  
  /*get src address and src size*/
  UINT32 SrcAddr= _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start;
  UINT32 SrcSize = (_arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Offset- \
                      _arTsGoldenFile[DMX_TS_FROM_DDI].TS_Addr_Start);
  /*init hw path */
  _DMX_EMU_CIPLUS_Init();

   /*set playback pid for ts_idx0*/
  if(!_DMX_EMU_SetPlayPid(0,0,PID_TABLE[0],DMX_PID_TYPE_PES))
  {
    Printf("set playback pid for ts_idx0 error.\r\n");
    return -1;
  }
  /*set playback pid for ts_idx1*/
  if(!_DMX_EMU_SetPlayPid(1,1,PID_TABLE[1],DMX_PID_TYPE_PES))
  {
    Printf("set playback pid for ts_idx1 error.\r\n");
    return -1;
  }

  /*enable DDI engine*/
  if(!_DMX_EMU_DDI_Transfer(0,SrcAddr,SrcSize))
  {
    Printf("DDI move datas error,test Fail.\r\n");
    return -1;    
  }

  /*compare source and datas */
  /*ts_idx=0*/
   SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
   DstpesAddr=_arPidBuffer[0].u4BufAddr;
   u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[0].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match0 = FALSE;
        Printf("ts_idx0,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }
   /*ts_idx=1*/
   SrcpesAddr=DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start;
   DstpesAddr=_arPidBuffer[1].u4BufAddr;
   u4size=(DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Offset - \
           DDI_TS_TO_FRAMER_GOLDEN_FILES[1].DDI_TS_TO_FRAMER_GOLDEN_PES_Start);

    if (!_MemCmp(SrcpesAddr, DstpesAddr,u4size, &u4SrcDiff, &u4DstDiff))
    {
        b_match1 = FALSE;
        Printf("ts_idx1,Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);       
    }

    if(b_match1&&b_match0)
    {
      Printf("Match!\r\n");
    }
    else
    {
      Printf("DisMatch!\r\n");
    }
    /*free the  resource */
    _DMX_EMU_FreePlaypid(0);
    _DMX_EMU_FreePlaypid(1);
    return TRUE;
    
}
//-----------------------------------------------------------------
/**function name: _DMXCI_PatternTest
 *   description:check whether the pattern data is available.
 */
//-----------------------------------------------------------------

static INT32 _DMXCI_PatternTest(INT32 i4Argc, const CHAR ** aszArgv)
{
   UINT32 u4idx = 0;
   BOOL fgret = FALSE;
   if(i4Argc!=2)
   {
      Printf("%s,Test ci command:pt [0-4].\r\n",aszArgv[0]);
      return FALSE;
   }
   /*init parameter for pattern test*/
   _DMX_EMU_TS_Parameter_Init();
   
   u4idx = StrToInt(aszArgv[1]);
   switch(u4idx)
   {
     case 0:fgret=_DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE);break;
     case 1:fgret=_DmxEmuGenPES(DMX_TS_FROM_PVR1,TRUE);break;
     case 2:fgret=_DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);break;
     case 3:fgret=_DmxEmuGenPES(DMX_TS_FROM_PVR3,TRUE);break;
     case 4:
     default:
             {
              fgret=_DmxEmuGenPES(DMX_TS_FROM_DDI,TRUE); 
              fgret= _DmxEmuGenPES(DMX_TS_FROM_PVR1,FALSE);
              fgret=_DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
              fgret=_DmxEmuGenPES(DMX_TS_FROM_PVR3,FALSE);
             }
            break;
   }

   if(fgret == FALSE)
   {
     Printf("%s,Create pattern data error.\r\n",__FUNCTION__);
   }

   return fgret;
   
}

//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:internal demod+external demod/play card->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_FE_Remux_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL fgon=FALSE;
      UINT8 ui1_remux_mask=0x7;
      UINT16 u2Pid1, u2Pid2;
      UINT32 i;

      if(i4Argc != 5)
      {
          Printf("%s,test command:remux timestamp[0|1] remuxflag pid1 pid2.\r\n",aszArgv[0]);
          return    -1;
      }

      if(i4Argc > 1)
      {
          fgon = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
          ui1_remux_mask = StrToInt(aszArgv[2]);
      }

      if(i4Argc > 3)
      {
          u2Pid1 = StrToInt(aszArgv[3]);
      }

      if(i4Argc > 4)
      {
          u2Pid2 = StrToInt(aszArgv[4]);
      }

      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();

      _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
      _DMX_SetFrontEndEx(1, DMX_FE_EXT_P);
      /*set reocrd    pid for u1_tsidx0*/
      if(!_DMX_EMU_SetRecordPid(0,10,u2Pid1,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
    
       /*set record  pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,11,u2Pid2,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }

      /*set remux record global region.*/
      //set remux record global region
      //disable buffer
      for (i = 0; i < (FVR_GBL_COUNT-1) ; i++)
      {
          FVR_GBL_ARY_W(i, 2) = 0x100;     // Disable record region 0
      }
      _SetPVRGBLEx3(4,fgon,ui1_remux_mask);
      _FVR_Start(4);

       /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4End = FVR_GBL_ARY_W(4, 1);
       x_thread_delay(200);//should delay some time, if not, uP will not remux the ts
       while (1)
       {   
           UINT32 u4Wp;
           u4Wp = FVR_GBL_ARY_W(4, 3);
           if (u4End - u4Wp <= 192)// there is something wrong with hardware, uP can't move data to end address,but 192 to end address FIX ME!!!
           {      
               // Complete
               fgRet = TRUE;
               //Printf("    Wait count: %u\n", i);
               break;
           }     
           HAL_Delay_us(10); //because data is very large ,dely hardware.
          //x_thread_delay(20);
        }
        

       x_thread_delay(250);
       HalFlushInvalidateDCache();
       Printf("Transfter is ok.\r\n");
       FVR_Stop(4);
       
       FVR_FreePid(10);
       FVR_FreePid(11);
       FVR_FreeGBL(4);
      
       return TRUE;
        
}

//----------------------------------------------------------------
 /* description:check whetherv  the record interrupt is ok or not.
 *    test path:tuner(ts0)+playcard(ts1)+pvr(ts2)->Remux Record Buffer.
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_FE_Remux_Isr_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      UINT32 ui4_pkt_cnt0=0xFFFF,ui4_timer_notify=0;
      UINT16 u2Pid1, u2Pid2;
      if(i4Argc != 5)
      {
        Printf("%s,Test command:remuxint pkt_count timer_notify.\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        ui4_pkt_cnt0 = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
        ui4_timer_notify = StrToInt(aszArgv[2]);
      }

      if(i4Argc > 3)
      {
          u2Pid1 = StrToInt(aszArgv[3]);
      }

      if(i4Argc > 4)
      {
          u2Pid2 = StrToInt(aszArgv[4]);
      }
      
      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
      /*generate the pes datas for IP*/
      _DmxEmuGenPES(DMX_TS_FROM_PVR2,TRUE);
       /*init hw path */
       _DMX_EMU_CIPLUS_Init();
       FVR_Init();
       /*customize the IP path    */
       _DMX_EMU_CustPvrPath(IP_PVR,2,FALSE);
      /*set mini-pvr*/
      _DmxEmuSetPVRPlay(IP_PVR,FALSE);

      _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
      _DMX_SetFrontEndEx(1, DMX_FE_EXT_P);

      /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,10,u2Pid1,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
    
       /*set record  pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,11,u2Pid2,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }
      
      /*set reocrd    pid for ts_idx2*/
      u1pidx=_AllocatePididx_By_InputSource(DMX_TS_FROM_PVR2);
      if(!_DMX_EMU_SetRecordPid(2,u1pidx,PID_TABLE[u1pidx],FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx2 error.\r\n");
        return -1;
      }
      /*for pvr2 ,if you need to do record ,you just  need set bit[9] to 1, dbm->Pid filter*/
      _DMX_Set_PB_STEER_BIT_9(1,TRUE);
    
      /*set remux record global region.*/
      _SetPVRGBLEx(4,TRUE,ui4_pkt_cnt0); //set remux record global region
      _SetPVRGBLTimerNotify(4, ui4_timer_notify);
      _FVR_Start(4);
      x_thread_delay(200);
      /*start pvr engine*/
       /*enable start pvr engine*/
      _DmxEmuStartPVRPlay(IP_PVR);
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
       x_thread_delay(200);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status
    
                // Complete
                fgRet = TRUE;
                //Printf("     Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }
       x_thread_delay(10);
       HalFlushInvalidateDCache();
       Printf("Transfter is ok.\r\n");

       UINT32 i = 0;
       while(i<50)
       {
        x_thread_delay(500);
        i++;
       }
    
       _DmxEmuStopPVRPlay(IP_PVR);
       _DMX_EmuPVRPlay_FreeBuf(IP_PVR);
       FVR_Stop(4);

       
       FVR_FreePid(u1pidx);
       FVR_FreePid(10);
       FVR_FreePid(11);
       FVR_FreeGBL(4);

       return TRUE;
        
}

//----------------------------------------------------------------
 /*   function description: generate the remux data.
 *     parameter:fgon:true,the ts packet contains timestamp.
 */
//-----------------------------------------------------------------

static BOOL _DmxEmuGenFeRemuxData(BOOL fgon,UINT32 pattern, UINT16 u2Pid1, UINT16 u2Pid2)
{
      UINT32 i = 0;
      
      /*init ts parameter*/
      _DMX_EMU_TS_Parameter_Init();
      /*init hw path */
      _DMX_EMU_CIPLUS_Init();

      _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
      _DMX_SetFrontEndEx(1, DMX_FE_EXT_P);
    
       /*set record  pid for ts_idx0*/
      if(!_DMX_EMU_SetRecordPid(0,10,u2Pid1,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
      }
      
      /*set record pid for ts_idx1*/
      if(!_DMX_EMU_SetRecordPid(1,11,u2Pid2,FALSE,FALSE,FALSE))
      {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
      }

      //set remux record global region
      //disable buffer
      for (i = 0; i < (FVR_GBL_COUNT-1) ; i++)
      {
          FVR_GBL_ARY_W(i, 2) = 0x100;     // Disable record region 0
      }
      
      /*set remux record global region.*/
      _SetPVRGBLEx2(4,fgon,pattern);
      _FVR_Start(4);

      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4End = FVR_GBL_ARY_W(4, 1);
       x_thread_delay(200);//should delay some time, if not, uP will not remux the ts
       while (1)
       {   
           UINT32 u4Wp;
           u4Wp = FVR_GBL_ARY_W(4, 3);
           if (u4End - u4Wp <= 192)// there is something wrong with hardware, uP can't move data to end address,but 192 to end address FIX ME!!!
           {      
               // Complete
               fgRet = TRUE;
               //Printf("    Wait count: %u\n", i);
               break;
           }
           HAL_Delay_us(10); //because data is very large ,dely hardware.
          //x_thread_delay(20);
        }

       x_thread_delay(250);
       HalFlushInvalidateDCache();
       FVR_Stop(4);

       /*get ciplus1.4 record buffer.*/
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Start= FVR_GBL_ARY_W(4, 0);
       _arTsGoldenFile[DMX_TS_FROM_PVR3].TS_Addr_Offset= FVR_GBL_ARY_W(4,3);
       Printf("%s:here\n",__FUNCTION__);
       FVR_FreePid(10);
       FVR_FreePid(11);
       FVR_FreeGBL(4);

       return TRUE;
        
}

//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *    test path:tuner+playcard -> redcord buffer  -> tsout 
 */
//-----------------------------------------------------------------
static INT32 _DMXCI_Fe_MultiStream_Playback_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
      UINT8 u1pidx=0;
      BOOL fg_ts_idx0_bypass=FALSE,fg_ts_idx1_bypass=FALSE;
      UINT16 u2Pid1, u2Pid2;
      BOOL b_match0=TRUE,b_match1=TRUE,b_match2=TRUE;
      UINT32 SrcpesAddr,DstpesAddr,u4size;
      UINT32 u4SrcDiff,u4DstDiff;
      UINT32 u4Reg,i=0;
      if(i4Argc != 5)
      {
        Printf("%s,Test command:multiplay ts_idx0_bypass[0|1],     \
                 ts_idx1_bypass[0|1],u2Pid1,u2Pid2.\r\n",aszArgv[0]);
        return    -1;
      }

      if(i4Argc > 1)
      {
        fg_ts_idx0_bypass = StrToInt(aszArgv[1]);
      }

      if(i4Argc > 2)
      {
        fg_ts_idx1_bypass = StrToInt(aszArgv[2]);
      }

      if(i4Argc > 3)
      {
        u2Pid1 = StrToInt(aszArgv[3]);
      }

      if(i4Argc > 4)
      {
        u2Pid2 = StrToInt(aszArgv[4]);
      }

        /*generate the remux datas*/
      _DmxEmuGenFeRemuxData(FALSE,0x40414243, u2Pid1, u2Pid2);
       Printf("%s:GenData finish\n",__FUNCTION__);
       /*set hardware path*/
       /*init hardaware path*/
       //DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<29)));
       DMXCMD_WRITE32(DMX_REG_DBM_CONTROL,(DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<28)));
       _DMX_EMU_CustPvrPath(CI_REMUX_PVR,2,FALSE); 
        /*enable remux pid filter*/
       DmxSetLocalTSEnable(TRUE,FALSE);
        /*set ts_idx3 for multi-stream playback settings*/
       DmxSetTSStream(2,MULTI_STREAM);
       DMXSetLocalTSID(2,0x40414243,0x3); /*ts3,ts2,ts1,ts0*/

      // Set Full pull mode
      u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
      u4Reg |= 0xC0000000;
      DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

        /*init dst buffer*/
      for(i=0;i<DMX_TEST_PID_TOTAL_NUM;i++)
      {
        x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
      }
   
#if 1
    /*for FPGA, you need config it ts-out two-bit.*/
    if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(2, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    
#endif
    /*set multi-stream playback pid */
      if(!fg_ts_idx0_bypass)
      {
        //_DMX_EMU_SetPlayPidEx1(2,0,10,u2Pid1,DMX_PID_TYPE_PES);
        _DMX_EMU_SetPlayPcrPid(2,0,10,u2Pid1,DMX_PID_TYPE_PES);
      }
      
      if(!fg_ts_idx1_bypass)
      {
        //_DMX_EMU_SetPlayPidEx1(2,1,11,PID_TABLE[1],DMX_PID_TYPE_PES);
        _DMX_EMU_SetPlayPcrPid(2,1,11,u2Pid2,DMX_PID_TYPE_PES);
      }
      
       /*set mini-pvr buffer and whether timestamp is used.*/
      if(_DmxEmuSetPVRPlayExt(CI_REMUX_PVR,FALSE,FALSE) == FALSE)
      {
        Printf("%s ,set pvr buffer fails.\r\n",__FUNCTION__);
        return FALSE;
      }
      
      /*start  pvr engine*/
        /*enable start pvr engine*/
      Printf("%s:PVR start\n",__FUNCTION__);
      _DmxEmuStartPVRPlay(CI_REMUX_PVR);
      /*wait for finish*/
       BOOL fgRet = FALSE;
       UINT32 u4Rp;
       UINT32 u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
        while (1)
        {
            UINT32 u4Rp;
            u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
            if (u4Rp == u4Wp)
            {
                ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status

                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            HAL_Delay_us(10); //because data is very large ,dely hardware.
        }

    /*free source*/
    _DmxEmuStopPVRPlay(CI_REMUX_PVR);
    Printf("%s:PVR stop\n",__FUNCTION__);
    _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);
    Printf("%s:here1\n",__FUNCTION__);
    _DMX_EMU_FreePlaypid(10);
    _DMX_EMU_FreePlaypid(11);
    Printf("%s:here2\n",__FUNCTION__);
          
    return TRUE;
        
}


//----------------------------------------------------------------
 /*   description:check whetherv record for every ts_idx  is ok
 *     test path:tuner(ts0)+playcard(ts1)->Remux Record Buffer->pvr3->tsout2->ts3.
 *     make sure that:the local ts id is unique for every ts_idx which need be remuxed.
 */
//-----------------------------------------------------------------
static INT32 _DMXCIPLUS_Fe_Multi_Play_Test(INT32 i4Argc, const CHAR ** aszArgv)
{

    UINT16 u2Pid1, u2Pid2;
    UINT32 u4Reg,i=0;
    UINT8 u1TsIn = 3;
    UINT8 u1TsOut = 2;
    if(i4Argc != 3)
    {
        Printf("%s,Test command:feplay pid1 pid2",aszArgv[0]);
        return    -1;
    }

    if(i4Argc > 1)
    {
        u2Pid1 = StrToInt(aszArgv[1]);
    }

    if(i4Argc > 2)
    {
        u2Pid2 = StrToInt(aszArgv[2]);
    }
    DMX_CiPlus14_Init();
    /*Init the golden file */
    _DMX_EMU_TS_Parameter_Init();

    /*init hardaware path*/
    //_DMX_EMU_CIPLUS_Init();

    _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
    _DMX_SetFrontEndEx(1, DMX_FE_EXT_P);



    //disable buffer
    for (i = 0; i < (FVR_GBL_COUNT-1) ; i++)
    {
        FVR_GBL_ARY_W(i, 2) = 0x100;     // Disable record region 0
    }


    DMX_CI_Set_StreamMode(MULTI_STREAM_MODE);
    DMX_CI_Set_MultiStream_TsIdx(u1TsIn);
    for(i=0;i<4;i++)
    {
        DMX_CI_LOCAL_TS_STRUCT_T rLocalTs;
        rLocalTs.u1LocalId = 0x53 - i;
        rLocalTs.uiLocalTsidx = i;
        if(i == 0 || i == 1)
        {
            rLocalTs.bValid = TRUE;
        }
        else
        {
            rLocalTs.bValid = FALSE;
        }
        DMX_CI_Set_MultiStream_LocalTs(&rLocalTs);
    }
       
   
    DMX_CI_Set_PvrBuffer(u1TsOut);

    if(!_DMX_EMU_SetRecordPidExt(0,0,u2Pid1,FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
    }
    
    if(!_DMX_EMU_SetRecordPidExt(1,1,u2Pid2,FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx1 error.\r\n");
        return -1;
    }
    
    if(!_DMX_EMU_SetPlayPidEx1(u1TsIn,0,0,u2Pid1,DMX_PID_TYPE_PES))//u1Tsidx = 2 or 3?
    {
        Printf("set play pid for ts_idx0 error.\r\n");
        return -1;
    }
        
    if(!_DMX_EMU_SetPlayPidEx1(u1TsIn,1,1,u2Pid2,DMX_PID_TYPE_PES))
    {
       Printf("set play pid for ts_idx1 error.\r\n");
       return -1;
    }


    u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~0xFFFF | 0x60A8;
        
    //ci tso/ci tso2 data clock enable (1:diable,0:enable), default is disable
    //bit31:CI TSO2 sync,data,valid output disable
    //bit30:CI TSO   sync,data,valid output disable
    //bit29:CI TSO2 clock output disable
    //bit28:CI TSO   clock output disable
    u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(0xF<<28);
    CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

    //bypass CI framer
    //0: from the CI framer 
    //1: bypass CI framer, mux reslut direct output. 
    //bit0:CI TSO output model select 
    //bit1:CI TSO2 output model select 
    u4Reg = CKGEN_READ32(CKGEN_TSIF2_CFG) | 0xF;
    CKGEN_WRITE32(CKGEN_TSIF2_CFG, u4Reg);
    _DMX_CI_SetTsoutPath(u1TsOut);
    _DMX_SetFrontEndEx(u1TsIn, DMX_PINMUX_EXT4_S);
     
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    _DMX_PVRPlay_SetPort(u1TsOut,DMX_PVRPLAY_PORT_TSOUT);

    if (!DMX_TsOut_SetDataWidthEx(u1TsOut, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    if (!DMX_TsIn_SetDataWidth(u1TsIn, DMX_TSOUT_DATAWIDTH_1))
    {
    Printf("Failed to set data width!\n");
    return -1;
    }
    
    if(!DMX_PVRPlay_StartEx(u1TsOut))
    {
        Printf("DMX_PVRPlay_StartEx fail!\n ");
    }   
    //x_thread_delay(250); 
    HalFlushInvalidateDCache();
    DMX_CI_Record_Start();
      
    i=0;
    while(i<50)
    { 
        x_thread_delay(500); //delay 150ms
        i++;
    }

    Printf("Data Transfer is ok ,input <stop> command  to compare the datas.\r\n ");
    HalFlushInvalidateDCache();
    /*compare golden pes files*/  
    return TRUE;
        
}

static INT32 _DMXCIPLUS_Fe_Multi_Stop_Test(INT32 i4Argc, const CHAR ** aszArgv)
{
  
  if(i4Argc != 1)
  {
    Printf("%s,Test command:festop",aszArgv[0]);
    return    -1;
  }

  DMX_CI_Record_Stop();

  _DmxEmuStopPVRPlay(CI_REMUX_PVR); //stop remux pvr playback

  /*free playback pid */
  _DMX_EMU_FreePlaypid(0);
  _DMX_EMU_FreePlaypid(1); /*for multi-stream playback*/
  /*for ci+1.4 remux record pid*/
  FVR_FreePid(0);
  FVR_FreePid(1); 
  /*free playback or record buffer*/
  _DMX_EmuPVRPlay_FreeBuf(CI_REMUX_PVR);
  Printf("Stop finish.\n");
}

static INT32 _DMXCIPLUS_r3(INT32 i4Argc, const CHAR ** aszArgv)
{

    UINT16 u2Pid1, u2Pid2;
    UINT32 u4Reg,i=0;
    DMX_CI_LOCAL_TS_STRUCT_T rLocalTs;
    UINT32 u4Addr=0,u4Size=0;
    UINT32 u4Wp,u4Rp;
    UINT32 u4PvrWp=0;
    if(i4Argc != 3)
    {
        Printf("%s,Test command:feplay pid1 pid2",aszArgv[0]);
        return    -1;
    }

    if(i4Argc > 1)
    {
        u2Pid1 = StrToInt(aszArgv[1]);
    }

    if(i4Argc > 2)
    {
        u2Pid2 = StrToInt(aszArgv[2]);
    }

    
    
    /*init ES dst buffer*/
    for(i=0;i<(DMX_TEST_PID_TOTAL_NUM+1);i++)
    {
      _arPidBuffer[i].u4BufAddr = PES_RESULT_ADDR+i*PES_RESULT_PERPID_SIZE;
      _arPidBuffer[i].u4BufSize = PES_RESULT_PERPID_SIZE;
      x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
    }
   
      
    //generate the remux datas:
    _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
    //enable multi-stream
    DMX_CI_Set_StreamMode(MULTI_STREAM_MODE);
    //enable multi-tsidx
    DMX_CI_Set_MultiStream_TsIdx(2);
    //set  local id pattern
    for(i=0;i<DMX_FRAMER_TOTAL_INDEX;i++)
   {
     if((i==2) || (i==3))
     {
       rLocalTs.bValid = FALSE;
     }
     else
     {
       rLocalTs.bValid = TRUE;
     }
     rLocalTs.u1LocalId = (0x48+i);
     rLocalTs.uiLocalTsidx =i;
     _DMX_CI_Set_MultiStream_LocalTs(&rLocalTs);
   }

    //start record 
    DMX_CI_Record_Start();

    //set record pid for remux 
     if(!_DMX_EMU_SetRecordPidExt(0,0,u2Pid1,FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
    }

    while (1)
    {   
       UINT32 u4Wp1;
       u4Wp1 = FVR_GBL_ARY_W(4, 3);
       x_thread_delay(100);
       UINT32 u4Wp2;
       u4Wp2 = FVR_GBL_ARY_W(4, 3);
       if(u4Wp1==u4Wp2)
       {
           if(DMXCMD_READ32(DMX_REG_FTuP_ERROR_STATUS_REG) & 0x2)
           {
               FVR_Stop(4);
           }
           break;
       }      
   }
   u4PvrWp = FVR_GBL_ARY_W(4, 3);
   HalFlushInvalidateDCache();
   FVR_FreePid(0);
   FVR_FreeGBL(4);

   Printf("Remux is ok,Continue Playback.\r\n");

    //set ts-out2->ts_in->framer2
    _DMX_PVRPlay_SetPort(2,DMX_PVRPLAY_PORT_FRAMER2);

    //set ts->out->serial
       if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
    {
     Printf("Failed to set data width!\n");
     return -1;
    }
    if (!DMX_TsIn_SetDataWidth(2, DMX_TSOUT_DATAWIDTH_1))
    {
     Printf("Failed to set data width!\n");
     return -1;
    }

    //set mini-pvr2 buffer
    _DMX_CI_Set_PvrBuffer(2);
    
    //update wp for mini-pvr2
    //_DMX_CI_GetRecordBuffer(&u4Addr,&u4Size);

     DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP3,u4PvrWp);
    
    //set playback pid for multi-stream(2),local ts_idx=0.
    _DMX_EMU_SetPlayPidEx1(2,0,0,u2Pid1,DMX_PID_TYPE_PES);

    //start pvr
    DMX_PVRPlay_StartEx(2);

    u4Wp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
    while (1)
    {
        u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
        if (u4Rp == u4Wp)
        {
            ASSERT((DMXCMD_READ32(DMX_REG_PVR_READ_BUFFER_STATUS) & 0x1) == 0); // 0x17380, pvr read buffer status
             break;
        }
         HAL_Delay_us(10); //because data is very large ,dely hardware.
   }

   i=0;
   while(i<50)
   {
     x_thread_delay(250);
     i++;
   }

       /*free source*/
    _DMX_PVRPlay_Stop(2,TRUE);
    _DMX_PVRPlay_FreeBuf(2);

    _DMX_EMU_FreePlaypid(0);

    Printf("Playback is Finish.\r\n");

    return TRUE;
}


static INT32 _DMXCIPLUS_r3_i(INT32 i4Argc, const CHAR ** aszArgv)
{

    UINT16 u2Pid1, u2Pid2;
    UINT32 u4Reg,i=0;
    DMX_CI_LOCAL_TS_STRUCT_T rLocalTs;
    UINT32 u4Addr=0,u4Size=0;
    UINT32 u4Wp,u4Rp;
    UINT32 u4PvrWp=0;
    if(i4Argc != 3)
    {
        Printf("%s,Test command:feplay pid1 pid2",aszArgv[0]);
        return    -1;
    }

    if(i4Argc > 1)
    {
        u2Pid1 = StrToInt(aszArgv[1]);
    }

    if(i4Argc > 2)
    {
        u2Pid2 = StrToInt(aszArgv[2]);
    }
    //int thread 
    _DMX_CiPlus14_Init();
    
    /*init ES dst buffer*/
    for(i=0;i<(DMX_TEST_PID_TOTAL_NUM+1);i++)
    {
      _arPidBuffer[i].u4BufAddr = PES_RESULT_ADDR+i*PES_RESULT_PERPID_SIZE;
      _arPidBuffer[i].u4BufSize = PES_RESULT_PERPID_SIZE;
      x_memset((VOID*)_arPidBuffer[i].u4BufAddr,0,_arPidBuffer[i].u4BufSize);
    }
   
      
    //generate the remux datas:
    _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
    //enable multi-stream
    DMX_CI_Set_StreamMode(MULTI_STREAM_MODE);
    //enable multi-tsidx
    DMX_CI_Set_MultiStream_TsIdx(2);
    //set  local id pattern
    for(i=0;i<DMX_FRAMER_TOTAL_INDEX;i++)
   {
     if((i==2) || (i==3))
     {
       rLocalTs.bValid = FALSE;
     }
     else
     {
       rLocalTs.bValid = TRUE;
     }
     rLocalTs.u1LocalId = (0x48+i);
     rLocalTs.uiLocalTsidx =i;
     _DMX_CI_Set_MultiStream_LocalTs(&rLocalTs);
   }
    //mulit-stream mode
    //1,change  ts out path: tsout2->ts_in->framer2->
    _DMX_PVRPlay_SetPort(2,DMX_PVRPLAY_PORT_FRAMER2);

       if (!DMX_TsOut_SetDataWidthEx(2, DMX_TSOUT_DATAWIDTH_1))
    {
     Printf("Failed to set data width!\n");
     return -1;
    }
    if (!DMX_TsIn_SetDataWidth(2, DMX_TSOUT_DATAWIDTH_1))
    {
     Printf("Failed to set data width!\n");
     return -1;
    }
    //2,set mini-pvr2 buffer
    _DMX_CI_Set_PvrBuffer(2);
    //3,start mini-pvr2 buffer
    DMX_PVRPlay_StartEx(2);

    //start record 
    DMX_CI_Record_Start();

    //set multi-playback pid{multi-stream(2),local ts_idx=0.}
    _DMX_EMU_SetPlayPidEx1(2,0,0,u2Pid1,DMX_PID_TYPE_PES);
    
    //set record pid for remux 
     if(!_DMX_EMU_SetRecordPidExt(0,0,u2Pid1,FALSE,FALSE,FALSE,DMX_PID_TYPE_PES))
    {
        Printf("set record pid for ts_idx0 error.\r\n");
        return -1;
    }

 
   while(1)
   {
     x_thread_delay(250);
   }

    //stop operation
    FVR_Stop(4);
    FVR_FreeGBL(4);
    FVR_FreePid(0);

    _DMX_PVRPlay_Stop(2,TRUE);
    _DMX_PVRPlay_FreeBuf(2);
    _DMX_EMU_FreePlaypid(0);
    
    Printf("Playback is Finish.\r\n");

    return TRUE;
}



//-----------------------------------------------------------------------------
// cli command table
//-----------------------------------------------------------------------------

/*add new command  list for ci1.l4*/
CLI_EXEC_T _arCiEmuCmdTbl[] =
{
  {
      "ci1.4 pattern test","pt",_DMXCI_PatternTest,NULL,"ci pattern test: pt [0-4] ",CLI_SUPERVISOR
  }, 
  {
      "ci1.4 framer test","ddiframer",_DMXCI_DDI_TS_TO_FramerTest,NULL,"ci1.4 ddi to framer test:ddiframer ",CLI_SUPERVISOR
  },  
  {
    "ci1.4 ddi_to_ca test","ddica",_DMXCI_DDI_TS_TO_CA,NULL,"test command:ddica",CLI_SUPERVISOR
  },
  {
    "ci1.4 ip test","ip",_DMXCI_IP_TS_TO_DEMUX,NULL,"test command:ip",CLI_SUPERVISOR
  },
  {
    "ci1.4 record test","record",_DMXCI_Record_Test,NULL,"test command:record",CLI_SUPERVISOR
  },
  {
    "ci1.4 remux test","remux",_DMXCI_Remux_Test,NULL,"test command:remux timestamp[0|1] remuxflag",CLI_SUPERVISOR
  },
  {
    "ci1.4 remux test","remuxint",_DMXCI_Remux_Interrupt_Test,NULL,"test command:remuxint pkt_count timer",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-play test","multiplay",_DMXCI_MultiStream_Playback_Test,NULL,"test command:multiplay",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-ca test","multica",_DMXCI_MultiStream_CaOut_Test,NULL,"test command:multica",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-timestamp test","multitime",_DMXCI_TimeStamp_CaOut_Test,NULL,"test command:multitime",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-play pcr test","pcr",_DMXCI_MultiStream_PCR_Test,NULL,"test command:pcr",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-play integration test","play",_DMXCIPLUS_Multi_Play_Test,NULL,"test command:play",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-stop integration test","stop",_DMXCIPLUS_Multi_Stop_Test,NULL,"test command:stop",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-play scramble test","dplay",_DMXCIPLUS_Multi_Scramble_Play_Test,NULL,"test command:dplay",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-stop scramble test","dstop",_DMXCIPLUS_Multi_Scramble_Stop_Test,NULL,"test command:dstop",CLI_SUPERVISOR
  },
  {
    "ci1.4 multi-cl  test","cl",_DMXCIPLUS_CFlag_Test,NULL,"test command:cl",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end remux test","feremux",_DMXCI_FE_Remux_Test,NULL,"test command:feremux timestamp[0|1] remuxflag pid1 pid2",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end remux test","feremuxint",_DMXCI_FE_Remux_Isr_Test,NULL,"test command:feremuxint pkt_count timer pid1 pid2",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end pcr playback test","femultiplay",_DMXCI_Fe_MultiStream_Playback_Test,NULL,"test command:feremuxint pkt_count timer pid1 pid2",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end multi-play integration test","feplay",_DMXCIPLUS_Fe_Multi_Play_Test,NULL,"test command:feplay pid1 pid2",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end multi-stop integration test","festop",_DMXCIPLUS_Fe_Multi_Stop_Test,NULL,"test command:festop",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end r3 integration test","r3",_DMXCIPLUS_r3,NULL,"test command:r3",CLI_SUPERVISOR
  },
  {
    "ci1.4 front end r3 integration test","r3_i",_DMXCIPLUS_r3_i,NULL,"test command:r3_i",CLI_SUPERVISOR
  },
  {
    NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR
  }
};

