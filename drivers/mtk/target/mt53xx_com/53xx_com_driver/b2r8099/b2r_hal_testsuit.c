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
 * $RCSfile: b2r_hal_testsuit.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file b2r_hal_testsuit.c
 *  this file contains function test.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_drv_cli.h"
#include "b2r_debug.h"
#include "b2r_hal.h"

#ifdef B2R_EMU
#include "x_stl_lib.h"
#include "x_bim.h"
#include "x_hal_arm.h"
#include "x_mid.h"
#include "fbm_drvif.h"
#include "b2r_emu.h"
#include "vdp_frc.h"
#include "vdp_hw.h"
#include "b2r_timing.h"

//-----------------------------------------------------------------------------
// Configurations for test case
//-----------------------------------------------------------------------------


#define B2R_EMU_TMP_BUFF_SIZE      (1024*10)
#define B2R_EMU_CFG_BUFF_SIZE      (1024*48)

#define B2R_RATE_DEFAULT            _u4B2rRate
#define B2R_SCALE_DEFAULT           _u4B2rScale


#define B2R_BUFF_SIZE               0x1C00000    // 10M
#define B2R_BUFF_OFFSET             0x1380000
#define EMU_SRC_DEFAULT             EMU_SRC_USB
#define B2R_SCRIPT_SIZE             30*1024
#define B2R_TEST_FILE               "f:\\FPGA_82xx\\lists.txt"
#define B2R_PATH_SIZE               512



#define B2R0_MAX_WIDTH               3840
#define B2R0_MAX_HEIGHT              2176
#define B2R1_MAX_WIDTH               1920
#define B2R1_MAX_HEIGHT              1088

#define EMU_NOTIFY_Q_NUM             10

#define FLIP_MASK                    0x1
#define MIRROR_MASK                  0x2
#define PATH_1                       0
#define PATH_2                       1
#define PATH_NS                      2
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define ST_PRFX_MASK        0xff00 
#define CRC_ST_PRFX         0x100 
#define VSYNC_ST_PRFX       0x200 

static __TIMING_TBL_T VB1TimingTab[] =
{
    {"1920x2160/30P",   2200, 2250, 120,   1,    297000000}
   
};

typedef enum
{
    EMU_ST_IDLE              = 0,
    EMU_ST_CRC_START         = CRC_ST_PRFX + 1,
    EMU_ST_CRC_WAIT          = CRC_ST_PRFX + 2,
    EMU_ST_CRC_DOING         = CRC_ST_PRFX + 3,
    EMU_ST_CRC_DONE          = CRC_ST_PRFX + 4,

    EMU_ST_VSYNC_START       = VSYNC_ST_PRFX + 1,
    EMU_ST_VSYNC_DOING       = VSYNC_ST_PRFX + 2,
    EMU_ST_VSYNC_DONE        = VSYNC_ST_PRFX + 3,

    EMU_ST_MAX

} EMU_ST_T;

typedef struct
{
    EMU_ST_T eSt;
} EMU_NTY_MSG_T;


typedef struct 
{
    UINT32 HTotal;
    UINT32 VTotal;
    UINT32 u4MaxWidth;
    UINT32 u4MaxHeight;
    UINT32 u4DefRate;
    UINT32 u4DefScale;
} B2R_HVTotal_T;

typedef struct 
{
    UINT32 u4Max;
    UINT32 u4Min;
    UINT32 u4Ave;
    UINT32 u4Last;
} B2R_VSYNC_T;

typedef struct 
{
    B2R_HAL_COMM;

    //working buffer
    UINT32 u4BuffAddr;
    UINT32 u4BuffSize;
    UINT32 u4BuffPtr;

    CHAR *pcScriptPtr;

    //list
    EMU_LIST_T *ptLists;
    UINT32 u4MaxList;
    UINT32 u4Lists;
    EMU_LIST_HEAD_T rListHead;
    EMU_LIST_T *ptCurrList;

    //item
    EMU_ITEM_T *ptItems;
    UINT32 u4MaxItem;
    UINT32 u4Items;
    EMU_ITEM_HEAD_T rItemHead;
    EMU_ITEM_T *ptCurrItem;

    B2R_CFG_FILE tCfg;
    UINT32 TWO_ADDR;
    B2R_HANDLE_T hB2r;
    BOOL fgOldB2r;
    CHAR* pcPath;

    UINT32 u4Rate;
    UINT32 u4Scale;

    //Left
    UINT32 u4YAddr0;
    UINT32 u4CAddr0;
    //Right
    UINT32 u4YAddr1;
    UINT32 u4CAddr1;

    //Left Len
    UINT32 u4YAddrLen0;
    UINT32 u4CAddrLen0;

    //Right Len
    UINT32 u4YAddrLen1;
    UINT32 u4CAddrLen1;

    //CRC 
    UINT32 u4CRC0[8];
    UINT32 u4CRC1[8];
    UINT32 u4CRCFlag0;
    UINT32 u4CRCFlag1;

    UINT32 au4PatCRC0[8];
    UINT32 au4PatCRC1[8];

    EMU_ST_T eSt;
    HANDLE_T hNotifyMsg;

    UINT32 u4Idx;
    UINT32 u4Total;
    UINT32 u4WaitCnt;
    
    B2R_VSYNC_T tVsync;

} B2R_EMU_HOST_T;


typedef struct 
{
    UINT32 u4Data;
    VOID * pvData;
} B2R_CfG_T;

typedef struct 
{
    UINT32 u4Start;
    UINT32 u4End;
    UINT32 u4Step;
} B2R_RANGE_T;

typedef struct 
{
    CHAR acYRaw[128];
    CHAR acYRawLen[128];
    CHAR acCRaw[128];
    CHAR acCRawLen[128];
    CHAR acY422[128];
    CHAR acC422[128];
    UINT32 u4RawPitch;
    UINT32 u4422Pitch;
    UINT32 u4Depth;
    B2R_PACK_MODE_T ePackMode;
} B2R_EXT_DATA;

typedef struct 
{
    UINT32 u4Reg;
    UINT32 u4Value;
} B2R_REG_EXT;

typedef struct
{
    UINT8 u1Id;
    BOOL  fgOldB2r;
    BOOL  fgFlip;
    BOOL  fgMirror;
    BOOL  bFgEn;
} B2R_DYNAMIC_PAR_T;

//-----------------------------------------------------------------------------
// extern functions
//-----------------------------------------------------------------------------
extern void _B2REmu_HW_V1_Init(UINT8 u1Id);
extern void _B2REmu_HW_V1_ISR(UINT16 u2Vector);

extern void _B2REmu_HW_V2_Init(UINT8 u1Id);
extern void _B2REmu_HW_V2_ISR(UINT16 u2Vector);

//from b2r_cmodel
int b2r_cmodel_get_crc(void);
int b2r_cmodel_get_field_crc(int top);
void b2r_cmodel_set_parm(int _hdew, int _vdew);
void b2r_cmodel_get_addr(unsigned int *_y_dram, unsigned int *_c_dram);
void b2r_cmodel_init(void);
void b2r_cmodel_do(void);
void b2r_cmodel_do_ext(char *raw_y, char * raw_c, char *y_422, char * uv_422, int pitch);
void b2r_cmodel_set_crc_mode(int isOld);
void b2r_cmodel_set_color(unsigned int _y_color, unsigned int _c_color);
void b2r_cmodel_set_max_hv(unsigned int max_hdew, unsigned int max_vdew, unsigned int pitch);
void b2r_cmodel_set_bit_depth(int depth);




