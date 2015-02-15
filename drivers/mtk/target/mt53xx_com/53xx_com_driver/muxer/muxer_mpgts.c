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
 * $RCSfile: muxer_mpgts.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_mpgts.c
 *  Transport stream multiplexer driver - implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "muxer_if.h"
#include "muxer_drvif.h"
#include "muxer_util.h"
#include "muxer_mpgts.h"
#include "muxer_debug.h"
#ifdef CC_SUPPORT_VENC
#include "venc_if.h"
#endif
#include "x_mux.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//#define TS_PCM_SUPPORT


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#if 1
UINT32 u4MxPcrPreload = 18000;
UINT32 u4MxPcrThrd = 3600;
#define MUXER_TS_PCR_PRELOAD u4MxPcrPreload
#define MUXER_TS_PCR_THRD u4MxPcrThrd
#else
#define MUXER_TS_PCR_PRELOAD 18000 // 200ms
#define MUXER_TS_PCR_THRD 3600 // 40ms, PCR should appear within 40ms~100ms
#endif

#define MUXER_TS_ID (0x0001 & 0xFFFF)
#define MUXER_TS_PROGRAM_NUMBER (0x0001 & 0xFFFF)

#define MUXER_TS_NETWORK_PID  MUX_TS_NETWORK_PID
#define MUXER_TS_PMT_PID      MUX_TS_PMT_PID
#define MUXER_TS_PCR_PID      MUX_TS_PCR_PID
#define MUXER_TS_V_PID        MUX_TS_V_PID
#define MUXER_TS_A_PID        MUX_TS_A_PID
#define MUXER_TS_TT_PID       MUX_TS_TT_PID

#define USE_STC_AS_TIMESTAMP

#define MX_TS_SIZE (188)
#define MX_TS_1ST_PES_SZ (MX_TS_SIZE - 18)
#define MX_TS_NON_1ST_PES_SZ (MX_TS_SIZE - 4)
#define MX_TS_NON_1ST_PSI_SZ (MX_TS_SIZE - 5)

#ifdef MUXER_VID_HDR_BUF_SIZE
#if MUXER_VID_HDR_BUF_SIZE > MX_TS_1ST_PES_SZ
#error "MUXER_VID_HDR_BUF_SIZE should not larger than MX_TS_1ST_PES_SZ"
#endif
#endif


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define NET_ACC_CONTINUITY    (prMpgMuxInfo->u1NetContinuity++ & 0x0F)
#define PAT_ACC_CONTINUITY    (prMpgMuxInfo->u1PatContinuity++ & 0x0F)
#define PMT_ACC_CONTINUITY    (prMpgMuxInfo->u1PmtContinuity++ & 0x0F)
#define VID_ACC_CONTINUITY    (prMpgMuxInfo->u1VidContinuity++ & 0x0F)
#define AUD_ACC_CONTINUITY    (prMpgMuxInfo->u1AudContinuity++ & 0x0F)
#define TT_ACC_CONTINUITY    (prMpgMuxInfo->u1TtxContinuity++ & 0x0F)


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

extern UINT32 _AUD_ReadPsrStc1(void);
extern VOID MxFlushRing(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp);
extern VOID MxScramble(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp);


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

static VOID vMxPutPSIWithTT(MUXER_INFO_T *prMuxInfo);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


static VOID vMxPutPts(MUXER_INFO_T *prMuxInfo, UINT32 u4Pts)
{
    UINT8 u1Tmp;
    UINT16 u2Tmp;

    u1Tmp = (UINT8)(u4Pts >> 30);
    u1Tmp <<= 1;
    u1Tmp |= 0x21;
    vMxPutBits(prMuxInfo, u1Tmp, 8);
    /* 4b: '0010' */
    /* 3b: PTS[32..30] */
    /* 1b: marker_bit: 1 */

    u2Tmp = (UINT16)((u4Pts & 0x3FFF8000) >> 15);
    u2Tmp <<= 1;
    u2Tmp |= 1;
    vMxPutBits(prMuxInfo, u2Tmp, 16);
    /* 15b: PTS[29..15] */
    /* 1b: marker_bit: 1 */

    u2Tmp = (UINT16)(u4Pts & 0x7FFF);
    u2Tmp <<= 1;
    u2Tmp |= 1;
    vMxPutBits(prMuxInfo, u2Tmp, 16);
    /* 15b: PTS[14..0] */
    /* 1b: marker_bit: 1 */
}


// calculate crc considering input ring case
static UINT32 u4MxCalCrc(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp)
{
    UINT32 u4Crc;

    if (u4Wp > u4Rp)
    {
        u4Crc = u4CRC(prMuxInfo->pucMxBuf + u4Rp, u4Wp - u4Rp);
    }
    else if (u4Wp < u4Rp)
    {
        UINT8 au1Tmp[183];
        UINT32 u4Size1 = prMuxInfo->u4MxBufSz - u4Rp;

        x_memcpy(au1Tmp, prMuxInfo->pucMxBuf + u4Rp, u4Size1);
        x_memcpy(au1Tmp + u4Size1, prMuxInfo->pucMxBuf, u4Wp);
        u4Crc = u4CRC(au1Tmp, u4Size1 + u4Wp);
    }
    else
    {
        LOG(0, "%s: u4Wp == u4Rp(0x%08x)\n", __FUNCTION__, u4Rp);
        ASSERT(0);
        return 0xffffffff;
    }

    return u4Crc;
}


static VOID MxAddStuff(MUXER_INFO_T *prMuxInfo, UINT32 u4AddSize)
{
    while (u4AddSize >= 4)
    {
        vMxPutBits(prMuxInfo, 0xFFFFFFFF, 32);
        u4AddSize -= 4;
    }

    while (u4AddSize)
    {
        vMxPutBits(prMuxInfo, 0xFF, 8);
        u4AddSize--;
    }
}


static VOID MxAddPSIStuff(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp)
{
    UINT32 u4AddSize = 0;

    if (u4Wp > u4Rp)
    {
        u4AddSize = 188 - (u4Wp - u4Rp);
    }
    else if (u4Wp < u4Rp)
    {
        u4AddSize = 188 - (prMuxInfo->u4MxBufSz - u4Rp + u4Wp);
    }

    MxAddStuff(prMuxInfo, u4AddSize);
}


//#define USE_STC_AS_TIMESTAMP
static VOID vMxTsIns192Hdr(MUXER_INFO_T *prMuxInfo)
{
    if (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
    {
#ifndef USE_STC_AS_TIMESTAMP
        HAL_RAW_TIME_T rRawTime;
        HAL_GetRawTime(&rRawTime);
        vMxPutBits(prMuxInfo, rRawTime.u4Cycles, 32); // 192 timestamp
#else
        MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
        UINT32 u4Timestamp = prMpgMuxInfo->u4Timestamp;
        prMpgMuxInfo->u4Timestamp++;
        vMxPutBits(prMuxInfo, u4Timestamp, 32); // 192 timestamp
#endif
    }
}


static VOID vMxTsChkPcr(MUXER_INFO_T *prMuxInfo)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UINT32 u4Tmp, u4Pts;

    u4Pts = _AUD_ReadPsrStc1();
    if (prMpgMuxInfo->u4Pcr != 0 && (MUXER_TS_PCR_THRD + prMpgMuxInfo->u4Pcr) > u4Pts)
    {
        return;
    }

    vMxPutPSIWithTT(prMuxInfo);

    prMpgMuxInfo->u4Pcr = u4Pts;
#ifdef USE_STC_AS_TIMESTAMP
    if (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
    {
        prMpgMuxInfo->u4Timestamp = u4Pts * 300;
    }
#endif
    u4Pts = (u4Pts > MUXER_TS_PCR_PRELOAD)? (u4Pts - MUXER_TS_PCR_PRELOAD) : 0;

    if (!MxWaitBufferAvail(prMuxInfo, 192))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return;
    }

    vMxTsIns192Hdr(prMuxInfo);
    if(prMuxInfo->u2PcrPid != 0)
    {
        u4Tmp = 0x47000020 | ((prMuxInfo->u2PcrPid & 0x1FFF) << 8);
    }
    else
    {
        u4Tmp = 0x47000020 | (MUXER_TS_PCR_PID << 8);
    }
    vMxPutBits(prMuxInfo, u4Tmp, 32);
    /* 8b: sync_byte: 0x47 */
    /* 1b: transport_error_indicator: 0 */
    /* 1b: payload_unit_start_indicator: 0 */
    /* 1b: transport_priority: 0 */
    /* 13b: PID: MUXER_TS_PCR_PID */
    /* 2b: transport_scrambling_control: 0 */
    /* 2b: adaptation_field_control: 2 (adaptation_field only) */
    /* 4b: continuity_counter: 0 (should not incremented when adaptation_field_control is 0 or 2) */

    vMxPutBits(prMuxInfo, 183, 8);
    /* 8b: adaptation_field_length: 183 */

    vMxPutBits(prMuxInfo, 0x90, 8);
    /* 1b: discontinuity_indicator: 1 */
    /* 1b: random_access_indicator: 0 */
    /* 1b: elementary_stream_priority_indicator: 0 */
    /* 1b: PCR_flag: 1 */
    /* 1b: OPCR_flag: 0 */
    /* 1b: splicing_point_flag: 0 */
    /* 1b: transport_private_data_flag: 0 */
    /* 1b: adaptation_field_extension_flag: 0 */

    u4Tmp = (u4Pts & 0xFFFFFFFE) >> 1;
    vMxPutBits(prMuxInfo, u4Tmp, 32);
    /* 32b: program_clock_reference_base[33..1] */

    u4Tmp = ((u4Pts & 1) << 15) | 0x7E00;
    vMxPutBits(prMuxInfo, u4Tmp, 16);
    /* 1b: program_clock_reference_base[0] */
    /* 6b: reserved: 0x3F */
    /* 9b: program_clock_reference_extension: 0 */

    MxAddStuff(prMuxInfo, 176);
}


