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
 * $RCSfile: dmx_pcr.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file sample.c
 *  Brief of file dmx_pcr.c.
 *  Details of file dmx_pcr.c (optional).
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "dmx.h"
#include "dmx_pcr.h"
#include "dmx_debug.h"
#ifdef ENABLE_MULTIMEDIA
#include "swdmx_drvif.h"
#endif
#include "x_lint.h"

//#include "drv_stc.h"
#include "b2r_drvif.h"
#include "stc_drvif.h"
#include "aud_if.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "x_hal_5381.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_timer.h"
#include "x_syslog.h"
#include "x_pinmux.h"
#include "drv_dbase.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//#define CC_VCXO_ON

// LOG Info
#define DMX_PCR_LOG

// Test for Smart VCxO, need DMX_PCR_LOG & CC_SMART_VCXO
//#define CC_SMART_VCXO_TEST

#ifdef CC_VCXO_ON
// Turn off VCxO slowly
#define CC_TURN_OFF_VCXO
#endif

#define CC_VCXO_CLK_SWITCH

//#define CC_SUPPORT_ONE_STC

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

#define STC_CLOCK_FREQ              (27000000)  // 27 MHz Clock
#define STC_PWMH_INIT               (128)       // PWMH Initial Value
#define STC_INVALID_THD             (270000)    // Threshold for STC drift, 270K/27M (10 ms)

#define STC_BIG_DRIFT_THD           (27000000)  // Threshold for Big STC drift, (1 second)

#ifdef CC_SMART_VCXO_TEST
#define STC_DRIFT_THD               (270000)    // For test only
#else
#define STC_DRIFT_THD               (27000)     // Threshold for STC drift, 27K/27M (1 ms)
#endif

#define STC_INVALID_90K_THD         (STC_INVALID_THD/300)
#define STC_DRIFT_90K_THD           (STC_DRIFT_THD/300)

#define STC_UPDATE_NONE             0
#define STC_UPDATE_RATE             1
#define STC_UPDATE_PHASE            2
#define STC_UPDATE_INVALID          3

#define IIR_K0                      8
#define IIR_K1                      4
#define IIR_K2                      15

#define LOOP_MAX		            ((127) * (1 << IIR_K2))
#define LOOP_MIN		            ((-128) * (1 << IIR_K2))
#define LOOP_ZERO		            ((128) * (1 << IIR_K2))

#define AUD_MIN_ES_PKT_NS           1
#define VDO_MIN_ES_PKT_NS           4

/* STC STATUS */
#define DMX_STC_STATUS_UNKNOWN      0
#define DMX_STC_STATUS_INVALID      1
#define DMX_STC_STATUS_BIG_DRIFT    2
#define DMX_STC_STATUS_VALID        3
#define DMX_STC_STATUS_TRACKING     4
#define DMX_STC_STATUS_DRIFT        5       // 1~10 ms Drift, Still Valid
#define DMX_STC_STATUS_NS           6

/* Parser Module Register Definition */
#define PARSER_BASE                 PARSER0_BASE
#ifndef	CC_MT5391_AUD_SUPPORT
#define REG_STC_CTRL                0x1f4
#define REG_A_STC_1                 0x1c4
#else
#define PARSER_BASE2               PARSER0_BASE2//added by ling
#define REG_STC_CTRL                0x0b4
#define REG_A_STC_1                 0x84

#endif

#define REG_V_STC_1                 0x04c

#define STC_DELTA_OVERFLOW_THD		(0xFFFFFFFF / 300)
#define STC_DELTA_MSB				(0x80000000)
#define STC_DELTA_INVALID			(0xFFFFFFFF)

#define B2R_PCR_CTRL_ERR_HD         (0x304)
#define B2R_PCR_CTRL_ERR_SD         (0x328)
/* Turn off VCxO */
#define DMX_VCXO_OFF_TIME_STEP      (1000)
#define DMX_VCXO_OFF_PWM_STEP       (4)

#define STC_VERIFY_NULL(ucStcId)                                    \
          do {                                              \
            if (ucStcId >= STC_SRC_NS) { \
                LOG(0, "%s, ucStcId %d > %d, oops....\n", __FUNCTION__, ucStcId, STC_SRC_NS); \
                }      \
          } while (0)
          
#define STC_VERIFY_NULL_ERROR(ucStcId)                                    \
          do {                                              \
            if (ucStcId >= STC_SRC_NS) { \
                LOG(0, "%s, ucStcId %d > %d, oops....\n", __FUNCTION__, ucStcId, STC_SRC_NS); \
                return FALSE; \
                }      \
          } while (0)
//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

/** DMX PCR ID (differ from STC ID)
 */
typedef enum {
    DMX_PCR_1 = 0, 
    DMX_PCR_2,
    DMX_PCR_NS
} DMX_PCR_ID;

/** Brief of DMX_STATUS_1
 *  Demux Module Register Definition.
 */
typedef struct
{
    UINT32 u4NonErr     :    8;
    UINT32              :    8;
    UINT32 u4PidIndex   :    8;
    UINT32              :    8;

    LINT_SUPPRESS_NEXT_EXPRESSION(950)
} __attribute__ ((packed))   DMX_STATUS_1;

/** Brief of DMX_STATUS_1_UNION_T
 */
typedef union
{
    UINT32 u4Reg;
    DMX_STATUS_1 rField;
} DMX_STATUS_1_UNION_T;

/** Brief of DMX_STATUS_2
 */
typedef struct
{
    UINT32 u4ContCnt    :    4;
    UINT32 u4DiscontInd :    1;
    UINT32              :    11;
    UINT32 u4PwmAdjust  :    16;

    LINT_SUPPRESS_NEXT_EXPRESSION(950)
} __attribute__ ((packed))   DMX_STATUS_2;

/** Brief of DMX_STATUS_2_UNION_T
 */
typedef union
{
    UINT32 u4Reg;
    DMX_STATUS_2 rField;
} DMX_STATUS_2_UNION_T;

/** Brief of DMX_TIME_EXT
 */
typedef struct
{
    UINT32 u4Ext        :    9;
    UINT32              :    22;
    UINT32 fgBase32     :    1;

    LINT_SUPPRESS_NEXT_EXPRESSION(950)
} __attribute__ ((packed))   DMX_TIME_EXT;

/** Brief of DMX_TIME_EXT_UNION_T
 */
typedef union
{
    UINT32 u4Reg;
    DMX_TIME_EXT rField;
} DMX_TIME_EXT_UNION_T;

/** Brief of DMX_PCR
 */
typedef struct
{
    UINT32 u4PcrBase;
    DMX_TIME_EXT_UNION_T uPcrExt;
} DMX_PCR;

/** Brief of DMX_PCR_MSSG
 */
typedef struct
{
    DMX_PCR_ID eDmxPcrId;
    DMX_PCR rPcr;
    DMX_STATUS_1_UNION_T uStatus1;
    DMX_STATUS_2_UNION_T uStatus2;
} DMX_PCR_MSSG;

/** Brief of PWM_REGISTER_T
 */
typedef struct
{
    UINT32 u4Enable      :    1;
    UINT32 u4LoadDmx     :    1;
    UINT32               :    6;
    UINT32 u4Prescaler   :    8;
    UINT32 u4High        :    8;
    UINT32 u4Resolution  :    8;

    LINT_SUPPRESS_NEXT_EXPRESSION(950)
} __attribute__ ((packed))   PWM_REGISTER_T;

// #define HAL_5371_PWM0_LOCAL_REG             (0x230)
#define PWM_EN                          (BIT0)
#define PWM_LOAD_DMX                    (BIT1)
#define PWM_PRESCALER                   (4)
#define PWM_HIGH                        (16)
#define PWM_RESOLUTION                  (24)

/** Brief of DMX_DELTA_UNION_T
 */
typedef union _DMX_DELTA_UNION_T
{
    INT64 i8Delta;
    UINT32 au4Delta[2];
} DMX_DELTA_UNION_T;

#ifdef CC_53XX_SWDMX_V2
typedef UINT32 (*STC_READ_FUNC)(UINT8 u1StcId);
typedef void   (*STC_LOAD_FUNC)(UINT8 u1StcId, UINT32 u4Stc);
#else
typedef UINT32 (*STC_READ_FUNC)(VOID);
typedef void   (*STC_LOAD_FUNC)(const DMX_PCR*);
#endif

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

#define STC_CHG_STATUS(ucStcId, status)                                         \
    do {                                                                        \
        SYSLOG(PCR_PREFIX, ucStcId, au4StcStatus[ucStcId], status);             \
        LOG(3, "STC(%d) %d >> %d\n", ucStcId, au4StcStatus[ucStcId], status);   \
        au4StcStatus[ucStcId] = status;                                         \
    } while (0)

#define PCR_27M_2_90K(pPcr)                                                     \
    (pPcr->u4PcrBase)

#define PCR_27M(pPcr)                                                           \
    (((pPcr->u4PcrBase & 0x1FFFFF) * 300) + pPcr->uPcrExt.rField.u4Ext)

#define PARSER_READ32(offset)           IO_READ32(PARSER_BASE, (offset))

#ifdef	CC_MT5391_AUD_SUPPORT
#define PARSER_2_READ32(offset)           IO_READ32(PARSER_BASE2, (offset)) //added by ling
#endif

#define PARSER_WRITE32(offset, value)   IO_WRITE32(PARSER_BASE, (offset), (value))

#define PARSER_REG32(offset)            IO_REG32(PARSER_BASE, (offset))

//added by ling
#ifdef	CC_MT5391_AUD_SUPPORT
#define PARSER_2_WRITE32(offset, value)   IO_WRITE32(PARSER_BASE2, (offset), (value))
#endif
//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

static void _DmxPcrRx(DMX_PCR_MSSG* prPcrMssg);
#ifndef CC_53XX_SWDMX_V2
static UINT32 _StcReadA1(void);
static UINT32 _StcReadV1(void);
static void _StcLoadA1(const DMX_PCR* prPcr);
static void _StcLoadV1(const DMX_PCR* prPcr);
#endif

#ifdef CC_TURN_OFF_VCXO
static void _DmxTurnOffVCxO(HANDLE_T      pt_tm_handle, VOID* pv_tag);
#endif

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static UINT32 _u4Initiated = 0;

static UINT32 au4StcEnable[STC_SRC_NS];
static UINT32 au4StcStatus[STC_SRC_NS];
//static UINT32 au4StcDrift[STC_SRC_NS];
static UINT32 au4StcAudTarget[STC_SRC_NS];
static UINT32 au4StcVidTarget[STC_SRC_NS];
static UINT32 au4StcStarted[STC_SRC_NS];
//static INT32 ai4StcDrift[STC_SRC_NS];
static UCHAR aucPidIdx2Stc[DMX_NUM_PID_INDEX];
static UCHAR au4VidTarget2Stc[MAX_DEVICE_ID];
static UCHAR au4AudTarget2Stc[MAX_DEVICE_ID];