//-----------------------------------------------------------------------------
// test case declare
//-----------------------------------------------------------------------------
static INT32 _B2rEmuSetPar(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuStressTestPar(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase1(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase2(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase3(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase4(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase5(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase6(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase7(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase8(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase9(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rEmuTestCase10(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rStartTimingChg(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _B2rStop(INT32 i4Argc, const CHAR ** szArgv);

VOID _B2rEmuTestList(CHAR *pucListPath, UINT8 u1Path);
UINT8 _B2rFindPath(B2R_EMU_HOST_T *ptHost);

//-----------------------------------------------------------------------------
// test case table
//-----------------------------------------------------------------------------
CLI_EXEC_T _arB2rV1TestCaseTbl[] =
{
    {"sdp",   NULL,  _B2rEmuSetPar,  NULL, "Single test for 2D",          CLI_SUPERVISOR},
    {"sts",   NULL,  _B2rEmuStressTestPar,  NULL, "Stress Test",          CLI_SUPERVISOR},   
    {"t1",    NULL,  _B2rEmuTestCase1,  NULL, "Single test for 2D",          CLI_SUPERVISOR},
    {"t2",    NULL,  _B2rEmuTestCase2,  NULL, "Single test for two Addr",    CLI_SUPERVISOR},
    {"t3",    NULL,  _B2rEmuTestCase3,  NULL, "Standard timing test",        CLI_SUPERVISOR},
    {"t4",    NULL,  _B2rEmuTestCase4,  NULL, "Function Test case 3",        CLI_SUPERVISOR},
    {"t5",    NULL,  _B2rEmuTestCase5,  NULL, "Batch Test",                  CLI_SUPERVISOR},
    {"t6",    NULL,  _B2rEmuTestCase6,  NULL, "Two B2R Batch Test",          CLI_SUPERVISOR},
    {"t7",    NULL,  _B2rEmuTestCase7,  NULL, "Just Run B2R",                CLI_SUPERVISOR},
    {"t8",    NULL,  _B2rEmuTestCase8,  NULL, "Just Stop B2R",               CLI_SUPERVISOR},
    {"t9",    NULL,  _B2rEmuTestCase9,  NULL, "Boundary Test",               CLI_SUPERVISOR},
    {"t10",    NULL, _B2rEmuTestCase10,  NULL, "VB1 Load raw data",           CLI_SUPERVISOR},
    {"t11",    "st", _B2rStartTimingChg,  NULL, "VB1 Start Timing change",           CLI_SUPERVISOR},
    {"t12",    "sp", _B2rStop,  NULL, "VB1 Timing Stop",           CLI_SUPERVISOR},
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};


B2R_HVTotal_T HVTotal[] =
{
   {858, 525, 720, 480, 30, 1},
   {864, 625, 720, 576, 25, 1},
   {1650, 750, 1280, 720, 30, 1},
   {2200, 1125, 1920, 1080, 30, 1},
   {4400, 2250, 3840, 2160, 30, 1}
};

UINT32 FpsBase[] = {5, 10, 20, 30, 60, 120, 6, 12, 24, 48, 25, 50, 100 };


//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
#define B2R_MAX_NS   2
static INT32 _u4B2rTimingChg = 0xFF;
static BOOL  fgB2ROpen=FALSE;
static BOOL  fgB2RChg =FALSE;
static UINT32  u4Y0;
static UINT32  u4C0;
static UINT32  u4Y0_Len;
static UINT32  u4C0_Len;
static UINT32  u4Y1;
static UINT32  u4C1;
static UINT32  u4Y1_Len;
static UINT32  u4C1_Len;
static UINT32  u4B2rTimingStatus;
static B2R_CFG_FILE *ptCfgFileG;


static B2R_EMU_HOST_T _rB2rEmuHost[2];
static HANDLE_T _hNotifyMsgQ[2];

static UINT32 _u4B2rRate  = 30;
static UINT32 _u4B2rScale = 1;

static UINT32 _u4B2rHDEW  = 704;
static UINT32 _u4B2rVDEW  = 480;
static UINT32 _u4B2rPitch = 768;

static CHAR   _ucFilePath[256];
B2R_EXT_DATA  _b2rExtData;

B2R_REG_EXT _arRegExt[8];
static B2R_DYNAMIC_PAR_T _b2r_par[PATH_NS];

static UINT32 _uStressTestCnt = 0;
static UINT8*  pu1StressFilepath = NULL;


//-----------------------------------------------------------------------------
// function declares
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Macro 
//-----------------------------------------------------------------------------
#define _B2rEmuGetBufferPtr(obj)      (obj->u4BuffPtr)
#define _B2rEmuGetBuffer(obj,_size)   (obj->u4BuffPtr); obj->u4BuffPtr += (_size)
#define _B2rEmuChkBufferOk(obj)       ((obj->u4BuffPtr > obj->u4BuffAddr + obj->u4BuffSize)?FALSE:TRUE)
#define _B2rEmuAlignBufferAddr(obj,a)  obj->u4BuffPtr=((obj->u4BuffPtr + (a-1))& (~(a-1)))

#define _B2rEmuChkId(id)  if (id > 1) { return; }

#define FOR_EACH_FRAME(a)   for(;a->u4Idx < a->u4Total;a->u4Idx ++)


#define _B2rGetRefTime() *((UINT32 *)0xf000004c)
#define _B2rResetRefTime() *((UINT32 *)0xf000004c) = 0

//-----------------------------------------------------------------------------
// static  functions
//-----------------------------------------------------------------------------
inline B2R_EMU_HOST_T * _B2rEmuId2Obj(UINT8 id)
{
    return (id < 2) ? (&_rB2rEmuHost[id]) : NULL;
}

static VOID _B2rIsrCb_CRC_Old(B2R_EMU_HOST_T *ptHost, B2R_HAL_STATUS_T *ptStatus)
{
    if (!ptHost || !ptStatus)
    {
        return;
    }

    //record CRC
    switch (ptHost->eSt)
    {
    default:
        break;
    case EMU_ST_CRC_START:
        
        if (ptHost->tCfg.OUT_I && ptHost->TWO_ADDR)
        {
            ptHost->u4CRCFlag0 = 0x03;
            ptHost->u4CRCFlag1 = 0x03;
        }
        else if (ptHost->TWO_ADDR)
        {
            ptHost->u4CRCFlag0 = 0x01;
            ptHost->u4CRCFlag1 = 0x01;
        }
        else if (ptHost->tCfg.OUT_I)
        {
            ptHost->u4CRCFlag0 = 0x03;
            ptHost->u4CRCFlag1 = 0x0;
        }
        else 
        {
            ptHost->u4CRCFlag0 = 0x01;
            ptHost->u4CRCFlag1 = 0x0;
        }
        ptHost->u4WaitCnt = 0;
        ptHost->eSt = EMU_ST_CRC_WAIT;
        break;
    case EMU_ST_CRC_WAIT:
        if (ptHost->u4WaitCnt++ > 8)
        {
            ptHost->eSt = EMU_ST_CRC_DOING;
        }
        break;
    case EMU_ST_CRC_DOING:
        
        //record crc
        if (ptHost->tCfg.OUT_I && ptHost->TWO_ADDR)
        {
            //left top
            if ((ptHost->u4CRCFlag0 & 0x1) && ptStatus->fgLeft && !ptStatus->fgTop)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //left bottom
            if ((ptHost->u4CRCFlag0 & 0x2) && ptStatus->fgLeft && ptStatus->fgTop)
            {
                ptHost->u4CRC0[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x2;
            }

            //right top
            if ((ptHost->u4CRCFlag1 & 0x1) && !ptStatus->fgLeft && !ptStatus->fgTop)
            {
                ptHost->u4CRC1[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x1;
            }
            //right bottom
            if ((ptHost->u4CRCFlag1 & 0x2) && !ptStatus->fgLeft && ptStatus->fgTop)
            {
                ptHost->u4CRC1[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x2;
            }
        
        }
        else if (ptHost->TWO_ADDR)
        {
            //left
            if ((ptHost->u4CRCFlag0 & 0x1) && ptStatus->fgLeft)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //right
            if ((ptHost->u4CRCFlag1 & 0x1) && !ptStatus->fgLeft)
            {
                ptHost->u4CRC1[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x1;
            }
        }
        else if (ptHost->tCfg.OUT_I)
        {
            //top 
            if ((ptHost->u4CRCFlag0 & 0x1) && ptStatus->fgTop)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //bottom 
            if ((ptHost->u4CRCFlag0 & 0x2) && !ptStatus->fgTop)
            {
                ptHost->u4CRC0[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x2;
            }
        }
        else
        {
            if (ptHost->u4CRCFlag0 & 0x1)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }
        }
        
        if (!ptHost->u4CRCFlag0 && !ptHost->u4CRCFlag1)
            ptHost->eSt = EMU_ST_CRC_DONE;
        
        break;
    case EMU_ST_CRC_DONE:
        // notify 
        ptHost->eSt = EMU_ST_IDLE;
        if (ptHost->hNotifyMsg)
        {
            EMU_NTY_MSG_T rMsg;
            rMsg.eSt = EMU_ST_CRC_DONE;
           VERIFY(x_msg_q_send(ptHost->hNotifyMsg, (void *)(&rMsg), sizeof(EMU_NTY_MSG_T), 0) == OSR_OK);
        }
        break;
    }
}

static VOID _B2rIsrCb_CRC_New(B2R_EMU_HOST_T *ptHost, B2R_HAL_STATUS_T *ptStatus)
{
    if (!ptHost || !ptStatus)
    {
        return;
    }

    //record CRC
    switch (ptHost->eSt)
    {
    default:
        break;
    case EMU_ST_CRC_START:
        
        if (ptHost->tCfg.OUT_I && ptHost->TWO_ADDR)
        {
            ptHost->u4CRCFlag0 = 0x03;
            ptHost->u4CRCFlag1 = 0x03;
        }
        else if (ptHost->TWO_ADDR)
        {
            ptHost->u4CRCFlag0 = 0x01;
            ptHost->u4CRCFlag1 = 0x01;
        }
        else if (ptHost->tCfg.OUT_I)
        {
            ptHost->u4CRCFlag0 = 0x03;
            ptHost->u4CRCFlag1 = 0x0;
        }
        else 
        {
            ptHost->u4CRCFlag0 = 0x01;
            ptHost->u4CRCFlag1 = 0x0;
        }
        ptHost->u4WaitCnt = 0;
        ptHost->eSt = EMU_ST_CRC_WAIT;
        break;
    case EMU_ST_CRC_WAIT:
        if (ptHost->u4WaitCnt++ > 8)
        {
            ptHost->eSt = EMU_ST_CRC_DOING;
        }
        break;
    case EMU_ST_CRC_DOING:
        
        //record crc
        if (ptHost->tCfg.OUT_I && ptHost->TWO_ADDR)
        {
            //left top
            if ((ptHost->u4CRCFlag0 & 0x1) && ptStatus->fgLeft && ptStatus->fgTop)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //left bottom
            if ((ptHost->u4CRCFlag0 & 0x2) && ptStatus->fgLeft && !ptStatus->fgTop)
            {
                ptHost->u4CRC0[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x2;
            }

            //right top
            if ((ptHost->u4CRCFlag1 & 0x1) && !ptStatus->fgLeft && ptStatus->fgTop)
            {
                ptHost->u4CRC1[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x1;
            }
            //right bottom
            if ((ptHost->u4CRCFlag1 & 0x2) && !ptStatus->fgLeft && !ptStatus->fgTop)
            {
                ptHost->u4CRC1[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x2;
            }
        
        }
        else if (ptHost->TWO_ADDR)
        {
            //left
            if ((ptHost->u4CRCFlag0 & 0x1) && ptStatus->fgLeft)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //right
            if ((ptHost->u4CRCFlag1 & 0x1) && !ptStatus->fgLeft)
            {
                ptHost->u4CRC1[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag1 ^= 0x1;
            }
        }
        else if (ptHost->tCfg.OUT_I)
        {
            //top 
            if ((ptHost->u4CRCFlag0 & 0x1) && !ptStatus->fgTop)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }

            //bottom 
            if ((ptHost->u4CRCFlag0 & 0x2) && ptStatus->fgTop)
            {
                ptHost->u4CRC0[1] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x2;
            }
        }
        else
        {
            if (ptHost->u4CRCFlag0 & 0x1)
            {
                ptHost->u4CRC0[0] = ptStatus->u4CrC;
                ptHost->u4CRCFlag0 ^= 0x1;
            }
        }
        
        if (!ptHost->u4CRCFlag0 && !ptHost->u4CRCFlag1)
            ptHost->eSt = EMU_ST_CRC_DONE;
        
        break;
    case EMU_ST_CRC_DONE:
        // notify 
        ptHost->eSt = EMU_ST_IDLE;
        if (ptHost->hNotifyMsg)
        {
            EMU_NTY_MSG_T rMsg;
            rMsg.eSt = EMU_ST_CRC_DONE;
            x_msg_q_send(ptHost->hNotifyMsg, (void *)(&rMsg), sizeof(EMU_NTY_MSG_T), 0);
        }
        break;
    }
}

static VOID _B2rIsrCb_Vsync(B2R_EMU_HOST_T *ptHost)
{
    static UINT32 u4Cnt;
    static UINT32 u4MaxCnt;
    B2R_VSYNC_T *ptVsync;
    UINT32 u4Curr, u4Det;
    
    if (!ptHost)
    {
        return;
    }

    u4Curr  = _B2rGetRefTime();
    ptVsync = &(ptHost->tVsync);

    switch (ptHost->eSt)
    {
    default:
        break;
    case EMU_ST_VSYNC_START:
        ptVsync->u4Ave = 0;
        ptVsync->u4Max = 0;
        ptVsync->u4Min = 0xFFFFFFFF;
        u4Cnt = 0;
        u4MaxCnt = 100;
        _B2rResetRefTime();
        ptHost->eSt = EMU_ST_VSYNC_DOING;
        break;
    case EMU_ST_VSYNC_DOING:

        if (u4Cnt > u4MaxCnt)
        {
            ptVsync->u4Ave = ptVsync->u4Ave /(u4MaxCnt);
            ptHost->eSt = EMU_ST_VSYNC_DONE;
            break;
        }

        if (u4Cnt)
        {
            u4Det = u4Curr - ptVsync->u4Last;
            ptVsync->u4Max = MAX(u4Det, ptVsync->u4Max);
            ptVsync->u4Min = MIN(u4Det, ptVsync->u4Min);
            ptVsync->u4Ave += u4Det;
        }

        u4Cnt++;
        ptVsync->u4Last = u4Curr;
        break;
    case EMU_ST_VSYNC_DONE:
        // notify 
        ptHost->eSt = EMU_ST_IDLE;
        if (ptHost->hNotifyMsg)
        {
            EMU_NTY_MSG_T rMsg;
            rMsg.eSt = EMU_ST_VSYNC_DONE;
           VERIFY(x_msg_q_send(ptHost->hNotifyMsg, (void *)(&rMsg), sizeof(EMU_NTY_MSG_T), 0) == OSR_OK);
        }
        break;
    }
}



VOID _B2rIsrCb(VOID *pvHost, VOID * pvArg)
{
    B2R_EMU_HOST_T *ptHost = NULL;
    B2R_HAL_STATUS_T *ptStatus;

    if (!pvHost || !pvArg)
    {
        return;
    }

    ptHost   = (B2R_EMU_HOST_T *)pvHost;
    ptStatus = (B2R_HAL_STATUS_T *)pvArg;

    switch (ptHost->eSt)
    {
    case EMU_ST_IDLE:
        break;
    default:
        switch ((ptHost->eSt & ST_PRFX_MASK))
        {
        case CRC_ST_PRFX:
            if (ptHost->fgOldB2r)
            {
                _B2rIsrCb_CRC_Old(ptHost, ptStatus);
            }
            else
            {
                _B2rIsrCb_CRC_New(ptHost, ptStatus);
            }
            break;
        case VSYNC_ST_PRFX:
            _B2rIsrCb_Vsync(ptHost);
            break;
        }
        break;
    }

  /*  LOG(10, "_B2rIsrCb(%d),cnt(%d) T(%d) L(%d),CRC(0x%08x) reg(0x%08x) st(%d)\n",
        ptHost->hB2r,
        ptStatus->u4LineCnt, 
        ptStatus->fgTop, ptStatus->fgLeft, ptStatus->u4CrC, ptStatus->u4Status,
        ptHost->eSt);
*/
}


static UINT32 _B2rEmuCfgStr2int(CHAR *ptr)
{
    UINT32 u4Value;
    CHAR *pbStr;
    //pattern: xx = 1
    if (!ptr)
    {
        return 0;
    }
    pbStr = ptr;

    while(*pbStr && *pbStr++ != '=')
    {
        ;
    }

    while(*pbStr && *pbStr++ != ' ')
    {
        ;
    }

    u4Value = 0;
    while(*pbStr && *pbStr >= '0' && *pbStr <= '9')
    {
        u4Value = u4Value*10 + (*pbStr++ - '0');
    }

    return u4Value;
}

static CHAR * _B2rEmuGetCfgDataPtr(CHAR *ptr)
{
    CHAR *pbStr;
    pbStr = ptr;

    if (!ptr)
    {
        return NULL;
    }

    while(*pbStr && *pbStr++ != '=')
    {
        ;
    }

    while(*pbStr && *pbStr++ != ' ')
    {
        ;
    }

    return pbStr;
}

static void _B2rEmuBufferInit(UINT8 Id)
{
    FBM_POOL_T rFbmPool;
    FBM_POOL_T *prFbmPool;
    B2R_EMU_HOST_T *ptObj = NULL;
    MID_AGENT_ID_T eAgentId;
    UINT32 u4Region;
    UINT8  u1Path;

    _B2rEmuChkId(Id);
    ptObj = _B2rEmuId2Obj(Id);

    //buffer
#ifdef B2R_FBM_CFG
    UNUSED(rFbmPool);
    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_MPEG);
#else
    rFbmPool.u4Addr = 0x04000000;//;0x0a9fff00;
    rFbmPool.u4Size = 0x03000000; //16L<<20;
    prFbmPool = &rFbmPool;
#endif

    ptObj->u4BuffSize = B2R_BUFF_SIZE;

    if (Id == 0)
    {
        ptObj->u4BuffAddr = prFbmPool->u4Addr;
    }
    else
    {
       ptObj->u4BuffAddr = prFbmPool->u4Addr;// ptObj->u4BuffAddr = prFbmPool->u4Addr + B2R_BUFF_SIZE;
    }

    u1Path = _B2rFindPath(ptObj);
    if(u1Path == PATH_2 &&
        _b2r_par[u1Path].bFgEn)
    {
        ptObj->u4BuffAddr += B2R_BUFF_OFFSET;
    }
    ptObj->u4BuffPtr  = ptObj->u4BuffAddr;

    // use mid to protect buffer
    eAgentId = MID_AGENT_CPU;
    u4Region = 3 + Id;
    MID_SetRegion(u4Region, eAgentId, ptObj->u4BuffAddr, ptObj->u4BuffAddr + B2R_BUFF_SIZE);
    MID_EnableRegionProtect(u4Region);
    Printf("Mid protect: Id(%d), Region %d: %08x~%08x\n", Id, u4Region, ptObj->u4BuffAddr, ptObj->u4BuffAddr + B2R_BUFF_SIZE);
    
}

static void _B2rEmuBufferDeinit(UINT8 Id)
{
    B2R_EMU_HOST_T *ptObj = NULL;
    UINT32 u4Region;

    _B2rEmuChkId(Id);
    ptObj = _B2rEmuId2Obj(Id);

    x_memset((void *)ptObj, 0, sizeof(B2R_EMU_HOST_T));

    // close mid's protection
    u4Region = 3 + Id;
    MID_ResetRegion(u4Region);
}

static void _B2rEmuVariableInit(UINT8 Id)
{
    UINT32 u4Val;
    B2R_EMU_HOST_T *ptObj = NULL;
    _B2rEmuChkId(Id);
    ptObj = _B2rEmuId2Obj(Id);

    // create NOTIFY Message Queue
    if (!_hNotifyMsgQ[Id])
    {
        CHAR szBuf[16];
        x_snprintf(szBuf, sizeof(szBuf), "EMU-NTY%2d", Id);
        VERIFY(x_msg_q_create(&_hNotifyMsgQ[Id], szBuf, sizeof(EMU_NTY_MSG_T), EMU_NOTIFY_Q_NUM) == OSR_OK);
    }
    

    ptObj->pvData = (VOID *)ptObj;
    ptObj->IsrCb  = _B2rIsrCb;

    ptObj->eSt = EMU_ST_IDLE;
    ptObj->hNotifyMsg = _hNotifyMsgQ[Id];

    u4Val = B2R_SCRIPT_SIZE;
    ptObj->pcScriptPtr  = (CHAR *)_B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->pcScriptPtr  = (CHAR *)VIRTUAL((UINT32)ptObj->pcScriptPtr);

    u4Val = B2R_PATH_SIZE;
    ptObj->pcPath       = (CHAR *)_B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->pcPath       = (CHAR *)VIRTUAL((UINT32)ptObj->pcPath);

    _B2rEmuAlignBufferAddr(ptObj, 4);
    ptObj->u4MaxItem      = 200;
    u4Val                 = ptObj->u4MaxItem * sizeof(EMU_ITEM_T);
    ptObj->ptItems        = (EMU_ITEM_T *)_B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->ptItems        = (EMU_ITEM_T *)VIRTUAL((UINT32)ptObj->ptItems);
    ptObj->rItemHead.next = NULL;

    _B2rEmuAlignBufferAddr(ptObj, 4);
    ptObj->u4MaxList      = 20;
    u4Val                 = ptObj->u4MaxList * sizeof(EMU_LIST_T);
    ptObj->ptLists        = (EMU_LIST_T *)_B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->ptLists        = (EMU_LIST_T *)VIRTUAL((UINT32)ptObj->ptLists);
    ptObj->rListHead.next = NULL;
    ptObj->ptCurrList     =  &ptObj->ptLists[0];

    u4Val = 32*1024;
    _B2rEmuAlignBufferAddr(ptObj, 256);
    ptObj->u4YAddrLen0    = _B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->u4YAddrLen0    = VIRTUAL(ptObj->u4YAddrLen0); 
    ptObj->u4CAddrLen0    = _B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->u4CAddrLen0    = VIRTUAL(ptObj->u4CAddrLen0); 

    ptObj->u4YAddrLen1    = _B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->u4YAddrLen1    = VIRTUAL(ptObj->u4YAddrLen1); 
    ptObj->u4CAddrLen1    = _B2rEmuGetBuffer(ptObj, u4Val);
    ptObj->u4CAddrLen1    = VIRTUAL(ptObj->u4CAddrLen1); 


    //frame buffer 
    if (Id == 0)
        u4Val = B2R0_MAX_WIDTH * B2R0_MAX_HEIGHT;
    else
        u4Val = B2R1_MAX_WIDTH * B2R1_MAX_HEIGHT;

    _B2rEmuAlignBufferAddr(ptObj, 2048);
    ptObj->u4YAddr0       = _B2rEmuGetBuffer(ptObj, u4Val*2);
    Printf("YAddr0 : 0x%x!\n",ptObj->u4YAddr0);
    ptObj->u4YAddr0       = VIRTUAL(ptObj->u4YAddr0); 
    ptObj->u4CAddr0       = _B2rEmuGetBuffer(ptObj, u4Val);
    Printf("CAddr0 : 0x%x!\n",ptObj->u4CAddr0);
    ptObj->u4CAddr0       = VIRTUAL(ptObj->u4CAddr0); 

    ptObj->u4YAddr1       = _B2rEmuGetBuffer(ptObj, u4Val*2);
    Printf("YAddr1 : 0x%x!\n",ptObj->u4YAddr1);
    ptObj->u4YAddr1       = VIRTUAL(ptObj->u4YAddr1); 
    ptObj->u4CAddr1       = _B2rEmuGetBuffer(ptObj, u4Val);
    Printf("CAddr1 : 0x%x!\n",ptObj->u4CAddr1);
    ptObj->u4CAddr1       = VIRTUAL(ptObj->u4CAddr1); 

    if (!_B2rEmuChkBufferOk(ptObj))
    {
        Printf("out of memory, please check!\n");
    }

}

static void _B2rV1EnvInit(UINT8 Id, BOOL fgOld)
{
    _B2rEmuChkId(Id);

    _B2rEmuBufferInit(Id);
    _B2rEmuVariableInit(Id);

    if (fgOld)
    {
        x_os_isr_fct pfnOldIsr;
        _B2REmu_HW_V1_Init(Id);
        VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V1_ISR, &pfnOldIsr) == OSR_OK);
    }
    else
    {
        x_os_isr_fct pfnOldIsr;
        _B2REmu_HW_V2_Init(Id);
        VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V2_ISR, &pfnOldIsr) == OSR_OK);
    }
}

static void _B2rV1EnvDeinit(UINT8 Id, BOOL fgOld)
{
    _B2rEmuChkId(Id);

    if (fgOld)
    {
        _B2REmu_HW_V1_Init(Id);
    }
    else
    {
        _B2REmu_HW_V2_Init(Id);
    }

    _B2rEmuBufferDeinit(Id);    
}

static void _B2rEmuProcCfgFile(B2R_CFG_FILE * ptCfg, CHAR* s)
{
    CHAR *pcToken;

    if (!ptCfg)
    {
        return;
    }

    //just for test, use default value
    if (s == NULL)
    {
        ptCfg->ADDR_MODE   = 0;
        ptCfg->ADDR_SWAP   = 2;
        ptCfg->FIELD       = 1;
        ptCfg->FLIP        = 0;
        ptCfg->HDEW        = _u4B2rHDEW;
        ptCfg->H_OFFSET    = 0;
        ptCfg->LINE_OFFSET = _u4B2rPitch; //48 * 16;
        ptCfg->M_422       = 0;
        ptCfg->OUT_I       = 1;
        ptCfg->VDEW        = _u4B2rVDEW;
        ptCfg->V_OFFSET    = 0;
        ptCfg->DEPTH       = 8;

        return;
    }

    //default value
    ptCfg->DEPTH = 8;
    ptCfg->PACK_MODE = 0;

    while ((pcToken = emu_strsep(&s, "\n")) != NULL)
    {
        Printf("token: %s\n", pcToken);
        if (x_strncmp(pcToken, "ADDR_MODE", 9) == 0)
        {
            ptCfg->ADDR_MODE = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "ADDR_SWAP", 9) == 0)
        {
            ptCfg->ADDR_SWAP = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "C_420I", 6) == 0)
        {
            //ptCfg->C_420I = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "FIELD", 5) == 0)
        {
            ptCfg->FIELD = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "FLIP", 4) == 0)
        {
            ptCfg->FLIP = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "HDEW", 4) == 0)
        {
            ptCfg->HDEW = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "H_OFFSET", 8) == 0)
        {
            ptCfg->H_OFFSET = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "LINE_OFFSET", 11) == 0)
        {
            ptCfg->LINE_OFFSET = _B2rEmuCfgStr2int(pcToken) * 16;
        }
        else if (x_strncmp(pcToken, "MIRROR", 6) == 0)
        {
            ptCfg->MIRROR = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "M_422", 5) == 0)
        {
            ptCfg->M_422 = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "OUT_I", 5) == 0)
        {
            ptCfg->OUT_I = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "VDEW", 4) == 0)
        {
            ptCfg->VDEW = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "V_OFFSET", 8) == 0)
        {
            ptCfg->V_OFFSET = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "PACK_MODE", 9) == 0)
        {
            ptCfg->PACK_MODE = _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "DEPTH", 5) == 0)
        {
            ptCfg->DEPTH = _B2rEmuCfgStr2int(pcToken);
        }
		else if (x_strncmp(pcToken, "SRCWIDTH", 8) == 0)
        {
            ptCfg->u4SrcWidth = _B2rEmuCfgStr2int(pcToken);
        }
		else if (x_strncmp(pcToken, "SRCHEIGHT", 9) == 0)
        {
            ptCfg->u4SrcHeight= _B2rEmuCfgStr2int(pcToken);
        }
		else if (x_strncmp(pcToken, "TAGWIDTH", 8) == 0)
        {
            ptCfg->u4TagWidth= _B2rEmuCfgStr2int(pcToken);
        }
		else if (x_strncmp(pcToken, "TAGHEIGHT", 9) == 0)
        {
            ptCfg->u4TagHeight= _B2rEmuCfgStr2int(pcToken);
        }
        else if (x_strncmp(pcToken, "TAGPITCH", 8) == 0)
        {
            ptCfg->u4_Tag_LINE_OFFSET= _B2rEmuCfgStr2int(pcToken)* 16;
        }
    }
}


static UINT32 _B2rEmuCrcStr2int(CHAR *ptr)
{
    UINT32 u4Value;
    CHAR *pbStr;
    pbStr = ptr;

    while(*pbStr && (*pbStr++ != ' '));
    
    u4Value = 0;
    while(*pbStr && ((*pbStr >= '0' && *pbStr <= '9') || (*pbStr >= 'a' && *pbStr <= 'f')))
    {
        u4Value = u4Value << 4; 
        if (pbStr[0] > '9')
        {
            u4Value = u4Value + (*pbStr - 'a' + 10);    
        }
        else
        {
            u4Value = u4Value + (*pbStr - '0'); 
        }
        pbStr++;
    }

    return u4Value;
}


static void _B2rEmuProcCRC_P(UINT32 * pu4Arg, CHAR* s)
{
    CHAR *pcToken;

    if (!pu4Arg || !s)
    {
        return;
    }

    while ((pcToken = emu_strsep(&s, "\n")) != NULL)
    {
        if (x_strncmp(pcToken, "P_F0_M0", 7) == 0)
        {
            pu4Arg[0] = _B2rEmuCrcStr2int(pcToken+7);
            //Printf("P_F0_M0 = 0x%08x\n", pu4Arg[0]);
        }
        else if (x_strncmp(pcToken, "P_F1_M0", 7) == 0)
        {
            pu4Arg[1] = _B2rEmuCrcStr2int(pcToken+7);
            //Printf("P_F1_M0 = 0x%08x\n", pu4Arg[1]);
        }
        else if (x_strncmp(pcToken, "P_F0_M1", 7) == 0)
        {
            pu4Arg[2] = _B2rEmuCrcStr2int(pcToken+7);
            //Printf("P_F0_M1 = 0x%08x\n", pu4Arg[2]);
        }
        else if (x_strncmp(pcToken, "P_F1_M1", 7) == 0)
        {
            pu4Arg[3] = _B2rEmuCrcStr2int(pcToken+7);
            //Printf("P_F1_M1 = 0x%08x\n", pu4Arg[3]);
        }
    }
}

static void _B2rEmuProcCRC_I(UINT32 * pu4Arg, CHAR* s)
{
    CHAR *pcToken;

    if (!pu4Arg || !s)
    {
        return;
    }

    while ((pcToken = emu_strsep(&s, "\n")) != NULL)
    {
        if (x_strncmp(pcToken, "I0_F0_M0", 8) == 0)
        {
            pu4Arg[0] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I0_F0_M0 = 0x%08x\n", pu4Arg[0]);
        }
        else if (x_strncmp(pcToken, "I0_F1_M0", 8) == 0)
        {
            pu4Arg[1] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I0_F1_M0 = 0x%08x\n", pu4Arg[1]);
        }
        else if (x_strncmp(pcToken, "I0_F0_M1", 8) == 0)
        {
            pu4Arg[2] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I0_F0_M1 = 0x%08x\n", pu4Arg[2]);
        }
        else if (x_strncmp(pcToken, "I0_F1_M1", 8) == 0)
        {
            pu4Arg[3] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I0_F1_M1 = 0x%08x\n", pu4Arg[3]);
        }
        else if (x_strncmp(pcToken, "I1_F0_M0", 8) == 0)
        {
            pu4Arg[4] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I1_F0_M0 = 0x%08x\n", pu4Arg[4]);
        }
        else if (x_strncmp(pcToken, "I1_F1_M0", 8) == 0)
        {
            pu4Arg[5] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I1_F1_M0 = 0x%08x\n", pu4Arg[5]);
        }
        else if (x_strncmp(pcToken, "I1_F0_M1", 8) == 0)
        {
            pu4Arg[6] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I1_F0_M1 = 0x%08x\n", pu4Arg[6]);
        }
        else if (x_strncmp(pcToken, "I1_F1_M1", 8) == 0)
        {
            pu4Arg[7] = _B2rEmuCrcStr2int(pcToken + 8);
            //Printf("I1_F1_M1 = 0x%08x\n", pu4Arg[7]);
        }
    }
}

static void _B2rEmuProcCRC(UINT32 * pu4Arg, CHAR* s, BOOL fgInterlaced)
{
    if (fgInterlaced)
    {
        _B2rEmuProcCRC_I(pu4Arg, s);
    }
    else
    {
        _B2rEmuProcCRC_P(pu4Arg, s);
    }
}

BOOL _B2rCmpCRC(UINT32 *pu4Pat, UINT32 *pu4Cmp, UINT32 u4Mask)
{
    // bit 2  field or frame , bit 1 flip,  bit 0 mirror
    UINT32 idx;

    if (!pu4Pat || !pu4Cmp)
    {
        return FALSE;
    }

    idx = (u4Mask & 0x3 ); 

    if (u4Mask & 0x4)
    {
        return ((pu4Cmp[0] == pu4Pat[idx]) && ((pu4Cmp[1] == pu4Pat[4+idx])));
    }
    else
    {
        return (pu4Cmp[0] == pu4Pat[idx]);
    }

}


VOID _B2rDumpCRC(UINT32 *pu4Pat, UINT32 *pu4Cmp, UINT32 u4Mask)
{
    // bit 2  field or frame , bit 1 flip,  bit 0 mirror
    UINT32 idx;

    if (!pu4Pat || !pu4Cmp)
    {
        return;
    }

    idx = (u4Mask & 0x3 ); 

    if (u4Mask & 0x4)
    {
        Printf("pat:0x%08x, 0x%08x; real:0x%08x, 0x%08x\n", pu4Pat[idx], pu4Pat[4+idx], pu4Cmp[0], pu4Cmp[1]);
    }
    else
    {
        Printf("pat: 0x%08x; real:0x%08x\n", pu4Pat[idx], pu4Cmp[0]);
    }

}

UINT8 _B2rFindPath(B2R_EMU_HOST_T *ptHost)
{
    UINT8 u1Path = PATH_NS;
    UINT8 u1Idx  = 0;
    
    do
    {
        if(!ptHost ||
            !ptHost->ptCurrList)
        {
            break;
        }

        for(u1Idx = 0; u1Idx < PATH_NS; u1Idx ++)
        {
            if(ptHost->ptCurrList->tCfg.u1B2rId == _b2r_par[u1Idx].u1Id &&
                ptHost->ptCurrList->tCfg.fgOld == _b2r_par[u1Idx].fgOldB2r &&
                _b2r_par[u1Idx].bFgEn)
            {
                u1Path = u1Idx;
                break;
            }
        }
    }while(0);

    return u1Path;
}

static BOOL _B2rEmu2D(B2R_HANDLE_T hB2r, B2R_CFG_FILE *ptCfg, B2R_EMU_HOST_T *ptHost)
{
    B2R_HAL_TIMING_T tTiming;
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    B2R_HAL_COLOR_FMT_T tColorFmt;
    B2R_HAL_3D_T t3D;
    B2R_HAL_MF_T tMF;
    __TIMING_TBL_T *ptTbl;

    
    if (hB2r == B2R_NULL_HANDLE || !ptCfg || !ptHost)
    {
        return FALSE;
    }


    if (ptCfg->OUT_I)
    {
        ptTbl = _b2r_query_I_timing(ptCfg->HDEW, ptCfg->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }
    else
    {
        ptTbl = _b2r_query_P_timing(ptCfg->HDEW, ptCfg->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }

    if (ptTbl == NULL)
    {
        return FALSE;
    }

    Printf("Timing name: %s\n", ptTbl->acName);


    //3D type
    t3D.fg3D = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_3DTYPE, &t3D) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //Timing 
    tTiming.fgProg   = ptCfg->OUT_I ? 0 : 1;
    tTiming.u4HTotal = ptTbl->u2HTotal;
    tTiming.u4VTotal = ptTbl->u2VTotal;
    tTiming.u4Rate   = ptHost->u4Rate;
    tTiming.u4Scale  = ptHost->u4Scale;
    tTiming.u4SrcW   = ptCfg->HDEW;
    tTiming.u4SrcH   = tTiming.fgProg? ptCfg->VDEW : (ptCfg->VDEW << 1);    
    tTiming.u4Clock  = ptTbl->u4Clock; 
    if (B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //dram layout
    tDramLayout.eAddrMode = (B2R_ADDR_MODE_T)ptCfg->ADDR_MODE;
    tDramLayout.eSwapMode = (B2R_SWAP_MODE_T)ptCfg->ADDR_SWAP;
    tDramLayout.ePackMode = (B2R_PACK_MODE_T)ptCfg->PACK_MODE;
    tDramLayout.u4Pitch   = ptCfg->LINE_OFFSET;
    if (B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //color format
    tColorFmt.eMode   = (ptCfg->M_422 == 1) ? B2R_CM_422 : B2R_CM_420;
    tColorFmt.u1Depth = ptCfg->DEPTH;
    if (B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //mirror or flip
    tMF.fgFlip   = FALSE;
    tMF.fgMirror = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    B2R_HAL_Config(hB2r);


    return TRUE;
    
}


static BOOL _B2rEmuTest2D(UINT32 u1B2rId, BOOL fgOld, UINT32 u4Rate, UINT32 u4Scale, CHAR *ucRootPath, UINT32 u4FrmCnt)
{
    B2R_HANDLE_T hB2r;
    B2R_CFG_FILE *ptCfgFile;
    UINT32 u4Idx;
    UINT32 u4CmpErrCnt;
    UINT32 u4ErrCnt;
    B2R_EMU_HOST_T *ptHost;
    B2R_CfG_T tCfg;
    UCHAR ucFilePath[256];
    UINT32 u4Size;
    EMU_FILE_HANDLE hFile;

    //Printf("\n***2D CRC Test (%s, %s)******\n", (u1B2rId == 0 ? "HD":"SD"), (fgOld ? "Old":"New"));
    if (!ucRootPath)
    {
        Printf("Please input the root path.\n");
    }

    //Global initialization 
    B2R_HAL_Init();
    _B2rV1EnvInit(u1B2rId, fgOld);
    ptHost = _B2rEmuId2Obj(u1B2rId);

    ptCfgFile = &ptHost->tCfg;

    //load cfg file
    x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/b2r.cfg", ucRootPath);
    hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", ucFilePath);
        return 0;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, ptHost->pcScriptPtr,u4Size);
    emu_fclose(hFile);

    ptHost->pcScriptPtr[u4Size] = '\0';
    _B2rEmuProcCfgFile(ptCfgFile, ptHost->pcScriptPtr);

    ptHost->u4Idx   = 0;
    ptHost->u4Total = u4FrmCnt;
    ptHost->u4Rate  = u4Rate;
    ptHost->u4Scale = u4Scale;

    ptHost->fgOldB2r = fgOld;

    //Start test B2R     
    //@open b2r     
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    tCfg.u4Data = u4Idx;
    tCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&tCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw\n");
        return FALSE;
    }

    B2R_HAL_Reset(hB2r);

    //3D type
    {
    B2R_HAL_3D_T t3D;
    t3D.fg3D = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_3DTYPE, &t3D) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //Timing 
    {
    B2R_HAL_TIMING_T tTiming;
    __TIMING_TBL_T *ptTbl;

    if (ptCfgFile->OUT_I)
    {
        ptTbl = _b2r_query_I_timing(ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }
    else
    {
        ptTbl = _b2r_query_P_timing(ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }

    Printf("Timing name: %s\n", ptTbl->acName);
 
    tTiming.fgProg   = ptCfgFile->OUT_I ? 0 : 1;
    tTiming.u4HTotal = ptTbl->u2HTotal;
    tTiming.u4VTotal = ptTbl->u2VTotal;
    tTiming.u4Rate   = ptHost->u4Rate;
    tTiming.u4Scale  = ptHost->u4Scale;
    tTiming.u4SrcW   = ptCfgFile->HDEW;
    tTiming.u4SrcH   = tTiming.fgProg? ptCfgFile->VDEW : (ptCfgFile->VDEW << 1);    
    tTiming.u4Clock  = ptTbl->u4Clock; 
    if (B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }
    
    //dram layout
    {
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    tDramLayout.eAddrMode = (B2R_ADDR_MODE_T)ptCfgFile->ADDR_MODE;
    tDramLayout.eSwapMode = (B2R_SWAP_MODE_T)ptCfgFile->ADDR_SWAP;
    tDramLayout.ePackMode = (B2R_PACK_MODE_T)ptCfgFile->PACK_MODE;
    tDramLayout.u4Pitch   = ptCfgFile->LINE_OFFSET;
    if (B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //color format
    {
    B2R_HAL_COLOR_FMT_T tColorFmt;
    tColorFmt.eMode   = (ptCfgFile->M_422 == 1) ? B2R_CM_422 : B2R_CM_420;
    tColorFmt.u1Depth = ptCfgFile->DEPTH;
    if (B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //mirror or flip
    {
    B2R_HAL_MF_T tMF;
    tMF.fgFlip   = FALSE;
    tMF.fgMirror = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    B2R_HAL_Config(hB2r);

    B2R_HAL_Enable(hB2r);

    //@test frame    
    u4ErrCnt = u4CmpErrCnt = 0;
    FOR_EACH_FRAME(ptHost)
    {
        B2R_HAL_ADDR_T tFrmAddr;
        EMU_NTY_MSG_T rMsg;
        UINT16 u2MsgQIdx;
        SIZE_T zMsgSize;

        //-load  rawdata
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddr0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr0, u4Size);

        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddr0,u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr0, u4Size);

        //load CRC
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
        emu_fclose(hFile);
        _B2rEmuProcCRC(ptHost->au4PatCRC0, ptHost->pcScriptPtr, ptCfgFile->OUT_I);

    
        //-set test pattern 
        tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
        tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
        tFrmAddr.u4Y1 = tFrmAddr.u4C1 = 0;
        if (B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr) !=  B2R_HAL_OK)
        {
            return FALSE;
        }
        
        //-record crc
        ptHost->eSt = EMU_ST_CRC_START;
        
        //-wait finish
        zMsgSize = sizeof(EMU_NTY_MSG_T);
        VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
        
        //-compare crc
        if (rMsg.eSt == EMU_ST_CRC_DONE)
        {
            UINT32 u4Mask;
            u4Mask = ptCfgFile->OUT_I ? 0x4:0;
            //-load  CRC
            if (!_B2rCmpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask))
            {
                Printf("compare fail at (%d/%d)\n", ptHost->u4Idx, ptHost->u4Total);
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                u4CmpErrCnt++;
            }
            else
            {
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
            }
        }
        else
        {
            //other error
            u4ErrCnt++;
        }
    }

    //@close b2r
    B2R_HAL_Disable(hB2r);
    B2R_HAL_Close(hB2r);

    //deinitialization
    _B2rV1EnvDeinit(u1B2rId, fgOld);

    //print result
    if (u4ErrCnt || u4CmpErrCnt)
    {
        Printf("list:%s  compare fail, error type(%d, %d), please check\n", ucRootPath, u4ErrCnt, u4CmpErrCnt);
    }
    else
    {
        Printf("list:%s, items(%d), compare Ok\n", ucRootPath, u4FrmCnt);
    }

    return TRUE;

}


static BOOL _B2rEmuTest3DTwoAddr(UINT32 u1B2rId, BOOL fgOld, UINT32 u4Rate, UINT32 u4Scale, CHAR *ucRootPath, UINT32 u4FrmCnt)
{
    B2R_HANDLE_T hB2r;
    B2R_CFG_FILE *ptCfgFile;
    UINT32 u4Idx;
    UINT32 u4CmpErrCnt;
    UINT32 u4ErrCnt;
    B2R_EMU_HOST_T *ptHost;
    B2R_CfG_T tCfg;
    UCHAR ucFilePath[256];
    UINT32 u4Size;
    EMU_FILE_HANDLE hFile;
    UINT32 u4IdxOffset = 0;

    //Printf("\n***2D CRC Test (%s, %s)******\n", (u1B2rId == 0 ? "HD":"SD"), (fgOld ? "Old":"New"));
    if (!ucRootPath)
    {
        Printf("Please input the root path.\n");
    }

    //Global initialization 
    B2R_HAL_Init();
    _B2rV1EnvInit(u1B2rId, fgOld);
    ptHost = _B2rEmuId2Obj(u1B2rId);

    ptCfgFile = &ptHost->tCfg;

    //load cfg file
    x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/b2r.cfg", ucRootPath);
    hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", ucFilePath);
        return 0;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, ptHost->pcScriptPtr,u4Size);
    emu_fclose(hFile);

    ptHost->pcScriptPtr[u4Size] = '\0';
    _B2rEmuProcCfgFile(ptCfgFile, ptHost->pcScriptPtr);

    ptHost->u4Idx   = 0;
    ptHost->u4Total = u4FrmCnt >> 1;
    ptHost->u4Rate  = u4Rate;
    ptHost->u4Scale = u4Scale;

    ptHost->fgOldB2r = fgOld;


    //Start test B2R     
    //@open b2r     
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    tCfg.u4Data = u4Idx;
    tCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&tCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw\n");
        return FALSE;
    }

    B2R_HAL_Reset(hB2r);

    //3D type
    {
    B2R_HAL_3D_T t3D;
    t3D.fg3D = TRUE;
    t3D.u4Layout = B2R_3D_FRAME_SEQUENTIAL;
    ptHost->TWO_ADDR = 1;
    if (B2R_HAL_Set(hB2r, B2R_HAL_3DTYPE, &t3D) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //Timing 
    {
    B2R_HAL_TIMING_T tTiming;
    __TIMING_TBL_T *ptTbl;

    if (ptCfgFile->OUT_I)
    {
        ptTbl = _b2r_query_I_timing(ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }
    else
    {
        ptTbl = _b2r_query_P_timing(ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }

    Printf("Timing name: %s\n", ptTbl->acName);
 
    tTiming.fgProg   = ptCfgFile->OUT_I ? 0 : 1;
    tTiming.u4HTotal = ptTbl->u2HTotal;
    tTiming.u4VTotal = ptTbl->u2VTotal;
    tTiming.u4Rate   = ptHost->u4Rate;
    tTiming.u4Scale  = ptHost->u4Scale;
    tTiming.u4SrcW   = ptCfgFile->HDEW;
    tTiming.u4SrcH   = tTiming.fgProg? ptCfgFile->VDEW : (ptCfgFile->VDEW << 1);    
    tTiming.u4Clock  = (ptTbl->u4Clock << 1); 
    if (B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }
    
    //dram layout
    {
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    tDramLayout.eAddrMode = (B2R_ADDR_MODE_T)ptCfgFile->ADDR_MODE;
    tDramLayout.eSwapMode = (B2R_SWAP_MODE_T)ptCfgFile->ADDR_SWAP;
    tDramLayout.ePackMode = (B2R_PACK_MODE_T)ptCfgFile->PACK_MODE;
    tDramLayout.u4Pitch   = ptCfgFile->LINE_OFFSET;
    if (B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //color format
    {
    B2R_HAL_COLOR_FMT_T tColorFmt;
    tColorFmt.eMode   = (ptCfgFile->M_422 == 1) ? B2R_CM_422 : B2R_CM_420;
    tColorFmt.u1Depth = ptCfgFile->DEPTH;
    if (B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    //mirror or flip
    {
    B2R_HAL_MF_T tMF;
    tMF.fgFlip   = FALSE;
    tMF.fgMirror = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF) !=  B2R_HAL_OK)
    {
        return FALSE;
    }
    }

    B2R_HAL_Config(hB2r);

    B2R_HAL_Enable(hB2r);

    //@test frame 
    u4IdxOffset = u4FrmCnt >> 1;
    u4ErrCnt = u4CmpErrCnt = 0;
    FOR_EACH_FRAME(ptHost)
    {
        B2R_HAL_ADDR_T tFrmAddr;
        EMU_NTY_MSG_T rMsg;
        UINT16 u2MsgQIdx;
        SIZE_T zMsgSize;

        //-load  rawdata
        //@left view
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddr0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr0, u4Size);

        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddr0,u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr0, u4Size);

        //load CRC
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
        emu_fclose(hFile);
        _B2rEmuProcCRC(ptHost->au4PatCRC0, ptHost->pcScriptPtr, ptCfgFile->OUT_I);

        //@right view
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx + u4IdxOffset);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddr1, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr1, u4Size);

        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx + u4IdxOffset);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddr1,u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr1, u4Size);

        //load CRC
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx + u4IdxOffset);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            break;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
        emu_fclose(hFile);
        _B2rEmuProcCRC(ptHost->au4PatCRC1, ptHost->pcScriptPtr, ptCfgFile->OUT_I);


    
        //-set test pattern 
        tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
        tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
        tFrmAddr.u4Y1 = PHYSICAL(ptHost->u4YAddr1);
        tFrmAddr.u4C1 = PHYSICAL(ptHost->u4CAddr1);;
        if (B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr) !=  B2R_HAL_OK)
        {
            return FALSE;
        }
        
        //-record crc
        ptHost->eSt = EMU_ST_CRC_START;
        
        //-wait finish
        zMsgSize = sizeof(EMU_NTY_MSG_T);
        VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
        
        //-compare crc
        if (rMsg.eSt == EMU_ST_CRC_DONE)
        {
            UINT32 u4Mask;
            u4Mask = ptCfgFile->OUT_I ? 0x4:0;
            //-load  CRC
            if (!_B2rCmpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask) || 
                !_B2rCmpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask))
            {
                Printf("compare fail at (%d/%d)\n", ptHost->u4Idx, ptHost->u4Total);
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                _B2rDumpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask);
                u4CmpErrCnt++;
            }
            else
            {
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                _B2rDumpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask);
            }
        }
        else
        {
            //other error
            u4ErrCnt++;
        }
    }

    //@close b2r
    B2R_HAL_Disable(hB2r);
    B2R_HAL_Close(hB2r);

    //deinitialization
    _B2rV1EnvDeinit(u1B2rId, fgOld);

    //print result
    if (u4ErrCnt || u4CmpErrCnt)
    {
        Printf("list:%s  compare fail, error type(%d, %d), please check\n", ucRootPath, u4ErrCnt, u4CmpErrCnt);
    }
    else
    {
        Printf("list:%s, items(%d), compare Ok\n", ucRootPath, u4FrmCnt);
    }

    return TRUE;

}

/*
it's a common test cmd, and all the parameters come from cfg file
*/
//bit[0-7]     fail or ok
//bit[8-16]   other error
//bit[17-32]  cmp error

static BOOL _B2rEmuTestItem(B2R_EMU_HOST_T *ptHost, CHAR *ucRootPath, UINT32 u4FrmCnt)
{
    B2R_CFG_FILE *ptCfgFile;
    UINT32 u4CmpErrCnt;
    UINT32 u4ErrCnt;
    UINT32 u4IdxOffset = 0;
    UINT8  u1Path;
    
    //file
    UINT32 u4Size;
    EMU_FILE_HANDLE hFile;
    UCHAR ucFilePath[256];

    //b2r part
    B2R_HANDLE_T hB2r;     
    B2R_HAL_3D_T t3D;
    B2R_HAL_TIMING_T tTiming;
    __TIMING_TBL_T *ptTbl;
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    B2R_HAL_COLOR_FMT_T tColorFmt;
    B2R_HAL_MF_T tMF;

    if (!ucRootPath || !ptHost)
    {
        Printf("Arg invalid, please check.\n");
        return FALSE;
    }

    hB2r = ptHost->hB2r;
    ptCfgFile = &ptHost->tCfg;

    //Printf("Test item, path = %s, cnt = %d\n", ucRootPath, u4FrmCnt);

    //load cfg file
    x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/b2r.cfg", ucRootPath);
    hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", ucFilePath);
        return FALSE;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, ptHost->pcScriptPtr,u4Size);
    emu_fclose(hFile);

    ptHost->pcScriptPtr[u4Size] = '\0';
    _B2rEmuProcCfgFile(ptCfgFile, ptHost->pcScriptPtr);

    if (B2R_HAL_Reset(hB2r) != B2R_HAL_OK)
    {
        return FALSE;
    }

    //3D type
    if (ptHost->TWO_ADDR)
    {
        t3D.fg3D = TRUE;
        t3D.u4Layout = B2R_3D_FRAME_SEQUENTIAL;
    }
    else if (ptCfgFile->TAG_REMOVAL)
    {
        t3D.fg3D = TRUE;
        t3D.u4Layout = B2R_3D_SIDE_BY_SIDE_REAL_D;
        t3D.u4Data   = B2R_MK_REALD_DATA(2, ptCfgFile->H_OFFSET);
    }
    else if (ptCfgFile->H_OFFSET)
    {
        t3D.fg3D = TRUE;
        t3D.u4Layout = B2R_3D_SIDE_BY_SIDE;
        t3D.u4Data   = ptCfgFile->H_OFFSET;
    }
    else if(ptCfgFile->V_OFFSET)
    {
        t3D.fg3D = TRUE;
        t3D.u4Layout = B2R_3D_TOP_N_BOTTOM;
        t3D.u4Data   = ptCfgFile->V_OFFSET;
    }
    else
    {
        t3D.fg3D = FALSE;
        t3D.u4Layout = B2R_3D_NONE;
    }
    B2R_HAL_Set(hB2r, B2R_HAL_3DTYPE, &t3D);

    //Timing 
    if (ptCfgFile->OUT_I)
    {
        ptTbl = _b2r_query_I_timing(ptCfgFile->HDEW, (ptCfgFile->VDEW << 1), ptHost->u4Rate, ptHost->u4Scale);
    }
    else
    {
        ptTbl = _b2r_query_P_timing(ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate, ptHost->u4Scale);
    }

    if (!ptTbl)
    {
        Printf("non-standard timing, please check! arg(%d,%d,%d,%d) \n", ptCfgFile->HDEW, ptCfgFile->VDEW, ptHost->u4Rate,ptHost->u4Scale);
        return FALSE;
    }
    
    tTiming.fgProg   = ptCfgFile->OUT_I ? 0 : 1;
    tTiming.u4HTotal = ptTbl->u2HTotal;
    tTiming.u4VTotal = ptTbl->u2VTotal;
    tTiming.u4Rate   = ptHost->u4Rate;
    tTiming.u4Scale  = ptHost->u4Scale;
    tTiming.u4SrcW   = ptCfgFile->HDEW;
    tTiming.u4SrcH   = tTiming.fgProg? ptCfgFile->VDEW : (ptCfgFile->VDEW << 1);    
    tTiming.u4Clock  = t3D.fg3D ? (ptTbl->u4Clock << 1) : ptTbl->u4Clock; 
    B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming);
    Printf("Base Timing name: %s; 3D:%s \n", ptTbl->acName, t3D.fg3D ? "On":"Off");

 
    //dram layout
    tDramLayout.eAddrMode = (B2R_ADDR_MODE_T)ptCfgFile->ADDR_MODE;
    tDramLayout.eSwapMode = (B2R_SWAP_MODE_T)ptCfgFile->ADDR_SWAP;
    tDramLayout.ePackMode = (B2R_PACK_MODE_T)ptCfgFile->PACK_MODE;
    tDramLayout.u4Pitch   = ptCfgFile->LINE_OFFSET;
    B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout);

 
    //color format
    tColorFmt.eMode   = (ptCfgFile->M_422 == 1) ? B2R_CM_422 : B2R_CM_420;
    tColorFmt.u1Depth = ptCfgFile->DEPTH;
    B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt);

    u1Path = _B2rFindPath(ptHost);
    //mirror or flip
    if(u1Path != PATH_NS)
    {
        tMF.fgFlip   = _b2r_par[u1Path].fgFlip;
        tMF.fgMirror = _b2r_par[u1Path].fgMirror;
        Printf("B2rId(%d),fgOldB2r(%d)!\n",
            _b2r_par[u1Path].u1Id, _b2r_par[u1Path].fgOldB2r);
    }
    else
    {
        tMF.fgFlip   = ptCfgFile->FLIP;
        tMF.fgMirror = ptCfgFile->MIRROR;
    }
    Printf("Flip(%d),Mirror(%d)!\n", tMF.fgFlip, tMF.fgMirror);
    B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF);
 
    B2R_HAL_Config(hB2r);
    B2R_HAL_Enable(hB2r);

    //@test frame 
    ptHost->u4Idx   = 0;
    ptHost->u4Total = ptHost->TWO_ADDR ? (u4FrmCnt>>1) : u4FrmCnt;
    u4IdxOffset = u4FrmCnt >> 1;  //for two addr mode use
    u4ErrCnt = u4CmpErrCnt = 0;
    FOR_EACH_FRAME(ptHost)
    {
        B2R_HAL_ADDR_T tFrmAddr;
        EMU_NTY_MSG_T rMsg;
        UINT16 u2MsgQIdx;
        SIZE_T zMsgSize;

        //-load  rawdata
        //@left view
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddr0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr0, u4Size);

        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddr0,u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr0, u4Size);

        //load len
        if (ptCfgFile->PACK_MODE & 0xF0)
        {
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenY", ucRootPath, ptHost->u4Idx);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4YAddrLen0,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddrLen0, u4Size);

            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenCbCr", ucRootPath, ptHost->u4Idx);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4CAddrLen0,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddrLen0, u4Size);
            
        }

        //load CRC
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
        emu_fclose(hFile);
        _B2rEmuProcCRC(ptHost->au4PatCRC0, ptHost->pcScriptPtr, ptCfgFile->OUT_I);


        //@right view        
        if (ptHost->TWO_ADDR)
        {
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4YAddr1, u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr1, u4Size);

            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4CAddr1,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr1, u4Size);

            //load len
            if (ptCfgFile->PACK_MODE & 0xF0)
            {
                x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenY", ucRootPath, ptHost->u4Idx + u4IdxOffset);
                hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
                if (EMU_FILE_HANDLE_NULL == hFile)
                {
                    Printf("Open file fail: %s\n", ucFilePath);
                    u4ErrCnt++;
                    continue;
                }
                u4Size = emu_fsize(hFile);
                emu_fread(hFile, (CHAR*)ptHost->u4YAddrLen1,u4Size);
                emu_fclose(hFile);
                HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddrLen1, u4Size);
            
                x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenCbCr", ucRootPath, ptHost->u4Idx + u4IdxOffset);
                hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
                if (EMU_FILE_HANDLE_NULL == hFile)
                {
                    Printf("Open file fail: %s\n", ucFilePath);
                    u4ErrCnt++;
                    continue;
                }
                u4Size = emu_fsize(hFile);
                emu_fread(hFile, (CHAR*)ptHost->u4CAddrLen1,u4Size);
                emu_fclose(hFile);
                HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddrLen1, u4Size);
                
            }


            //load CRC
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
            emu_fclose(hFile);
            _B2rEmuProcCRC(ptHost->au4PatCRC1, ptHost->pcScriptPtr, ptCfgFile->OUT_I);
        }

        //-set test pattern 
        tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
        tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
        tFrmAddr.u4Y1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4YAddr1) : 0;
        tFrmAddr.u4C1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4CAddr1) : 0;
        B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr);

        //load len
        if (ptCfgFile->PACK_MODE & 0xF0)
        {
            B2R_HAL_ADDR_LEN_T tFrmAddrlen;
            tFrmAddrlen.u4YLen0 = PHYSICAL(ptHost->u4YAddrLen0);
            tFrmAddrlen.u4CLen0 = PHYSICAL(ptHost->u4CAddrLen0);
            tFrmAddrlen.u4YLen1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4YAddrLen1) : 0;
            tFrmAddrlen.u4CLen1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4CAddrLen1) : 0;
            B2R_HAL_Set(hB2r, B2R_HAL_ADDR_LEN, &tFrmAddrlen);
        }
        
        //-record crc
        ptHost->eSt = EMU_ST_CRC_START;
        
        //-wait finish
        zMsgSize = sizeof(EMU_NTY_MSG_T);
        VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
        
        //-compare crc
        if (rMsg.eSt == EMU_ST_CRC_DONE)
        {
            UINT32 u4Mask;
            BOOL   fgRet = 0;
            u4Mask = ptCfgFile->OUT_I ? 0x4:0;
            if(u1Path != PATH_NS)
            {
                if(_b2r_par[u1Path].fgFlip)
                {
                    u4Mask = u4Mask | FLIP_MASK;
                }
                if(_b2r_par[u1Path].fgMirror)
                {
                    u4Mask = u4Mask | MIRROR_MASK;
                }
            }
            else
            {
                if(ptCfgFile->FLIP)
                {
                    u4Mask = u4Mask | FLIP_MASK;
                }
                if(ptCfgFile->MIRROR)
                {
                    u4Mask = u4Mask | MIRROR_MASK;
                }
            }
            //-load  CRC
            fgRet = !_B2rCmpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
            fgRet |= (ptHost->TWO_ADDR ? !_B2rCmpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask) : 0);

            if (fgRet)
            {
                Printf("compare fail at (%d/%d)\n", ptHost->u4Idx, ptHost->u4Total);
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                if (ptHost->TWO_ADDR)
                {
                    _B2rDumpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask);
                }
                u4CmpErrCnt++;
            }
            else
            {
                _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                if (ptHost->TWO_ADDR)
                {
                    _B2rDumpCRC(ptHost->au4PatCRC1, ptHost->u4CRC1, u4Mask);
                }
            }
        }
        else
        {
            //other error
            u4ErrCnt++;
        }
    }

    //@close b2r
    B2R_HAL_Disable(hB2r);

    //record reslut
    ptHost->ptCurrItem->total = ptHost->u4Total;
    ptHost->ptCurrItem->fail  = (u4CmpErrCnt << 16)| u4ErrCnt ;
    ptHost->ptCurrItem->flag  = (u4ErrCnt || u4CmpErrCnt) ? 0x00000001 : 0;

    return TRUE;
    
}