/* H.222.0 (2006) Amendment 2
Stream type assignments
Value    Description
0x00     ITU-T | ISO/IEC Reserved
0x01     ISO/IEC 11172-2 Video
0x02     ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
0x03     ISO/IEC 11172-3 Audio
0x04     ISO/IEC 13818-3 Audio
0x05     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
0x06     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
0x07     ISO/IEC 13522 MHEG
0x08     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC
0x09     ITU-T Rec. H.222.1
0x0A     ISO/IEC 13818-6 type A
0x0B     ISO/IEC 13818-6 type B
0x0C     ISO/IEC 13818-6 type C
0x0D     ISO/IEC 13818-6 type D
0x0E     ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
0x0F     ISO/IEC 13818-7 Audio with ADTS transport syntax
0x10     ISO/IEC 14496-2 Visual
0x11     ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3/Amd.1
0x12     ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
0x13     ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC 14496_sections
0x14     ISO/IEC 13818-6 Synchronized Download Protocol
0x15     Metadata carried in PES packets
0x16     Metadata carried in metadata_sections
0x17     Metadata carried in ISO/IEC 13818-6 Data Carousel
0x18     Metadata carried in ISO/IEC 13818-6 Object Carousel
0x19     Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol
0x1A     IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP)
0x1B     AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video
0x1C     ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS
0x1D     ISO/IEC 14496-17 Text
0x1E     Auxiliary video stream as defined in ISO/IEC 23002-3
0x1F-0x7E ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
0x7F     IPMP stream
0x80-0xFF User Private
 */

// Program Specific Information with teletext
static VOID vMxPutPSIWithTT(MUXER_INFO_T *prMuxInfo)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UINT32 u4MxWpBak;//record for crc
    UINT32 u4MxWpBak2;//record for stuff
    UINT32 u4Crc, u4Tmp;
    UINT32 u4ProgramInfoLen = 0;
    UINT32 u4VidInfoLen = 0;
    UINT32 u4AudInfoLen = 0 + 3/* audio_stream_descriptor */;
    UINT32 u4VidLen = (prMuxInfo->fgEnVideo)? (5 + u4VidInfoLen) : 0;
    UINT32 u4AudLen = (prMuxInfo->fgEnAudio)? (5 + u4AudInfoLen) : 0;
    UINT32 u4TeletextLen = (prMuxInfo->fgEnTeletext)? 12 : 0;

    if (!MxWaitBufferAvail(prMuxInfo, 192 * 2))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return;
    }

    vMxTsIns192Hdr(prMuxInfo);
    u4MxWpBak2 = prMuxInfo->u4MxBufWp;
    // PAT section: 2.4.4.3
    vMxPutBits(prMuxInfo, 0x47400010 | PAT_ACC_CONTINUITY, 32);
    /* 8b: sync_byte: 0x47 */
    /* 1b: transport_error_indicator: 0 */
    /* 1b: payload_unit_start_indicator: 1 */
    /* 1b: transport_priority: 0 */
    /* 13b: PID: 0 */
    /* 2b: transport_scrambling_control: 0 */
    /* 2b: adaptation_field_control: 1 (payload only) */
    /* 4b: continuity_counter: PAT_ACC_CONTINUITY */
    vMxPutBits(prMuxInfo, 0x00, 8);
    /* 8b: pointer_field: 0 - 2.4.4.2 */
    u4MxWpBak = prMuxInfo->u4MxBufWp;
    vMxPutBits(prMuxInfo, 0x00, 8);
    /* 8b: table_id: 0 (PAT) - table 2-26 */
    vMxPutBits(prMuxInfo, 0xB, 4);
    /* 1b: section_syntax_indicator: 1 */
    /* 1b: '0' */
    /* 2b: reserved: 3 */
    vMxPutBits(prMuxInfo, 0x00D, 12);
    /* 12b: section_length: 0x11 */
    vMxPutBits(prMuxInfo, MUXER_TS_ID, 16);
    /* 16b: transport_stream_id: MUXER_TS_ID */
    vMxPutBits(prMuxInfo, 0xC1, 8);
    /* 2b: reserved: 3 */
    /* 5b: version_number: 0 */
    /* 1b: current_next_indicator: 1 */
    vMxPutBits(prMuxInfo, 0x0000, 16);
    /* 8b: section_number: 0 */
    /* 8b: last_section_number: 0 */
    //vMxPutBits(prMuxInfo, 0x0000E000 | MUXER_TS_NETWORK_PID, 32);
    /* 16b: program_number: 0 */
    /* 3b: reserved: 7 */
    /* 13b: network_PID: MUXER_TS_NETWORK_PID */
    u4Tmp = (MUXER_TS_PROGRAM_NUMBER << 16) | 0xE000 | MUXER_TS_PMT_PID;
    vMxPutBits(prMuxInfo, u4Tmp, 32);
    /* 16b: program_number: MUXER_TS_PROGRAM_NUMBER */
    /* 3b: reserved: 7 */
    /* 13b: program_map_PID: MUXER_TS_PMT_PID */

    u4Crc = u4MxCalCrc(prMuxInfo, u4MxWpBak, prMuxInfo->u4MxBufWp);
    vMxPutBits(prMuxInfo, u4Crc, 32);

    MxAddPSIStuff(prMuxInfo, u4MxWpBak2, prMuxInfo->u4MxBufWp);

    vMxTsIns192Hdr(prMuxInfo);
    u4MxWpBak2 = prMuxInfo->u4MxBufWp;
    // PMT section: 2.4.4.8
    u4Tmp = 0x47400010 | (MUXER_TS_PMT_PID << 8) | PMT_ACC_CONTINUITY;
    vMxPutBits(prMuxInfo, u4Tmp, 32);
    /* 8b: sync_byte: 0x47 */
    /* 1b: transport_error_indicator: 0 */
    /* 1b: payload_unit_start_indicator: 1 */
    /* 1b: transport_priority: 0 */
    /* 13b: PID: MUXER_TS_PMT_PID */
    /* 2b: transport_scrambling_control: 0 */
    /* 2b: adaptation_field_control: 1 (payload only) */
    /* 4b: continuity_counter: PMT_ACC_CONTINUITY */
    vMxPutBits(prMuxInfo, 0x00, 8);
    /* 8b: pointer_field: 0 - 2.4.4.2 */
    u4MxWpBak = prMuxInfo->u4MxBufWp;
    vMxPutBits(prMuxInfo, 0x02, 8);
    /* 8b: table_id: 2 (PMT) - table 2-26 */
    vMxPutBits(prMuxInfo, 0xB, 4);
    /* 1b: section_syntax_indicator: 1 */
    /* 1b: '0' */
    /* 2b: reserved: 3 */
    u4Tmp = 13 + u4ProgramInfoLen + u4VidLen + u4AudLen + u4TeletextLen;
    vMxPutBits(prMuxInfo, u4Tmp, 12);
    /* 12b: section_length: 13 + u4VidInfoLen + u4AudInfoLen + u4TeletextLen */
    vMxPutBits(prMuxInfo, MUXER_TS_PROGRAM_NUMBER, 16);
    /* 16b: program_number: MUXER_TS_PROGRAM_NUMBER */
    vMxPutBits(prMuxInfo, 0xC1, 8);
    /* 2b: reserved: 3 */
    /* 5b: version_number: 0 */
    /* 1b: current_next_indicator: 1 */
    vMxPutBits(prMuxInfo, 0x0000, 16);
    /* 8b: section_number: 0 */
    /* 8b: last_section_number: 0 */
    if(prMuxInfo->u2PcrPid != 0)
    {        
        vMxPutBits(prMuxInfo, 0xE000 | (prMuxInfo->u2PcrPid & 0x1FFF), 16);
    }
    else
    {
        vMxPutBits(prMuxInfo, 0xE000 | MUXER_TS_PCR_PID, 16);
    }
    /* 3b: reserved: 7 */
    /* 13b: PCR_PID: MUXER_TS_PCR_PID */
    vMxPutBits(prMuxInfo, 0xF000 | (u4ProgramInfoLen & 0x3FF), 16);
    /* 4b: reserved: 0xF */
    /* 12b: program_info_length: u4ProgramInfoLen - 1st 2 bit should be '00' */
#if 0 // combine with u4ProgramInfoLen
    vMxPutBits(prMuxInfo, 0x0504, 16);
    /* 8b: descriptor_tag: 0x5 (registration_descriptor) - table 2-39 */
    /* 8b: descriptor_length: 4 */
    vMxPutBits(prMuxInfo, 0x48444D56, 32);
    /* 32b: format_identifier: HDMV */
    vMxPutBits(prMuxInfo, 0x8804, 16);
    /* 8b: descriptor_tag: 0x88 (User Private) - table 2-39 */
    /* 8b: descriptor_length: 4 */
    vMxPutBits(prMuxInfo, 0x0FFFFCFC, 32);
    /* 32b: format_identifier: 0x0FFFFCFC */