static HAL_TIME_T arBigDriftStartTime[STC_SRC_NS];
static UINT32 arBigDriftWaitTime[STC_SRC_NS];
//static INT32 _i4DecoderDrift[STC_SRC_NS] = {3000,3000};
static INT32 _i4DecoderDrift = 3000;
static BOOL _fgEnablePcrInfo = FALSE;

#ifdef CC_53XX_SWDMX_V2
#ifndef CC_SUPPORT_2STC
static UINT32 _u4StcSource[STC_SRC_NS] = {0};
#endif
//static HANDLE_T _hSoftwareStcThread = NULL;
static HANDLE_T hWaitDataTimer = (HANDLE_T)NULL;
#define SOFT_STC_TIMER (15)
#endif

#ifdef CC_FAST_CHANGE_CHANNEL
static UINT32 au4VdoFifo[STC_SRC_NS];
static UINT32 au4VdoPkt[STC_SRC_NS];
static UINT32 au4VdoFifoRdy[STC_SRC_NS];
static UINT32 au4AudFifo[STC_SRC_NS];
static UINT32 au4AudPkt[STC_SRC_NS];
static UINT32 au4AudFifoRdy[STC_SRC_NS];
static UINT32 au4StartPts[STC_SRC_NS];
#endif

#ifdef CC_VCXO_ON
UINT32 au4VCxO[STC_SRC_NS] = {1, 0};
#else

UINT32 au4VCxO[STC_SRC_NS] = {0, 0};
#endif

UINT32 au4PcrNs[STC_SRC_NS]= {0, 0};

#ifdef CC_53XX_SWDMX_V2
STC_READ_FUNC apfStcRead[STC_SRC_NS] =
{
    STC_GetStcValue,
    STC_GetStcValue,
};
STC_LOAD_FUNC apfStcLoad[STC_SRC_NS] =
{
    STC_SetStcValue,
    STC_SetStcValue,
};

#else
STC_READ_FUNC apfStcRead[STC_SRC_NS] =
{
    _StcReadA1,
    _StcReadV1,
};
STC_LOAD_FUNC apfStcLoad[STC_SRC_NS] =
{
    _StcLoadA1,
    _StcLoadV1,
};
#endif

static DMX_PCR_MSSG _rPcrMssg;

static UINT32 _u4PwmAdjust = 0;

#ifdef CC_TURN_OFF_VCXO
static HANDLE_T _hTimer = 0;
#endif

#ifdef CC_GET_PCR_PTS_INFO
static DRV_DBASE_T* _prDbase = NULL;
#endif

static UINT32 _au4MmAvSyncMode[STC_SRC_NS] = {AV_SYNC_MODE_NONE};
static BOOL _afgStcStart[STC_SRC_NS] = {FALSE};

static BOOL _fgTimeShift = FALSE;

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** _StcReadA1
 *  Read STC A1
 */
//-------------------------------------------------------------------------
#ifndef CC_53XX_SWDMX_V2
static UINT32 _StcReadA1(VOID)
{

#ifndef	CC_MT5391_AUD_SUPPORT
    return PARSER_READ32(REG_A_STC_1);
#else
    return PARSER_2_READ32(REG_A_STC_1);//modify by ling
#endif
  
}

//-------------------------------------------------------------------------
/** _StcReadV1
 *  Read STC V1
 */
//-------------------------------------------------------------------------
static UINT32 _StcReadV1(void)
{
    return PARSER_READ32(REG_V_STC_1);
}

//-------------------------------------------------------------------------
/** _StcLoadA1
 *  Load STC A1
 */
//-------------------------------------------------------------------------
static void _StcLoadA1(const DMX_PCR* prPcr)
{
    if (prPcr != NULL)
    {
#ifndef	CC_MT5391_AUD_SUPPORT
        PARSER_WRITE32(REG_A_STC_1, PCR_27M_2_90K(prPcr));
#else
        PARSER_2_WRITE32(REG_A_STC_1, PCR_27M_2_90K(prPcr));//modify by ling
#endif
    }
}

//-------------------------------------------------------------------------
/** _StcLoadV1
 *  Load STC V1
 */
//-------------------------------------------------------------------------

static void _StcLoadV1(const DMX_PCR* prPcr)
{
    if (prPcr != NULL)
    {
        PARSER_WRITE32(REG_V_STC_1, PCR_27M_2_90K(prPcr));
    }
}
#endif

//-------------------------------------------------------------------------
/** _DmxPcrRx
 *  Hardware IIR Filter Mode
 */
//-------------------------------------------------------------------------

void _DmxSetPcrInfo(BOOL fgEnable)
{
    _fgEnablePcrInfo = fgEnable;
}

static void _DmxPcrRx(DMX_PCR_MSSG* prPcrMssg)
{
    UINT32 u4Var;
    UINT32 u4StcId;
    DMX_PCR_ID eDmxPcrId;

    if (prPcrMssg == NULL)
    {
        return;
    }

#ifndef CC_SUPPORT_2STC
    eDmxPcrId = prPcrMssg->eDmxPcrId;
    u4StcId = STC_SRC_A1;
#else
    eDmxPcrId = prPcrMssg->eDmxPcrId;
    u4StcId = aucPidIdx2Stc[prPcrMssg->uStatus1.rField.u4PidIndex];
    if (u4StcId >= STC_SRC_NS)
    {
        return;
    }
#endif

#ifdef ENABLE_MULTIMEDIA
    if(!SWDMX_PcrCallBack(prPcrMssg->rPcr.u4PcrBase))
    {
        return;
    }
#endif
    if (au4StcEnable[u4StcId])
    {
        DMX_PCR* prPcr;
        prPcr = &(prPcrMssg->rPcr);
        au4PcrNs[u4StcId]++;
        //if (au4VCxO[u4StcId])    // with VCxO
        {
            UINT32 u4Interrupt;

#ifdef DMX_PCR_LOG
            DMX_PCR rSnapStc;
            DMX_DELTA_UNION_T rHwDelta;
            INT32 i4HwDrift;

            if(eDmxPcrId == DMX_PCR_1)
            {
                rSnapStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_SNAP_EXTENSION);
                rSnapStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC_SNAP_BASE);

                rHwDelta.au4Delta[0] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_LOWER);
                rHwDelta.au4Delta[1] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_UPPER);
            }
            else
            {
                rSnapStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC2_SNAP_EXTENSION);
                rSnapStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC2_SNAP_BASE);

                rHwDelta.au4Delta[0] = DMXCMD_READ32(DMX_REG_PCR2_ERR_TERM_LOWER);
                rHwDelta.au4Delta[1] = DMXCMD_READ32(DMX_REG_PCR2_ERR_TERM_UPPER);
                if(eDmxPcrId != DMX_PCR_2)
                {
                    LOG(0,"warning error dmx pcr id = %\n", eDmxPcrId);
                }
            }

            i4HwDrift = (INT32) rHwDelta.i8Delta;

            //LOG(1, "PCR (%x) STC SNAP(%x) D(%d) (0x%x)\n", PCR_27M((prPcr)), PCR_27M((&rSnapStc)), i4HwDrift, IO_READ32(DEMUX0_BASE, 0x2d8));
            if(_fgEnablePcrInfo)
            {
                LOG(0, "DmxPcrId(%d) StcId(%d): PCR(%x)DMXSTC(%x)STC(%x) D(%d)NER1(%x)NER2(%x)ER(%d) STA(%d)SyncMode(%d)\n", 
                    eDmxPcrId, u4StcId,
                    (prPcr->u4PcrBase),             // PCR packet
                    rSnapStc.u4PcrBase,             // DMX STC
                    apfStcRead[u4StcId](u4StcId),   // Driver used STC
                    i4HwDrift, prPcrMssg->uStatus1.u4Reg, prPcrMssg->uStatus2.u4Reg, prPcrMssg->uStatus1.rField.u4NonErr,   // PCR interrupt info
                    au4StcStatus[u4StcId], _au4MmAvSyncMode[u4StcId]);
            }

#else
            LOG(9, "PCR STATUS(%d) PCR(0x%x-0x%x) STC(0x%x) ERR(%d)\n",
                au4StcStatus[u4StcId],
                prPcr->u4PcrBase,
                prPcr->uPcrExt.rField.u4Ext,
                apfStcRead[u4StcId](u4StcId),
                prPcrMssg->uStatus1.rField.u4NonErr);
#endif

#ifdef HAL_5371_PWM0_LOCAL_REG
            // Handle PCR Load Interrupt
            if (prPcrMssg->uStatus1.rField.u4NonErr == 2)
            {
                INT32 i4Initial;
                UINT32 u4PwmHigh;

                // Set Y1, Y2 initial value according to current PWM setting

                u4PwmHigh = (IO_REG32(BIM_BASE, HAL_5371_PWM0_LOCAL_REG) >> PWM_HIGH) & 0xFF;

                i4Initial = (INT32) (u4PwmHigh) - 128;

                i4Initial *= (1 << IIR_K2);

                DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN1, (UINT32)i4Initial);
                DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN2, (UINT32)i4Initial);

                LOG(9, "Initial = %d\n", i4Initial);
            }
