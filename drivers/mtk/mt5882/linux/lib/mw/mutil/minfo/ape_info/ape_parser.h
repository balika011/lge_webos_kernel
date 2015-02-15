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
 * $RCSfile: mkv_parser.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/2 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0916b8010fb2d9ebf453b3d4efdbc5e9 $ Asa
 *
 * Description: 
 *         This file exports public API of the mkv parser function
 *---------------------------------------------------------------------------*/

#ifndef _APE_PARSER_H_
#define _APE_PARSER_H_

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#ifndef __KERNEL__
#include "u_common.h"
#include "x_mm_common.h"
#include "u_handle.h"
#include "mutil/minfo/minfo.h"
#include "mutil/minfo/minfo_dbg.h"
#include "u_mfmtrecg.h"
#include "../../mm_common/rfc_ebml/rfc_ebml.h"

#else

#include "u_common.h"
#include "x_mm_common.h"
#include "u_handle.h"
#include "../minfo.h"
#include "../minfo_dbg.h"
#include "../../mfmtrecg/u_mfmtrecg.h"
#include "../../mm_common/rfc_ebml/rfc_ebml.h"

#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MINFO_APE_COMMON_HEAD_T
{
    UINT8               ui1_id[4];
    UINT16              ui4_version;
}MINFO_APE_COMMON_HEAD_T;

typedef struct _MINFO_APE_DESCRIPTOR_INFO_T
{    
    UINT8               ui1_id[4];
    UINT32              ui4_version;

    UINT32              ui4_descrptorbytes;
    UINT32              ui4_headerbytes;
    UINT32              ui4_seektablebytes;
    UINT32              ui4_waveheaderbytes;
    UINT32              ui4_framedatabytes_low;
    UINT32              ui4_framedatabytes_high;
    UINT32              ui4_terminatingdata_bytes;
    UINT8               ui1_filemd5[16];
}MINFO_APE_DESCRIPTOR_INFO_T;

typedef struct _MINFO_APE_HEADER_INFO_T
{
    UINT16              ui2_compresslevel;
    UINT16              ui2_formatflags;
    UINT32              ui4_blockperframe;
    UINT32              ui4_finalframeblocks;
    UINT32              ui4_totalframes;
    UINT16              ui2_bitsperframe;
    UINT16              ui2_channels;
    UINT32              ui4_samplerate;
}MINFO_APE_HEADER_INFO_T;

typedef struct _MINFO_APE_HEADER_OLD_INFO_T
{
    UINT8               ui1_id[4];
    UINT16              ui4_version;
    
    UINT16              ui2_compresslevel;
    UINT16              ui2_formatflags;    
    UINT16              ui2_channels;
    UINT32              ui4_samplerate;
    UINT32              ui4_headerbytes;
    UINT32              ui4_terminatingdata_bytes;
    UINT32              ui4_totalframes;
    UINT32              ui4_finalframeblocks;
}MINFO_APE_HEADER_OLD_INFO_T;


typedef struct _MINFO_APE_WAVEHEADER_INFO_T
{
    UINT8               ui1_riffheader[4];
    UINT32              ui4_riffbytes;
    UINT8               ui4_datatypeid[4];
    UINT8               ui1_formatheader[4];
    UINT32              ui4_formatbytes;
    UINT16              ui2_formattag;
    UINT16              ui2_channels;
    UINT32              ui4_samplepersec;
    UINT32              ui4_avgbytespersec;
    UINT16              ui2_blockalign;
    UINT16              ui2_bitspersample;
    UINT8               ui1_dataheader[4];
    UINT32              ui4_databytes;
}MINFO_APE_WAVEHEADER_INFO_T;

typedef struct _MINFO_APE_AUDIO_INFO_T
{
    MINFO_INFO_AUDIO_ENC_T      e_enc;
    BOOL                b_audio_only;
    UINT32              ui4_version;
    UINT16              ui2_compresslevel;
    UINT32              ui4_blockperframe;
    UINT32              ui4_finalframeblocks;
    UINT32              ui4_totalframes;
    UINT16              ui2_bitsperframe;
    UINT16              ui2_channels;
    UINT32              ui4_samplerate;
    UINT16              ui2_bitpersample;
} MINFO_APE_AUDIO_INFO_T;

typedef struct MINFO_APE_FILE_INFO_T
{
    /* file info */
    BOOL                        b_is_old;
    MINFO_APE_DESCRIPTOR_INFO_T t_ape_desc_info;
    MINFO_APE_HEADER_INFO_T     t_ape_header_info;
    MINFO_APE_HEADER_OLD_INFO_T t_ape_header_old_info;
    MINFO_APE_WAVEHEADER_INFO_T t_ape_wavheader_info;
    
    /* output info */
    MINFO_APE_AUDIO_INFO_T      t_ape_audio_info;
    UINT64                      ui8_filesize;
    UINT64                      ui8_duration;
    UINT64                      ui8_seektableoffset;
    UINT64                      ui8_seektablebytes;
    /* parse info */
    UINT64                      ui8_curr_parser_offset;
}MINFO_APE_FILE_INFO_T;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

extern INT32 ape_parse_elements(MINFO_OBJ_T* pt_minfo_obj);

#ifdef __cplusplus
}
#endif

#endif /* _APE_MINFO_H_ */