#endif

    if (prMuxInfo->fgEnVideo)
    {
        // video
        vMxPutBits(prMuxInfo, 0x1B, 8);
        /* 8b: stream_type: 0x1B (H.264) - table 2-29 */
        if(prMuxInfo->u2VideoPid != 0)
        {        
            vMxPutBits(prMuxInfo, 0xE000 | (prMuxInfo->u2VideoPid & 0x1FFF), 16);
        }
        else
        {
            vMxPutBits(prMuxInfo, 0xE000 | MUXER_TS_V_PID, 16);
        }
        /* 3b: reserved: 7 */
        /* 13b: elementary_PID: MUXER_TS_V_PID */
        vMxPutBits(prMuxInfo, 0xF000 | (u4VidInfoLen & 0x3FF), 16);
        /* 4b: reserved: 0xF */
        /* 12b: ES_info_length: u4VidInfoLen - 1st 2 bit should be '00' */
#if 0 // combine with u4VidInfoLen
        vMxPutBits(prMuxInfo, 0x5201090E, 32);
        /* 8b: descriptor_tag: 0x52 (User Private) - table 2-39 */
        /* 8b: descriptor_length: 1?? */
        vMxPutBits(prMuxInfo, 0x03C0C7FD, 32);
        vMxPutBits(prMuxInfo, 0x5006F10B, 32);
        vMxPutBits(prMuxInfo, 0x01676572, 32);
#endif
    }

    if (prMuxInfo->fgEnAudio)
    {
        // audio
        if (ENUM_MUXER_AENC_MP3 == prMuxInfo->eAudFmt)
        {
            u4Tmp = 3; /* stream_type: ISO/IEC 11172-3 Audio */
        }
        else if(ENUM_MUXER_AENC_AAC == prMuxInfo->eAudFmt)
        {
            u4Tmp = 0x0F; /* stream_type: ISO/IEC 11172-3 Audio */
        }
        else
        {
            u4Tmp = 0x80; /* stream_type: User Private */
        }
        vMxPutBits(prMuxInfo, u4Tmp, 8);
        /* 8b: stream_type - table 2-29 */
        if(prMuxInfo->u2AudioPid != 0)
        {                    
            vMxPutBits(prMuxInfo, 0xE000 | (prMuxInfo->u2AudioPid & 0x1FFF), 16);
        }
        else
        {
            vMxPutBits(prMuxInfo, 0xE000 | MUXER_TS_A_PID, 16);
        }
        /* 3b: reserved: 7 */
        /* 13b: elementary_PID: MUXER_TS_A_PID */
        vMxPutBits(prMuxInfo, 0xF000 | (u4AudInfoLen & 0x3FF), 16);
        /* 4b: reserved: 0xF */
        /* 12b: ES_info_length: u4AudInfoLen - 1st 2 bit should be '00' */
#if 1 // combine with u4AudInfoLen
        vMxPutBits(prMuxInfo, 0x030157, 24);
        /* 8b: descriptor_tag: 0x3 (audio_stream_descriptor) - table 2-39 */
        /* 8b: descriptor_length: 0x1 */
        /* 1b: free_format_flag: 0 */
        /* 1b: ID: 1 */
        /* 2b: layer: 1 (Layer III) */
        /* 1b: variable_rate_audio_indicator: 0 */
        /* 3b: reserved: 7 */
#endif
#if 0 // combine with u4AudInfoLen
        vMxPutBits(prMuxInfo, 0x0508, 16);
        /* 8b: descriptor_tag: 0x5 (registration_descriptor) - table 2-39 */
        /* 8b: descriptor_length: 0x8 */
        vMxPutBits(prMuxInfo, 0x48444D56, 32);
        /* 32b: format_identifier: HDMV */
        vMxPutBits(prMuxInfo, 0xFF80317F, 32);
        /* 8b * 4: additional_identification_info */
#endif
    }

    if (prMuxInfo->fgEnTeletext)
    {
        // teletext
        vMxPutBits(prMuxInfo, 0x06, 8);
        /* 8b: stream_type: 0x6 (PES packets containing private data) - table 2-29 */
        vMxPutBits(prMuxInfo, 0xE000 | MUXER_TS_TT_PID, 16);
        /* 3b: reserved: 7 */
        /* 13b: elementary_PID: MUXER_TS_TT_PID */
        vMxPutBits(prMuxInfo, 0xF007, 16);
        /* 4b: reserved: 0xF */
        /* 12b: ES_info_length: 7 */
        vMxPutBits(prMuxInfo, 0x56, 8);
        /* 8b: descriptor_tag: 0x56 (teletext_descriptor) - DVB BlueBook */
        vMxPutBits(prMuxInfo, 0x05, 8);
        /* 8b: descriptor_length: 5 */
        vMxPutBits(prMuxInfo, 0x667265, 24);
        /* 24b: ISO_639_language_code: 0x667265(fre) */
        vMxPutBits(prMuxInfo, 0x0900, 16);
        /* 3b: teletext_type: 0 */
        /* 5b: teletext_magazine_number: 9 */
        /* 8b: teletext_page_number: 0 */
    }

    u4Crc = u4MxCalCrc(prMuxInfo, u4MxWpBak, prMuxInfo->u4MxBufWp);
    vMxPutBits(prMuxInfo, u4Crc, 32);

    MxAddPSIStuff(prMuxInfo, u4MxWpBak2, prMuxInfo->u4MxBufWp);

#if 0 // Network Information Table
    vMxPutBits(prMuxInfo, 0x47400010 | (MUXER_TS_NETWORK_PID << 8) | NET_ACC_CONTINUITY, 32);
    vMxPutBits(prMuxInfo, 0x007FF019, 32);
    vMxPutBits(prMuxInfo, 0xFFFFC100, 32);
    vMxPutBits(prMuxInfo, 0x00F00A63, 32);
    vMxPutBits(prMuxInfo, 0x08C15AAE, 32);
    vMxPutBits(prMuxInfo, 0xFFFFFFFF, 32);
    vMxPutBits(prMuxInfo, 0xFF000180, 32);
    vMxPutBits(prMuxInfo, 0x00341EE7, 32);
    vMxPutBits(prMuxInfo, 0x4E, 8);
    MxAddStuff(prMuxInfo, 155);
#endif
}


#ifdef MUXER_VID_HDR_BUF_SIZE
static BOOL _MUXER_MpgTsHandleVideoHdr(
    MUXER_INFO_T *prMuxInfo,
    VENC_PES_INFO_T *prVidPesInfo)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UINT32 u4FrmSa = VIRTUAL(prVidPesInfo->u4StartAddr);
    UINT32 u4FrmSz = prVidPesInfo->u4FrameSize;
    UINT32 u4FifoSa = VIRTUAL(prVidPesInfo->u4FifoStart);
    UINT32 u4FifoEa = VIRTUAL(prVidPesInfo->u4FifoEnd);
    UINT32 u4VidHdrBufWp = prMpgMuxInfo->u4VidHdrBufWp;
    UCHAR *pucVidHdrBuf = prMpgMuxInfo->aucVidHdrBuf;

    if (u4VidHdrBufWp + u4FrmSz > MUXER_VID_HDR_BUF_SIZE)
    {
        LOG(0, "%s: %d + %d is larger than buffer size(%d)\n",
            __FUNCTION__, u4VidHdrBufWp, u4FrmSz, MUXER_VID_HDR_BUF_SIZE);
        ASSERT(0);
        return FALSE;
    }

    if (0 == u4FrmSz)
    {
        LOG(0, "%s: frame size is 0 !!!\n", __FUNCTION__);
        ASSERT(0);
        return TRUE;
    }

    if (u4FrmSa + u4FrmSz < u4FifoEa)
    {
        x_memcpy((VOID *)&pucVidHdrBuf[u4VidHdrBufWp], (VOID *)u4FrmSa, u4FrmSz);
        u4VidHdrBufWp += u4FrmSz;
    }
    else
    {
        UINT32 u4Size1 = u4FifoEa - u4FrmSa;
        UINT32 u4Size2 = u4FrmSz - u4Size1;
        x_memcpy((VOID *)&pucVidHdrBuf[u4VidHdrBufWp], (VOID *)u4FrmSa, u4Size1);
        u4VidHdrBufWp += u4Size1;
        x_memcpy((VOID *)&pucVidHdrBuf[u4VidHdrBufWp], (VOID *)u4FifoSa, u4Size2);
        u4VidHdrBufWp += u4Size2;
    }

    prMpgMuxInfo->u4VidHdrBufWp = u4VidHdrBufWp;
    prMpgMuxInfo->u4VidHdrBufSz += u4FrmSz;

    return TRUE;
}
#endif


static BOOL _MUXER_MpgTsHandleVideoPes(
    MUXER_INFO_T *prMuxInfo,
    VENC_PES_INFO_T *prVidPesInfo)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
#ifdef MUXER_VID_HDR_BUF_SIZE
    UINT32 u4VidHdrBufSz = prMpgMuxInfo->u4VidHdrBufSz;
    UINT32 u4FrmSz = prVidPesInfo->u4FrameSize + u4VidHdrBufSz;
#else
    UINT32 u4FrmSz = prVidPesInfo->u4FrameSize;