#endif

            // Error handling: always enable STC_A1 if use PCR update STC
            if (u4StcId == STC_SRC_A1)
            {
#ifndef	CC_MT5391_AUD_SUPPORT
                if (PARSER_READ32(REG_STC_CTRL) != 0)
#else
                if (PARSER_2_READ32(REG_STC_CTRL) != 0)
#endif
                {
                    LOG(1, "Re-enable STC_A1\n");
#ifndef	CC_MT5391_AUD_SUPPORT
                    PARSER_WRITE32(REG_STC_CTRL, 0);
#else
                    PARSER_2_WRITE32(REG_STC_CTRL, 0);
#endif
                }
            }            

            u4Interrupt = 0;
            switch (au4StcStatus[u4StcId])
            {
            case DMX_STC_STATUS_UNKNOWN:
            case DMX_STC_STATUS_INVALID:
                
                if (u4StcId == STC_SRC_A1)
                {
                    apfStcLoad[u4StcId](u4StcId, prPcr->u4PcrBase);
                }
                
                // Phase Compare Interrupt
                if (prPcrMssg->uStatus1.rField.u4NonErr == 1)
                {
                    STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_VALID);
                    //au4StcDrift[u4StcId] = 0;
                }
                else
                {
                    LOG(1, "[Warning] PCR Status INVALID to DRIFT\n");
                    STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_DRIFT);
                }
                break;

            case DMX_STC_STATUS_BIG_DRIFT:

                if (u4StcId == STC_SRC_A1)
                {
                    apfStcLoad[u4StcId](u4StcId, prPcr->u4PcrBase);
                }
                
                do
                {
                    HAL_TIME_T rTime;
                    HAL_TIME_T rDeltaTime = {0};

                    HAL_GetTime(&rTime);
                    HAL_GetDeltaTime(&rDeltaTime, &arBigDriftStartTime[u4StcId], &rTime);

                    // wait (1~10) second (DMX_STC_STATUS_BIG_DRIFT to DMX_STC_STATUS_DRIFT)
                    if (rDeltaTime.u4Seconds >= arBigDriftWaitTime[u4StcId])
                    {
                        STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_DRIFT);

                        LOG(1, "Stop Big Drift\n");
                    }
                } while(0);

                break;

            case DMX_STC_STATUS_DRIFT:

                if (u4StcId == STC_SRC_A1)
                {
                    apfStcLoad[u4StcId](u4StcId, prPcr->u4PcrBase);
                }
                
                // Phase Compare Interrupt
                if (prPcrMssg->uStatus1.rField.u4NonErr == 1)
                {
                    STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_VALID);
                    //au4StcDrift[u4StcId] = 0;
                }
                break;

            case DMX_STC_STATUS_VALID:

                // Phase Compare Interrupt
                if (prPcrMssg->uStatus1.rField.u4NonErr != 1)
                {
                    DMX_DELTA_UNION_T rHwDelta;
                    INT32 i4HwDrift;

                    if(eDmxPcrId == DMX_PCR_1)
                    {
                        rHwDelta.au4Delta[0] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_LOWER);
                        rHwDelta.au4Delta[1] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_UPPER);
                        i4HwDrift = (INT32) rHwDelta.i8Delta;
                    }
                    else
                    {
                        rHwDelta.au4Delta[0] = DMXCMD_READ32(DMX_REG_PCR2_ERR_TERM_LOWER);
                        rHwDelta.au4Delta[1] = DMXCMD_READ32(DMX_REG_PCR2_ERR_TERM_UPPER);
                        i4HwDrift = (INT32) rHwDelta.i8Delta;
                        if(eDmxPcrId != DMX_PCR_2)
                        {
                            LOG(0,"warning error dmx pcr id = %\n", eDmxPcrId);
                        }
                    }
                    // How about PCR jitter ?
                    // Can we do something at hardware IIR mode ? Do not load STC value

                    /*if(u4StcId == STC_SRC_A1)
                    {
                        apfStcLoad[u4StcId](prPcr);
                    }*/

#if 0
                    LOG(1, "Drift %d (%x) (%x) (%x)\n",
                        i4HwDrift,
                        apfStcRead[u4StcId](),
                        DMXCMD_READ32(143));
#endif

#if 1

#ifdef CC_GET_PCR_PTS_INFO
                    if (((i4HwDrift >= STC_BIG_DRIFT_THD) || ((-i4HwDrift) >= STC_BIG_DRIFT_THD)) &&
                            (_prDbase->rVdp.arFbCounter[0][0].u4DispOk != 0) && !_fgTimeShift)
#else
                    if (((i4HwDrift >= STC_BIG_DRIFT_THD) || ((-i4HwDrift) >= STC_BIG_DRIFT_THD)) && !_fgTimeShift)
#endif // CC_GET_PCR_PTS_INFO
                    {
                        UINT32 u4BigDrift;

                        if (i4HwDrift > 0)
                        {
                            u4BigDrift = (UINT32) i4HwDrift;
                        }
                        else
                        {
                            //u4BigDrift = (UINT32) (-i4HwDrift);
                            u4BigDrift = STC_BIG_DRIFT_THD;
                        }

                        // Big Difference, PCR un-reliable
                        STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_BIG_DRIFT);
                        HAL_GetTime(&arBigDriftStartTime[u4StcId]);

                        if (i4HwDrift == 0)
                        {
                            arBigDriftWaitTime[u4StcId] = 1;
                        }
                        else
                        {
                            arBigDriftWaitTime[u4StcId] = u4BigDrift / STC_BIG_DRIFT_THD;
                        }

                        if (arBigDriftWaitTime[u4StcId] == 0)
                        {
                            arBigDriftWaitTime[u4StcId] = 1;
                        }
                        else if(arBigDriftWaitTime[u4StcId] > 20) /* Regarded as loop case */
                        {
                            arBigDriftWaitTime[u4StcId] = 1;
                        }
                        else if (arBigDriftWaitTime[u4StcId] > 10)
                        {
                            arBigDriftWaitTime[u4StcId] = 10;
                        }

                        LOG(1, "Start Big Drift (%d) (%d)\n", arBigDriftWaitTime[u4StcId], i4HwDrift);
                    }
                    else
#endif
                    if ((i4HwDrift >= STC_INVALID_THD) || ((-i4HwDrift) >= STC_INVALID_THD))
                    {
                        // Big Difference, PCR un-reliable
                        STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_INVALID);
                    }
                    else
                    {
                        // Small Difference, PCR reliable
                        STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_DRIFT);
                    }
                }
                else
                {
#ifdef CC_VCXO_ON
                    u4Interrupt = 1;
#endif

#ifdef DMX_PCR_LOG
                    u4Interrupt = 0;
#endif
                }

                if (!au4VCxO[u4StcId])    // without VCxO
                {
                    if (prPcrMssg->uStatus2.rField.u4PwmAdjust != _u4PwmAdjust)
                    {
                        UINT32 u4Adjust;
                        UINT32 u4AudAdjust;
                        UINT32 u4VdoAdjust;
#if 0
                        UINT32 u4Reg140;
                        UINT32 u4Reg141;
                        INT32 i4HwDrift;
                        DMX_DELTA_UNION_T rHwDelta;

                        u4Reg140 = DMXCMD_READ32(140);
                        u4Reg141 = DMXCMD_READ32(141);

                        rHwDelta.au4Delta[0] = u4Reg140;
                        rHwDelta.au4Delta[1] = u4Reg141;
                        i4HwDrift = (INT32) rHwDelta.i8Delta;

                        LOG(1, "Smart B2R %d >> %d D(%d)\n" , (_u4PwmAdjust & 0xFF), (prPcrMssg->uStatus2.rField.u4PwmAdjust & 0xFF), i4HwDrift);
#endif

                        u4Adjust = (prPcrMssg->uStatus2.rField.u4PwmAdjust & 0xFF);

                        //LOG(1, "pwm(%d), Adjust(%d) PCR(%x) STC(%x) \n", u4Adjust, _u4PwmAdjust, (prPcr->u4PcrBase), apfStcRead[u4StcId]());

                        if (u4Adjust > 128)
                        {
                            u4Adjust = ((u4Adjust - 128) << 1);
                            if (u4Adjust > 0xff)
                            {
                                u4Adjust = 0xff;
                            }
                            u4VdoAdjust = 0x100 + u4Adjust;
                            u4AudAdjust = 0x100 + u4Adjust;
                        }
                        else
                        {
                            u4Adjust = ((128 - u4Adjust) << 1);
                            if (u4Adjust > 0xff)
                            {
                                u4Adjust = 0xff;
                            }
                            u4VdoAdjust = 0x000 + u4Adjust;
                            u4AudAdjust = u4Adjust;
                        }

                        if (eDmxPcrId == DMX_PCR_1)
                        {
                            IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_ERR_HD) = u4VdoAdjust;
                        }
                        else
                        {
                            IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_ERR_SD) = u4VdoAdjust;
                        }
                        // Bit 9: STC ID
                        u4AudAdjust = ((u4AudAdjust & 0x1FF) | (u4StcId << 9));
                        AUD_SendApllAdjustCmd(u4AudAdjust);

                        _u4PwmAdjust = prPcrMssg->uStatus2.rField.u4PwmAdjust;

                        // important, DMX 27M STC >> PSR 90K STC

                        if (u4StcId == STC_SRC_A1)
                        {
                            apfStcLoad[u4StcId](u4StcId, prPcr->u4PcrBase);
                        }
                    }
                }

                break;

            case DMX_STC_STATUS_TRACKING:
            default:
                ASSERT(0);
                break;
            }

            if(eDmxPcrId == DMX_PCR_1)
            {
                u4Var = DMXCMD_READ32(DMX_REG_PCR_CTRL);
                u4Var &= 0xFFFFFFF9;    // Remove New PCR, Add Phase Compare Interrupt
                if (u4Interrupt)
                {
                    u4Var |= 4;         // Disable Phase Compare Interrupt
                }
                DMXCMD_WRITE32(DMX_REG_PCR_CTRL, u4Var);
            }
            else if(eDmxPcrId == DMX_PCR_2)
            {
                u4Var = DMXCMD_READ32(DMX_REG_PCR2_CTRL);
                u4Var &= 0xFFFFFFF9;    // Remove New PCR, Add Phase Compare Interrupt
                if (u4Interrupt)
                {
                    u4Var |= 4;         // Disable Phase Compare Interrupt
                }
                DMXCMD_WRITE32(DMX_REG_PCR2_CTRL, u4Var);
            }
            else
            {
                LOG(0,"error dmx pcr id %d\n", eDmxPcrId);
            }

        }

#if 0
        else                    // without VCxO
        {
            UINT32 u4Drift;

#ifdef DMX_PCR_LOG
            DMX_PCR rSnapStc;
            DMX_DELTA_UNION_T rHwDelta;
            INT32 i4HwDrift;

            rSnapStc.uPcrExt.u4Reg = DMXCMD_READ32(138);
            rSnapStc.u4PcrBase = DMXCMD_READ32(139);

            rHwDelta.au4Delta[0] = DMXCMD_READ32(140);
            rHwDelta.au4Delta[1] = DMXCMD_READ32(141);
            i4HwDrift = (INT32) rHwDelta.i8Delta;

            LOG(1, "PCR (%x) STC SNAP(%x) D(%d)\n", PCR_27M((prPcr)), PCR_27M((&rSnapStc)), i4HwDrift);
            //LOG(1, "90K PCR (%x) STC SNAP(%x) A-STC(%x)\n", PCR_27M_2_90K((prPcr)), PCR_27M_2_90K((&rSnapStc)), apfStcRead[u4StcId]());
#else

            LOG(9, "PCR(0x%x-0x%x) STC(0x%x)\n",
                prPcr->u4PcrBase,
                prPcr->uPcrExt.rField.u4Ext,
                apfStcRead[u4StcId]());
#endif

            u4Drift = 0;
            if (apfStcRead[u4StcId])
            {
                UINT32 u4Stc;

                u4Stc = apfStcRead[u4StcId]();

                if (u4Stc > prPcr->u4PcrBase)
                {
                    u4Drift = u4Stc - prPcr->u4PcrBase;
                }
                else
                {
                    u4Drift = prPcr->u4PcrBase - u4Stc;
                }
            }

            if (u4Drift > STC_INVALID_90K_THD)
            {
                if (au4StcDrift[u4StcId] != 0)
                {
#if 0
                    LOG(1, "Drift Load %d (%x) (%x)\n",
                        u4Drift,
                        apfStcRead[u4StcId](),
                        DMXCMD_READ32(143));
#endif

                    // Previous also Invalid Drift, update
                    apfStcLoad[u4StcId](prPcr);
                    STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_VALID);

                    au4StcDrift[u4StcId] = 0;
                }
                else
                {
#if 0
                    LOG(1, "Drift Wait Next %d (%x) (%x)\n",
                        u4Drift,
                        apfStcRead[u4StcId](),
                        DMXCMD_READ32(143));
#endif
                    // Invalid Drift, wait next
                    au4StcDrift[u4StcId]++;
                }
            }
            else
            {
                // VCxO is NOT avaiable, just re-load for every PCR
                apfStcLoad[u4StcId](prPcr);
                STC_CHG_STATUS(u4StcId, DMX_STC_STATUS_VALID);

                au4StcDrift[u4StcId] = 0;
            }
        }