static void _B2rEmuListResult(EMU_LIST_T *ptList)
{
    EMU_ITEM_T *ptItem;
    UINT32 j,i,k;

    Printf("==============list result==============\n");
    Printf("list name: %s\n", ptList->data);

    i = j = k = 0;
    ptItem = ptList->rItemHead.next;
    while (ptItem)
    {
        if (ptItem->flag & 0x00000001)
        {
            j++;
        }

        if (!ptItem->total)
        {
            k++;
        }

        i++;
        ptItem = ptItem->next;
    }
    Printf("test items: %d, fail %d\n", i, j + k);

    if (j)
    {
        Printf("fail items:\n");
        ptItem = ptList->rItemHead.next;
        i = 0;
        while (ptItem)
        {
            if (ptItem->flag & 0x00000001)
            {
                Printf("  <%d>%s; total:%d, fail:(%d,%d) \n", i++, ptItem->data, ptItem->total, (ptItem->fail>>16), (ptItem->fail & 0xFFFF));
            }
            ptItem = ptItem->next;
        }
    }

    if (k)
    {
        Printf("warning items (total is 0) \n");
        ptItem = ptList->rItemHead.next;
        i = 0;
        while (ptItem)
        {
            if (!ptItem->total)
            {
                Printf("  <%d>%s\n", i++, ptItem->data);
            }
            ptItem = ptItem->next;
        }
    }

}