#endif
    UINT32 u4FrmSa = VIRTUAL(prVidPesInfo->u4StartAddr);
    UINT32 u4FifoSa = VIRTUAL(prVidPesInfo->u4FifoStart);
    UINT32 u4FifoEa = VIRTUAL(prVidPesInfo->u4FifoEnd);
    UINT32 u4UpdateSz, u4Tmp;
    UINT32 u4PesSz, u4PacketSz;

    if (u4FrmSz >= prMuxInfo->u4MxBufSz)
    {
        LOG(0, "%s: frame size(%d) is larger than buffer size(%d)\n",
            __FUNCTION__, u4FrmSz, prMuxInfo->u4MxBufSz);
        ASSERT(0);
        return FALSE;
    }

    if (0 == u4FrmSz)
    {
        LOG(0, "%s: frame size is 0 !!!\n", __FUNCTION__);
        ASSERT(0);
        return TRUE;
    }

    u4PacketSz = (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
        ? 192
        : 188;

    if (u4FrmSz <= MX_TS_1ST_PES_SZ)
    {
        u4PesSz = u4PacketSz;
    }
    else
    {
        u4Tmp = u4FrmSz - MX_TS_1ST_PES_SZ + MX_TS_NON_1ST_PES_SZ - 1;
        u4PesSz = (u4Tmp / MX_TS_NON_1ST_PES_SZ + 1) * u4PacketSz;
    }

    if (!MxWaitBufferAvail(prMuxInfo, u4PesSz))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return FALSE;
    }

    vMxTsIns192Hdr(prMuxInfo);
    // put the 1st TS PES
    if (u4FrmSz >= MX_TS_1ST_PES_SZ)
    {
        if(prMuxInfo->u2VideoPid != 0)
        {            
            u4Tmp = 0x47400010 | VID_ACC_CONTINUITY | ((prMuxInfo->u2VideoPid & 0x1FFF) << 8);
        }
        else
        {
            u4Tmp = 0x47400010 | VID_ACC_CONTINUITY | (MUXER_TS_V_PID << 8);
        }
        vMxPutBits(prMuxInfo, u4Tmp, 32);
        /* 8b: sync_byte: 0x47 */
        /* 1b: transport_error_indicator: 0 */
        /* 1b: payload_unit_start_indicator: 1 */
        /* 1b: transport_priority: 0 */
        /* 13b: PID: MUXER_TS_V_PID */
        /* 2b: transport_scrambling_control: 0 */
        /* 2b: adaptation_field_control: 1 (payload only) */
        /* 4b: continuity_counter: VID_ACC_CONTINUITY */
    }
    else
    {
        // adaption field with stuffing (2.4.3.5)
        UINT32 u4AdpLen = MX_TS_1ST_PES_SZ - u4FrmSz - 1;

        if(prMuxInfo->u2VideoPid != 0)
        {            
            u4Tmp = 0x47400030 | VID_ACC_CONTINUITY | ((prMuxInfo->u2VideoPid & 0x1FFF) << 8);
        }
        else
        {
            u4Tmp = 0x47400030 | VID_ACC_CONTINUITY | (MUXER_TS_V_PID << 8);
        }
        vMxPutBits(prMuxInfo, u4Tmp, 32);
        /* 8b: sync_byte: 0x47 */
        /* 1b: transport_error_indicator: 0 */
        /* 1b: payload_unit_start_indicator: 1 */
        /* 1b: transport_priority: 0 */
        /* 13b: PID: MUXER_TS_V_PID */
        /* 2b: transport_scrambling_control: 0 */
        /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
        /* 4b: continuity_counter: VID_ACC_CONTINUITY */

        vMxPutBits(prMuxInfo, u4AdpLen, 8);
        /* 8b: adaptation_field_length: u4AdpLen */
        if (u4AdpLen > 0)
        {
            vMxPutBits(prMuxInfo, 0x00, 8);
            /* 1b: discontinuity_indicator: 0 */
            /* 1b: random_access_indicator: 0 */
            /* 1b: elementary_stream_priority_indicator: 0 */
            /* 1b: PCR_flag: 0 */
            /* 1b: OPCR_flag: 0 */
            /* 1b: splicing_point_flag: 0 */
            /* 1b: transport_private_data_flag: 0 */
            /* 1b: adaptation_field_extension_flag: 0 */
            MxAddStuff(prMuxInfo, u4AdpLen - 1);
        }
        else
        {
            ASSERT(u4FrmSz == (MX_TS_1ST_PES_SZ - 1));
        }
    }
    // PES packet: 2.4.3.6
    vMxPutBits(prMuxInfo, 0x000001E0, 32);
    /* 24b: packet_start_code_prefix: 1 */
    /* 8b: stream_id: 0xE0 (video stream number 0) */
    vMxPutBits(prMuxInfo, 0x0000, 16);
    /* 16b: PES_packet_length */
    vMxPutBits(prMuxInfo, 0x808005, 24);
    /* 2b: '10' */
    /* 2b: PES_scrambling_control: 0 */
    /* 1b: PES_priority: 0 */
    /* 1b: data_alignment_indicator: 0 */
    /* 1b: copyright: 0 */
    /* 1b: original_or_copy: 0 */
    /* 2b: PTS_DTS_flags: 2 */
    /* 1b: ESCR_flag: 0 */
    /* 1b: ES_rate_flag: 0 */
    /* 1b: DSM_trick_mode_flag: 0 */
    /* 1b: additional_copy_info_flag: 0 */
    /* 1b: PES_CRC_flag: 0 */
    /* 1b: PES_extension_flag: 0 */
    /* 8b: PES_header_data_length: 5 */

    vMxPutPts(prMuxInfo, prVidPesInfo->u4Pts);

    // copy payload start
    u4UpdateSz = (u4FrmSz >= MX_TS_1ST_PES_SZ)? MX_TS_1ST_PES_SZ : u4FrmSz;
#ifdef MUXER_VID_HDR_BUF_SIZE
    if (u4VidHdrBufSz)
    {
        vMxCopyBytes(prMuxInfo, (UINT32)prMpgMuxInfo->aucVidHdrBuf, u4VidHdrBufSz);
        u4UpdateSz -= u4VidHdrBufSz;
        u4FrmSz -= u4VidHdrBufSz;
        prMpgMuxInfo->u4VidHdrBufWp = 0;
        prMpgMuxInfo->u4VidHdrBufSz = 0;
    }