#endif
    }
}

//-------------------------------------------------------------------------
/** _DmxTurnOffVCxO
 *  Turn Off VCxO Slowly
 */
//-------------------------------------------------------------------------
#ifdef CC_TURN_OFF_VCXO
static void _DmxTurnOffVCxO(HANDLE_T pt_tm_handle, VOID* pv_tag)
{
#ifdef HAL_5371_PWM0_LOCAL_REG
    UINT32 u4Step;
    UINT32 u4PwmHigh;
    UINT32 u4Reg;

    u4Reg = IO_REG32(BIM_BASE, HAL_5371_PWM0_LOCAL_REG);
    u4PwmHigh = (u4Reg >> PWM_HIGH) & 0xFF;

    if (u4PwmHigh > STC_PWMH_INIT)
    {
        u4Step = (u4PwmHigh - STC_PWMH_INIT);

        if (u4Step > DMX_VCXO_OFF_PWM_STEP)
        {
            u4Step = DMX_VCXO_OFF_PWM_STEP;
        }

        u4PwmHigh -= u4Step;
    }
    else
    {
        u4Step = (STC_PWMH_INIT - u4PwmHigh);

        if (u4Step > DMX_VCXO_OFF_PWM_STEP)
        {
            u4Step = DMX_VCXO_OFF_PWM_STEP;
        }

        u4PwmHigh += u4Step;
    }

    IO_REG32(BIM_BASE, HAL_5371_PWM0_LOCAL_REG) = (((u4Reg) & 0xFF00FFFF) | (u4PwmHigh << PWM_HIGH));

    if ((u4PwmHigh != STC_PWMH_INIT) &&
            (au4StcEnable[STC_SRC_A1] == 0))
    {
        VERIFY(x_timer_start(_hTimer,
                             DMX_VCXO_OFF_TIME_STEP,
                             X_TIMER_FLAG_ONCE,
                             _DmxTurnOffVCxO,
                             NULL) == OSR_OK);
    }

    LOG(1, "VCxO (%d)\n", u4PwmHigh);

    UNUSED(pt_tm_handle);
    UNUSED(pv_tag);
#endif
}
#endif

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

void _DMX_PcrReInitHW(void)
{
    INT32 i4Coef;
    UINT32 u4Var;

    // Auto Load + New PCR + 8 Bit Offset PWM + PWM Shift + Y Shift
    u4Var = (UINT32)(((IIR_K2 + IIR_K1) << 16) + ((IIR_K1) << 8));
    DMXCMD_WRITE32(DMX_REG_PCR_CTRL, (UINT32)((0x11000003) | u4Var));
    DMXCMD_WRITE32(DMX_REG_PCR2_CTRL, (UINT32)((0x11000003) | u4Var));

    // |PCR-STC| Threshold
    DMXCMD_WRITE32(DMX_REG_PCR_ERR_THRESHOLD, STC_DRIFT_THD);
    DMXCMD_WRITE32(DMX_REG_PCR2_ERR_THRESHOLD, STC_DRIFT_THD);

    // Reset X[n-1], Y[n-1], Y[n-2]
    DMXCMD_WRITE32(DMX_REG_PCR_IIR_XN1, 0);
    DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN1, 0);
    DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN2, 0);

    DMXCMD_WRITE32(DMX_REG_PCR2_IIR_XN1, 0);
    DMXCMD_WRITE32(DMX_REG_PCR2_IIR_YN1, 0);
    DMXCMD_WRITE32(DMX_REG_PCR2_IIR_YN2, 0);

    // IIR Coefficient A0
    i4Coef = (INT32)(1 << IIR_K0);
    DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF0, (UINT32)i4Coef);
    DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF0, (UINT32)i4Coef);

    // IIR Coefficient A1
    i4Coef = -i4Coef;
    DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF1, (UINT32)i4Coef);
    DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF1, (UINT32)i4Coef);

    // IIR Coefficient B1
    i4Coef = (INT32)(2 * (1 << IIR_K1)) - 1;
    DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF2, (UINT32)i4Coef);
    DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF2, (UINT32)i4Coef);

    // IIR Coefficient B2
    i4Coef = -(INT32)((1 << IIR_K1) - 1);
    DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF3, (UINT32)i4Coef);
    DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF3, (UINT32)i4Coef);
}

//-------------------------------------------------------------------------
/** _DMX_PcrInit
 *  Initialize PCR module
 */
//-------------------------------------------------------------------------
void _DMX_PcrInit(void)
{
    // When demux re-init, it will reset registers, which include PCR part
    // Therefore, PCR should also do re-init

//create software stc thread for multiple instance
#ifdef CC_53XX_SWDMX_V2
    if(hWaitDataTimer == (HANDLE_T)NULL)
    {
        VERIFY(x_timer_create(&hWaitDataTimer) == OSR_OK);
        VERIFY(x_timer_start(hWaitDataTimer, SOFT_STC_TIMER, X_TIMER_FLAG_REPEAT,
                             STC_SoftwareStcISR, (VOID*)(UINT32)0) == OSR_OK); //10sec
    }
#endif

#ifdef CC_GET_PCR_PTS_INFO
    CRIT_STATE_T _rCritState;
#endif

    UNUSED(_u4Initiated);

#ifdef CC_GET_PCR_PTS_INFO
    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));
#endif

#ifdef CC_VCXO_CLK_SWITCH
#ifdef CC_VCXO_ON

    // VCXO27_CK = VCXO_CK
    IO_REG32(CKGEN_BASE, 0x204) = ((IO_REG32(CKGEN_BASE, 0x204) & 0xFFFFCFFF) | 0x1000);

    ////Pin Mux of OPWM0
    UNUSED(BSP_PinSet(OPWM0_O, PINMUX_FUNCTION0));

#ifdef CC_MT5391
    //SYSPLL from VCXO27_CK
    IO_REG32(CKGEN_BASE, 0x160) = ((IO_REG32(CKGEN_BASE, 0x160) & 0xFFF3FFFF) | 0x10000);
#else
    IO_REG32(CKGEN_BASE, 0x164) = ((IO_REG32(CKGEN_BASE, 0x164) & 0xDFFFFFFF) | 0x20000000);
#endif


    // Analog MUX, PLL Input = VCxO, 0xD164[29] = 1
//    IO_REG32(CKGEN_BASE, 0x164) = IO_REG32(CKGEN_BASE, 0x164) | 0x20000000;

    // PIN-MUX, PWM0
    //UNUSED(BSP_PinSet(OPWM0_O, PINMUX_FUNCTION1));
//    IO_REG32(CKGEN_BASE, 0x400) = ((IO_REG32(CKGEN_BASE, 0x400) & 0xF3FFFFFF) | 0x04000000);
#else
    // VCXO27_CK = TVDPLL/20 = 27MHz
    IO_REG32(CKGEN_BASE, 0x204) = ((IO_REG32(CKGEN_BASE, 0x204) & 0xFFFFCFFF) | 0x2000);

#endif
#endif

#ifdef CC_SUPPORT_2STC
    LOG(1, "PcrInit: 2 STC enabled\n");
#else
    LOG(1, "PcrInit: 2 STC disabled\n");
#endif