static VOID _B2rEmuListThread(VOID *pvArg)
{
    B2R_EMU_HOST_T *ptHost;
    UINT8 u1B2rId;
    BOOL  fgOld;
    UINT32 u4Idx;
    B2R_CfG_T rB2rCfg;
    B2R_HANDLE_T hB2r;

    if (!pvArg)
    {
        return;
    }

    //Printf("Enter %s\n", __FUNCTION__);

    ptHost = (B2R_EMU_HOST_T *)(*(UINT32 *)pvArg);

    //Get cfg from list 
    u1B2rId = ptHost->ptCurrList->tCfg.u1B2rId;
    fgOld   = ptHost->ptCurrList->tCfg.fgOld;

    Printf("Enter b2r %s, %s\n", (fgOld?"Old":"New"), (u1B2rId == 0 ? "HD":"SD"));
    Printf("RootPath %s\n", ptHost->ptCurrList->tCfg.acRootPath);

    B2R_HAL_Init();

    if (fgOld)
    {
        static BOOL fgRegIsr = 0;
        _B2REmu_HW_V1_Init(u1B2rId);
        if (!fgRegIsr)
        {
            x_os_isr_fct pfnOldIsr;
            VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V1_ISR, &pfnOldIsr) == OSR_OK);
            //fgRegIsr = TRUE;
        }
    }
    else
    {
        static BOOL fgRegIsr = 0;
        _B2REmu_HW_V2_Init(u1B2rId);
        if (!fgRegIsr)
        {
            x_os_isr_fct pfnOldIsr;
            VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V2_ISR, &pfnOldIsr) == OSR_OK);
            //fgRegIsr = TRUE;
        }
    }

    //Start test B2R     
    //@open b2r     
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    rB2rCfg.u4Data = u4Idx;
    rB2rCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&rB2rCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw\n");
        return ;
    }

    ptHost->fgOldB2r = fgOld;
    ptHost->hB2r     = hB2r;
    ptHost->u4Rate   = ptHost->ptCurrList->tCfg.u4Rate;
    ptHost->u4Scale  = ptHost->ptCurrList->tCfg.u4Scale;
    ptHost->TWO_ADDR = ptHost->ptCurrList->tCfg.fgTwoAddr;


    for (ptHost->ptCurrItem = ptHost->rItemHead.next ; ptHost->ptCurrItem; ptHost->ptCurrItem = ptHost->ptCurrItem->next)
    {
        char *pcPath;
        char *pcTmp;
        char *pcCnt;
        UINT32 u4Cnt;

        pcTmp = ptHost->pcScriptPtr;

        //process item cmd
        x_snprintf(pcTmp, 512, "%s%s", ptHost->ptCurrList->tCfg.acRootPath, ptHost->ptCurrItem->data);

        //cut ',' char in the ptCurrItem->data
        pcPath = emu_strsep(&pcTmp, ",");
        pcCnt  = emu_strsep(&pcTmp, "\n");
        u4Cnt  = emu_str2int(pcCnt);

        x_snprintf(ptHost->pcPath, 512, "%s", pcPath);

        Printf("Item start: %s, cnt:%d\n", pcPath, u4Cnt);

        if (!_B2rEmuTestItem(ptHost, ptHost->pcPath, u4Cnt))
        {
            //other error
            ptHost->ptCurrItem->total = 0;
        }

        Printf("Item finish, total:%d, fail:0x%08x)\n", ptHost->ptCurrItem->total, ptHost->ptCurrItem->fail);
    
    }

    B2R_HAL_Close(hB2r);

    //print result
    _B2rEmuListResult(ptHost->ptCurrList);

    Printf("Leave %s\n", __FUNCTION__);

    _B2rEmuBufferDeinit(u1B2rId);

    if(_uStressTestCnt != 0)
    {
        UINT8* pu1FilePath = NULL;

        pu1FilePath = VIRTUAL(pu1StressFilepath);
        _uStressTestCnt -- ;
        _B2rEmuTestList(pu1FilePath, PATH_1);
    }
    else
    {
        if(pu1StressFilepath)
        {
            BSP_FreeAlignedDmaMemory(pu1StressFilepath);
            pu1StressFilepath = NULL;
        }
    }
    
    x_thread_exit();
}