#endif
    if (u4FrmSa + u4UpdateSz < u4FifoEa)
    {
        vMxCopyBytes(prMuxInfo, u4FrmSa, u4UpdateSz);
        u4FrmSa += u4UpdateSz;
    }
    else
    {
        UINT32 u4Size1 = u4FifoEa - u4FrmSa;
        UINT32 u4Size2 = u4UpdateSz - u4Size1;
        vMxCopyBytes(prMuxInfo, u4FrmSa, u4Size1);
        vMxCopyBytes(prMuxInfo, u4FifoSa, u4Size2);
        u4FrmSa = u4FifoSa + u4Size2;
    }
    u4FrmSz -= u4UpdateSz;
    // copy payload end

    // put the 2nd+ TS packets
    while (u4FrmSz)
    {
        vMxTsIns192Hdr(prMuxInfo);
        if (u4FrmSz >= MX_TS_NON_1ST_PES_SZ)
        {
            if(prMuxInfo->u2VideoPid != 0)
            {            
                u4Tmp = 0x47000010 | VID_ACC_CONTINUITY | ((prMuxInfo->u2VideoPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47000010 | VID_ACC_CONTINUITY | (MUXER_TS_V_PID << 8);
            }
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 0 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_V_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 1 (payload only) */
            /* 4b: continuity_counter: VID_ACC_CONTINUITY */
        }
        else
        {
            UINT32 u4AdpLen = MX_TS_NON_1ST_PES_SZ - u4FrmSz - 1;
            if(prMuxInfo->u2VideoPid != 0)
            {                
                u4Tmp = 0x47000030 | VID_ACC_CONTINUITY | ((prMuxInfo->u2VideoPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47000030 | VID_ACC_CONTINUITY | (MUXER_TS_V_PID << 8);
            }
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 0 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_V_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
            /* 4b: continuity_counter: VID_ACC_CONTINUITY */
            vMxPutBits(prMuxInfo, u4AdpLen, 8);
            /* 8b: adaptation_field_length: u4AdpLen */
            if (u4AdpLen > 0)
            {
                vMxPutBits(prMuxInfo, 0x00, 8);
                /* 1b: discontinuity_indicator: 0 */
                /* 1b: random_access_indicator: 0 */
                /* 1b: elementary_stream_priority_indicator: 0 */
                /* 1b: PCR_flag: 0 */
                /* 1b: OPCR_flag: 0 */
                /* 1b: splicing_point_flag: 0 */
                /* 1b: transport_private_data_flag: 0 */
                /* 1b: adaptation_field_extension_flag: 0 */
                MxAddStuff(prMuxInfo, u4AdpLen - 1);
            }
            else
            {
                ASSERT(u4FrmSz == (MX_TS_NON_1ST_PES_SZ - 1));
            }
        }

        // copy payload start
        u4UpdateSz = (u4FrmSz >= MX_TS_NON_1ST_PES_SZ)? MX_TS_NON_1ST_PES_SZ : u4FrmSz;
        if (u4FrmSa + u4UpdateSz < u4FifoEa)
        {
            vMxCopyBytes(prMuxInfo, u4FrmSa, u4UpdateSz);
            u4FrmSa += u4UpdateSz;
        }
        else
        {
            UINT32 u4Size1 = u4FifoEa - u4FrmSa;
            UINT32 u4Size2 = u4UpdateSz - u4Size1;
            vMxCopyBytes(prMuxInfo, u4FrmSa, u4Size1);
            vMxCopyBytes(prMuxInfo, u4FifoSa, u4Size2);
            u4FrmSa = u4FifoSa + u4Size2;
        }
        u4FrmSz -= u4UpdateSz;
        // copy payload end
    }

    return TRUE;
}


#ifdef TS_PCM_SUPPORT
static VOID _MUXER_TsMuxPCMEs(MUXER_INFO_T *prMuxInfo, UINT32 u4Sa, UINT32 u4Sz, UINT32 u4Pts)
{
    UCHAR *pucMxBuf = prMuxInfo->pucMxBuf;
    UINT32 i, u4Tmp;

    if (prMuxInfo->u4MxBufWp + u4Sz >= prMuxInfo->u4MxBufSz)
    {
        LOG(0, "[MX] buffer overflow!!!\n");
        ASSERT(0);
        return;
    }

    if (u4Sz > 0)
    {
        vMxTsIns192Hdr(prMuxInfo);
        // put the first TS packet
        if (u4Sz >= 188 - 30)
        {
            // adaption field with only PCR
            if(prMuxInfo->u2AudioPid != 0)
            {                
                u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
            }
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 1 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_A_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
            /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
            vMxPutBits(prMuxInfo, 0x07900000, 32);
            vMxPutBits(prMuxInfo, 0x01C6FF08, 32);
        }
        else
        {
            if(prMuxInfo->u2AudioPid != 0)
            {                
                u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
            }
            // adaption field with PCR + stuffing
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 1 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_A_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
            /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
            vMxPutBits(prMuxInfo, 166 - u4Sz - 1, 8);  // adaption_field_length
            vMxPutBits(prMuxInfo, 0x900000, 24);
            vMxPutBits(prMuxInfo, 0x01C6FF08, 32);
            ASSERT(u4Sz <= 157);
            for (i = 0; i < 159 - u4Sz - 1; i++)
            {
                vMxPutBits(prMuxInfo, 0xFF, 8);  // stuffing
            }
        }
        vMxPutBits(prMuxInfo, 0x000001BD, 32);  // PES packet start code for private stream 1
        vMxPutBits(prMuxInfo, 0x03CC, 16);      // PES packet length
        vMxPutBits(prMuxInfo, 0x848005, 24);    // flags and optional field length
        vMxPutPts(prMuxInfo, u4Pts);
        vMxPutBits(prMuxInfo, 0x03C0, 16);      // audio data payload size
        vMxPutBits(prMuxInfo, 0x31, 8);         // channel assignment and sampling frequency
        vMxPutBits(prMuxInfo, 0x40, 8);         // bits per sample and start flag and reserved
        if (u4Sz >= 188 - 30)
        {
            x_memcpy(pucMxBuf + prMuxInfo->u4MxBufWp, (UCHAR *)u4Sa, 188 - 30);
            prMuxInfo->u4MxBufWp += (188 - 30);
            u4Sz -= (188 - 30);
            u4Sa += (188 - 30);
        }
        else
        {
            x_memcpy(pucMxBuf + prMuxInfo->u4MxBufWp, (UCHAR *)u4Sa, u4Sz);
            prMuxInfo->u4MxBufWp += u4Sz;
            u4Sz = 0;
        }

        // put the 2nd+ TS packets
        while (u4Sz)
        {
            vMxTsIns192Hdr(prMuxInfo);
            if (u4Sz >= MX_TS_NON_1ST_PES_SZ)
            {
                if(prMuxInfo->u2AudioPid != 0)
                {                    
                    u4Tmp = 0x47000010 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
                }
                else
                {
                    u4Tmp = 0x47000010 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
                }
                vMxPutBits(prMuxInfo, u4Tmp, 32);
                /* 8b: sync_byte: 0x47 */
                /* 1b: transport_error_indicator: 0 */
                /* 1b: payload_unit_start_indicator: 0 */
                /* 1b: transport_priority: 0 */
                /* 13b: PID: MUXER_TS_A_PID */
                /* 2b: transport_scrambling_control: 0 */
                /* 2b: adaptation_field_control: 1 (payload only) */
                /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
            }
            else
            {
                UINT32 u4AdpLen = MX_TS_NON_1ST_PES_SZ - u4Sz - 1;
                if(prMuxInfo->u2AudioPid != 0)
                {                    
                    u4Tmp = 0x47000030 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
                }
                else
                {
                    u4Tmp = 0x47000030 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
                }
                vMxPutBits(prMuxInfo, u4Tmp, 32);
                /* 8b: sync_byte: 0x47 */
                /* 1b: transport_error_indicator: 0 */
                /* 1b: payload_unit_start_indicator: 0 */
                /* 1b: transport_priority: 0 */
                /* 13b: PID: MUXER_TS_A_PID */
                /* 2b: transport_scrambling_control: 0 */
                /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
                /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
                vMxPutBits(prMuxInfo, u4AdpLen, 8);
                /* 8b: adaptation_field_length: u4AdpLen */
                if (u4AdpLen > 0)
                {
                    vMxPutBits(prMuxInfo, 0x00, 8);
                    /* 1b: discontinuity_indicator: 0 */
                    /* 1b: random_access_indicator: 0 */
                    /* 1b: elementary_stream_priority_indicator: 0 */
                    /* 1b: PCR_flag: 0 */
                    /* 1b: OPCR_flag: 0 */
                    /* 1b: splicing_point_flag: 0 */
                    /* 1b: transport_private_data_flag: 0 */
                    /* 1b: adaptation_field_extension_flag: 0 */
                    MxAddStuff(prMuxInfo, u4AdpLen - 1);
                }
                else
                {
                    ASSERT(u4Sz == (MX_TS_NON_1ST_PES_SZ - 1));
                }
            }

            if (u4Sz >= 188 - 4)
            {
                vMxCopyBytes(prMuxInfo, u4Sa, 188 - 4);
                u4Sz -= (188 - 4);
                u4Sa += (188 - 4);
            }
            else
            {
                vMxCopyBytes(prMuxInfo, u4Sa, u4Sz);
                u4Sz = 0;
            }
        }
    }
}


static BOOL _MUXER_MpgTsHandlePCM(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    //UCHAR *pucTmpFrmBuf = prMuxInfo->pucTmpFrmBuf;
    UCHAR *pucAudResidueBuf = prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf;
    UINT32 u4FrmSa = VIRTUAL(prAudPesInfo->u4StartAddr);
    UINT32 u4FrmSz = prAudPesInfo->u4FrameSize;
    UINT32 u4FifoSa = prAudPesInfo->u4FifoStart;
    UINT32 u4FifoEa = prAudPesInfo->u4FifoEnd;
    UINT32 u4Consume;

    if (u4FifoSa + u4FifoEa == 0 || u4FrmSa + u4FrmSz <= u4FifoEa)
    {
        if (prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen > 0)
        {
            ASSERT(prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen < 960);
            if (prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen + u4FrmSz >= 960)
            {
                u4Consume = 960 - prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen;
                x_memcpy((VOID *)(pucAudResidueBuf + prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen), (VOID *)u4FrmSa, u4Consume);
                _MUXER_TsMuxPCMEs(prMuxInfo, (UINT32)pucAudResidueBuf, 960, prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResiduePts);
                u4FrmSz -= u4Consume;
                u4FrmSa += u4Consume;
                prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen = 0;
                while (u4FrmSz >= 960)
                {
                    _MUXER_TsMuxPCMEs(prMuxInfo, u4FrmSa, 960, prAudPesInfo->u4Pts);
                    u4FrmSz -= 960;
                    u4FrmSa += 960;
                }
                if (u4FrmSz > 0)  // copy the residue to tmp buffer
                {
                    x_memcpy((VOID *)pucAudResidueBuf, (VOID *)u4FrmSa, u4FrmSz);
                    prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen = u4FrmSz;
                    prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResiduePts = prAudPesInfo->u4Pts;
                }
            }
            else
            {
                x_memcpy((VOID *)(pucAudResidueBuf + prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen), (VOID *)u4FrmSa, u4FrmSz);
                prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen += u4FrmSz;
            }
        }
        else
        {
            while (u4FrmSz >= 960)
            {
                _MUXER_TsMuxPCMEs(prMuxInfo, u4FrmSa, 960, prAudPesInfo->u4Pts);
                u4FrmSz -= 960;
                u4FrmSa += 960;
            }
            if (u4FrmSz > 0)
            {
                x_memcpy((VOID *)pucAudResidueBuf, (VOID *)u4FrmSa, u4FrmSz);
                prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResidueLen = u4FrmSz;
                prMuxInfo->rFmtInfo.rMuxerMpgInfo.u4AudResiduePts = prAudPesInfo->u4Pts;
            }
        }
    }
    else
    {
        ASSERT(0);  // should never happen
#if 0
        x_memcpy((VOID *)pucTmpFrmBuf, (VOID *)u4FrmSa, u4FifoEa - u4FrmSa);
        x_memcpy((VOID *)(pucTmpFrmBuf + u4FifoEa - u4FrmSa), (VOID *)u4FifoSa, u4FrmSa + u4FrmSz - u4FifoEa);
        _MUXER_TsMuxPCMEs(prMuxInfo, (UINT32)pucTmpFrmBuf, u4FrmSz, prAudPesInfo->u4Pts);
#endif
    }

    return TRUE;
}
#endif

static BOOL _MUXER_MpgTsHandleSection(UINT32 u4Num, MUXER_INFO_T *prMuxInfo)
{   
    UINT32 u4Tmp;
	UINT32 u4StLen = prMuxInfo->arSection[u4Num].rAddSection.i4sectionlen;
    UINT32 u4StAdrr = (UINT32)prMuxInfo->arSection[u4Num].rAddSection.pu1sectiondata;
    UINT32 u2StPid = (UINT16)prMuxInfo->arSection[u4Num].rAddSection.u2Pid;
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    
    ASSERT((VOID*)u4StAdrr != NULL);
    LOG(2, "czg MUXER prMuxInfo->u4StLen is %d\n", u4StLen);

	// put the 1st TS EIT
	vMxTsIns192Hdr(prMuxInfo);
	if (u4StLen >= MX_TS_NON_1ST_PSI_SZ)
	{
		u4Tmp = 0x47400010 | (prMpgMuxInfo->u1SectContinuity[u4Num]++ & 0x0F) | 
		    ((u2StPid & 0x1FFF) << 8);
		vMxPutBits(prMuxInfo, u4Tmp, 32);
		vMxPutBits(prMuxInfo, 0x00, 8);
	
		vMxCopyBytes(prMuxInfo, u4StAdrr, MX_TS_NON_1ST_PSI_SZ);
		u4StAdrr += MX_TS_NON_1ST_PSI_SZ;
		u4StLen -= MX_TS_NON_1ST_PSI_SZ;
	}
	else
	{
		u4Tmp = 0x47400010 | (prMpgMuxInfo->u1SectContinuity[u4Num]++ & 0x0F) | 
		    ((u2StPid & 0x1FFF) << 8);
		vMxPutBits(prMuxInfo, u4Tmp, 32);
		vMxPutBits(prMuxInfo, 0x00, 8);
		
		vMxCopyBytes(prMuxInfo, u4StAdrr, u4StLen);
		MxAddStuff(prMuxInfo, MX_TS_NON_1ST_PSI_SZ - u4StLen);
        u4StAdrr += u4StLen;
        u4StLen -= u4StLen;
	}

    while (u4StLen > 0)
    {
        vMxTsIns192Hdr(prMuxInfo);
        if (u4StLen >= MX_TS_NON_1ST_PES_SZ)
        {
            u4Tmp = 0x47000010 | (prMpgMuxInfo->u1SectContinuity[u4Num]++ & 0x0F) | 
                ((u2StPid & 0x1FFF) << 8);
            vMxPutBits(prMuxInfo, u4Tmp, 32);

	        vMxCopyBytes(prMuxInfo, u4StAdrr, MX_TS_NON_1ST_PES_SZ);
	        u4StAdrr += MX_TS_NON_1ST_PES_SZ;
	        u4StLen -= MX_TS_NON_1ST_PES_SZ;
        }
        else
        {
        	u4Tmp = 0x47000010 | (prMpgMuxInfo->u1SectContinuity[u4Num]++ & 0x0F) | 
        	    ((u2StPid & 0x1FFF) << 8);
            vMxPutBits(prMuxInfo, u4Tmp, 32);
			
			vMxCopyBytes(prMuxInfo, u4StAdrr, u4StLen);
			MxAddStuff(prMuxInfo, MX_TS_NON_1ST_PES_SZ - u4StLen);
			
			u4StAdrr += u4StLen;
			u4StLen -= u4StLen;
		}
    }

    return TRUE;
}

static BOOL _MUXER_MpgTsHandleAudioEs(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UINT32 u4FrmSa = VIRTUAL(prAudPesInfo->u4StartAddr);
    UINT32 u4FrmSz = prAudPesInfo->u4FrameSize;
    UINT32 u4UpdateSz, u4Tmp;
    UINT32 u4Pts = prAudPesInfo->u4Pts;
    UINT32 u4PesSz, u4PacketSz;

    LOG(7, "_MUXER_MpgTsHandleAudioEs: pts 0x%08x, addr 0x%08x, 0x%08x 0x%08x\n",
        u4Pts, u4FrmSa, *(UINT32 *)u4FrmSa, *(UINT32 *)(u4FrmSa + 4));

    if (u4FrmSz >= prMuxInfo->u4MxBufSz)
    {
        LOG(0, "%s: frame size(%d) is larger than buffer size(%d)\n",
            __FUNCTION__, u4FrmSz, prMuxInfo->u4MxBufSz);
        ASSERT(0);
        return FALSE;
    }

    if (0 == u4FrmSz)
    {
        LOG(0, "%s: frame size is 0 !!!\n", __FUNCTION__);
        ASSERT(0);
        return TRUE;
    }

    ASSERT(0 == prAudPesInfo->u4FifoStart);
    ASSERT(0 == prAudPesInfo->u4FifoEnd);

    u4PacketSz = (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
        ? 192
        : 188;

    if (u4FrmSz <= MX_TS_1ST_PES_SZ)
    {
        u4PesSz = u4PacketSz;
    }
    else
    {
        u4Tmp = u4FrmSz - MX_TS_1ST_PES_SZ + MX_TS_NON_1ST_PES_SZ - 1;
        u4PesSz = (u4Tmp / MX_TS_NON_1ST_PES_SZ + 1) * u4PacketSz;
    }

    if (!MxWaitBufferAvail(prMuxInfo, u4PesSz))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return FALSE;
    }

    vMxTsIns192Hdr(prMuxInfo);
    // put the 1st TS PES
    if (u4FrmSz >= MX_TS_1ST_PES_SZ)
    {
        if(prMuxInfo->u2AudioPid != 0)
        {            
            u4Tmp = 0x47400010 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
        }
        else
        {
            u4Tmp = 0x47400010 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
        }
        vMxPutBits(prMuxInfo, u4Tmp, 32);
        /* 8b: sync_byte: 0x47 */
        /* 1b: transport_error_indicator: 0 */
        /* 1b: payload_unit_start_indicator: 1 */
        /* 1b: transport_priority: 0 */
        /* 13b: PID: MUXER_TS_A_PID */
        /* 2b: transport_scrambling_control: 0 */
        /* 2b: adaptation_field_control: 1 (payload only) */
        /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
    }
    else
    {
        // adaption field with stuffing (2.4.3.5)
        UINT32 u4AdpLen = MX_TS_1ST_PES_SZ - u4FrmSz - 1;

        if(prMuxInfo->u2AudioPid != 0)
        {            
            u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
        }
        else
        {
            u4Tmp = 0x47400030 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
        }
        vMxPutBits(prMuxInfo, u4Tmp, 32);
        /* 8b: sync_byte: 0x47 */
        /* 1b: transport_error_indicator: 0 */
        /* 1b: payload_unit_start_indicator: 1 */
        /* 1b: transport_priority: 0 */
        /* 13b: PID: MUXER_TS_A_PID */
        /* 2b: transport_scrambling_control: 0 */
        /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
        /* 4b: continuity_counter: AUD_ACC_CONTINUITY */

        vMxPutBits(prMuxInfo, u4AdpLen, 8);
        /* 8b: adaptation_field_length: u4AdpLen */
        if (u4AdpLen > 0)
        {
            vMxPutBits(prMuxInfo, 0x00, 8);
            /* 1b: discontinuity_indicator: 0 */
            /* 1b: random_access_indicator: 0 */
            /* 1b: elementary_stream_priority_indicator: 0 */
            /* 1b: PCR_flag: 0 */
            /* 1b: OPCR_flag: 0 */
            /* 1b: splicing_point_flag: 0 */
            /* 1b: transport_private_data_flag: 0 */
            /* 1b: adaptation_field_extension_flag: 0 */
            MxAddStuff(prMuxInfo, u4AdpLen - 1);
        }
        else
        {
            ASSERT(u4FrmSz == (MX_TS_1ST_PES_SZ - 1));
        }
    }
    // PES packet: 2.4.3.6
    vMxPutBits(prMuxInfo, 0x000001C0, 32);
    /* 24b: packet_start_code_prefix: 1 */
    /* 8b: stream_id: 0xC0 (audio stream number 0) */
    u4Tmp = u4FrmSz + 8;
    ASSERT(u4Tmp < 0x10000); // If bitrate is 128kbps, 0x10000 could contain 4 second data
    vMxPutBits(prMuxInfo, u4Tmp, 16);
    /* 16b: PES_packet_length */
    vMxPutBits(prMuxInfo, 0x848005, 24);
    /* 2b: '10' */
    /* 2b: PES_scrambling_control: 0 */
    /* 1b: PES_priority: 0 */
    /* 1b: data_alignment_indicator: 1 */
    /* 1b: copyright: 0 */
    /* 1b: original_or_copy: 0 */
    /* 2b: PTS_DTS_flags: 2 */
    /* 1b: ESCR_flag: 0 */
    /* 1b: ES_rate_flag: 0 */
    /* 1b: DSM_trick_mode_flag: 0 */
    /* 1b: additional_copy_info_flag: 0 */
    /* 1b: PES_CRC_flag: 0 */
    /* 1b: PES_extension_flag: 0 */
    /* 8b: PES_header_data_length: 5 */

    vMxPutPts(prMuxInfo, u4Pts);

    // copy payload start
    u4UpdateSz = (u4FrmSz >= MX_TS_1ST_PES_SZ)? MX_TS_1ST_PES_SZ : u4FrmSz;
    vMxCopyBytes(prMuxInfo, u4FrmSa, u4UpdateSz);
    u4FrmSa += u4UpdateSz;
    u4FrmSz -= u4UpdateSz;
    // copy payload end

    // put the 2nd+ TS packets
    while (u4FrmSz)
    {
        vMxTsIns192Hdr(prMuxInfo);
        if (u4FrmSz >= MX_TS_NON_1ST_PES_SZ)
        {
            if(prMuxInfo->u2AudioPid != 0)
            {            
                u4Tmp = 0x47000010 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47000010 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
            }
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 0 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_A_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 1 (payload only) */
            /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
        }
        else
        {
            UINT32 u4AdpLen = MX_TS_NON_1ST_PES_SZ - u4FrmSz - 1;
            if(prMuxInfo->u2AudioPid != 0)
            {
                u4Tmp = 0x47000030 | AUD_ACC_CONTINUITY | ((prMuxInfo->u2AudioPid & 0x1FFF) << 8);
            }
            else
            {
                u4Tmp = 0x47000030 | AUD_ACC_CONTINUITY | (MUXER_TS_A_PID << 8);
            }
            vMxPutBits(prMuxInfo, u4Tmp, 32);
            /* 8b: sync_byte: 0x47 */
            /* 1b: transport_error_indicator: 0 */
            /* 1b: payload_unit_start_indicator: 0 */
            /* 1b: transport_priority: 0 */
            /* 13b: PID: MUXER_TS_A_PID */
            /* 2b: transport_scrambling_control: 0 */
            /* 2b: adaptation_field_control: 3 (adaptation_field followed by payload) */
            /* 4b: continuity_counter: AUD_ACC_CONTINUITY */
            vMxPutBits(prMuxInfo, u4AdpLen, 8);
            /* 8b: adaptation_field_length: u4AdpLen */
            if (u4AdpLen > 0)
            {
                vMxPutBits(prMuxInfo, 0x00, 8);
                /* 1b: discontinuity_indicator: 0 */
                /* 1b: random_access_indicator: 0 */
                /* 1b: elementary_stream_priority_indicator: 0 */
                /* 1b: PCR_flag: 0 */
                /* 1b: OPCR_flag: 0 */
                /* 1b: splicing_point_flag: 0 */
                /* 1b: transport_private_data_flag: 0 */
                /* 1b: adaptation_field_extension_flag: 0 */
                MxAddStuff(prMuxInfo, u4AdpLen - 1);
            }
            else
            {
                ASSERT(u4FrmSz == (MX_TS_NON_1ST_PES_SZ - 1));
            }
        }

        u4UpdateSz = (u4FrmSz >= MX_TS_NON_1ST_PES_SZ)? MX_TS_NON_1ST_PES_SZ : u4FrmSz;
        vMxCopyBytes(prMuxInfo, u4FrmSa, u4UpdateSz);
        u4FrmSa += u4UpdateSz;
        u4FrmSz -= u4UpdateSz;
    }

    return TRUE;
}


static BOOL _MUXER_MpgTsHandleAACEs(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    return(_MUXER_MpgTsHandleAudioEs(prMuxInfo, prAudPesInfo));
}


static BOOL _MUXER_MpgTsHandleMP3Es(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    return(_MUXER_MpgTsHandleAudioEs(prMuxInfo, prAudPesInfo));
}


static BOOL _MUXER_MpgTsHandleAudPes(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    BOOL fgRet = FALSE;

    LOG(7, "MUXER audioFmt is %d\n", prMuxInfo->eAudFmt);
    if (ENUM_MUXER_AENC_MP3 == prMuxInfo->eAudFmt)
    {
        fgRet = _MUXER_MpgTsHandleMP3Es(prMuxInfo, prAudPesInfo);
    }
    else if(ENUM_MUXER_AENC_AAC == prMuxInfo->eAudFmt)
    {
        fgRet = _MUXER_MpgTsHandleAACEs(prMuxInfo, prAudPesInfo);
    }
#ifdef TS_PCM_SUPPORT
    else if (ENUM_MUXER_AENC_PCM == prMuxInfo->eAudFmt)
    {
        fgRet = _MUXER_MpgTsHandlePCM(prMuxInfo, prAudPesInfo);
    }
#endif
    else
    {
        LOG(0, "%s: audio format %d is not supported yet\n", __FUNCTION__, prMuxInfo->eAudFmt);
    }

    return fgRet;
}


static BOOL _MUXER_TsMuxTTXEs(MUXER_INFO_T *prMuxInfo, UINT32 u4Sa, UINT32 u4Sz, UINT32 u4Pts)
{
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UINT32 i;
    UINT32 u4UnitNs;
    UINT32 u4Tmp;
    UINT16 u2PktLen;
    UINT8 u1FieldLine;
    UINT32 u4PesSz;

    if (u4Sz >= prMuxInfo->u4MxBufSz)
    {
        LOG(0, "%s: frame size(%d) is larger than buffer size(%d)\n",
            __FUNCTION__, u4Sz, prMuxInfo->u4MxBufSz);
        ASSERT(0);
        return FALSE;
    }

    if (0 == u4Sz)
    {
        return TRUE;
    }

    u4UnitNs = u4Sz / 48;
    u2PktLen = (u4UnitNs / 4 + 1) * 184 - 6;

    u4PesSz = (u4UnitNs / 4 + 1) * 192;
    if (!MxWaitBufferAvail(prMuxInfo, u4PesSz))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return FALSE;
    }

    vMxTsIns192Hdr(prMuxInfo);
    u4Tmp = 0x47400010 | TT_ACC_CONTINUITY | (MUXER_TS_TT_PID << 8);
    // put the first TS packet
    // no adaption field
    vMxPutBits(prMuxInfo, u4Tmp, 32);
    /* 8b: sync_byte: 0x47 */
    /* 1b: transport_error_indicator: 0 */
    /* 1b: payload_unit_start_indicator: 1 */
    /* 1b: transport_priority: 0 */
    /* 13b: PID: MUXER_TS_TT_PID */
    /* 2b: transport_scrambling_control: 0 */
    /* 2b: adaptation_field_control: 1 (payload only) */
    /* 4b: continuity_counter: TT_ACC_CONTINUITY */
    vMxPutBits(prMuxInfo, 0x000001BD, 32);
    /* 24b: packet_start_code_prefix: 1 */
    /* 8b: stream_id: 0xBD (private_stream_1) */
    vMxPutBits(prMuxInfo, u2PktLen, 16);
    /* 16b: PES_packet_length */
    vMxPutBits(prMuxInfo, 0x848024, 24);
    /* 2b: '10' */
    /* 2b: PES_scrambling_control: 0 */
    /* 1b: PES_priority: 0 */
    /* 1b: data_alignment_indicator: 1 */
    /* 1b: copyright: 0 */
    /* 1b: original_or_copy: 0 */
    /* 2b: PTS_DTS_flags: 2 */
    /* 1b: ESCR_flag: 0 */
    /* 1b: ES_rate_flag: 0 */
    /* 1b: DSM_trick_mode_flag: 0 */
    /* 1b: additional_copy_info_flag: 0 */
    /* 1b: PES_CRC_flag: 0 */
    /* 1b: PES_extension_flag: 0 */
    /* 8b: PES_header_data_length: 5 */

    vMxPutPts(prMuxInfo, u4Pts);
    MxAddStuff(prMuxInfo, 31);
    vMxPutBits(prMuxInfo, 0x10, 8);
    /* 8b: data_identifier: 0x10 (EBU data) */
    for (i = 0; i < 3 && i < u4UnitNs; i++)
    {
        vMxPutBits(prMuxInfo, 0x022C, 16);
        /* 8b: data_unit_id: 0x02 (EBU Teletext non-subtitle data) */
        /* 8b: data_unit_length: 0x2C */
        u1FieldLine = *(UCHAR *)(u4Sa + 42);
        u1FieldLine = (0xC0 | ((u1FieldLine & 0x80) >> 2) | (u1FieldLine & 0x1F));
        vMxPutBits(prMuxInfo, u1FieldLine, 8);
        /* 2b: reserved_future_use: 3 */
        /* 1b: field_parity */
        /* 5b: line_offset */
        vMxPutBits(prMuxInfo, 0x27/*0xE4*/, 8);
        /* 8b: framing_code: 0x27(b'00100111, reverse of 0xE4, why?) */
        vMxCopyBytes(prMuxInfo, u4Sa, 42);
        /* 16b: magazine_and_packet_address */
        /* 320b: data_block */
        u4Sa += 48;
    }
    u4UnitNs -= i;
    for (; i < 3; i++)  // put stuffing units if any
    {
        vMxPutBits(prMuxInfo, 0xFF2C, 16);
        /* 8b: data_unit_id: 0xFF (data_unit for stuffing) */
        /* 8b: data_unit_length: 0x2C */
        MxAddStuff(prMuxInfo, 44);
    }

    // put the 2nd+ TS packets
    if (u4UnitNs)
    {
        i = 0;
        while (u4UnitNs)
        {
            if (i % 4 == 0)
            {
                vMxTsIns192Hdr(prMuxInfo);
                u4Tmp = 0x47000010 | TT_ACC_CONTINUITY | (MUXER_TS_TT_PID << 8);
                vMxPutBits(prMuxInfo, u4Tmp, 32);
                /* 8b: sync_byte: 0x47 */
                /* 1b: transport_error_indicator: 0 */
                /* 1b: payload_unit_start_indicator: 0 */
                /* 1b: transport_priority: 0 */
                /* 13b: PID: MUXER_TS_TT_PID */
                /* 2b: transport_scrambling_control: 0 */
                /* 2b: adaptation_field_control: 1 (payload only) */
                /* 4b: continuity_counter: TT_ACC_CONTINUITY */
            }

            vMxPutBits(prMuxInfo, 0x022C, 16);
            /* 8b: data_unit_id: 0x02 (EBU Teletext non-subtitle data) */
            /* 8b: data_unit_length: 0x2C */
            u1FieldLine = *(UCHAR *)(u4Sa + 42);
            u1FieldLine = (0xC0 | ((u1FieldLine & 0x80) >> 2) | (u1FieldLine & 0x1F));
            vMxPutBits(prMuxInfo, u1FieldLine, 8);
            /* 2b: reserved_future_use: 3 */
            /* 1b: field_parity */
            /* 5b: line_offset */
            vMxPutBits(prMuxInfo, 0x27/*0xE4*/, 8);
            /* 8b: framing_code: 0x27(b00100111, reverse of 0xE4, why?) */
            vMxCopyBytes(prMuxInfo, u4Sa, 42);
            /* 16b: magazine_and_packet_address */
            /* 320b: data_block */
            u4Sa += 48;

            u4UnitNs--;
            i++;
        }

        // put the tailing stuffing unit if any
        i = 4 - i % 4;
        while (i)
        {
            vMxPutBits(prMuxInfo, 0xFF2C, 16);
            /* 8b: data_unit_id: 0xFF (data_unit for stuffing) */
            /* 8b: data_unit_length: 0x2C */
            MxAddStuff(prMuxInfo, 44);
            i--;
        }
    }

    return TRUE;
}


// currently, only one kind of VBI is supported: TTX
static BOOL _MUXER_MpgTsHandleVBIPes(
    MUXER_INFO_T *prMuxInfo,
    VBIENC_PES_INFO_T *prVBIPesInfo)
{
    BOOL fgRet = FALSE;
    UINT32 u4FrmSa = prVBIPesInfo->u4StartAddr;
    UINT32 u4FrmSz = prVBIPesInfo->u4FrameSize;
    UINT32 u4FifoSa = prVBIPesInfo->u4FifoStart;
    UINT32 u4FifoEa = prVBIPesInfo->u4FifoEnd;

    //LOG(0, "VBIES: pts = %d (%d s)  ", prVBIPesInfo->u4Pts, prVBIPesInfo->u4Pts/90000);
    if (u4FifoSa + u4FifoEa == 0 || u4FrmSa + u4FrmSz <= u4FifoEa)
    {
        fgRet = _MUXER_TsMuxTTXEs(prMuxInfo, VIRTUAL(u4FrmSa), u4FrmSz, prVBIPesInfo->u4Pts);
    }
    else
    {
        ASSERT(0);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
// Iner-file functions
//-----------------------------------------------------------------------------

BOOL _MUXER_MpgTsStart(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    UCHAR ucMxNum = 0;

#ifdef TS_PCM_SUPPORT
    if (ENUM_MUXER_AENC_PCM == prMuxInfo->eAudFmt)
    {
        ASSERT(prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf == NULL);
        prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf = (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(1024, 16));
        ASSERT(prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf != NULL);
    }
#endif

#ifdef USE_STC_AS_TIMESTAMP
    prMpgMuxInfo->u4Timestamp = 0;
#endif
    prMpgMuxInfo->u4Pcr = 0;

    prMpgMuxInfo->u1NetContinuity = 1;
    prMpgMuxInfo->u1PatContinuity = 0;
    prMpgMuxInfo->u1PmtContinuity = 0;
    prMpgMuxInfo->u1VidContinuity = 1;
    prMpgMuxInfo->u1AudContinuity = 1;
    prMpgMuxInfo->u1TtxContinuity = 1;
    for(ucMxNum = 0; ucMxNum < MUXER_MAX_SECTION_NS; ucMxNum++)
    {
        prMpgMuxInfo->u1SectContinuity[ucMxNum] = 1;   
    }

#ifdef MUXER_VID_HDR_BUF_SIZE
    prMpgMuxInfo->u4VidHdrBufWp = 0;
    prMpgMuxInfo->u4VidHdrBufSz = 0;
#endif

    return TRUE;
}


BOOL _MUXER_MpgTsStop(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

#ifdef TS_PCM_SUPPORT
    if (ENUM_MUXER_AENC_PCM == prMuxInfo->eAudFmt)
    {
        ASSERT(prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf != NULL);
        BSP_FreeAlignedDmaMemory(prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf);
        prMuxInfo->rFmtInfo.rMuxerMpgInfo.pucAudResidueBuf = NULL;
    }
#endif

    UNUSED(prMuxInfo);
    return TRUE;
}


MUXER_HDR_STATE_T _MUXER_HdrMuxMpgTs(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    MUXER_MPG_INFO_T *prMpgMuxInfo = &prMuxInfo->rFmtInfo.rMuxerMpgInfo;
    ENUM_MUXER_STRMTYPE_T eStrm;
    UINT32 u4StrmNo;
    UINT32 u4Wp, u4WpBak, u4Pts;
    UINT32 u4Num = 0;

#if 0
        LOG(0, "VidPts = 0x%08x, VbiPts = 0x%08x, AudPts = 0x%08x\n",
            prMpgMuxInfo->u4CurVidPts,
            prMpgMuxInfo->u4CurVbiPts,
            prMpgMuxInfo->u4CurAudPts);
#endif

    if (prMuxInfo->u4MxBufFrmCnt == 0)
    {
        UINT32 i;

        // TODO: check why need 8 PSI make TSReaderLite work
        for (i = 0; i < 8; i++)
        {
            vMxPutPSIWithTT(prMuxInfo);
        }
        prMuxInfo->u4MxBufFrmCnt = 1;
    }
    
    for(u4Num = 0; u4Num < MUXER_MAX_SECTION_NS; u4Num++)
    {
        if(prMuxInfo->arSection[u4Num].fgEnabled)
        {
            u4Pts = _AUD_ReadPsrStc1();
            if (prMuxInfo->arSection[u4Num].u4SecTionPts != 0 && 
                (prMuxInfo->arSection[u4Num].u4SecTionPts + prMuxInfo->arSection[u4Num].rAddSection.i4Repetitiontime) > u4Pts)
            {
                continue;
            }

            _MUXER_MpgTsHandleSection(u4Num, prMuxInfo);

            prMuxInfo->arSection[u4Num].u4SecTionPts = u4Pts;
        }
    }

    _MxStrmSelect(prMuxInfo, &eStrm, &u4StrmNo);

    if (eMUXER_STRMTYPE_AUDIO == eStrm)
    {
        _MUXER_ReceiveAudioPes(ucMxId);

        if (prMuxInfo->rAEncPesInfo.u4FrameSize == 0)
        {
            LOG(1, "Audio frame is 0, audio not send pes or send pes is 0!\n");
            return eMUXER_HDR_SUCCEED;
        }
        if (prMuxInfo->rAEncPesInfo.u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        vMxTsChkPcr(prMuxInfo);

        u4WpBak = prMuxInfo->u4MxBufWp;
        _MUXER_MpgTsHandleAudPes(prMuxInfo, &prMuxInfo->rAEncPesInfo);
        u4Wp = prMuxInfo->u4MxBufWp;
        if (u4Wp != u4WpBak)
        {
            MxFlushRing(prMuxInfo, u4WpBak, u4Wp);

            if (prMuxInfo->fgScramble)
            {
                MxScramble(prMuxInfo, u4WpBak, u4Wp);
            }
        }

        prMuxInfo->u4AudCnt++;
        //LOG(0, "Audio Count = (%ld) \n", prMuxInfo->u4AudCnt);

        u4Pts = prMpgMuxInfo->u4CurAudPts = prMuxInfo->rAEncPesInfo.u4Pts;
        _vMxUpdatePts(prMuxInfo, u4StrmNo, prMpgMuxInfo->u4CurAudPts);
    }
    else if (eMUXER_STRMTYPE_VIDEO == eStrm)
    {
        VENC_PES_INFO_T *prVEncPesInfo = &prMuxInfo->rVEncPesInfo;
        _MUXER_ReceiveVideoPes(ucMxId);

        if (prVEncPesInfo->u4FrameSize == 0)
        {
            LOG(1, "video frame is 0, video not send pes or send pes is 0!\n");
            return eMUXER_HDR_SUCCEED;
        }
        
        if (prVEncPesInfo->u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        ++prMuxInfo->u4MxBufFrmCnt;
        if ((prMuxInfo->u4MxBufFrmCnt & 0x7f) == 0)
        {
            vMxPutPSIWithTT(prMuxInfo);
        }

        vMxTsChkPcr(prMuxInfo);

        u4WpBak = prMuxInfo->u4MxBufWp;
        _MxVideoTypeAnalyze(prMuxInfo, prVEncPesInfo);
#ifdef MUXER_VID_HDR_BUF_SIZE
        if (!prMuxInfo->fgFrame)
        {
            LOG(6, "MxTs(%d): not Frame (%d)\n", ucMxId, prVEncPesInfo->u4VideoType);
            _MUXER_MpgTsHandleVideoHdr(prMuxInfo, prVEncPesInfo);
#ifdef CC_SUPPORT_VENC
            LOG(7, "MxTs(%d): unlock venc 2 (0x%x, 0x%x, %d)\n", ucMxId, 
                prVEncPesInfo->pvTag,
                prVEncPesInfo->u4StartAddr,
                prVEncPesInfo->u4FrameSize);
            VENC_UnlockBuf(prVEncPesInfo->pvTag,
                prVEncPesInfo->u4StartAddr,
                prVEncPesInfo->u4FrameSize);
#endif
            return eMUXER_HDR_SUCCEED;
        }
#endif
        _MUXER_MpgTsHandleVideoPes(prMuxInfo, prVEncPesInfo);
#ifdef CC_SUPPORT_VENC
        LOG(7, "MxTs(%d): unlock venc (0x%x, 0x%x, %d)\n", ucMxId, 
            prVEncPesInfo->pvTag,
            prVEncPesInfo->u4StartAddr,
            prVEncPesInfo->u4FrameSize);
        VENC_UnlockBuf(prVEncPesInfo->pvTag,
            prVEncPesInfo->u4StartAddr,
            prVEncPesInfo->u4FrameSize);
#endif
        u4Wp = prMuxInfo->u4MxBufWp;
        if (u4Wp != u4WpBak)
        {
            MxFlushRing(prMuxInfo, u4WpBak, u4Wp);

            if (prMuxInfo->fgScramble)
            {
                MxScramble(prMuxInfo, u4WpBak, u4Wp);
            }
        }

        prMuxInfo->u4VidCnt++;
        //LOG(0, "Video Count = (%ld) \n", prMuxInfo->u4VidCnt);

        u4Pts = prMpgMuxInfo->u4CurVidPts = prVEncPesInfo->u4Pts;
        _vMxUpdatePts(prMuxInfo, u4StrmNo, prMpgMuxInfo->u4CurVidPts);
    }
    else if (eMUXER_STRMTYPE_VBI == eStrm)
    {
        _MUXER_ReceiveVBIPes(ucMxId);

        if (prMuxInfo->rVBIEncPesInfo.u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        vMxTsChkPcr(prMuxInfo);

        _MUXER_MpgTsHandleVBIPes(prMuxInfo, &prMuxInfo->rVBIEncPesInfo);

        prMuxInfo->u4VBICnt++;
        //LOG(0, "VBI Count = (%ld) \n", prMuxInfo->u4VBICnt);

        u4Pts = prMpgMuxInfo->u4CurVbiPts = prMuxInfo->rVBIEncPesInfo.u4Pts;
        _vMxUpdatePts(prMuxInfo, u4StrmNo, prMpgMuxInfo->u4CurVbiPts);
    }
    else if (eMUXER_STRMTYPE_MAX == eStrm)
    {
        return eMUXER_HDR_NO_DATA;
    }
    else
    {
        LOG(0, "MxTs(%d): invalid stream (%d)\n", ucMxId, eStrm);
        ASSERT(0);
        x_thread_delay(1);
        return eMUXER_HDR_FAIL;
    }

    prMuxInfo->u4MxBufWpNfy = prMuxInfo->u4MxBufWp;
    if (!prMuxInfo->u4NfyPeriod)
    {
        vMxNotifyDataReady(prMuxInfo, u4Pts);
    }

    return eMUXER_HDR_SUCCEED;
}