//    if (!_u4Initiated)
    {
        INT32 i4Coef;
        UINT32 u4Var;
        UINT32 u4StcId;
        UINT32 u4PidIdx;

#ifdef CC_TURN_OFF_VCXO
        if (_hTimer == 0)
        {
            VERIFY(x_timer_create(&_hTimer) == OSR_OK);
        }
#endif

        // always enable, do not hold STC
#ifndef	CC_MT5391_AUD_SUPPORT
        PARSER_WRITE32(REG_STC_CTRL, 0);
#else
        PARSER_2_WRITE32(REG_STC_CTRL, 0); //modify by ling
#endif
        for (u4StcId = 0; u4StcId < STC_SRC_NS; u4StcId++)
        {
            au4StcEnable[u4StcId] = 0;
            au4StcStatus[u4StcId] = DMX_STC_STATUS_UNKNOWN;
            //au4StcDrift[u4StcId] = 0;
            au4StcAudTarget[u4StcId] = 0xFF;
            au4StcVidTarget[u4StcId] = 0xFF;
            au4StcStarted[u4StcId] = 0;
            //ai4StcDrift[u4StcId] = 0;

#ifdef CC_FAST_CHANGE_CHANNEL
            au4VdoFifo[u4StcId] = 0xFFFFFFFF;
            au4VdoPkt[u4StcId] = 0;
            au4VdoFifoRdy[u4StcId] = 0;

            au4AudFifo[u4StcId] = 0xFFFFFFFF;
            au4AudPkt[u4StcId] = 0;
            au4AudFifoRdy[u4StcId] = 0;

            au4StartPts[u4StcId] = 0;
#endif
        }

        for (u4PidIdx = 0; u4PidIdx < DMX_NUM_PID_INDEX; u4PidIdx++)
        {
            aucPidIdx2Stc[u4PidIdx] = STC_SRC_NS;
        }

        for (u4PidIdx = 0; u4PidIdx < MAX_DEVICE_ID; u4PidIdx++)
        {
            au4VidTarget2Stc[u4PidIdx] = STC_SRC_NS;
            au4AudTarget2Stc[u4PidIdx] = STC_SRC_NS;
        }

        // PWM Init
        // PWM Enable & High = 128 >> 27MHz
#ifdef CC_VCXO_ON
#ifdef HAL_5371_PWM0_LOCAL_REG
        IO_REG32(BIM_BASE, HAL_5371_PWM0_LOCAL_REG) =
            ((((UINT32)STC_PWMH_INIT) << PWM_HIGH) |
             (((UINT32)255) << PWM_RESOLUTION) |
             (((UINT32)0) << PWM_PRESCALER) |
             PWM_EN | (PWM_LOAD_DMX * au4VCxO[STC_SRC_A1]));
#endif
#else
#ifdef HAL_5371_PWM0_LOCAL_REG
        IO_REG32(BIM_BASE, HAL_5371_PWM0_LOCAL_REG) =
            ((((UINT32)STC_PWMH_INIT) << PWM_HIGH) |
             (((UINT32)255) << PWM_RESOLUTION) |
             (((UINT32)0) << PWM_PRESCALER) |
             PWM_EN);
#endif
        // PCR Recovery Enable (B2R HD)
        //IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_EN) |= B2R_PCR_CTRL_EN_HD_M;
        //IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_ERR_HD) = 0;

        // PCR Recovery Enable (B2R SD)
        //IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_EN) |= B2R_PCR_CTRL_EN_SD_M;
        //IO_REG32(BLK2RS_BASE, B2R_PCR_CTRL_ERR_SD) = 0;
        B2R_EnalbleSmartPcr(B2R_1, TRUE);
        B2R_EnalbleSmartPcr(B2R_2, TRUE);
#endif

        // Prepare Hardware IIR Filter

        // Auto Load + New PCR + 8 Bit Offset PWM + PWM Shift + Y Shift
        u4Var = (UINT32)(((IIR_K2 + IIR_K1) << 16) + ((IIR_K1) << 8));
        DMXCMD_WRITE32(DMX_REG_PCR_CTRL, (UINT32)((0x11000003) | u4Var));
        DMXCMD_WRITE32(DMX_REG_PCR2_CTRL, (UINT32)((0x11000003) | u4Var));

        // |PCR-STC| Threshold
        DMXCMD_WRITE32(DMX_REG_PCR_ERR_THRESHOLD, STC_DRIFT_THD);
        DMXCMD_WRITE32(DMX_REG_PCR2_ERR_THRESHOLD, STC_DRIFT_THD);

        // Reset X[n-1], Y[n-1], Y[n-2]
        DMXCMD_WRITE32(DMX_REG_PCR_IIR_XN1, 0);
        DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN1, 0);
        DMXCMD_WRITE32(DMX_REG_PCR_IIR_YN2, 0);

        DMXCMD_WRITE32(DMX_REG_PCR2_IIR_XN1, 0);
        DMXCMD_WRITE32(DMX_REG_PCR2_IIR_YN1, 0);
        DMXCMD_WRITE32(DMX_REG_PCR2_IIR_YN2, 0);


        // IIR Coefficient A0
        i4Coef = (INT32)(1 << IIR_K0);
        DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF0, (UINT32)i4Coef);
        DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF0, (UINT32)i4Coef);

        // IIR Coefficient A1
        i4Coef = -i4Coef;
        DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF1, (UINT32)i4Coef);
        DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF1, (UINT32)i4Coef);

        // IIR Coefficient B1
        i4Coef = (INT32)(2 * (1 << IIR_K1)) - 1;
        DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF2, (UINT32)i4Coef);
        DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF2, (UINT32)i4Coef);

        // IIR Coefficient B2
        i4Coef = -(INT32)((1 << IIR_K1) - 1);
        DMXCMD_WRITE32(DMX_REG_PCR_FIR_FILTER_COEFF3, (UINT32)i4Coef);
        DMXCMD_WRITE32(DMX_REG_PCR2_FIR_FILTER_COEFF3, (UINT32)i4Coef);

        LOG(9, "IIR A0 = %d, A1 = %d, B1 = %d, B2 = %d, K0(%d), K1(%d), K2(%d)\n",
            (INT32) DMXCMD_READ32(DMX_REG_PCR_FIR_FILTER_COEFF0),
            (INT32) DMXCMD_READ32(DMX_REG_PCR_FIR_FILTER_COEFF1),
            (INT32) DMXCMD_READ32(DMX_REG_PCR_FIR_FILTER_COEFF2),
            (INT32) DMXCMD_READ32(DMX_REG_PCR_FIR_FILTER_COEFF3),
            IIR_K0, IIR_K1, IIR_K2);

        LOG(9, "IIR A0 = %d, A1 = %d, B1 = %d, B2 = %d, K0(%d), K1(%d), K2(%d)\n",
            (INT32) DMXCMD_READ32(DMX_REG_PCR2_FIR_FILTER_COEFF0),
            (INT32) DMXCMD_READ32(DMX_REG_PCR2_FIR_FILTER_COEFF1),
            (INT32) DMXCMD_READ32(DMX_REG_PCR2_FIR_FILTER_COEFF2),
            (INT32) DMXCMD_READ32(DMX_REG_PCR2_FIR_FILTER_COEFF3),
            IIR_K0, IIR_K1, IIR_K2);

        _u4Initiated = 1;
    }
}

static UINT32 AdjustPCRStatus1Reg(UINT32 u4Reg)
{
    UINT8 u1Pidx, i;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4PidxTable;
    u1Pidx = (u4Reg >> 16) & 0xFF;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return u4Reg;
    }

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    if(prPidStruct->ePidType == DMX_PID_TYPE_NONE)
    {
        return u4Reg;
    }

    u4PidxTable = (PID_INDEX_TABLE(u1Pidx) & 0xFFFF0000) | 0x8000;

    for(i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        UINT32 u4Tmp;
        u4Tmp = PID_INDEX_TABLE(i);
        if((u4Tmp & 0xFFFF8000) == u4PidxTable)
        {
            break;
        }
    }

    if(i == DMX_NUM_PID_INDEX)
    {
        return u4Reg;
    }

    u4Reg = (u4Reg & 0xFF00FFFF) | (i << 16);

    return u4Reg;
}

//-------------------------------------------------------------------------
/** _DMX_IntPcr
 */
//-------------------------------------------------------------------------
void _DMX_IntPcr(UINT32 u4Status)
{
    UINT32 u4Reg;

    if (u4Status & DMX_INT_STATUS_PCR)
    {
        _rPcrMssg.eDmxPcrId = DMX_PCR_1;
        _rPcrMssg.rPcr.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_PCR_EXTENSION);
        _rPcrMssg.rPcr.u4PcrBase = DMXCMD_READ32(DMX_REG_PCR_BASE);

        _rPcrMssg.uStatus1.u4Reg = DMXCMD_READ32(DMX_REG_PCR_NONERR_STATUS_REG1);
        _rPcrMssg.uStatus2.u4Reg = DMXCMD_READ32(DMX_REG_PCR_NONERR_STATUS_REG2);

        _rPcrMssg.uStatus1.u4Reg = AdjustPCRStatus1Reg(_rPcrMssg.uStatus1.u4Reg);

        _DmxPcrRx(&_rPcrMssg);

        LOG(9, "PCR status: 0x%08x, 0x%08x\n", _rPcrMssg.uStatus1.u4Reg,
            _rPcrMssg.uStatus2.u4Reg);

        //clear current INT to pop up next, write any value
        DMXCMD_WRITE32(DMX_REG_PCR_NONERR_STATUS_REG1, 1);

        // Debug
        if ((_rPcrMssg.uStatus1.u4Reg & 8) != 0)
        {
            static UINT32 _u4Count = 0;
            _u4Count++;
            LOG(3, "Received periodic PCR interrupt %u\n", _u4Count);
        }
    }

    if (u4Status & DMX_INT_STATUS_PCR2)
    {
        _rPcrMssg.eDmxPcrId = DMX_PCR_2;
        _rPcrMssg.rPcr.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_PCR2_EXTENSION);
        _rPcrMssg.rPcr.u4PcrBase = DMXCMD_READ32(DMX_REG_PCR2_BASE);

        _rPcrMssg.uStatus1.u4Reg = DMXCMD_READ32(DMX_REG_PCR2_NONERR_STATUS_REG1);
        _rPcrMssg.uStatus2.u4Reg = DMXCMD_READ32(DMX_REG_PCR2_NONERR_STATUS_REG2);

        _rPcrMssg.uStatus1.u4Reg = AdjustPCRStatus1Reg(_rPcrMssg.uStatus1.u4Reg);

        _DmxPcrRx(&_rPcrMssg);

        //clear current INT to pop up next, write any value
        DMXCMD_WRITE32(DMX_REG_PCR2_NONERR_STATUS_REG1, 1);
    }

    if(u4Status & DMX_INT_ERR_PCR)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_PCR_ERROR_STATUS_REG);
        DMXCMD_WRITE32(DMX_REG_PCR_ERROR_STATUS_REG, 1);
        LOG(5, "PCR error interrupt: 0x%x\n", u4Reg);
    }

    if(u4Status & DMX_INT_ERR_PCR2)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_PCR2_ERROR_STATUS_REG);
        DMXCMD_WRITE32(DMX_REG_PCR2_ERROR_STATUS_REG, 1);
        LOG(5, "PCR error interrupt: 0x%x\n", u4Reg);
    }

    UNUSED(u4Reg);
}

void _DMX_SetTimeShiftMode(BOOL fgEnable)
{
    _fgTimeShift = fgEnable;
}

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** DMX_SetStcSrc
 *  Link PID index to STC
 */