static CHAR *_B2rEmuProcListCfg(EMU_LIST_CFG_T *ptListCfg, CHAR* s)
{
    CHAR *pcToken;

    if (!ptListCfg || !s)
    {
        return NULL;
    }

    pcToken = emu_strsep(&s, "\n");

    while (pcToken != NULL)
    {
        //segment
        if (pcToken[0] == '[')
        {
            if (x_strncmp(pcToken, "[CFG]", 5) == 0)
            {
                while ((pcToken = emu_strsep(&s, "\n")) != NULL)
                {
                    if (pcToken[0] == '[') //end
                    {
                        break;
                    }
                    else if (pcToken[0] == '#')
                    {
                        //mark line
                    }
                    else if (x_strncmp(pcToken, "ver:", 4) == 0)
                    {
                        printf("token: %s\n", pcToken);
                        if (x_strncmp(pcToken + 4, "v1", 2) == 0)  //old b2r
                        {
                            ptListCfg->fgOld = TRUE;
                        }
                        else if (x_strncmp(pcToken + 4, "v2", 2) == 0)  //new b2r
                        {
                            ptListCfg->fgOld = FALSE;
                        }
                    }
                    else if (x_strncmp(pcToken, "id:", 3) == 0)
                    {
                        printf("token: %s\n", pcToken);        
                        ptListCfg->u1B2rId = 0xFF;
                        if (x_strncmp(pcToken + 3, "0", 1) == 0)  //  hd b2r
                        {
                            ptListCfg->u1B2rId = 0;
                        }
                        else if (x_strncmp(pcToken + 3, "1", 1) == 0)  //sd b2r
                        {
                            ptListCfg->u1B2rId = 1;
                        }
                    }
                    else if (x_strncmp(pcToken, "RootPath:", 9) == 0)
                    {
                        x_memset(ptListCfg->acRootPath, 0, 256);
                        x_strcpy(ptListCfg->acRootPath, pcToken + 9);
                        printf("token: %s\n", pcToken);        
                    }
                    else if (x_strncmp(pcToken, "rate:", 5) == 0)
                    {
                        printf("token: %s\n", pcToken);
                        ptListCfg->u4Rate = emu_str2int(pcToken + 5);
                    }
                    else if (x_strncmp(pcToken, "scale:", 6) == 0)
                    {
                        printf("token: %s\n", pcToken);
                        ptListCfg->u4Scale = emu_str2int(pcToken + 6);
                    }
                    else if (x_strncmp(pcToken, "two_addr:", 6) == 0)
                    {
                        printf("token: %s\n", pcToken);
                        ptListCfg->fgTwoAddr = 1;
                    }
                }
            }
            else if (x_strncmp(pcToken, "[ITEM]", 6) == 0)
            {
                break;
            }
            else
            {
                pcToken = emu_strsep(&s, "\n");
            }
        }
        else
        {
            pcToken = emu_strsep(&s, "\n");
        }
    }

    return pcToken;
}

static VOID _B2rEmuProcListItem(B2R_EMU_HOST_T *ptHost, CHAR* s)
{
    CHAR *pcToken;
    UINT32 i = 0;
    
    if (!ptHost || !s)
    {
        return;
    }

    if (x_strncmp(s, "[ITEM]", 6) != 0)
    {
        return;
    }
    
    while (*s++ != '\0');

    while ((pcToken = emu_strsep(&s, "\n")) != NULL)
    {
        //Printf("token: %s\n", pcToken);
    
        if (pcToken[0] == '[')
        {
            break;
        }
        else if (pcToken[0] == '#')
        {
            continue;
        }
        else if (x_strncmp(pcToken, "__END__", 7) == 0)
        {
            break;
        }
        else
        {
            if (i < ptHost->u4MaxItem)
            {
                emu_item_add(ptHost->ptItems, i, pcToken);
            }
            i++;
            Printf("Item find:%s\n", pcToken);
        }
    }


    if (i)
    {
        ptHost->rItemHead.next = &(ptHost->ptItems[0]);
    }

    ptHost->u4Items = i;
    
}

VOID _B2rEmuTestList(CHAR *pucListPath, UINT8 u1Path)
{
    HANDLE_T hThrd;
    CHAR aucThrdName[20];
    B2R_EMU_HOST_T *ptHost;
    UINT32 u4HostAddr;
    UINT8 u1Id = 0;
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;
    CHAR *pucTmp  = NULL;
    UINT32 u4BuffAddr = 0;
    EMU_LIST_CFG_T* ptListCfg;

    if (!pucListPath)
    {
        Printf("invalid list path, please check!\n");
        return;
    }

    if(u1Path >= PATH_NS)
    {
        Printf("invalid u1Path, please check!\n");
        return;
    }

    if(_uStressTestCnt > 0 &&
        pu1StressFilepath)
    {
        UINT8* pu1FilePath = NULL;

        pu1FilePath = VIRTUAL(pu1StressFilepath);
        strcpy((char *)pu1FilePath, (char *)pucListPath);
    }
    
    //open list
    hFile = emu_fopen(pucListPath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", pucListPath);
        return;
    }
    u4Size = emu_fsize(hFile);
    u4BuffAddr = BSP_AllocAlignedDmaMemory(u4Size + 16 + sizeof(EMU_LIST_CFG_T), 32);
    pucTmp  = (CHAR *)VIRTUAL(u4BuffAddr);
    emu_fread(hFile, pucTmp,u4Size);
    pucTmp[u4Size] = pucTmp[u4Size + 1] = '\0';
    emu_fclose(hFile);
    
    if (!emu_str_find(pucTmp, "#!list!"))
    {
        Printf("invalid list file");
        return;
    }

    //proc list cfg segment
    ptListCfg = (EMU_LIST_CFG_T *)(pucTmp + u4Size + 8);
    ptListCfg->u1B2rId   = 0xFF;
    ptListCfg->u4Rate    = B2R_RATE_DEFAULT;
    ptListCfg->u4Scale   = B2R_SCALE_DEFAULT;
    ptListCfg->fgTwoAddr = FALSE;
    pucTmp = _B2rEmuProcListCfg(ptListCfg, pucTmp);
    
    if(_b2r_par[u1Path].bFgEn)
    {
        ptListCfg->u1B2rId = _b2r_par[u1Path].u1Id;
        ptListCfg->fgOld   = _b2r_par[u1Path].fgOldB2r;
    }
    
    u1Id = ptListCfg->u1B2rId;
    
    if (u1Id > 1)
    {
        Printf("Cfg file fail: %s\n", pucListPath);
        BSP_FreeAlignedDmaMemory(u4BuffAddr);
        return;
    }
    
    //init host
    _B2rEmuBufferInit(u1Id);
    _B2rEmuVariableInit(u1Id);
    ptHost = _B2rEmuId2Obj(u1Id);

    //proc list item segment
    _B2rEmuProcListItem(ptHost, pucTmp);

    x_snprintf(ptHost->ptCurrList->data, 256, "%s", pucListPath);
    x_memcpy((VOID *)(&ptHost->ptCurrList->tCfg), (VOID *)ptListCfg, sizeof(EMU_LIST_CFG_T));
    ptHost->ptCurrList->rItemHead = ptHost->rItemHead;

    //free buffer
    BSP_FreeAlignedDmaMemory(u4BuffAddr);


    //create a test thread to do test
    x_snprintf(aucThrdName, sizeof(aucThrdName), "B2rEmu%2d%5d", u1Id, _uStressTestCnt);
    u4HostAddr = (UINT32)ptHost;
    VERIFY(x_thread_create(&hThrd,
        aucThrdName, VSYNC_STACK_SIZE, VSYNC_THREAD_PRIORITY,
        (x_os_thread_main_fct)_B2rEmuListThread, sizeof(UINT32), (VOID *)&u4HostAddr) == OSR_OK);

}

static BOOL _B2rEmuTestStandardTiming(UINT32 u1B2rId, BOOL fgOld)
{
    B2R_HANDLE_T hB2r;
    B2R_CFG_FILE tCfgFile;
    UINT32 u4Idx;
    B2R_EMU_HOST_T *ptHost;
    B2R_CfG_T tCfg;
    UINT32 i, j, k;
    __TIMING_MATRIX_T *ptMatrix = NULL;

    
    Printf("\n***standard timing test (%s, %s)******\n", (u1B2rId == 0 ? "HD":"SD"), (fgOld ? "Old":"New"));

    //Global initialization 
    B2R_HAL_Init();
    _B2rV1EnvInit(u1B2rId, fgOld);
    ptHost = _B2rEmuId2Obj(u1B2rId);

    //load cfg file
    _B2rEmuProcCfgFile(&tCfgFile, NULL);

    for (k = 0; k < 2; k++)
    {
        tCfgFile.OUT_I = k % 2;
    
        ptMatrix = tCfgFile.OUT_I ? _standard_timing_matrix_I : _standard_timing_matrix;
        
        for (j = 0; ptMatrix[j].u2MaxHAct != 0; j++)
        {
            for (i = 0; i < ptMatrix[j].u2Size; i++)
            {
                ptHost->u4Idx   = 0;
                ptHost->u4Total = 3;
                ptHost->u4Rate  = ptMatrix[j].ptTbl[i].u4Rate;
                ptHost->u4Scale = ptMatrix[j].ptTbl[i].u4Scale;
                ptHost->fgOldB2r = fgOld;
        
                //cfg timing
                tCfgFile.HDEW = ptMatrix[j].u2MaxHAct;
                tCfgFile.VDEW = tCfgFile.OUT_I ? (ptMatrix[j].u2MaxVAct>>1) : ptMatrix[j].u2MaxVAct;
                tCfgFile.LINE_OFFSET = ((tCfgFile.HDEW + 31)>>5)<<5;
        
                if (ptMatrix[j].ptTbl[i].u4Clock > 297000000)
                {
                    continue;
                }
            
                Printf("verify doing (%d, %d) (%d, %d)\n", tCfgFile.HDEW, tCfgFile.VDEW, ptHost->u4Rate, ptHost->u4Scale);
        
                //Start test B2R     
                //@open b2r     
                u4Idx = 0x10;
                u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
                u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
                tCfg.u4Data = u4Idx;
                tCfg.pvData = (VOID *)ptHost;
                if ((hB2r = B2R_HAL_Open(&tCfg)) == B2R_NULL_HANDLE)
                {
                    Printf("Can't open a b2r hw\n");
                    return FALSE;
                }
        
                //@cfg b2r     
                B2R_HAL_Reset(hB2r);
                _B2rEmu2D(hB2r, &tCfgFile, ptHost);
                B2R_HAL_Enable(hB2r);
        
                //@test frame    
                FOR_EACH_FRAME(ptHost)
                {
                    B2R_HAL_ADDR_T tFrmAddr;
                    EMU_NTY_MSG_T rMsg;
                    UINT16 u2MsgQIdx;
                    SIZE_T zMsgSize;
                
                    //-set test pattern 
                    tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
                    tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
                    tFrmAddr.u4Y1 = tFrmAddr.u4C1 = 0;
                    if (B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr) !=  B2R_HAL_OK)
                    {
                        return FALSE;
                    }
                    
                    //-record info
                    ptHost->eSt = EMU_ST_VSYNC_START;
                    
                    //-wait finish
                    zMsgSize = sizeof(EMU_NTY_MSG_T);
                    VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
                    
                    //-compare crc
                    if (rMsg.eSt == EMU_ST_VSYNC_DONE)
                    {
                        UINT32 u4Detal;
                        B2R_VSYNC_T *ptVsync;
                        ptVsync = &(ptHost->tVsync);

                        u4Detal = 90000*ptHost->u4Scale/ptHost->u4Rate;
                        if (tCfgFile.OUT_I)
                        {
                            u4Detal >>= 1;
                        }

                        Printf("ref Detal(%d), real Detal M/N/A => (%d,%d,%d)\n", u4Detal, ptVsync->u4Max,ptVsync->u4Min, ptVsync->u4Ave);
                    }
                    else
                    {
                        //other error
                    }
                }
        
                //@close b2r
                B2R_HAL_Disable(hB2r);
                B2R_HAL_Close(hB2r);
        
            }
        
        }

    }

    //deinitialization
    _B2rV1EnvDeinit(u1B2rId, fgOld);

    Printf("verify done\n");

    return TRUE;
}


static BOOL _B2rEmuRunB2r(UINT8 u1B2rId, BOOL fgOld)
{
    UINT32 u4Idx;
    B2R_HANDLE_T hB2r;
    B2R_CFG_FILE tCfgFile;
    B2R_EMU_HOST_T *ptHost;
    B2R_CfG_T tCfg;
    B2R_HAL_ADDR_T tFrmAddr;
    
    Printf("\n***Run B2r (%s, %s)******\n", (u1B2rId ? "SD":"HD"), (fgOld? "Old":"New"));

    //Global initialization 
    B2R_HAL_Init();
    _B2rV1EnvInit(u1B2rId, fgOld);
    ptHost = _B2rEmuId2Obj(u1B2rId);


    //load cfg file
    _B2rEmuProcCfgFile(&tCfgFile, NULL);

        
    //Start test B2R     
    //@open b2r 
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    tCfg.u4Data = u4Idx;
    tCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&tCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw 0\n");
        return FALSE;
    }
    ptHost->hB2r = hB2r;
    ptHost->fgOldB2r = fgOld;

  
    //@cfg b2r
    B2R_HAL_Reset(hB2r);
    ptHost->u4Rate  = 30;
    ptHost->u4Scale = 1;
    _B2rEmu2D(hB2r, &tCfgFile, ptHost);
    B2R_HAL_Enable(hB2r);
 

    tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
    tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
    tFrmAddr.u4Y1 = tFrmAddr.u4C1 = 0;
    B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr);

    Printf("B2R start, Please check\n");

    return TRUE;
}