//-------------------------------------------------------------------------
INT32 DMX_SetStcSrc(UCHAR ucPidIdx, UCHAR ucStcId)
{
    LOG(3, "PID(%d) SRC(%d)\n", ucPidIdx, ucStcId);

    if (ucPidIdx >= DMX_NUM_PID_INDEX)
    {
        ASSERT(ucPidIdx < DMX_NUM_PID_INDEX);
        return STC_SET_FAIL;
    }

    if (ucStcId > STC_SRC_NS)
    {
        ucStcId = STC_SRC_NS;
    }

    if (ucStcId < STC_SRC_NS)
    {
        UINT32 u4PidId;

        // check if more than one PID idx connect to the same STC source
        for (u4PidId = 0; u4PidId < DMX_NUM_PID_INDEX; u4PidId++)
        {
            if ((aucPidIdx2Stc[u4PidId] == ucStcId) && (u4PidId != ucPidIdx))
            {
                aucPidIdx2Stc[u4PidId] = STC_SRC_NS;
            }
        }

        if (aucPidIdx2Stc[ucPidIdx] < STC_SRC_NS)
        {
            STC_CHG_STATUS(aucPidIdx2Stc[ucPidIdx], DMX_STC_STATUS_INVALID);
            au4StcEnable[aucPidIdx2Stc[ucPidIdx]] = 0;
        }

        au4StcEnable[ucStcId] = 1;

        // Reset PCR Counter
        IO_READ32(DEMUX0_BASE, 0x250) = 0;
    }
    else
    {
        if (aucPidIdx2Stc[ucPidIdx] < STC_SRC_NS)
        {
            STC_CHG_STATUS(aucPidIdx2Stc[ucPidIdx], DMX_STC_STATUS_INVALID);
            au4StcEnable[aucPidIdx2Stc[ucPidIdx]] = 0;
        }
    }

    aucPidIdx2Stc[ucPidIdx] = ucStcId;

    if (ucStcId < STC_SRC_NS)
    {
        // set new PCR bit
        if (au4VCxO[ucStcId])
        {
            UINT32 u4Var;

            // Enable Phase Compare Interrupt
            u4Var = DMXCMD_READ32(DMX_REG_PCR_CTRL);
            u4Var &= 0xFFFFFFF9;    // Remove New PCR, Add Phase Compare Interrupt
            u4Var |= 2;             // Add New PCR
            DMXCMD_WRITE32(DMX_REG_PCR_CTRL, u4Var);

            // Enable Phase Compare Interrupt
            u4Var = DMXCMD_READ32(DMX_REG_PCR2_CTRL);
            u4Var &= 0xFFFFFFF9;    // Remove New PCR, Add Phase Compare Interrupt
            u4Var |= 2;             // Add New PCR
            DMXCMD_WRITE32(DMX_REG_PCR2_CTRL, u4Var);
        }
    }

    // Turn off VCxO Slowly
#ifdef CC_TURN_OFF_VCXO
    if (au4VCxO[STC_SRC_A1] != 0)
    {
        LOG(3, "STC_SRC_A1 %d\n", au4StcEnable[STC_SRC_A1]);

        if (au4StcEnable[STC_SRC_A1] != 0)
        {
            VERIFY(x_timer_stop(_hTimer) == OSR_OK);
        }
        else
        {
            VERIFY(x_timer_start(_hTimer,
                                 DMX_VCXO_OFF_TIME_STEP,
                                 X_TIMER_FLAG_ONCE,
                                 _DmxTurnOffVCxO,
                                 NULL) == OSR_OK);
        }
    }
#endif

    return STC_SET_OK;
}


#ifdef CC_GET_PCR_PTS_INFO
//-------------------------------------------------------------------------
/** STC_GetPcr
 */
//-------------------------------------------------------------------------
void STC_GetPcr(STC_CLOCK* prPcr)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        DMX_PCR rStc;

        rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);
        prPcr->ucBaseHi = rStc.uPcrExt.rField.fgBase32;
        prPcr->u2Ext = rStc.uPcrExt.rField.u4Ext;
        prPcr->u4Base = DMXCMD_READ32(DMX_REG_STC_BASE);
    }
    else
    {
        prPcr->ucBaseHi = 0;
        prPcr->u2Ext = 0;
        prPcr->u4Base = 0;
    }
}

//-------------------------------------------------------------------------
/** STC_GetPcrStcDelta
 */
//-------------------------------------------------------------------------
void STC_GetPcrStcDelta(INT64* pi8Delta)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        INT32 i4HwDrift;
        DMX_DELTA_UNION_T rHwDelta;

        rHwDelta.au4Delta[0] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_LOWER);
        rHwDelta.au4Delta[1] = DMXCMD_READ32(DMX_REG_PCR_ERR_TERM_UPPER);
        i4HwDrift = (INT32) rHwDelta.i8Delta;

        *pi8Delta = (INT64) i4HwDrift;
    }
    else
    {
        *pi8Delta = 0;
    }
}

//-------------------------------------------------------------------------
/** STC_GetAudioPts
 */
//-------------------------------------------------------------------------
void STC_GetAudioPts(STC_CLOCK* prAudioPts)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        DMX_PCR rStc;

        rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);
        prAudioPts->ucBaseHi = rStc.uPcrExt.rField.fgBase32;
        prAudioPts->u2Ext = 0;

        if (_prDbase != NULL)
        {
            prAudioPts->u4Base = _prDbase->rPts.u4AudioPts;
        }
    }
    else
    {
        prAudioPts->ucBaseHi = 0;
        prAudioPts->u2Ext = 0;
        prAudioPts->u4Base = 0;
    }
}

//-------------------------------------------------------------------------
/** STC_GetAudioPtsStcDelta
 */
//-------------------------------------------------------------------------
void STC_GetAudioPtsStcDelta(INT64* pi8Delta)
{
    *pi8Delta = 0;

    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        if (_prDbase != NULL)
        {
            *pi8Delta = _prDbase->rPts.i4AudioPtsStcDelta;
        }
    }
}

//-------------------------------------------------------------------------
/** STC_GetAudioSync
 */
//-------------------------------------------------------------------------
BOOL STC_GetAudioSync(void)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        if (_prDbase != NULL)
        {
            return _prDbase->rPts.fgAudioSync;
        }
    }

    return FALSE;
}

//-------------------------------------------------------------------------
/** STC_GetVideoPts
 */
//-------------------------------------------------------------------------
void STC_GetVideoPts(STC_CLOCK* prVideoPts)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        DMX_PCR rStc;

        rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);
        prVideoPts->ucBaseHi = rStc.uPcrExt.rField.fgBase32;
        prVideoPts->u2Ext = 0;

        if (_prDbase != NULL)
        {
            prVideoPts->u4Base = _prDbase->rPts.u4VideoPts;
        }
    }
    else
    {
        prVideoPts->ucBaseHi = 0;
        prVideoPts->u2Ext = 0;
        prVideoPts->u4Base = 0;
    }
}

//-------------------------------------------------------------------------
/** STC_GetVideoPtsStcDelta
 */
//-------------------------------------------------------------------------
void STC_GetVideoPtsStcDelta(INT64* pi8Delta)
{
    *pi8Delta = 0;

    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        if (_prDbase != NULL)
        {
            *pi8Delta = _prDbase->rPts.i4VideoPtsStcDelta;
        }
    }
}

//-------------------------------------------------------------------------
/** STC_GetVideoSync
 */
//-------------------------------------------------------------------------
BOOL STC_GetVideoSync(void)
{
    if (au4StcEnable[STC_SRC_A1] != 0)
    {
        if (_prDbase != NULL)
        {
            return _prDbase->rPts.fgVideoSync;
        }
    }

    return FALSE;
}
#endif // CC_GET_PCR_PTS_INFO



//-------------------------------------------------------------------------
/** STC_GetSrc
 */
//-------------------------------------------------------------------------
INT32 STC_GetSrc(UCHAR ucStcId, STC_CLOCK* prStc)
{
    STC_VERIFY_NULL_ERROR(ucStcId);

    // consider audio master in the future
    if (prStc == NULL)
    {
        return STC_INVALID;
    }

    if (ucStcId >= STC_SRC_NS)
    {
        return STC_INVALID;
    }
    
    if (au4StcEnable[ucStcId] || (_au4MmAvSyncMode[ucStcId] != AV_SYNC_MODE_NONE))
    {
#ifdef CC_SUPPORT_2STC
#ifdef __KERNEL__
        if (_au4MmAvSyncMode[ucStcId] == AV_SYNC_MODE_NONE)
        {
            DMX_PCR rStc;
            if (ucStcId == STC_SRC_A1)
            {
                //rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC2_EXTENSION); // inverse DMX STC id
                //rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC2_BASE);
                rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION); // inverse DMX STC id
                rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC_BASE);
            }
            else
            {
                //rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);  // inverse DMX STC id
                //rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC_BASE);
                rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC2_EXTENSION); 
                rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC2_BASE);
            }
            prStc->u4Base = rStc.u4PcrBase;
            prStc->ucBaseHi = rStc.uPcrExt.rField.fgBase32;
            prStc->u2Ext = rStc.uPcrExt.rField.u4Ext;
        }
        else
#endif // __KERNEL__
        {
#ifdef CC_53XX_SWDMX_V2
            prStc->u4Base = apfStcRead[ucStcId](ucStcId);
#else
            prStc->u4Base = apfStcRead[ucStcId]();
#endif
            prStc->ucBaseHi = 0;
            prStc->u2Ext = 0;
        }
#else // CC_SUPPORT_2STC
#ifdef __KERNEL__
        if ((ucStcId == STC_SRC_A1) && (_au4MmAvSyncMode[ucStcId] == AV_SYNC_MODE_NONE))
        {
            DMX_PCR rStc;
            rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);
            rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC_BASE);
            prStc->u4Base = rStc.u4PcrBase;
            prStc->ucBaseHi = rStc.uPcrExt.rField.fgBase32;
            prStc->u2Ext = rStc.uPcrExt.rField.u4Ext;
        }
        else
#endif // __KERNEL__
        {
#ifdef CC_53XX_SWDMX_V2
            prStc->u4Base = apfStcRead[ucStcId](ucStcId);
#else
            prStc->u4Base = apfStcRead[ucStcId]();
#endif
            prStc->ucBaseHi = 0;
            prStc->u2Ext = 0;
        }
#endif // CC_SUPPORT_2STC

        //prStc->u4Base += VDP_GetAvsyncDelta();

        if ((au4StcStatus[ucStcId] >= DMX_STC_STATUS_VALID) || (_au4MmAvSyncMode[ucStcId] != AV_SYNC_MODE_NONE))
        {
            return STC_VALID;
        }
    }

    return STC_INVALID;
}

//-------------------------------------------------------------------------
/** STC_SetSrc
 */
//-------------------------------------------------------------------------
INT32 STC_SetSrc(UCHAR ucStcId, STC_CLOCK* prStc)
{
    UNUSED(ucStcId);
    UNUSED(prStc);

    return STC_INVALID;
}

//-------------------------------------------------------------------------
/** STC_SetSyncTarget
 */
//-------------------------------------------------------------------------
INT32 STC_SetSyncTarget(UCHAR ucType, UCHAR ucStcId, UCHAR ucTargetId)
{
    LOG(3, " STC_SetSyncTarget: StcId(%d) Type(%d) TargetId(%d)\n", ucStcId, ucType, ucTargetId);

    if (ucType >= AV_SYNC_TARGET_NS)
    {
        return STC_INVALID;
    }

    if (ucTargetId >= MAX_DEVICE_ID)
    {
        return STC_INVALID;
    }

    // clean
    if (ucStcId >= STC_SRC_NS)
    {
        UINT32 u4Idx;
        for (u4Idx = 0; u4Idx < STC_SRC_NS; u4Idx++)
        {
            if (ucType == AV_SYNC_TARGET_AUD)
            {
                if (au4StcAudTarget[u4Idx] == ucTargetId)
                {
                    au4StcAudTarget[u4Idx] = MAX_DEVICE_ID;
                    au4AudTarget2Stc[ucTargetId] = STC_SRC_NS;
                }
            }
            else
            {
                if (au4StcVidTarget[u4Idx] == ucTargetId)
                {
                    au4StcVidTarget[u4Idx] = MAX_DEVICE_ID;
                    au4VidTarget2Stc[ucTargetId] = STC_SRC_NS;
                }
            }
        }

        return STC_VALID;
    }
    else
    {
        UINT32 u4Idx;

        if (ucType == AV_SYNC_TARGET_AUD)
        {
            for (u4Idx = 0; u4Idx < MAX_DEVICE_ID; u4Idx++)
            {
                if (au4AudTarget2Stc[u4Idx] == ucStcId)
                {
                    au4AudTarget2Stc[u4Idx] = STC_SRC_NS;
                }
            }
        }
        else
        {
            for (u4Idx = 0; u4Idx < MAX_DEVICE_ID; u4Idx++)
            {
                if (au4VidTarget2Stc[u4Idx] == ucStcId)
                {
                    au4VidTarget2Stc[u4Idx] = STC_SRC_NS;
                }
            }
        }
    }

    if (ucType == AV_SYNC_TARGET_AUD)
    {
        au4StcAudTarget[ucStcId] = ucTargetId;
        au4AudTarget2Stc[ucTargetId] = ucStcId;

#ifdef CC_FAST_CHANGE_CHANNEL
        au4AudFifo[ucStcId] = 0xFFFFFFFF;
        au4AudPkt[ucStcId] = 0;
        au4AudFifoRdy[ucStcId] = 0;
#endif
    }
    else
    {
        au4StcVidTarget[ucStcId] = ucTargetId;
        au4StcStarted[ucStcId] = 0;
        au4VidTarget2Stc[ucTargetId] = ucStcId;

#ifdef CC_FAST_CHANGE_CHANNEL
        au4VdoFifo[ucStcId] = 0xFFFFFFFF;
        au4VdoPkt[ucStcId] = 0;
        au4VdoFifoRdy[ucStcId] = 0;

        au4StartPts[ucStcId] = 0;
#endif
    }

    return STC_VALID;
}

//-------------------------------------------------------------------------
/** STC_GetSyncTarget
 */
//-------------------------------------------------------------------------
UCHAR STC_GetSyncTarget(UCHAR ucType, UCHAR ucStcId)
{
    if (ucType >= AV_SYNC_TARGET_NS)
    {
        return MAX_DEVICE_ID;
    }

    if (ucType == AV_SYNC_TARGET_AUD)
    {
        return (UCHAR)au4StcAudTarget[ucStcId];
    }
    else
    {
        return (UCHAR)au4StcVidTarget[ucStcId];
    }
}

//-------------------------------------------------------------------------
/** STC_SetStartPts
 */
//-------------------------------------------------------------------------
INT32 STC_SetStartPts(UCHAR ucType, UCHAR ucStcId, UINT32 u4Pts)
{
    LOG(3, "STC Start PTS (0x%x) (%d, %d)\n", u4Pts, ucType, ucStcId);

    if ((ucType != AV_SYNC_TARGET_VDO) ||
            (ucStcId >= STC_SRC_NS))
    {
        return STC_INVALID;
    }

    // Protection
    if (au4StcStarted[ucStcId] == 1)
    {
        LOG(7, "--------------------\n");
        LOG(7, "STC Start Twice!\n");
        LOG(7, "--------------------\n");
    }

    au4StcStarted[ucStcId] = 1;

#ifdef CC_FAST_CHANGE_CHANNEL
    au4StartPts[ucStcId] = u4Pts;

    // Auto update audio start PTS
    do
    {
        UINT32 u4Ahead;

        u4Ahead = STC_GetFastChangeChangeAhead(ucStcId);

        if (u4Pts > u4Ahead)
        {
            u4Pts -= u4Ahead;
        }
        else
        {
            u4Pts = (0xFFFFFFFF) - (u4Ahead - u4Pts);
        }
    } while(0);
#endif

    if (!AUD_SetStartPts((UCHAR)au4StcAudTarget[ucStcId], u4Pts))
    {
        return STC_INVALID;
    }

    return STC_VALID;
}

//-------------------------------------------------------------------------
/** STC_SetPtsDrift
 */
//-------------------------------------------------------------------------
INT32 STC_SetPtsDrift(UCHAR ucStcId, INT32 i4Drift)
{
    if (ucStcId >= STC_SRC_NS)
    {
        return STC_INVALID;
    }

    LOG(1,"decoder set pts drift = %d\n",_i4DecoderDrift);
    _i4DecoderDrift = i4Drift;

    //ai4StcDrift[ucStcId] = i4Drift;

    return STC_VALID;
}

//-------------------------------------------------------------------------
/** STC_GetPtsDrift
 */
//-------------------------------------------------------------------------
INT32 STC_GetPtsDrift(UCHAR ucStcId)
{

    if (ucStcId >= STC_SRC_NS)
    {
        return 3000;
    }

    return _i4DecoderDrift;

#if 0
    if (ucStcId >= STC_SRC_NS)
    {
        return 0;
    }

    //return ai4StcDrift[ucStcId];
#endif
}

//-------------------------------------------------------------------------
/** STC_GetPtsDriftByAudDeviceId
 */
//-------------------------------------------------------------------------
INT32 STC_GetPtsDriftByAudDeviceId(UCHAR ucDevId)
{
    if (ucDevId >= STC_SRC_NS)
    {
        return 3000;
    }

    return _i4DecoderDrift;

#if 0
    if (ucDevId >= MAX_DEVICE_ID)
    {
        LOG(1,"dmx STC_GetPtsDriftByAudDeviceId , ucDevId >= MAX_DEVICE_ID\n");
        return 0;
    }
    if(au4AudTarget2Stc[ucDevId] >= STC_SRC_NS)
    {
        LOG(1,"dmx STC_GetPtsDriftByAudDeviceId , au4AudTarget2Stc[ucDevId] >= STC_SRC_NS\n");
        return 0;
    }
#if 0
    ucStcId = au4AudTarget2Stc[ucDevId];

    LOG(1, "GPA D(%d) S(%d)\n", ucDevId, ucStcId);

    return STC_GetPtsDrift(ucStcId);
#else

    //return ai4StcDrift[au4AudTarget2Stc[ucDevId]];

#endif
#endif
}

//-------------------------------------------------------------------------
/** STC_GetPtsDriftByVdoDeviceId
 */
//-------------------------------------------------------------------------
INT32 STC_GetPtsDriftByVdoDeviceId(UCHAR ucDevId)
{
    if (ucDevId >= STC_SRC_NS)
    {
        return 3000;
    }

    return _i4DecoderDrift;

#if 0
    if (ucDevId >= MAX_DEVICE_ID)
    {
        LOG(1,"dmx STC_GetPtsDriftByVdoDeviceId, ucDevId >= MAX_DEVICE_ID\n");
        return 0;
    }
    if(au4AudTarget2Stc[ucDevId] >= STC_SRC_NS)
    {
        LOG(1,"dmx STC_GetPtsDriftByAudDeviceId , au4AudTarget2Stc[ucDevId] >= STC_SRC_NS\n");
        return 0;
    }
#if 0
    ucStcId = au4VidTarget2Stc[ucDevId];

    LOG(1, "GPV D(%d) S(%d)\n", ucDevId, ucStcId);

    return STC_GetPtsDrift(ucStcId);
#else

    //return ai4StcDrift[au4VidTarget2Stc[ucDevId]];

#endif
#endif
}

//-------------------------------------------------------------------------
/** STC_RxEsPkt
 */
//-------------------------------------------------------------------------
void STC_RxEsPkt(UCHAR ucType, UCHAR ucDevId, UINT32 u4Pts)
{
#ifdef CC_FAST_CHANGE_CHANNEL
    UCHAR ucStcId;
    STC_CLOCK rStc;
    UINT32 u4Delta;

    if (ucType == AV_SYNC_TARGET_AUD)
    {
        ucStcId = au4AudTarget2Stc[ucDevId];

        if (au4AudFifoRdy[ucStcId] == 0)
        {
            if (STC_GetSrc(ucStcId, &rStc) == STC_VALID)
            {
                u4Delta = u4Pts - rStc.u4Base;

                // wrap protection
                if (u4Delta > 0x80000000)
                {
                    u4Delta = rStc.u4Base - u4Pts;
                }

                if (u4Delta < au4AudFifo[ucStcId])
                {
                    au4AudFifo[ucStcId] = u4Delta;
                }

                au4AudPkt[ucStcId]++;

                if (au4AudPkt[ucStcId] >= AUD_MIN_ES_PKT_NS)
                {
                    au4AudFifoRdy[ucStcId] = 1;

                    if (au4VdoFifoRdy[ucStcId] && au4StcStarted[ucStcId])
                    {
                        VERIFY(STC_SetStartPts(AV_SYNC_TARGET_VDO, ucStcId, au4StartPts[ucStcId]) == STC_VALID);
                    }
                }
            }
        }
    }
    else
    {
        ucStcId = au4VidTarget2Stc[ucDevId];

        if (au4VdoFifoRdy[ucStcId] == 0)
        {
            if (STC_GetSrc(ucStcId, &rStc) == STC_VALID)
            {
                u4Delta = u4Pts - rStc.u4Base;

                if (u4Delta < au4VdoFifo[ucStcId])
                {
                    au4VdoFifo[ucStcId] = u4Delta;
                }

                au4VdoPkt[ucStcId]++;

                if (au4VdoPkt[ucStcId] >= VDO_MIN_ES_PKT_NS)
                {
                    au4VdoFifoRdy[ucStcId] = 1;

                    if (au4AudFifoRdy[ucStcId] && au4StcStarted[ucStcId])
                    {
                        VERIFY(STC_SetStartPts(AV_SYNC_TARGET_VDO, ucStcId, au4StartPts[ucStcId]) == STC_VALID);
                    }
                }
            }
        }
    }

#else
    UNUSED(ucType);
    UNUSED(ucDevId);
    UNUSED(u4Pts);
#endif
}

//-------------------------------------------------------------------------
/** STC_GetFastChangeChangeAhead
 */
//-------------------------------------------------------------------------
UINT32 STC_GetFastChangeChangeAhead(UCHAR ucStcId)
{
#ifdef CC_FAST_CHANGE_CHANNEL
    if (ucStcId >= STC_SRC_NS)
    {
        return 0;
    }

    if (au4VdoFifoRdy[ucStcId] && au4AudFifoRdy[ucStcId])
    {
        UINT32 u4Ahead;

        u4Ahead = (au4VdoFifo[ucStcId] * CC_FAST_CHANGE_CHANNEL) / 100;

        if (u4Ahead > au4AudFifo[ucStcId])
        {
            return u4Ahead;
        }
    }

    return 0;
#else
    UNUSED(ucStcId);
    return 0;
#endif
}