static BOOL _B2rEmuStopB2r(UINT8 u1B2rId, BOOL fgOld)
{
    B2R_HANDLE_T hB2r;
    B2R_EMU_HOST_T *ptHost;
    
    Printf("\n***Stop B2r (%s, %s)******\n", (u1B2rId ? "SD":"HD"), (fgOld? "Old":"New"));

    ptHost = _B2rEmuId2Obj(u1B2rId);

    hB2r = ptHost? ptHost->hB2r : B2R_NULL_HANDLE;

    if (hB2r != B2R_NULL_HANDLE)
    {
        //@close b2r
        B2R_HAL_Disable(hB2r);
        B2R_HAL_Close(hB2r);

        //deinitialization
        _B2rV1EnvDeinit(u1B2rId, fgOld);
    }

    return TRUE;
}

static void _B2rEmuIntExtReg(void)
{
    UINT32 i;

    for (i = 0; i < sizeof(_arRegExt)/sizeof(_arRegExt[0]); i++)
    {
        _arRegExt[i].u4Reg   = 0;
        _arRegExt[i].u4Value = 0;
    }
}

static void _B2rEmuSetExtReg(void)
{
    UINT32 i;
    
    for (i = 0; i < sizeof(_arRegExt)/sizeof(_arRegExt[0]); i++)
    {
        if (_arRegExt[i].u4Reg)
        {
            Printf("Set ext reg:0x%08x = 0x%08x \n", _arRegExt[i].u4Reg, _arRegExt[i].u4Value);
            *((UINT32 *)_arRegExt[i].u4Reg) = _arRegExt[i].u4Value;
        }
    }
}

static void _B2rEmuLoadExtReg(UINT32 u4Idx, CHAR *ptStr)
{
    if (!ptStr || (u4Idx >= sizeof(_arRegExt)/sizeof(_arRegExt[0])))
    {
        return;
    }

    if (ptStr[0] == 'r' && ptStr[1] == 'e' && ptStr[2] == 'g' )
    {
        UINT32 i, u4Tmp;
        for (u4Tmp = 0, i = 0; i < 8; i++)
        {
            u4Tmp <<= 4;
            u4Tmp |= (ptStr[6 + i] >= 'a') ? (ptStr[6 + i] -'a' + 10) : (ptStr[6 + i] - '0');
        }
        _arRegExt[u4Idx].u4Reg   = u4Tmp;
        
        for (u4Tmp = 0, i = 0; i < 8; i++)
        {
            u4Tmp <<= 4;
            u4Tmp |= (ptStr[17 + i] >= 'a') ? (ptStr[17 + i] -'a' + 10) : (ptStr[17 + i] - '0');
        }
        _arRegExt[u4Idx].u4Value = u4Tmp;
    }
}

static BOOL _B2rEmuBoundaryTest(UINT32 u1B2rId, BOOL fgOld, B2R_RANGE_T *hRange, B2R_RANGE_T *vRange, 
    UINT32 u4YCol, UINT32 u4CCol, B2R_HVTotal_T *ptVH, BOOL fgInterlaced, B2R_EXT_DATA *ptExtData)
{
    B2R_CfG_T tCfg;
    UINT32 u4Idx;
    B2R_EMU_HOST_T *ptHost;
    UINT32 i, j;
    UINT32 u4MaxHDEW, u4MaxVDEW, u4Pitch;
    UINT32 u4CmpErrCnt, u4ErrCnt;
    BOOL OUT_I;
    //b2r part
     __TIMING_TBL_T *ptTbl;
    B2R_HANDLE_T hB2r;     
    UINT32 u4Depth;
    B2R_PACK_MODE_T ePackMode;    

    if (!hRange || !vRange || !ptVH)
    {
        Printf("arg invalid\n");
    }

    Printf("\n***Boundary test for V-active(%d-%d by %d) & H-active(%d-%d by %d)*****\n", 
        vRange->u4Start, vRange->u4End, vRange->u4Step,    
        hRange->u4Start, hRange->u4End, hRange->u4Step);


    //Global initialization 
    B2R_HAL_Init();
    _B2rV1EnvInit(u1B2rId, fgOld);
    ptHost = _B2rEmuId2Obj(u1B2rId);
    OUT_I = fgInterlaced;
    u4MaxHDEW = ptVH->u4MaxWidth;
    u4MaxVDEW = ptVH->u4MaxHeight;
    u4Pitch   = ((u4MaxHDEW + 15) /16) * 16;
    ptHost->u4Rate  = ptVH->u4DefRate;  //30;
    ptHost->u4Scale = ptVH->u4DefScale; //1;
    ptHost->tCfg.OUT_I = OUT_I;
    u4Depth   = 8;
    ePackMode = B2R_PM_NORMAL;
    
    if (ptExtData)
    {
        u4Pitch = ptExtData->u4RawPitch;
        u4Depth   = ptExtData->u4Depth;
        ePackMode = ptExtData->ePackMode;
    }

    b2r_cmodel_init();
    b2r_cmodel_set_max_hv(u4MaxHDEW, u4MaxVDEW, u4Pitch);
    b2r_cmodel_set_color(u4YCol, u4CCol);
    b2r_cmodel_set_crc_mode(fgOld);
    b2r_cmodel_set_bit_depth(u4Depth);

    if (ptExtData)
    {
        b2r_cmodel_do_ext(ptExtData->acYRaw, ptExtData->acCRaw, 
            ((ptExtData->acY422[0] == 0)? NULL : ptExtData->acY422), 
            ((ptExtData->acC422[0] == 0)? NULL : ptExtData->acC422),
            ptExtData->u4422Pitch);
    }
    else
    {
        b2r_cmodel_do();
    }

    if (OUT_I)
    {
        ptTbl = _b2r_query_I_timing(u4MaxHDEW, u4MaxVDEW, ptHost->u4Rate, ptHost->u4Scale);
    }
    else 
    {
        ptTbl = _b2r_query_P_timing(u4MaxHDEW, u4MaxVDEW, ptHost->u4Rate, ptHost->u4Scale);
    }


    if (!ptTbl)
    {
        Printf("non-standard timing, please check! arg(%d,%d,%d,%d) \n", u4MaxHDEW, u4MaxVDEW, 30, 1);
        return FALSE;
    }

    //load len
    if ((ePackMode & 0xF0) && ptExtData)
    {
        EMU_FILE_HANDLE hFile;
        UINT32 u4Size;
        hFile = emu_fopen(ptExtData->acYRawLen, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ptExtData->acYRawLen);
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddrLen0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddrLen0, u4Size);

        hFile = emu_fopen(ptExtData->acCRawLen, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ptExtData->acCRawLen);
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddrLen0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddrLen0, u4Size);
    }

        
    //Start test B2R     
    //@open b2r     
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    tCfg.u4Data = u4Idx;
    tCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&tCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw\n");
        return FALSE;
    }

    ptHost->fgOldB2r = fgOld;

    u4CmpErrCnt = u4ErrCnt = 0;
    
    for (i = hRange->u4Start; i <= hRange->u4End; i += hRange->u4Step) //width
    {
        for (j = vRange->u4Start; j <= vRange->u4End; j += vRange->u4Step)
        {
            //B2R_HAL_3D_T t3D;
            B2R_HAL_TIMING_T tTiming;
            B2R_HAL_DRAM_LAYOUT_T tDramLayout;
            B2R_HAL_COLOR_FMT_T tColorFmt;
            B2R_HAL_MF_T tMF;
            B2R_HAL_ADDR_T tFrmAddr;
            EMU_NTY_MSG_T rMsg;
            UINT16 u2MsgQIdx;
            SIZE_T zMsgSize;
            B2R_HAL_ADDR_LEN_T tFrmAddrlen;

            if (OUT_I)
            {
                b2r_cmodel_set_parm(i, (j>>1));
                ptHost->au4PatCRC0[0] = b2r_cmodel_get_field_crc(1);
                ptHost->au4PatCRC0[4] = b2r_cmodel_get_field_crc(0);
            }
            else
            {
                b2r_cmodel_set_parm(i, j);
                ptHost->au4PatCRC0[0] = b2r_cmodel_get_crc();
            }

            B2R_HAL_Reset(hB2r);

            tTiming.fgProg   = OUT_I ? 0 : 1;
            tTiming.u4HTotal = ptTbl->u2HTotal;
            tTiming.u4VTotal = ptTbl->u2VTotal;
            tTiming.u4Rate   = ptHost->u4Rate;
            tTiming.u4Scale  = ptHost->u4Scale;
            tTiming.u4SrcW   = i;
            tTiming.u4SrcH   = j;    
            tTiming.u4Clock  = ptTbl->u4Clock; 
            B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming);
            Printf("Base Timing name: %s; 3D:%s \n", ptTbl->acName, FALSE ? "On":"Off");
            Printf("Src Width: %d, Src Height: %d\n", tTiming.u4SrcW, tTiming.u4SrcH);
            
            //dram layout
            tDramLayout.eAddrMode = B2R_AM_BLOCK1;
            tDramLayout.eSwapMode = B2R_SM_NO_SWAP;
            tDramLayout.ePackMode = ePackMode;//B2R_PM_NORMAL;
            tDramLayout.u4Pitch   = u4Pitch;
            B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout);
            
            
            //color format
            tColorFmt.eMode   = B2R_CM_420;
            tColorFmt.u1Depth = (UINT8)u4Depth; //8;
            B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt);
            
            //mirror or flip
            tMF.fgFlip   = FALSE;
            tMF.fgMirror = FALSE;
            B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF);
            
            B2R_HAL_Config(hB2r);

            _B2rEmuSetExtReg();
            
            B2R_HAL_Enable(hB2r);

            //test it
            b2r_cmodel_get_addr(&tFrmAddr.u4Y0, &tFrmAddr.u4C0);            
            //tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
            //tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
            tFrmAddr.u4Y1 = 0;
            tFrmAddr.u4C1 = 0;
            B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr);

            tFrmAddrlen.u4YLen0 = PHYSICAL(ptHost->u4YAddrLen0);
            tFrmAddrlen.u4CLen0 = PHYSICAL(ptHost->u4CAddrLen0);
            tFrmAddrlen.u4YLen1 = 0;
            tFrmAddrlen.u4CLen1 = 0;
            B2R_HAL_Set(hB2r, B2R_HAL_ADDR_LEN, &tFrmAddrlen);
            
            //-record crc
            ptHost->eSt = EMU_ST_CRC_START;
            
            //-wait finish
            zMsgSize = sizeof(EMU_NTY_MSG_T);
            VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
            
            //-compare crc
            if (rMsg.eSt == EMU_ST_CRC_DONE)
            {
                UINT32 u4Mask;
                BOOL   fgRet = 0;
                u4Mask = OUT_I ? 0x4:0;
                //-load  CRC
                fgRet = !_B2rCmpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
            
                if (fgRet)
                {
                    Printf("compare fail at (%d/%d)\n", ptHost->u4Idx, ptHost->u4Total);
                    _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                    u4CmpErrCnt++;
                }
                else
                {
                    _B2rDumpCRC(ptHost->au4PatCRC0, ptHost->u4CRC0, u4Mask);
                }
            }
            else
            {
                //other error
                u4ErrCnt++;
            }
          

            //@close b2r
            B2R_HAL_Disable(hB2r);
        }
    }

    if (u4ErrCnt || u4CmpErrCnt)
    {
        Printf("Compare error %d, %d\n", u4ErrCnt, u4CmpErrCnt);
    }

    B2R_HAL_Close(hB2r);

    //deinitialization
    _B2rV1EnvDeinit(u1B2rId, fgOld);

    Printf("verify done\n");

    UNUSED(u1B2rId);
    UNUSED(fgOld);

    return TRUE;
}


//-----------------------------------------------------------------------------
// Public function : 
//-----------------------------------------------------------------------------