void STC_SetSTCSyncMode(UINT8 ucStcId,UINT32 u4SyncMode)
{
    LOG(3, "STC Set Stc Sync Mode: StcId(%d) SyncMode(%d)\n", ucStcId, u4SyncMode);
    
    if(ucStcId < STC_SRC_NS)
    {
        _au4MmAvSyncMode[ucStcId] = u4SyncMode;
    }
}


//-------------------------------------------------------------------------
/** STC_SetStcValue
 */
//-------------------------------------------------------------------------
#ifdef CC_53XX_SWDMX_V2
void STC_SetStcValue(UINT8 ucStcId, UINT32 u4Stc)
{
#else
void STC_SetStcValue(UINT32 u4Stc)
{
    UCHAR ucStcId = STC_SRC_A1;
#endif
    STC_VERIFY_NULL(ucStcId);

    if(ucStcId == STC_SRC_A1)
    {
#ifndef	CC_MT5391_AUD_SUPPORT
        PARSER_WRITE32(REG_A_STC_1, u4Stc);
#else
        PARSER_2_WRITE32(REG_A_STC_1, u4Stc);//modify by ling
#endif
    }
#ifdef CC_53XX_SWDMX_V2
    else
    {
        if(ucStcId < STC_SRC_NS)
        {
#ifndef CC_SUPPORT_2STC
            _u4StcSource[ucStcId] = u4Stc;
#else
            //lifang wang said that risc can set value to dmx hw stc
            //DMXCMD_WRITE32(DMX_REG_STC_BASE,u4Stc); 
            //DMXCMD_WRITE32(DMX_REG_STC_EXTENSION,0);
            DMXCMD_WRITE32(DMX_REG_STC2_BASE,u4Stc); 
            DMXCMD_WRITE32(DMX_REG_STC2_EXTENSION,0);
#endif
        }
    }
#endif
}


//-------------------------------------------------------------------------
/** STC_GetStcValue
 */
//-------------------------------------------------------------------------
#ifdef CC_53XX_SWDMX_V2
UINT32 STC_GetStcValue(UINT8 ucStcId)
{
#else
UINT32 STC_GetStcValue()
{
    UCHAR ucStcId = STC_SRC_A1;
#endif
    STC_VERIFY_NULL(ucStcId);

    if(ucStcId == STC_SRC_A1)
    {
#ifndef	CC_MT5391_AUD_SUPPORT
        return PARSER_READ32(REG_A_STC_1);
#else
        return PARSER_2_READ32(REG_A_STC_1);//modify by ling
#endif
    }
#ifdef CC_53XX_SWDMX_V2
    else
    {
        if(ucStcId < STC_SRC_NS)
        {
#ifndef CC_SUPPORT_2STC
            return _u4StcSource[ucStcId];
#else
            DMX_PCR rStc;
            //rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC_EXTENSION);
            //rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC_BASE);
            rStc.uPcrExt.u4Reg = DMXCMD_READ32(DMX_REG_STC2_EXTENSION);
            rStc.u4PcrBase = DMXCMD_READ32(DMX_REG_STC2_BASE);
            return (rStc.u4PcrBase);
#endif
        }
    }
#endif
    return 0;
}


//-------------------------------------------------------------------------
/** STC_StartStc
 */
//-------------------------------------------------------------------------
#ifdef CC_53XX_SWDMX_V2
void STC_StartStc(UINT8 ucStcId)
{
#else
void STC_StartStc()
{
    UCHAR ucStcId = STC_SRC_A1;
#endif
    // klocwork
    if(ucStcId >= STC_SRC_NS)
    {
        return;
    }

    LOG(3, "STC Start Stc: StcId(%d), MMAvSyncMode(%d)\n", ucStcId, _au4MmAvSyncMode[ucStcId]);

    if (_au4MmAvSyncMode[ucStcId] != AV_SYNC_MODE_VIDEO_MASTER)
    {        
        if(ucStcId == STC_SRC_A1)
        {
#ifndef	CC_MT5391_AUD_SUPPORT
            PARSER_WRITE32(REG_STC_CTRL, 0);
#else
            PARSER_2_WRITE32(REG_STC_CTRL, 0); //modify by ling
#endif
        }
    }
    else
    {        
        if(ucStcId == STC_SRC_A1)
        {
#ifndef	CC_MT5391_AUD_SUPPORT
            PARSER_WRITE32(REG_STC_CTRL, 0x2);
#else
            PARSER_2_WRITE32(REG_STC_CTRL, 0x2); //modify by ling
#endif
        }
    }

    _afgStcStart[ucStcId] = TRUE;
}


//-------------------------------------------------------------------------
/** STC_StopStc
 */
//-------------------------------------------------------------------------
#ifdef CC_53XX_SWDMX_V2
void STC_StopStc(UINT8 ucStcId)
{
#else
void STC_StopStc()
{
    UCHAR ucStcId = STC_SRC_A1;
#endif
    STC_VERIFY_NULL(ucStcId);

    LOG(3, "STC Stop Stc: StcId(%d), MMAvSyncMode(%d)\n", ucStcId, _au4MmAvSyncMode[ucStcId]);

    if (_au4MmAvSyncMode[ucStcId] != AV_SYNC_MODE_VIDEO_MASTER)
    {
        if(ucStcId == STC_SRC_A1)
        {
#ifndef	CC_MT5391_AUD_SUPPORT
            PARSER_WRITE32(REG_STC_CTRL, 0x2);
#else
            PARSER_2_WRITE32(REG_STC_CTRL, 0x2); //modify by ling
#endif
        }
    }

    _afgStcStart[ucStcId] = FALSE;
}

#ifdef CC_53XX_SWDMX_V2
BOOL STC_IsStartedStc(UCHAR ucStcId)
{
#else
BOOL STC_IsStartedStc()
{
    UCHAR ucStcId = STC_SRC_A1;
#endif
    return _afgStcStart[ucStcId];
}

UINT32 STC_GetSyncMode(UINT8 ucStcId)
{
    return _au4MmAvSyncMode[ucStcId];
}

#ifdef CC_53XX_SWDMX_V2
void STC_SetSyncMode(UINT8 ucStcId, UINT8 ucAdecId, UINT8 ucVdecId, UINT32 u4SyncMode)
{
#else
void STC_SetSyncMode(UINT32 u4SyncMode)
{
    UCHAR ucStcId = STC_SRC_A1;
    UCHAR ucAdecId = AUD_DEC_MAIN;
    UCHAR ucVdecId = ES0;
#endif
    //STC_VERIFY_NULL(ucStcId);
    // Klocwork

    LOG(3, "STC Set sync mode: StcId(%d), AdecId(%d), VdecId(%d), SyncMode(%d)\n", ucStcId, ucAdecId, ucVdecId, u4SyncMode);

    if(ucStcId >= STC_SRC_NS)
    {
        return;
    }

    _au4MmAvSyncMode[ucStcId] = u4SyncMode;

    switch (u4SyncMode)
    {
    case AV_SYNC_MODE_AUDIO_MASTER:
        AUD_SetAvSynMode(ucAdecId, AV_SYNC_AUDIO_MASTER);
        if(ucVdecId < VDEC_MAX_ES)
        {
            VDEC_SyncStc(ucVdecId, VID_SYNC_MODE_SLAVE_MM, ucStcId);
        }
        else
        {
            // video not exist
        }
        if (_afgStcStart[ucStcId])
        {
#ifdef CC_53XX_SWDMX_V2
            STC_StartStc(ucStcId);
#else
            STC_StartStc();
#endif
        }
        break;

    case AV_SYNC_MODE_VIDEO_MASTER:
        AUD_SetAvSynMode(ucAdecId, AV_SYNC_SLAVE);
        if(ucVdecId < VDEC_MAX_ES)
        {
            VDEC_SyncStc(ucVdecId, VID_SYNC_MODE_MASTER, ucStcId);
        }
        else
        {
            // video not exist
        }
        if (_afgStcStart[ucStcId])
        {
#ifdef CC_53XX_SWDMX_V2
            STC_StartStc(ucStcId);
#else
            STC_StartStc();
#endif
        }
        break;

    case AV_SYNC_MODE_SYSTEM_MASTER:
        AUD_SetAvSynMode(ucAdecId, AV_SYNC_SLAVE);
        if(ucVdecId < VDEC_MAX_ES)
        {
            VDEC_SyncStc(ucVdecId, VID_SYNC_MODE_SLAVE, ucStcId);
        }
        else
        {
            // video not exist
        }
        if (_afgStcStart[ucStcId])
        {
#ifdef CC_53XX_SWDMX_V2
            STC_StartStc(ucStcId);
#else
            STC_StartStc();
#endif
        }
        break;

    case AV_SYNC_MODE_NONE:
    default:
#ifdef CC_53XX_SWDMX_V2
        STC_StopStc(ucStcId);
#else
        STC_StopStc();
#endif
        AUD_SetAvSynMode(ucAdecId, AV_SYNC_FREE_RUN);
        if(ucVdecId < VDEC_MAX_ES)
        {
            VDEC_SyncStc(ucVdecId, VID_SYNC_MODE_NONE, ucStcId);
        }
        else
        {
            // video not exist
        }
        break;
    }
}

extern BOOL STC_ChkStcStatusDrift(UINT8 u1StcId)
{
    UINT32 u4StcStatus = DMX_STC_STATUS_UNKNOWN;

    if(u1StcId < STC_SRC_NS)
    {
        u4StcStatus = au4StcStatus[u1StcId];
    }
    else
    {
        u4StcStatus = DMX_STC_STATUS_INVALID;
    }

    if (u4StcStatus == DMX_STC_STATUS_DRIFT)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

extern BOOL STC_ChkStcStatusValid(UINT8 u1StcId)
{
    UINT32 u4StcStatus = DMX_STC_STATUS_UNKNOWN;

    if(u1StcId < STC_SRC_NS)
    {
        u4StcStatus = au4StcStatus[u1StcId];
    }
    else
    {
        u4StcStatus = DMX_STC_STATUS_INVALID;
    }

    if (u4StcStatus != DMX_STC_STATUS_VALID)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#if defined(CC_53XX_SWDMX_V2)
VOID STC_SoftwareStcISR(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    /*UINT8 u1Idx =0;
    for(u1Idx=STC_SRC_A2 ; u1Idx< STC_SRC_NS ; u1Idx++)
    {
        if(_afgStcStart[u1Idx])
        {
            UINT32 u4Tmp = STC_GetStcValue(u1Idx);
            STC_SetStcValue(u1Idx,u4Tmp+3000);
        }
    }*/
}
#endif