/*
it's a common test cmd, and all the parameters come from cfg file
*/
//bit[0-7]     fail or ok
//bit[8-16]   other error
//bit[17-32]  cmp error
static BOOL _B2rEmuTimingAlternate(B2R_HANDLE_T hB2r,UINT32 HDEW, UINT32 VDEW,UINT32 u4Rate,UINT32 u4Scale,UINT32 u4pitch,B2R_CFG_FILE *ptCfg)
{
    B2R_HAL_TIMING_T tTiming;
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    B2R_HAL_COLOR_FMT_T tColorFmt;
    B2R_HAL_3D_T t3D;
    B2R_HAL_MF_T tMF;
    __TIMING_TBL_T *ptTbl;

    
    if (hB2r == B2R_NULL_HANDLE)
    {
        return FALSE;
    }
    //  ptTbl = _b2r_query_I_timing(ptCfg->HDEW, ptCfg->VDEW, ptHost->u4Rate, ptHost->u4Scale);
     
    // ptTbl = _b2r_query_P_timing(1920, 2160, u4Rate, u4Scale);  VB1
    ptTbl=VB1TimingTab;

    if (ptTbl == NULL)
    {
        return FALSE;
    }

    Printf("Timing name: %s,u2HTotal=%d,u2VTotal=%d\n", ptTbl->acName,ptTbl->u2HTotal,ptTbl->u2VTotal);


    //3D type
    t3D.fg3D = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_3DTYPE, &t3D) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //Timing 
    tTiming.fgProg   = ptCfg->OUT_I ? 0 : 1;
    tTiming.u4HTotal = ptTbl->u2HTotal;
    tTiming.u4VTotal = ptTbl->u2VTotal;
    tTiming.u4Rate   = u4Rate;
    tTiming.u4Scale  = u4Scale;
    tTiming.u4SrcW   = HDEW;
    tTiming.u4SrcH   = tTiming.fgProg? VDEW : (VDEW << 1);    
    tTiming.u4Clock  = ptTbl->u4Clock; 
    if (B2R_HAL_Set(hB2r, B2R_HAL_TIMING, &tTiming) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //dram layout
    tDramLayout.eAddrMode = (B2R_ADDR_MODE_T)ptCfg->ADDR_MODE;
    tDramLayout.eSwapMode = (B2R_SWAP_MODE_T)ptCfg->ADDR_SWAP;
    tDramLayout.ePackMode = (B2R_PACK_MODE_T)ptCfg->PACK_MODE;
    tDramLayout.u4Pitch   = u4pitch;
    if (B2R_HAL_Set(hB2r, B2R_HAL_DRAM_LAYOUT, &tDramLayout) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //color format
    tColorFmt.eMode   = (ptCfg->M_422 == 1) ? B2R_CM_422 : B2R_CM_420;
    tColorFmt.u1Depth = ptCfg->DEPTH;
    if (B2R_HAL_Set(hB2r, B2R_HAL_COLOR_FMT, &tColorFmt) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    //mirror or flip
    tMF.fgFlip   = FALSE;
    tMF.fgMirror = FALSE;
    if (B2R_HAL_Set(hB2r, B2R_HAL_MIRROR_FLIP, &tMF) !=  B2R_HAL_OK)
    {
        return FALSE;
    }

    B2R_HAL_Config(hB2r);
    VDP_SetBypassHFph(2035);//0x7f3
    VDP_SetBypassHBph(2079);//0x81fss
    VDP_SetBypassVFph(2176);//0x880
    VDP_SetBypassVBph(2186);//0x88A
    VDP_SetBypassHVInvert(1,1);
    VDP_SetBypassHVInvert(0,1);
  //  _B2R_HW_Select(2);//New B2R HD 
    B2R_HAL_Enable(hB2r);
    NB2R_REG_VB1(0xd230,3);
            
    return TRUE;
    
}


static BOOL _B2rEmuTestItem_VB1(B2R_EMU_HOST_T *ptHost, CHAR *ucRootPath, UINT32 u4FrmCnt)
{
    B2R_CFG_FILE *ptCfgFile;
    UINT32 u4CmpErrCnt;
    UINT32 u4ErrCnt;
    UINT32 u4IdxOffset = 0;
    UINT16 u1ISRcout=0;
    //file
    UINT32 u4Size;
    EMU_FILE_HANDLE hFile;
    UCHAR ucFilePath[256];

    //b2r part
    B2R_HANDLE_T hB2r;     
    B2R_HAL_3D_T t3D;
    B2R_HAL_TIMING_T tTiming;
    __TIMING_TBL_T *ptTbl;
    B2R_HAL_DRAM_LAYOUT_T tDramLayout;
    B2R_HAL_COLOR_FMT_T tColorFmt;
    B2R_HAL_MF_T tMF;
    B2R_OBJECT_T *this=NULL;
    if (!ucRootPath || !ptHost)
    {
        Printf("Arg invalid, please check.\n");
        return FALSE;
    }

    hB2r = ptHost->hB2r;
    ptCfgFile = &ptHost->tCfg;
    ptCfgFileG =&ptHost->tCfg;
    //Printf("Test item, path = %s, cnt = %d\n", ucRootPath, u4FrmCnt);

    //load cfg file
    x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/b2r.cfg", ucRootPath);
    hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", ucFilePath);
        return FALSE;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, ptHost->pcScriptPtr,u4Size);
    emu_fclose(hFile);

    ptHost->pcScriptPtr[u4Size] = '\0';
    _B2rEmuProcCfgFile(ptCfgFile, ptHost->pcScriptPtr);

    if (B2R_HAL_Reset(hB2r) != B2R_HAL_OK)
    {
        return FALSE;
    }
     B2R_HAL_Reset(hB2r);
   
    _B2rEmuTimingAlternate(hB2r,ptCfgFile->u4SrcWidth,ptCfgFile->u4SrcHeight,30,1,ptCfgFile->LINE_OFFSET,ptCfgFile);
    //@test frame 
    ptHost->u4Idx   = 0;
    ptHost->u4Total = ptHost->TWO_ADDR ? (u4FrmCnt>>1) : u4FrmCnt;
    u4IdxOffset = u4FrmCnt >> 1;  //for two addr mode use
    u4ErrCnt = u4CmpErrCnt = 0;
    FOR_EACH_FRAME(ptHost)
    {
        B2R_HAL_ADDR_T tFrmAddr;
        EMU_NTY_MSG_T rMsg;
        UINT16 u2MsgQIdx;
        SIZE_T zMsgSize;

        //-load  rawdata
        //@left view
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4YAddr0, u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr0, u4Size);

        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, (CHAR*)ptHost->u4CAddr0,u4Size);
        emu_fclose(hFile);
        HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr0, u4Size);

        //load len
        if (ptCfgFile->PACK_MODE & 0xF0)
        {
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenY", ucRootPath, ptHost->u4Idx);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4YAddrLen0,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddrLen0, u4Size);

            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenCbCr", ucRootPath, ptHost->u4Idx);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4CAddrLen0,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddrLen0, u4Size);
            
        }

        //load CRC
        x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx);
        hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
        if (EMU_FILE_HANDLE_NULL == hFile)
        {
            Printf("Open file fail: %s\n", ucFilePath);
            u4ErrCnt++;
            continue;
        }
        u4Size = emu_fsize(hFile);
        emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
        emu_fclose(hFile);
        _B2rEmuProcCRC(ptHost->au4PatCRC0, ptHost->pcScriptPtr, ptCfgFile->OUT_I);


        //@right view        
      //  if (ptHost->TWO_ADDR)
        {
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramY", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4YAddr1, u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddr1, u4Size);

            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.ramCbCr", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, (CHAR*)ptHost->u4CAddr1,u4Size);
            emu_fclose(hFile);
            HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddr1, u4Size);

            //load len
            if (ptCfgFile->PACK_MODE & 0xF0)
            {
                x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenY", ucRootPath, ptHost->u4Idx + u4IdxOffset);
                hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
                if (EMU_FILE_HANDLE_NULL == hFile)
                {
                    Printf("Open file fail: %s\n", ucFilePath);
                    u4ErrCnt++;
                    continue;
                }
                u4Size = emu_fsize(hFile);
                emu_fread(hFile, (CHAR*)ptHost->u4YAddrLen1,u4Size);
                emu_fclose(hFile);
                HalFlushDCacheMultipleLine((UINT32)ptHost->u4YAddrLen1, u4Size);
            
                x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.LenCbCr", ucRootPath, ptHost->u4Idx + u4IdxOffset);
                hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
                if (EMU_FILE_HANDLE_NULL == hFile)
                {
                    Printf("Open file fail: %s\n", ucFilePath);
                    u4ErrCnt++;
                    continue;
                }
                u4Size = emu_fsize(hFile);
                emu_fread(hFile, (CHAR*)ptHost->u4CAddrLen1,u4Size);
                emu_fclose(hFile);
                HalFlushDCacheMultipleLine((UINT32)ptHost->u4CAddrLen1, u4Size);
                
            }


            //load CRC
            x_snprintf(ucFilePath, sizeof(ucFilePath), "%s/out_%d.crc", ucRootPath, ptHost->u4Idx + u4IdxOffset);
            hFile = emu_fopen(ucFilePath, EMU_SRC_DEFAULT);
            if (EMU_FILE_HANDLE_NULL == hFile)
            {
                Printf("Open file fail: %s\n", ucFilePath);
                u4ErrCnt++;
                continue;
            }
            u4Size = emu_fsize(hFile);
            emu_fread(hFile, ptHost->pcScriptPtr, u4Size);
            emu_fclose(hFile);
            _B2rEmuProcCRC(ptHost->au4PatCRC1, ptHost->pcScriptPtr, ptCfgFile->OUT_I);
        }

        //-set test pattern 
        tFrmAddr.u4Y0 = PHYSICAL(ptHost->u4YAddr0);
        tFrmAddr.u4C0 = PHYSICAL(ptHost->u4CAddr0);
     //   tFrmAddr.u4Y1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4YAddr1) : 0;
       // tFrmAddr.u4C1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4CAddr1) : 0;
        B2R_HAL_Set(hB2r, B2R_HAL_ADDR, &tFrmAddr);
        #if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)
        VDP_SetVB1ControlBitTest(this,ptCfgFile->u4SrcWidth,ptCfgFile->u4SrcHeight);
        #endif
        //load len
        if (ptCfgFile->PACK_MODE & 0xF0)
        {
            B2R_HAL_ADDR_LEN_T tFrmAddrlen;
            tFrmAddrlen.u4YLen0 = PHYSICAL(ptHost->u4YAddrLen0);
            tFrmAddrlen.u4CLen0 = PHYSICAL(ptHost->u4CAddrLen0);
            tFrmAddrlen.u4YLen1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4YAddrLen1) : 0;
            tFrmAddrlen.u4CLen1 = (ptHost->TWO_ADDR) ? PHYSICAL(ptHost->u4CAddrLen1) : 0;
            B2R_HAL_Set(hB2r, B2R_HAL_ADDR_LEN, &tFrmAddrlen);
        }
         u4Y0= PHYSICAL(ptHost->u4YAddr0);
         u4C0=PHYSICAL(ptHost->u4CAddr0);
         u4Y0_Len= PHYSICAL(ptHost->u4YAddrLen0);
         u4C0_Len= PHYSICAL(ptHost->u4CAddrLen0);
         
         u4Y1= PHYSICAL(ptHost->u4YAddr1);
         u4C1=PHYSICAL(ptHost->u4CAddr1);
         u4Y1_Len= PHYSICAL(ptHost->u4YAddrLen1);
         u4C1_Len= PHYSICAL(ptHost->u4CAddrLen1);
         
         if(ptCfgFile->PACK_MODE!=0)
         {
            u4Y0_Len= PHYSICAL(ptHost->u4YAddrLen0);
            u4C0_Len= PHYSICAL(ptHost->u4CAddrLen0);
            u4Y1_Len= PHYSICAL(ptHost->u4YAddrLen1);
            u4C1_Len= PHYSICAL(ptHost->u4CAddrLen1);
         }
         else
         {
            u4Y0_Len =0;
            u4C0_Len =0;
            u4Y1_Len =0;
            u4C1_Len =0;
         
         }
        //-record crc
        ptHost->eSt = EMU_ST_CRC_START;
        u4B2rTimingStatus=1;
        //-wait finish
        zMsgSize = sizeof(EMU_NTY_MSG_T);
        while(1)
        {
            VERIFY(x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize, &ptHost->hNotifyMsg, 1, X_MSGQ_OPTION_WAIT) == OSR_OK);


            if (rMsg.eSt == EMU_ST_CRC_DONE)
            {
                UINT32 u4Mask;
                BOOL   fgRet = 0;
                B2R_HAL_ADDR_LEN_T tFrmAddrlen;
                u4Mask = ptCfgFile->OUT_I ? 0x4:0;
                if(fgB2RChg)
                {
                    
                 
                    if(_u4B2rTimingChg ==0)
                    {
                       
                         switch(u4B2rTimingStatus)
                            {
                            #if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)
                             case 1:
                                 VDP_SetVB1ControlBitTest(this,ptCfgFile->u4TagWidth,ptCfgFile->u4TagHeight);
                              break;
                             case 2:
                                 VDP_SetVB1ControlBitTest(this,ptCfgFile->u4SrcWidth,ptCfgFile->u4SrcHeight);
                            #endif
                           
                              break;
                         
                            }
                    }
                    else if(_u4B2rTimingChg==2)
                    {
                         
                       
                           switch(u4B2rTimingStatus)
                            {
                             case 1:
                                 _B2R_VB1_Setting( ptCfgFile->u4_Tag_LINE_OFFSET,ptCfgFile->PACK_MODE,ptCfgFile->DEPTH,ptCfgFile->u4TagWidth,ptCfgFile->u4TagHeight,u4Y1,u4C1,u4Y1_Len,u4C1_Len);
                                 u4B2rTimingStatus =2;
                                 fgB2RChg=FALSE;
                              break;
                             case 2:
                                 _B2R_VB1_Setting( ptCfgFile->LINE_OFFSET,ptCfgFile->PACK_MODE,ptCfgFile->DEPTH,ptCfgFile->u4SrcWidth,ptCfgFile->u4SrcHeight,u4Y0,u4C0,u4Y0_Len,u4C0_Len);
                                 u4B2rTimingStatus =1;
                                 fgB2RChg=FALSE;
                              break;
                            }
                       
                    }
                      _u4B2rTimingChg++;
                }
               
               if(fgB2ROpen)
               {
                    fgB2RChg=FALSE;
                    fgB2ROpen=FALSE;
                    B2R_HAL_Disable(hB2r);
                    B2R_HAL_Close(hB2r);
                    _B2rEmuBufferDeinit(ptHost->ptCurrList->tCfg.u1B2rId);
                    x_thread_exit();
                    break;
                }

            }
	    }
    }

   
    //record reslut
    ptHost->ptCurrItem->total = ptHost->u4Total;
    ptHost->ptCurrItem->fail  = (u4CmpErrCnt << 16)| u4ErrCnt ;
    ptHost->ptCurrItem->flag  = (u4ErrCnt || u4CmpErrCnt) ? 0x00000001 : 0;

    return TRUE;
    
}

static VOID _B2rEmuListThread_VB1(VOID *pvArg)
{
    B2R_EMU_HOST_T *ptHost;
    UINT8 u1B2rId;
    BOOL  fgOld;
    UINT32 u4Idx;
    B2R_CfG_T rB2rCfg;
    B2R_HANDLE_T hB2r;

    if (!pvArg)
    {
        return;
    }

    //Printf("Enter %s\n", __FUNCTION__);

    ptHost = (B2R_EMU_HOST_T *)(*(UINT32 *)pvArg);

    //Get cfg from list 
    u1B2rId = ptHost->ptCurrList->tCfg.u1B2rId;
    fgOld   = ptHost->ptCurrList->tCfg.fgOld;

    Printf("Enter b2r %s, %s\n", (fgOld?"Old":"New"), (u1B2rId == 0 ? "HD":"SD"));
    Printf("RootPath %s\n", ptHost->ptCurrList->tCfg.acRootPath);

    B2R_HAL_Init();
/*
    if (fgOld)
    {
        static BOOL fgRegIsr = 0;
        _B2REmu_HW_V1_Init(u1B2rId);
        if (!fgRegIsr)
        {
            x_os_isr_fct pfnOldIsr;
            VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V1_ISR, &pfnOldIsr) == OSR_OK);
            //fgRegIsr = TRUE;
        }
    }
    else
    {
        static BOOL fgRegIsr = 0;
        _B2REmu_HW_V2_Init(u1B2rId);
        if (!fgRegIsr)
        {
            x_os_isr_fct pfnOldIsr;
            VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2REmu_HW_V2_ISR, &pfnOldIsr) == OSR_OK);
            //fgRegIsr = TRUE;
        }
    }
*/
    //Start test B2R     
    //@open b2r     
    u4Idx = 0x10;
    u4Idx = (u1B2rId == 0) ? (u4Idx + 0) : (u4Idx + 1);   
    u4Idx = (fgOld) ? (u4Idx + 0) : (u4Idx + 2);
    rB2rCfg.u4Data = u4Idx;
    rB2rCfg.pvData = (VOID *)ptHost;
    if ((hB2r = B2R_HAL_Open(&rB2rCfg)) == B2R_NULL_HANDLE)
    {
        Printf("Can't open a b2r hw\n");
        return ;
    }

    ptHost->fgOldB2r = fgOld;
    ptHost->hB2r     = hB2r;
    ptHost->u4Rate   = ptHost->ptCurrList->tCfg.u4Rate;
    ptHost->u4Scale  = ptHost->ptCurrList->tCfg.u4Scale;
    ptHost->TWO_ADDR = ptHost->ptCurrList->tCfg.fgTwoAddr;


    for (ptHost->ptCurrItem = ptHost->rItemHead.next ; ptHost->ptCurrItem; ptHost->ptCurrItem = ptHost->ptCurrItem->next)
    {
        char *pcPath;
        char *pcTmp;
        char *pcCnt;
        UINT32 u4Cnt;

        pcTmp = ptHost->pcScriptPtr;

        //process item cmd
        x_snprintf(pcTmp, 512, "%s%s", ptHost->ptCurrList->tCfg.acRootPath, ptHost->ptCurrItem->data);

        //cut ',' char in the ptCurrItem->data
        pcPath = emu_strsep(&pcTmp, ",");
        pcCnt  = emu_strsep(&pcTmp, "\n");
        u4Cnt  = emu_str2int(pcCnt);

        x_snprintf(ptHost->pcPath, 512, "%s", pcPath);

        Printf("Item start: %s, cnt:%d\n", pcPath, u4Cnt);

        if (!_B2rEmuTestItem_VB1(ptHost, ptHost->pcPath, u4Cnt))
        {
            //other error
            ptHost->ptCurrItem->total = 0;
        }

        Printf("Item finish, total:%d, fail:0x%08x)\n", ptHost->ptCurrItem->total, ptHost->ptCurrItem->fail);
    
    }

   
}


VOID _B2rEmuTestList_VB1(CHAR *pucListPath)
{
    HANDLE_T hThrd;
    CHAR aucThrdName[10];
    B2R_EMU_HOST_T *ptHost;
    UINT32 u4HostAddr;
    UINT8 u1Id = 0;
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;
    CHAR *pucTmp  = NULL;
    UINT32 u4BuffAddr = 0;
    EMU_LIST_CFG_T* ptListCfg;

    if (!pucListPath)
    {
        Printf("invalid list path, please check!\n");
        return;
    }
   
    //open list
    hFile = emu_fopen(pucListPath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", pucListPath);
        return;
    }
    u4Size = emu_fsize(hFile);
    u4BuffAddr = BSP_AllocAlignedDmaMemory(u4Size + 16 + sizeof(EMU_LIST_CFG_T), 32);
    pucTmp  = (CHAR *)VIRTUAL(u4BuffAddr);
    emu_fread(hFile, pucTmp,u4Size);
    pucTmp[u4Size] = pucTmp[u4Size + 1] = '\0';
    emu_fclose(hFile);
    
    if (!emu_str_find(pucTmp, "#!list!"))
    {
        Printf("invalid list file");
        return;
    }

    //proc list cfg segment
    ptListCfg = (EMU_LIST_CFG_T *)(pucTmp + u4Size + 8);
    ptListCfg->u1B2rId   = 0xFF;
    ptListCfg->u4Rate    = B2R_RATE_DEFAULT;
    ptListCfg->u4Scale   = B2R_SCALE_DEFAULT;
    ptListCfg->fgTwoAddr = FALSE;
    pucTmp = _B2rEmuProcListCfg(ptListCfg, pucTmp);
    u1Id = ptListCfg->u1B2rId;
    if (u1Id > 1)
    {
        Printf("Cfg file fail: %s\n", pucListPath);
        BSP_FreeAlignedDmaMemory(u4BuffAddr);
        return;
    }
    
    //init host
    _B2rEmuBufferInit(u1Id);
    _B2rEmuVariableInit(u1Id);
    ptHost = _B2rEmuId2Obj(u1Id);

    //proc list item segment
    _B2rEmuProcListItem(ptHost, pucTmp);

    x_snprintf(ptHost->ptCurrList->data, 256, "%s", pucListPath);
    x_memcpy((VOID *)(&ptHost->ptCurrList->tCfg), (VOID *)ptListCfg, sizeof(EMU_LIST_CFG_T));
    ptHost->ptCurrList->rItemHead = ptHost->rItemHead;

    //free buffer
    BSP_FreeAlignedDmaMemory(u4BuffAddr);


    //create a test thread to do test
    x_snprintf(aucThrdName, sizeof(aucThrdName), "B2rEmu%2d", u1Id);
    u4HostAddr = (UINT32)ptHost;
    VERIFY(x_thread_create(&hThrd,
        aucThrdName, VSYNC_STACK_SIZE, VSYNC_THREAD_PRIORITY,
        (x_os_thread_main_fct)_B2rEmuListThread_VB1, sizeof(UINT32), (VOID *)&u4HostAddr) == OSR_OK);

}

static INT32 _B2rEmuSetPar(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT8 u1Path;
    if (i4Argc == 7)
    {
        u1Path = StrToInt(szArgv[1]);
        _b2r_par[u1Path].u1Id     = StrToInt(szArgv[2]);
        _b2r_par[u1Path].fgOldB2r = StrToInt(szArgv[3]);
        _b2r_par[u1Path].bFgEn    = StrToInt(szArgv[4]);
        _b2r_par[u1Path].fgFlip   = StrToInt(szArgv[5]);
        _b2r_par[u1Path].fgMirror = StrToInt(szArgv[6]);
        Printf("PATH(%d),Id(%d),fgOldB2r(%d),En(%d),Flip(%d),Mirror(%d)!\n",
            u1Path, _b2r_par[u1Path].u1Id, _b2r_par[u1Path].fgOldB2r,
            _b2r_par[u1Path].bFgEn, _b2r_par[u1Path].fgFlip, _b2r_par[u1Path].fgMirror);
    }
    else
    {
        Printf("Usege:Path B2rId fgOld FgEn bFilp bMirror!\n");
        for(u1Path = 0; u1Path < PATH_NS; u1Path ++)
        {
            Printf("PATH(%d):Id(%d),fgOldB2r(%d),En(%d),Flip(%d),Mirror(%d)!\n",
                u1Path, _b2r_par[u1Path].u1Id, _b2r_par[u1Path].fgOldB2r,
                _b2r_par[u1Path].bFgEn, _b2r_par[u1Path].fgFlip, _b2r_par[u1Path].fgMirror);
        }
    }

    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

static INT32 _B2rEmuStressTestPar(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT8 u1Path;
    UINT32 u4TCntFst;
    UINT32 u4TCntSnd;
    
    if (i4Argc == 3)
    {
        u4TCntFst = StrToInt(szArgv[1]);
        u4TCntSnd = StrToInt(szArgv[2]);
        _uStressTestCnt = u4TCntFst * u4TCntSnd;
        Printf("Current Cnt(%d)!\n",_uStressTestCnt);
        if(_uStressTestCnt != 0)
        {
            pu1StressFilepath = BSP_AllocAlignedDmaMemory(256, 8); 
        }
        else
        {
            if(pu1StressFilepath)
            {
                BSP_FreeAlignedDmaMemory(pu1StressFilepath);
                pu1StressFilepath = NULL;
            }
        }
    }
    else
    {
        Printf("Usege:testCnt1 testCnt2!\n");
        Printf("Current Cnt(%d)!\n",_uStressTestCnt);
    }

    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 1 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase1(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR *fn;

    if (i4Argc == 2)
    {
        fn = (CHAR *)szArgv[1];
        _B2rEmuTest2D(0, TRUE, B2R_RATE_DEFAULT, B2R_SCALE_DEFAULT, fn, 1);
    }
    else if (i4Argc == 3)
    {
        UINT32 arg1;
        arg1 = StrToInt(szArgv[2]);
        fn = (CHAR *)szArgv[1];
        _B2rEmuTest2D(0, TRUE, B2R_RATE_DEFAULT, B2R_SCALE_DEFAULT, fn, arg1);
    }

    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 2 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase2(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR *fn;

    if (i4Argc == 2)
    {
        fn = (CHAR *)szArgv[1];
        _B2rEmuTest3DTwoAddr(0, TRUE, B2R_RATE_DEFAULT, B2R_SCALE_DEFAULT, fn, 1);
    }
    else if (i4Argc == 3)
    {
        UINT32 arg1;
        arg1 = StrToInt(szArgv[2]);
        fn = (CHAR *)szArgv[1];
        _B2rEmuTest3DTwoAddr(0, TRUE, B2R_RATE_DEFAULT, B2R_SCALE_DEFAULT, fn, arg1);
    }

    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}



//-----------------------------------------------------------------------------
// Test Case 3 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase3(INT32 i4Argc, const CHAR ** szArgv)
{
    if (i4Argc == 3)
    {
        UINT32 B2rId;
        BOOL fgOld;
        B2rId = StrToInt(szArgv[1]);
        fgOld = (StrToInt(szArgv[2])== 0) ? 1 : 0;

        if (B2rId >= 2)
        {
            Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");            
            return 0;
        }
        _B2rEmuTestStandardTiming(B2rId, fgOld);
    }
    else
    {
        Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Test Case 4 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase4(INT32 i4Argc, const CHAR ** szArgv)
{
    EMU_FILE_HANDLE hFile;
    B2R_EMU_HOST_T *ptHost;
    UINT8 u1B2rId = 0;
    UINT32 u4Size, u4Read;
    Printf("\n***************Test Case 3*******************\n");

    //Global initialization 
    _B2rEmuBufferInit(u1B2rId);
    _B2rEmuVariableInit(u1B2rId);
    ptHost = _B2rEmuId2Obj(u1B2rId);

    if (i4Argc == 2)
    {
        CHAR *fn = (CHAR *)szArgv[1];
        x_memcpy(_ucFilePath, fn, x_strlen(fn));
        Printf("path: %s\n", _ucFilePath);
    }

    hFile = emu_fopen(_ucFilePath, EMU_SRC_DEFAULT);

    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", _ucFilePath);
        return 0;
    }

    u4Size = emu_fsize(hFile);

    Printf("Open file Ok: name: %s, size:%d\n", _ucFilePath, u4Size);

    u4Read = emu_fread(hFile, ptHost->pcScriptPtr,u4Size);
    ptHost->pcScriptPtr[u4Read] = '\0';

    if (u4Read != u4Size)
    {
        Printf("read data fail: size:%d, ret:%d\n", u4Size, u4Read);
    }
    else
    {
        B2R_CFG_FILE tCfg;

        _B2rEmuProcCfgFile(&tCfg, ptHost->pcScriptPtr);

        Printf("tCfg.ADDR_MODE: %d\n", tCfg.ADDR_MODE);
        Printf("tCfg.ADDR_SWAP: %d\n", tCfg.ADDR_SWAP);
        Printf("tCfg.FIELD: %d\n", tCfg.FIELD);
        Printf("tCfg.HDEW: %d\n", tCfg.HDEW);
        Printf("tCfg.H_OFFSET: %d\n", tCfg.H_OFFSET);
        Printf("tCfg.LINE_OFFSET: %d\n", tCfg.LINE_OFFSET);
        Printf("tCfg.M_422: %d\n", tCfg.M_422);
        Printf("tCfg.OUT_I: %d\n", tCfg.OUT_I);
        Printf("tCfg.VDEW: %d\n", tCfg.VDEW);
        Printf("tCfg.V_OFFSET: %d\n", tCfg.V_OFFSET);

    }



    emu_fclose(hFile);

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 5 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase5(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR *fn;

    if (i4Argc == 2)
    {
        fn = (CHAR *)szArgv[1];
        _B2rEmuTestList(fn, PATH_1);
    }
    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 6 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase6(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR *fn1, *fn2;

    if (i4Argc == 3)
    {
        fn1 = (CHAR *)szArgv[1];
        fn2 = (CHAR *)szArgv[2];
        _B2rEmuTestList(fn1, PATH_1);
        _B2rEmuTestList(fn2, PATH_2);
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 7 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase7(INT32 i4Argc, const CHAR ** szArgv)
{

    if (i4Argc == 3)
    {
        UINT32 B2rId;
        BOOL fgOld;
        B2rId = StrToInt(szArgv[1]);
        fgOld = (StrToInt(szArgv[2])== 0) ? 1 : 0;

        if (B2rId >= 2)
        {
            Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");            
            return 0;
        }
        _B2rEmuRunB2r(B2rId, fgOld);
    }
    else
    {
        Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 8 : 
//-----------------------------------------------------------------------------
static INT32 _B2rEmuTestCase8(INT32 i4Argc, const CHAR ** szArgv)
{
    if (i4Argc == 3)
    {
        UINT32 B2rId;
        BOOL fgOld;
        B2rId = StrToInt(szArgv[1]);
        fgOld = (StrToInt(szArgv[2])== 0) ? 1 : 0;

        if (B2rId >= 2)
        {
            Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");            
            return 0;
        }
        _B2rEmuStopB2r(B2rId, fgOld);
    }
    else
    {
        Printf("cmd {0:HD, 1:SD} {0:Old, 1: new}\n");
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 9 : 
//-----------------------------------------------------------------------------
static void _TestCase9Usage(void)
{
  Printf("cmd -hw {0|1} -p {0|1} [[-t timing] [-vr range -hr range] [-fy value] [-fc value] | [-ext filepath...]]\n");
  Printf("  -hw\tspecify the hw, 0 is old b2r, 1 is new b2r\n");
  Printf("  -p\tspecify the path, 0 is hd , 1 is sd\n");
  Printf("  -vr\tspecify the timing.ex 480P, 480I, 720P, 1080P, 1080I, 2160P\n");
  Printf("  -vr\tspecify the v-active range\n");
  Printf("  -hr\tspecify the h-active range\n");
  Printf("    range:low high step\n");
  Printf("          test from 'low' to 'high', increase 'step' each time.\n"); 
  Printf("          ex. 16 32 2\n"); 
  Printf("  -fy\tfix the y color value (from 0 to 255)\n");
  Printf("  -fc\tfix the c color value (from 0 to 255)\n");

}

static void _TestCase9LoadExtCfg(CHAR *pcPath, B2R_EXT_DATA *ptExtData)
{
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;
    UINT32 u4Addr;
    CHAR * pcAddr;
    UINT32 i;
    CHAR * pcToken;
    CHAR * pcData;
    UINT32 regIdx;

    if (!pcPath || !ptExtData)
    {
        return;
    }

    //init
    sprintf(ptExtData->acY422, "\0\0");
    sprintf(ptExtData->acC422, "\0\0");

    //open list
    hFile = emu_fopen(pcPath, EMU_SRC_DEFAULT);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", pcPath);
        return;
    }
    u4Size = emu_fsize(hFile);
    u4Addr = BSP_AllocAlignedDmaMemory(u4Size + 2, 32);
    pcAddr  = (CHAR *)VIRTUAL(u4Addr);
    emu_fread(hFile, pcAddr, u4Size);
    pcAddr[u4Size] = pcAddr[u4Size + 1] = '\0';
    emu_fclose(hFile);

    //convert to unix format
    for (i = 0; i < u4Size; i++)
    {
        if (pcAddr[i] == 0x0D)
        {
            pcAddr[i] = 0x0A;
        }
    }

    //init
    _B2rEmuIntExtReg();
    regIdx = 0;
    
    while ((pcToken = emu_strsep(&pcAddr, "\n")) != NULL)
    {
        pcData = _B2rEmuGetCfgDataPtr(pcToken);
        //Printf("token: %s, %s\n", pcToken, pcData);
        if (pcToken[0] == '#')
        {
            continue;
        }
        if (x_strncmp(pcToken, "YRAW", 4) == 0)
        {
            sprintf(ptExtData->acYRaw, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "CRAW", 4) == 0)
        {
            sprintf(ptExtData->acCRaw, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "YR422", 5) == 0)
        {
            sprintf(ptExtData->acY422, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "CR422", 5) == 0)
        {
            sprintf(ptExtData->acC422, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "DEPTH", 5) == 0)
        {
            ptExtData->u4Depth = StrToInt(pcData);
        }
        else if (x_strncmp(pcToken, "PACK_MODE", 9) == 0)
        {
            ptExtData->ePackMode = StrToInt(pcData);
        }
        else if (x_strncmp(pcToken, "RAWPitch", 8) == 0)
        {
            ptExtData->u4RawPitch = StrToInt(pcData);
        }
        else if (x_strncmp(pcToken, "RPitch", 6) == 0)
        {
            ptExtData->u4422Pitch = StrToInt(pcData);
        }
        else if (x_strncmp(pcToken, "YLen", 4) == 0)
        {
            sprintf(ptExtData->acYRawLen, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "CLen", 4) == 0)
        {
            sprintf(ptExtData->acCRawLen, "%s\0", pcData);
        }
        else if (x_strncmp(pcToken, "reg:", 4) == 0)
        {
            if (regIdx < sizeof(_arRegExt)/sizeof(_arRegExt[0]))
            {
                _B2rEmuLoadExtReg(regIdx, pcToken);
            }
            regIdx++;
        }

    }

    //free buffer
    BSP_FreeAlignedDmaMemory(u4Addr);
    
}
static INT32 _B2rEmuTestCase10(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR *fn;

    if (i4Argc == 2)
    {
        fn = (CHAR *)szArgv[1];
		VDP_B2rSwitch(0,3);
        _B2rEmuTestList_VB1(fn);
    }
    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}
static INT32 _B2rStartTimingChg(INT32 i4Argc, const CHAR ** szArgv)
{
    

   _u4B2rTimingChg=0;
    fgB2RChg= TRUE;
  
    Printf("_B2rStartTimingChg\n");
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}
static INT32 _B2rStop(INT32 i4Argc, const CHAR ** szArgv)
{
    

    fgB2ROpen=TRUE;
    Printf("_B2rStop =%d\n",fgB2ROpen);
    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

static INT32 _B2rEmuTestCase9(INT32 i4Argc, const CHAR ** szArgv)
{
    B2R_RANGE_T vRange;  //height
    B2R_RANGE_T hRange;  //width
    INT32 ii = 0;
    UINT32 B2rId = B2R_MAX_NS;
    BOOL fgOld = 0;
    UINT32 u4YColor, u4CColor;
    UINT32 u4VHIdx;
    B2R_EXT_DATA *ptExtData = NULL;
    CHAR *pcExtFile = NULL;
    
    vRange.u4Start = 32;
    vRange.u4End   = 96;
    vRange.u4Step  = 4;

    hRange.u4Start = 32;
    hRange.u4End   = 128;
    hRange.u4Step  = 4;

    // 1024 mean use random value, less 1024 means use fixed value
    u4YColor = 1024;
    u4CColor = 1024;  

    //default timing is 480, bit[3:0] is P(0x0) or I(0x1) 
    u4VHIdx = 0x00;

    if (i4Argc < 2)
    {
        _TestCase9Usage();
        return 0;
    }

    while (ii < i4Argc)
    {
        if (szArgv[ii][0] == '-')
        {
            if (!x_strncmp("--help", szArgv[ii], 6) || !x_strncmp("-help", szArgv[ii], 5))
            {
                _TestCase9Usage();
                return 0;
            }
            else if (!x_strncmp("-hw", szArgv[ii], 3))
            {
                fgOld = (StrToInt(szArgv[ii+1]) == 0) ? 1 : 0;
            }
            else if (!x_strncmp("-p",szArgv[ii], 2))
            {
                B2rId = StrToInt(szArgv[ii+1]);
            }
            else if (!x_strncmp("-t",szArgv[ii], 2))
            {
                if (!x_strncmp("480P",szArgv[ii+1], 4))
                {
                    u4VHIdx = 0x00;
                }
                else if (!x_strncmp("480I",szArgv[ii+1], 4))
                {
                    u4VHIdx = 0x01;
                }
                else if (!x_strncmp("720P",szArgv[ii+1], 4))
                {
                    u4VHIdx = 0x20;
                }
                else if (!x_strncmp("1080P",szArgv[ii+1], 5))
                {
                    u4VHIdx = 0x30;
                }
                else if (!x_strncmp("1080I",szArgv[ii+1], 5))
                {
                    u4VHIdx = 0x31;
                }
                else if (!x_strncmp("2160P",szArgv[ii+1], 5))
                {
                    u4VHIdx = 0x40;
                }
            }
            else if (!x_strncmp("-vr",szArgv[ii], 3))
            {
                vRange.u4Start = StrToInt(szArgv[ii+1]);
                vRange.u4End   = StrToInt(szArgv[ii+2]);
                vRange.u4Step  = StrToInt(szArgv[ii+3]);
            }
            else if (!x_strncmp("-hr",szArgv[ii], 3))
            {
                hRange.u4Start = StrToInt(szArgv[ii+1]);
                hRange.u4End   = StrToInt(szArgv[ii+2]);
                hRange.u4Step  = StrToInt(szArgv[ii+3]);
            }
            else if (!x_strncmp("-fy",szArgv[ii], 3))
            {
                u4YColor = StrToInt(szArgv[ii+1]);
            }
            else if (!x_strncmp("-fc",szArgv[ii], 3))
            {
                u4CColor = StrToInt(szArgv[ii+1]);
            }
            else if (!x_strncmp("-ext",szArgv[ii], 4))
            {
                pcExtFile = (CHAR *)szArgv[ii + 1];
            }
        }
        ii++;
    }
    
    //process ext file
    if (pcExtFile)
    {
        #if 0
        sprintf(_b2rExtData.acYRaw, "D:/b2r_verify_file/boundary_test/raw420_8bitunpack_720x480.y\0");
        sprintf(_b2rExtData.acCRaw, "D:/b2r_verify_file/boundary_test/raw420_8bitunpack_720x480.c\0");
        sprintf(_b2rExtData.acY422, "D:/b2r_verify_file/boundary_test/ycbcr422_8bit_720x480.y\0");
        sprintf(_b2rExtData.acC422, "D:/b2r_verify_file/boundary_test/ycbcr422_8bit_720x480.c\0");
        _b2rExtData.u4Depth   = 8;
        _b2rExtData.ePackMode = B2R_PM_NORMAL;
        #endif
        ptExtData = &_b2rExtData;
        _TestCase9LoadExtCfg(pcExtFile, ptExtData); 
    }

    if (B2rId >= 2 ||
        (vRange.u4Start > vRange.u4End) ||
        (hRange.u4Start > hRange.u4End) ||
        vRange.u4End > HVTotal[(u4VHIdx>>4)].u4MaxHeight ||
        hRange.u4End > HVTotal[(u4VHIdx>>4)].u4MaxWidth)
    {
        _TestCase9Usage();
    }
    else
    {
        _B2rEmuBoundaryTest(B2rId, fgOld, &hRange, &vRange, u4YColor, u4CColor, 
            &HVTotal[(u4VHIdx>>4)], (u4VHIdx & 0x0F), ptExtData);
    }

    return 0;
}

#else // ifndef B2R_EMU

static INT32 _B2rEmuHelp(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("If you wanna run B2R EMU test,");
    Printf("you need to enable B2R_EMU macro in b2r_hal.h.\n");
    Printf("Then, you can see test cli in here.\n");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

CLI_EXEC_T _arB2rV1TestCaseTbl[] =
{
    {"help",    "h", _B2rEmuHelp,  NULL, "B2R EMU help",  CLI_SUPERVISOR},
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

#endif /*B2R_EMU*/
