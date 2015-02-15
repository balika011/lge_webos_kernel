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
 * $RCSfile: sbtl_ipcc_parser.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description:
 *
 * History:
 *
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#ifndef __KERNEL__
#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif

#ifdef INET_SUPPORT
#ifdef USE_SMART_FILE
#include "x_smart_file.h"
#else
#include "x_httpc_api.h"
#endif
#endif
#include "mutil/mm_util_input.h"

#include "x_common.h"
#include "inc/common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"
#include "x_lnk_list.h"

#include "mutil/mm_util.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_ipcc_parser.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_xml_page_inst_mngr.h"
#include "x_sbtl_ipcc.h"




/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define  PTS_FREQ        ((UINT64)(90000))
#define  TIMER_PERIOD    200
#define  IPCC_XML_MAX_HEAD_LEN  (128)
#define  IPCC_M4T_HEAD_FLAG_LEN (8)



/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
 #define SBTL_SAMPLE_MAX             1500
 #define dwBigFourCC(a,b,c,d)        ((((UINT32)a)<<24)|((b)<<16)|((c)<<8)|(d))
 #define MP4_BOX_FTYP                dwBigFourCC('f', 't', 'y', 'p')
 #define MP4_BOX_MOOV                dwBigFourCC('m', 'o', 'o', 'v')
 #define MP4_BOX_MVHD                dwBigFourCC('m', 'v', 'h', 'd')
 #define MP4_BOX_TRAK                dwBigFourCC('t', 'r', 'a', 'k')
 #define MP4_BOX_MVEX                dwBigFourCC('m', 'v', 'e', 'x')
 #define MP4_BOX_MEHD                dwBigFourCC('m', 'e', 'h', 'd')
 #define MP4_BOX_TREX                dwBigFourCC('t', 'r', 'e', 'x')
 #define MP4_BOX_MOOF                dwBigFourCC('m', 'o', 'o', 'f')
 #define MP4_BOX_MDAT                dwBigFourCC('m', 'd', 'a', 't')
 #define MP4_BOX_MFRA                dwBigFourCC('m', 'f', 'r', 'a')
 #define MP4_BOX_MFHD                dwBigFourCC('m', 'f', 'h', 'd')
 #define MP4_BOX_TRAF                dwBigFourCC('t', 'r', 'a', 'f')
 #define MP4_BOX_TFHD                dwBigFourCC('t', 'f', 'h', 'd')
 #define MP4_BOX_TRUN                dwBigFourCC('t', 'r', 'u', 'n')                    /* big endian */


typedef struct 
{
    UINT64  ui8_key_moof_ofst;
    UINT64  ui8_key_ext_duration_sum;
} MP4_MIDXBULD_KEY_ELMT_STRM_ENTRY_T; 
 typedef struct
{
    UINT32  ui4_pts;
    UINT32  ui4_ofst;
    UINT32  ui4_size;
    UINT8*   pui1_buf;
} MP4_MIDXBULD_ELMT_STRM_ENTRY_T;
 typedef struct 
{       
    UINT8*                pui1_file_cntt; 
    UINT64              ui8_total_duration; 
    UINT32              ui4_dft_sample_dur;
    UINT32              ui4_timescale;
    UINT32              ui4_key_num;
    MP4_MIDXBULD_KEY_ELMT_STRM_ENTRY_T* pt_key_entry;
    UINT32              ui4_sample_num;
    MP4_MIDXBULD_ELMT_STRM_ENTRY_T* pt_sample_entry;
}MP4_IDX_TBL_T;

MP4_IDX_TBL_T g_m4t_idx_tbl;

 SLIST_T(_SBTL_TTML_ELEMENT_P_T)  t_element_p_list = {NULL};
 SLIST_T(_SBTL_TTML_ATTR_T)  t_attr_cur = {NULL};
 SLIST_T(_SBTL_TTML_ATTR_ID_T)  t_common_id_attr_list = {NULL};
 
 BOOL     b_is_whitspace_valid = FALSE;

/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
 
static  SBTL_XML_HASH_INFO_T t_elm_hash[] =
{
    {"tt",         (INT32)SBTL_XML_ELM_TT},
    {"head",       (INT32)SBTL_XML_ELM_HEAD},
    {"metadata",   (INT32)SBTL_XML_ELM_METADATA},
    {"title",      (INT32)SBTL_XML_ELM_TITLE},
    {"desc",       (INT32)SBTL_XML_ELM_DESC},
    {"copyright",  (INT32)SBTL_XML_ELM_COPYRIGHT},
    {"styling",    (INT32)SBTL_XML_ELM_STYLING},
    {"style",      (INT32)SBTL_XML_ELM_STYLE},
    {"layout",     (INT32)SBTL_XML_ELM_LAYOUT},
    {"region",     (INT32)SBTL_XML_ELM_REGION},
    {"body",       (INT32)SBTL_XML_ELM_BODY},    
    {"div",        (INT32)SBTL_XML_ELM_DIV},
    {"p",          (INT32)SBTL_XML_ELM_PARA},
    {"span",       (INT32)SBTL_XML_ELM_SPAN},    
    {"br",         (INT32)SBTL_XML_ELM_BR},
    {"information",(INT32)SBTL_XML_ELM_INFO}
};


static  SBTL_XML_HASH_INFO_T t_attr_hash[] =
{       
    {"id",             (INT32)SBTL_XML_IPCC_ATTR_ID},
    {"region",         (INT32)SBTL_XML_IPCC_ATTR_REGION},    
    {"style",          (INT32)SBTL_XML_IPCC_ATTR_STYLE},
    
    {"begin",          (INT32)SBTL_XML_IPCC_ATTR_BEGIN},
    {"end",            (INT32)SBTL_XML_IPCC_ATTR_END}, 
    {"dur",            (INT32)SBTL_XML_IPCC_ATTR_DUR}, 
    {"fontfamily",     (INT32)SBTL_XML_IPCC_ATTR_FONTFAMILY},
    {"fontsize",       (INT32)SBTL_XML_IPCC_ATTR_FONTSIZE},
    {"textalign",      (INT32)SBTL_XML_IPCC_ATTR_TEXTALIGN},
    {"origin",         (INT32)SBTL_XML_IPCC_ATTR_ORIGIN},
    {"fontweight",     (INT32)SBTL_XML_IPCC_ATTR_FONT_WEIGHT},
    {"visibility",     (INT32)SBTL_XML_IPCC_ATTR_VISIBILY},
    {"backgroundcolor",(INT32)SBTL_XML_IPCC_ATTR_BGCOLOR},
    {"encoding",       (INT32)SBTL_XML_IPCC_ATTR_ECODING},
    {"color",          (INT32)SBTL_XML_IPCC_ATTR_COLOR},
    {"fontstyle",      (INT32)SBTL_XML_IPCC_ATTR_FONT_STYLE},  
    {"extent",         (INT32)SBTL_XML_IPCC_ATTR_FONT_EXTENT},
    {"cellresolution", (INT32)SBTL_XML_IPCC_ATTR_CELL_RESOLUTION},    
    {"textdecoration", (INT32)SBTL_XML_IPCC_ATTR_TEXT_DECORATION},
    
    {"opacity",        (INT32)SBTL_XML_IPCC_ATTR_WINDOW_OPACITY},    
    {"textoutline",    (INT32)SBTL_XML_IPCC_ATTR_TEXT_OUTLINE},
    {"warpoption",     (INT32)SBTL_XML_IPCC_ATTR_WRAP_OPTION},    
    {"space",          (INT32)SBTL_XML_IPCC_ATTR_WHITESPACE},
    {"lineheight",     (INT32)SBTL_XML_IPCC_ATTR_LINEHEIGHT},
    {"role",           (INT32)SBTL_XML_IPCC_ATTR_ROLE},
    {"mode",           (INT32)SBTL_XML_IPCC_ATTR_MODE},
    {"fakespan",       (INT32)SBTL_XML_IPCC_ATTR_FAKESPAN}
};
/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 i4_get_m4t_content(SBTL_PAGE_INST_MNGR_T*   pt_mngr, VOID* pv_buffer)

{
    UINT32                ui4_box_type    = 0; 
    UINT32                ui4_box_size    = 0; 
    UINT32                ui4_sample_size = 0; 
    UINT32                flag_tr         = 0; 
    UINT32                ui8_moof_offset = 0; 
    UINT32                ui4_sample_dur  = 0; 
    UINT32                ui4_sbtl_sample_dur_sum  = 0; 
    UINT32                ui4_i           = 0; 
    UINT32                ui4_ver         = 0; 
    UINT32                ui4_tmp         = 0; 
    UINT8*                pui1_buf        = NULL;             
    UINT32                ui4_file_size = 0;    
	
    MP4_MIDXBULD_ELMT_STRM_ENTRY_T* pt_idx_entry = NULL;
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
    SBTL_ENGINE_T*                  pt_engine;
	
	if (pt_mngr == NULL || pv_buffer == NULL)
	{
        return MMSBTLR_INV_ARG;
    }
       
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
    if(pt_engine == NULL || pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    x_memset(&g_m4t_idx_tbl, 0, sizeof(MP4_IDX_TBL_T));
    ui4_file_size = (UINT32)pt_engine->aui8_file_size[MM_SBTL_FEEDER_SUBTITLE];
    g_m4t_idx_tbl.pui1_file_cntt = (UINT8*)pv_buffer;
    pui1_buf =  g_m4t_idx_tbl.pui1_file_cntt;
	
    while(1)
    {
        if(pui1_buf >=  g_m4t_idx_tbl.pui1_file_cntt + ui4_file_size)
        {
            break;
        }
        
        LOADB_DWRD(pui1_buf,        ui4_box_size);
        LOADB_DWRD(pui1_buf + 4,    ui4_box_type);
        
        DBG_INFO(("ui4_box_size = 0x%x ui4_box_type=0x%x\n", ui4_box_size, ui4_box_type));
        switch(ui4_box_type)
        {
        case MP4_BOX_FTYP:
            pui1_buf += ui4_box_size;
            break;
        case MP4_BOX_MOOV:
            pui1_buf += 8;
            break;
        case MP4_BOX_MVHD:
            LOADB_DWRD(pui1_buf + 20, g_m4t_idx_tbl.ui4_timescale);
            pui1_buf += ui4_box_size;
            break;
        case MP4_BOX_TRAK:
            pui1_buf += ui4_box_size;
            break;
        case MP4_BOX_MVEX:
            pui1_buf += 8;
            break;
        case MP4_BOX_MEHD:
        {
            UINT8  ui1_ver = 0;
            UINT32  ui4_high= 0;
            UINT32  ui4_low = 0;
            ui1_ver = *(pui1_buf + 8);
            if (ui1_ver == 1)
            {
                LOADB_DWRD(pui1_buf + 12, ui4_high);
                LOADB_DWRD(pui1_buf + 16, ui4_low);
            }
            else
            {
                LOADB_DWRD(pui1_buf + 12, ui4_low);
            }
            g_m4t_idx_tbl.ui8_total_duration = (((UINT64)ui4_high<<32)) | ui4_low ;
            pui1_buf += ui4_box_size;
        }
            break;
        case MP4_BOX_TREX:
        {
            LOADB_DWRD(pui1_buf + 20, g_m4t_idx_tbl.ui4_dft_sample_dur);
            pui1_buf += ui4_box_size;
        }
            break;
        case MP4_BOX_MOOF:
            if(g_m4t_idx_tbl.pt_sample_entry == NULL)
            {
                g_m4t_idx_tbl.pt_sample_entry= (MP4_MIDXBULD_ELMT_STRM_ENTRY_T*)x_mem_alloc(sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T) * SBTL_SAMPLE_MAX);
                if(g_m4t_idx_tbl.pt_sample_entry == NULL)
                {
                    DBG_INFO(("malloc sample index table buf Failed !\n"));
                    return MMSBTLR_INV_ARG;                    
                }
                x_memset( g_m4t_idx_tbl.pt_sample_entry, 
                        0, 
                        sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T) * SBTL_SAMPLE_MAX);
            }
            ui8_moof_offset = (UINT64)(pui1_buf -  g_m4t_idx_tbl.pui1_file_cntt);
            DBG_INFO((" ui8_moof_offset=0x%llx !!!! \r\n", ui8_moof_offset));
            
            pui1_buf += 8;
            break;
        case MP4_BOX_MDAT:
            pui1_buf += ui4_box_size;
            break;
        case MP4_BOX_MFRA:
        {
            pui1_buf        += 16;
            ui4_ver          = *pui1_buf;    /* ver 1 flag 3 id 4 bit 4*/
            LOADB_DWRD(pui1_buf + 8, ui4_tmp);  
            ui4_tmp = (((ui4_tmp&0x30)>>4)+1) + (((ui4_tmp&0x0C)>>2)+1) + ((ui4_tmp&0x03)+1);

            LOADB_DWRD(pui1_buf + 12, g_m4t_idx_tbl.ui4_key_num);
            pui1_buf += 16; /* to key elem */  
            DBG_INFO(("key num = %d !\n", (UINT32)g_m4t_idx_tbl.ui4_key_num));

            g_m4t_idx_tbl.pt_key_entry = (MP4_MIDXBULD_KEY_ELMT_STRM_ENTRY_T*)x_mem_alloc(sizeof(MP4_MIDXBULD_KEY_ELMT_STRM_ENTRY_T) * g_m4t_idx_tbl.ui4_key_num);
            if(g_m4t_idx_tbl.pt_key_entry == NULL)
            {
               DBG_INFO(("malloc key buf Failed !\n"));
                return MMSBTLR_INV_ARG;                    
            }
            x_memset( g_m4t_idx_tbl.pt_key_entry, 
                    0, 
                    sizeof(MP4_MIDXBULD_KEY_ELMT_STRM_ENTRY_T) * g_m4t_idx_tbl.ui4_key_num);
    
            for(ui4_i=0; ui4_i < g_m4t_idx_tbl.ui4_key_num; ui4_i++)
            {
                UINT32        ui4_high_word  = 0; 
                UINT32        ui4_low_word   = 0; 
                UINT64        ui8_time_len   = 0; 
                UINT64        ui8_ofs_len    = 0; 
                if(ui4_ver == 1)
                {
                    LOADB_DWRD(pui1_buf, ui4_high_word);             
                    LOADB_DWRD(pui1_buf + 4, ui4_low_word);
                    ui8_time_len = ((UINT64)ui4_high_word << 32) | ui4_low_word;
                    
                    LOADB_DWRD(pui1_buf + 8, ui4_high_word); 
                    LOADB_DWRD(pui1_buf + 12, ui4_low_word);
                    ui8_ofs_len = ((UINT64)ui4_high_word << 32) | ui4_low_word;                
                }
                else if(ui4_ver == 0)
                {
                    LOADB_DWRD(pui1_buf, ui8_time_len);
                    LOADB_DWRD(pui1_buf + 4, ui4_low_word);  
                    ui8_ofs_len = ui4_low_word;
                }

                g_m4t_idx_tbl.pt_key_entry[ui4_i].ui8_key_moof_ofst         = ui8_ofs_len;
                g_m4t_idx_tbl.pt_key_entry[ui4_i].ui8_key_ext_duration_sum  = ui8_time_len/10000;    
                
                if(ui4_ver == 1)
                {
                     pui1_buf += 16;   
                }
                else if(ui4_ver == 0)
                {
                     pui1_buf += 8; 
                }
               
                pui1_buf += ui4_tmp;
            } 
        }
            break;

            
        case MP4_BOX_MFHD:
            pui1_buf += 16;
            break;
        case MP4_BOX_TRAF:
            pui1_buf += 8;
            break;                
        case MP4_BOX_TFHD:
        {
            LOADB_DWRD(pui1_buf + 8, ui4_tmp); /*ver & flag */
            pui1_buf += 16;
            
            ui4_sample_dur = g_m4t_idx_tbl.ui4_dft_sample_dur;
            pt_idx_entry = g_m4t_idx_tbl.pt_sample_entry + g_m4t_idx_tbl.ui4_sample_num;
            
            flag_tr = ui4_tmp & 0xFFFFFF;
            
            if(flag_tr & 0x01)/*base_data_offset_present*/
            {   
                UINT32 ui4_high_word = 0;
                UINT32 ui4_low_word  = 0;
                
                LOADB_DWRD(pui1_buf, ui4_high_word);             
                LOADB_DWRD(pui1_buf + 4, ui4_low_word);
                
                ui8_moof_offset =(UINT64) (((UINT64)ui4_high_word << 32) | ui4_low_word);
                DBG_INFO(("ui8_moof_offset=0x %llx\r\n", ui8_moof_offset));
                pui1_buf += 8;
            }
            
            if(flag_tr & 0x02)/*sample_description_index_present*/
            {   
                //LOADB_DWRD(pui1_buf, ui4_sd_idx);
                pui1_buf += 4;
            }
            
            if(flag_tr & 0x08)/*default_sample_duration_present*/
            {
                LOADB_DWRD(pui1_buf, ui4_sample_dur);
                pui1_buf += 4;
            }
            
            if(flag_tr & 0x10)/*default_sample_size_present*/
            {
                LOADB_DWRD(pui1_buf, ui4_sample_size);
                pui1_buf += 4;
            }
            
            if(flag_tr & 0x20)/*default_sample_flags_present*/
            {
                //LOADB_DWRD(pui1_buf, ui4_sample_size);
                pui1_buf += 4;
            }
        }
        break;
        case MP4_BOX_TRUN:
        {
            UINT32   ui4_mem_cnt         = 0;
            //UINT32    ui4_sample_ctts     = 0; /*reset first as no default ctts*/
            UINT32    ui4_sample_size_sum = 0;
            UINT64    ui8_base_offset     = ui8_moof_offset;
            
            pui1_buf += 8;
            LOADB_DWRD(pui1_buf , ui4_tmp);
            LOADB_DWRD(pui1_buf + 4, ui4_mem_cnt);
            pui1_buf += 8;
            
            flag_tr = ui4_tmp & 0xFFFFFF;
            if(flag_tr & 0x01)
            {
                UINT32 ui4_data_ofst = 0;
                /*add moof_ofs is first elem ofs in mdat,not include head8,
                need to modify , default_data_ofs concerned*/
                LOADB_DWRD(pui1_buf, ui4_data_ofst); 
                pui1_buf += 4;
                ui8_base_offset = ui8_moof_offset + ui4_data_ofst;
                DBG_INFO(("ui8_moof_offset=0x%llx, ui4_data_ofst=0x%x, ui8_base_offset=0x%llx!!\n",
                    ui8_moof_offset, ui4_data_ofst, ui8_base_offset));
                
            }
            
            if(flag_tr & 0x04)
            {/* First sample flags present*/
                pui1_buf += 4;
            }
            
            if(g_m4t_idx_tbl.ui4_sample_num + ui4_mem_cnt > SBTL_SAMPLE_MAX)
            {
                DBG_INFO(("Sample total number reached MAX!!!!\n"));
                break;
            }
                
            // read trun box data to get duration,size,to cal pts , ofs;
            for(ui4_i = 0; ui4_i < ui4_mem_cnt; ui4_i++)
            {
                if( flag_tr & 0x100)
                {
                    LOADB_DWRD(pui1_buf, ui4_sample_dur);
                    pui1_buf += 4; 
                }
                if(flag_tr & 0x200)
                {
                    LOADB_DWRD(pui1_buf, ui4_sample_size);                
                    pui1_buf += 4;    
                }
                if( flag_tr & 0x400)
                {                
                    pui1_buf += 4; 
                }
                if( flag_tr & 0x800)
                {
                    //LOADB_DWRD(pui1_buf, 0);//ui4_sample_ctts);                
                    pui1_buf += 4;                 
                }
               DBG_INFO(("#### ui4_sbtl_sample_dur_sum=%d, ui4_sample_dur=%d \r\n",
                            ui4_sbtl_sample_dur_sum, ui4_sample_dur));
                
                pt_idx_entry->ui4_pts  = ui4_sbtl_sample_dur_sum;
                pt_idx_entry->ui4_ofst = ui8_base_offset + ui4_sample_size_sum; 
                pt_idx_entry->ui4_size = ui4_sample_size;  
                pt_idx_entry++;   
                g_m4t_idx_tbl.ui4_sample_num++;
                
                ui4_sbtl_sample_dur_sum += (ui4_sample_dur/g_m4t_idx_tbl.ui4_timescale)*1000;//to ms  
                ui4_sample_size_sum += ui4_sample_size;
            }
        }
        break; 
        default:
            if(ui4_box_size == 0)
            {
                pui1_buf += 8;
            }
            else
            {
                pui1_buf += ui4_box_size;
            }
            break;
        }
    }

    if(g_m4t_idx_tbl.pt_key_entry != NULL)
    {
        for(ui4_i = 0; ui4_i < g_m4t_idx_tbl.ui4_key_num; ui4_i++)
        {
            DBG_INFO(("%d  ui8_key_moof_ofst=0x%llx, ui8_key_ext_duration_sum=%lld !\n", 
                   ui4_i, 
                   g_m4t_idx_tbl.pt_key_entry[ui4_i].ui8_key_moof_ofst, 
                   g_m4t_idx_tbl.pt_key_entry[ui4_i].ui8_key_ext_duration_sum));
        }
    }
    
    if(g_m4t_idx_tbl.pt_sample_entry  == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    for(ui4_i = 0; ui4_i < g_m4t_idx_tbl.ui4_sample_num; ui4_i++)
    {
        DBG_INFO(("%d  pts=%d, ofst=0x%x, size=0x%x !\n", 
               ui4_i, 
               g_m4t_idx_tbl.pt_sample_entry[ui4_i].ui4_pts, 
               g_m4t_idx_tbl.pt_sample_entry[ui4_i].ui4_ofst, 
               g_m4t_idx_tbl.pt_sample_entry[ui4_i].ui4_size));
    }

    
    for(ui4_i = 0; ui4_i < g_m4t_idx_tbl.ui4_sample_num; ui4_i++)
    {
        g_m4t_idx_tbl.pt_sample_entry[ui4_i].pui1_buf =  g_m4t_idx_tbl.pui1_file_cntt + g_m4t_idx_tbl.pt_sample_entry[ui4_i].ui4_ofst;
    }

    pt_private->pt_text_content = g_m4t_idx_tbl.pt_sample_entry[0].pui1_buf;
    pt_engine->aui8_file_size[MM_SBTL_FEEDER_SUBTITLE] = g_m4t_idx_tbl.pt_sample_entry[0].ui4_size;
    
    DBG_INFO(("####### Success: final content pt_private->pt_text_content=0x%x, size=0x%x ####### \r\n", (UINT32)pt_private->pt_text_content, g_m4t_idx_tbl.pt_sample_entry[0].ui4_size));
    return MMSBTLR_OK;
}


UINT8*  m4t_get_buffer( UINT8   ui1_sample_cur_idx, UINT32* pui4_size)
{   
    UINT8   * pui1_buf={0};
    if(ui1_sample_cur_idx < g_m4t_idx_tbl.ui4_sample_num)
    {
        *pui4_size = g_m4t_idx_tbl.pt_sample_entry[ui1_sample_cur_idx].ui4_size;        
        pui1_buf = g_m4t_idx_tbl.pt_sample_entry[ui1_sample_cur_idx].pui1_buf;        
        return pui1_buf;
    }

    return NULL;
}

VOID m4t_set_pts(UINT8   ui1_sample_cur_idx, UINT64 ui8_pts)
{
    if(ui1_sample_cur_idx < g_m4t_idx_tbl.ui4_sample_num)
    {
        g_m4t_idx_tbl.pt_sample_entry[ui1_sample_cur_idx].ui4_pts = (UINT32)(ui8_pts/(PTS_FREQ/1000));              
    } 
}

INT32  get_timecode(CHAR* ps_str,UINT32 *pui4_time)
{
  /*00:00:00:00*/
    UINT32 ui4_time = 0;
    UINT16 ui2_m = 0;
    UINT16 ui2_cnt = 0;
	UINT16 ui2_d   = 1;
    
    if (ps_str == NULL || pui4_time == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    while(*ps_str)
    {
        if (*ps_str >= '0' && *ps_str <= '9')
        {
            ui2_m = ui2_m*10+(*ps_str -'0');
			if (ui2_cnt == 3)
			{
				ui2_d *= 10;
				if (ui2_d >= 1000)break;
			}
        }
        if (*ps_str == ':' ||*ps_str == '.' )
        {
            ui4_time=ui4_time*60+ui2_m;
            ui2_m = 0;
			ui2_d = 1;
            ui2_cnt++;
        }
        ps_str++;
    }
    if (ui2_cnt == 2)
    {
        ui4_time = (ui4_time*60+ui2_m)*1000;/*to ms*/        
    }
    else if (ui2_cnt == 3)
    {
		ui2_m = ui2_m*(1000/ui2_d);
        ui4_time = ui4_time*1000+ui2_m;/*to ms*/
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
    *pui4_time =  (UINT32)((ui4_time)*PTS_FREQ/1000);
    return MMSBTLR_OK;
}



INT32 ipcc_get_xml_p_time(XML_NODE_T *pt_node_p,UINT32 ui4_p_node_idx,
                                UINT64 *pt_pts_start,UINT64 *pt_pts_end)
{   
    CHAR                                *ps_tm_dur;
    CHAR                                *ps_tm_begin;
    CHAR                                *ps_tm_end;
    INT32                                i4_ret = 0;
    UINT32                               ui4_pts_time;    
    XML_T                               *pt_p_node_idx_tree;
        
    pt_p_node_idx_tree = x_sbtl_xml_get_element_byidx(pt_node_p,ui4_p_node_idx);
    if (pt_p_node_idx_tree == NULL)
    {
         return MMSBTLR_INV_ARG;
    }
	do
	{
		ps_tm_begin= x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_BEGIN,pt_p_node_idx_tree);
		if (NULL == ps_tm_begin)
		{
			break;
		}
		i4_ret = get_timecode(ps_tm_begin, &ui4_pts_time);
		if (i4_ret != MMSBTLR_OK)
		{
			break;
		}
		*pt_pts_start = ui4_pts_time;
		
		ps_tm_end = x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_END,pt_p_node_idx_tree);
		if (ps_tm_end)
		{
			i4_ret = get_timecode(ps_tm_end, &ui4_pts_time);
			if (i4_ret != MMSBTLR_OK)
			{
				break;
			}
			*pt_pts_end = ui4_pts_time;
			return MMSBTLR_OK;
		}
		ps_tm_dur = x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_DUR,pt_p_node_idx_tree); 
		if (ps_tm_dur)
		{
			i4_ret = get_timecode(ps_tm_dur, &ui4_pts_time);
			if (i4_ret != MMSBTLR_OK)
			{
				break;
			}
			*pt_pts_end = ui4_pts_time+(*pt_pts_start);        
            return MMSBTLR_OK;
			
		}
		*pt_pts_end = 0xFFFFFFFF;
		return MMSBTLR_OK;
	}while(0);
	return MMSBTLR_INTERNAL_ERROR;
}

UINT8 ipcc_m4t_seek_sample(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                                 PTS_T                     t_pts_current)
{    
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;  
    UINT8                           ui1_sample_idx;
    UINT8                           ui1_sample_cur_idx;   
    
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if(NULL == pt_private)
    {
        ASSERT(0,("private NULL\n"));
    }
    
    ui1_sample_cur_idx = pt_private->ui1_m4t_sample_idx;

    UINT8      ui1_sample_num =(UINT8) (g_m4t_idx_tbl.ui4_sample_num);    
    for(ui1_sample_idx = 0;  ui1_sample_idx < ui1_sample_num-1;ui1_sample_idx++)
    {      
        UINT32      ui4_tm_high =  g_m4t_idx_tbl.pt_sample_entry[ui1_sample_idx+1].ui4_pts*90;        
        if(t_pts_current < ui4_tm_high)
        {            
            ui1_sample_cur_idx = ui1_sample_idx;
            break;
        }
    }
    if(ui1_sample_idx == ui1_sample_num-1)
    {        
        ui1_sample_cur_idx = ui1_sample_idx;
    }
    
    return ui1_sample_cur_idx;
}


INT32 ipcc_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                          PTS_T                     t_pts_current,
                          PTS_T*                    pt_pts_start,
                          PTS_T*                    pt_pts_end,
                          BOOL*                     pb_found)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
    SBTL_ENGINE_T*                  pt_engine;
    UINT32                          ui4_p_node_idx;
    UINT32                          ui4_p_node_idx_num;
    UINT32                          ui4_p_node_num = 0;
    XML_NODE_T*                     pt_node_p;
    PTS_T                           t_pts_start = 0;
    PTS_T                           t_pts_end   = 0;
    INT32                           i4_ret;
   
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));

    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
   if(NULL == pt_private)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
    if(pt_engine == NULL || pb_found == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    pt_node_p  = pt_private->pt_node_p[pt_private->ui1_m4t_sample_idx];
    ui4_p_node_idx = pt_private->ui4_node_p_idx_cur ;    
    ui4_p_node_num = pt_private->ui4_p_node_num;
    DBG_INFO(("========p node contain %d\n=========\n",(INT32)ui4_p_node_num));
    ui4_p_node_idx_num = 0;
    *pb_found = FALSE;
	pt_private->b_add_next = FALSE;
    while(ui4_p_node_idx < ui4_p_node_num)
    {
        i4_ret = ipcc_get_xml_p_time(pt_node_p,ui4_p_node_idx,&t_pts_start,&t_pts_end);
        if(MMSBTLR_OK != i4_ret)
        {
            return MMSBTLR_INV_ARG;
        }
        
        if (*pb_found)
        {
            if (t_pts_start >= *pt_pts_start &&
                t_pts_end >= *pt_pts_end &&
                t_pts_end - *pt_pts_end < PTS_FREQ/10)
            {
                ui4_p_node_idx_num++;
            }
            else
            {
                if (t_pts_start == *pt_pts_end)
                {
                    /*load extra P for fast remove*/
                    BOOL b_enh_rollup = FALSE;
                    i4_ret = x_sbtl_ipcc_get_fct(
                               SBTL_IPCC_GET_TYPE_ENHANCE_ROLLUP_MODE,
                               &b_enh_rollup,
                               sizeof(BOOL));
                    pt_private->b_add_next = b_enh_rollup;
                }
                break;
            }
        }
        else if (t_pts_current < t_pts_end)
        {
            *pb_found = TRUE;
            *pt_pts_start = t_pts_start;
            *pt_pts_end  =  t_pts_end;
            ui4_p_node_idx_num++; 
        }   
        ui4_p_node_idx++;
    }   
    if(*pb_found == TRUE)
    {        
        pt_private->ui4_node_p_idx_cur = ui4_p_node_idx -ui4_p_node_idx_num;
        pt_private->ui4_node_p_idx_num = ui4_p_node_idx_num;

        DBG_INFO(("  Data  Find!@!!get the pt_private->ui4_node_p_idx_cur is %d, and the num is %d\n",pt_private->ui4_node_p_idx_cur,ui4_p_node_idx_num));
        return MMSBTLR_OK;
    }
    else
    {        
        return MMSBTLR_INV_ARG;
    }
}

SBTL_TTML_ATTR_FONT_FAMILY   get_font_family_type_value(CHAR* ps_value)
{
    INT8   i1_idx =0;    
    CHAR  *ps_font_family[8] ={
        "default","monospaceSerif","proportionalSerif",
        "monospaceSansSerif","proportionalSansSerif",
        "Casual<TBD>","Cursive<TBD>","Samll<TBD>"};   
    for( ;i1_idx<8;i1_idx++)
    {
        if(0  == x_strcmp(ps_value, ps_font_family[i1_idx]))
        {
            break;
        }
    }
    switch(i1_idx)
    {
        case 0:
            return SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
        case 1:
            return SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF;
        case 2:
            return SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF;
        case 3:
            return SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF;
        case 4:                    
            return SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF;
        case 5:              
           return SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD;
        case 6:
           return SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD;
       case 7:
           return  SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD;
        default:
            return SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
    }
}

SBTL_TTML_ATTR_TEXT_ALIGIN   get_text_align_type_value(CHAR* ps_value)
{
    INT8    i1_idx =0;    
    CHAR    *ps_t_align[5] ={"start","end","left","right","center"};
    for( ;i1_idx<5;i1_idx++)
    {
        if(0  == x_strcmp(ps_value, ps_t_align[i1_idx]))
        {
            break;
        }
    }
    switch(i1_idx)
    {
        case 0:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_START;
        case 1:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_END;
        case 2:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT;
        case 3:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT;
        case 4:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER;
        default:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_START;
    }
}

SBTL_TTML_ATTR_VISIBILITY   get_visibility_type_value(CHAR* ps_value)
{
    if(0  == x_strcmp(ps_value, "hidden"))
    {
            return SBTL_TTML_ATTR_VISIBILITY_HIDDEN;
    }
   return SBTL_TTML_ATTR_VISIBILITY_VISIBLE;
}

 SBTL_TTML_ATTR_FONT_WEIGHT get_font_weight_type_value(CHAR* ps_value)
 {
     if(0  == x_strcmp(ps_value, "bold"))
     {
             return SBTL_TTML_ATTR_FONT_WEGHT_BOLD;
     }
     return SBTL_TTML_ATTR_FONT_WEGHT_NORMAL;
 }

 SBTL_TTML_ATTR_TEXT_DECORATION get_text_decoration_type_value(CHAR* ps_value)
{
     if(0  == x_strcmp(ps_value, "underline"))
     {
             return SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE;
     }    
     return SBTL_TTML_ATTR_TEXT_DECORATION_NONE;
}

 SBTL_TTML_ATTR_FONT_STYLE get_font_style_type_value(CHAR* ps_value)
{
     if(0  == x_strcmp(ps_value, "italic"))
     {
             return SBTL_TTML_ATTR_FONT_STYLE_ITALIC;
     }    
     return SBTL_TTML_ATTR_FONT_STYLE_NORMAL;
}


SBTL_TTML_ATTR_WHITESAPCE get_white_space_type_value(CHAR* ps_value)
{
    if(0  == x_strcmp(ps_value, "preserve"))
    {
          return SBTL_TTML_ATTR_WHITESPACE_PRESERVE;
    }    
    return SBTL_TTML_ATTR_WHITESPACE_DEFAULT;
}
  
CHAR* sbtl_xml_get_attr_name(SBTL_IPCC_ATTR_T e_attr)
{
    UINT32 ui4_i;
    for (ui4_i = 0;ui4_i<sizeof(t_attr_hash)/sizeof(SBTL_XML_HASH_INFO_T);ui4_i++)
    {
        if (t_attr_hash[ui4_i].i4_value == (INT32)e_attr)
        {
            return t_attr_hash[ui4_i].ps_key;
        }
    }
    return NULL;
}
SBTL_TTML_ATTR_T* ipcc_attr_check_existed(INT32 i4_type)
{
    SBTL_TTML_ATTR_T* pt_attr_s = NULL;
    BOOL              b_found   = FALSE;
    if(!SLIST_IS_EMPTY(&t_attr_cur))
    {                               
           SLIST_FOR_EACH(pt_attr_s, &t_attr_cur , t_attr_link)
           {
                if(pt_attr_s != NULL &&
                   i4_type == pt_attr_s->e_attr)
                {
                    b_found = TRUE;
                    break;
                }
           }
    }
    if (b_found == FALSE)
    {
        pt_attr_s = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
		if (pt_attr_s)
		{
        	SLIST_INSERT_HEAD(pt_attr_s,&t_attr_cur,t_attr_link);
		}
    }
    if (pt_attr_s)
    {
        pt_attr_s->e_attr = (SBTL_IPCC_ATTR_T)i4_type;
    }
    return pt_attr_s;
    
}

INT32 ipcc_iterate_attr(XML_ATTRIBUTE_T*   pt_attr,
                          VOID*              pv_arg,
                          INT32              i4_depth)
{   
    //INT32 i4_ret;
    // static BOOL     b_is_region = FALSE;
    // static BOOL     b_is_style = FALSE;
    SBTL_TTML_ATTR_T* pt_attr_s = NULL;
    if (pt_attr == NULL || 
        NULL == pt_attr->ps_value)
    {
        return MMSBTLR_INV_ARG;
    }
    if (i4_depth > 3 || pv_arg == NULL)
    {
        return MMSBTLR_OK;
    }
        
    if(SBTL_XML_IPCC_ATTR_BGCOLOR == pt_attr->i4_type)
    {
        SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_parse = 
                                     (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pv_arg;
        if (pt_parse->pt_attr_parent &&
            pt_parse->pt_attr_parent->i4_type == SBTL_XML_ELM_REGION)
        {
            pt_attr->i4_type = SBTL_XML_IPCC_ATTR_REGION_BGCOLOR;  
        }
    }
    pt_attr_s = ipcc_attr_check_existed(pt_attr->i4_type);
    if(NULL == pt_attr_s)
    {
        return MMSBTLR_OK;
    }
    switch(pt_attr->i4_type)
    {
       case SBTL_XML_IPCC_ATTR_ID:
       case SBTL_XML_IPCC_ATTR_STYLE:
       case SBTL_XML_IPCC_ATTR_REGION:         
       case SBTL_XML_IPCC_ATTR_FONTSIZE:
       case SBTL_XML_IPCC_ATTR_BGCOLOR:
       case SBTL_XML_IPCC_ATTR_COLOR: 
       case SBTL_XML_IPCC_ATTR_FONT_EXTENT:
       case SBTL_XML_IPCC_ATTR_CELL_RESOLUTION:
       case SBTL_XML_IPCC_ATTR_ORIGIN:                 //origin   
       case SBTL_XML_IPCC_ATTR_WINDOW_OPACITY:  //tts:opacity
       case SBTL_XML_IPCC_ATTR_TEXT_OUTLINE:
       case SBTL_XML_IPCC_ATTR_WRAP_OPTION:
       case SBTL_XML_IPCC_ATTR_LINEHEIGHT:
       case SBTL_XML_IPCC_ATTR_ROLE:
       case SBTL_XML_IPCC_ATTR_REGION_BGCOLOR:
            pt_attr_s->u.ps_value = pt_attr->ps_value;
       break;
        
       case SBTL_XML_IPCC_ATTR_BEGIN:
       case SBTL_XML_IPCC_ATTR_END:                
       case SBTL_XML_IPCC_ATTR_DUR:
       {   
           UINT32 ui4_tm = 0;
           get_timecode(pt_attr->ps_value, &ui4_tm);
           pt_attr_s->u.e_pts_tm = (PTS_T)ui4_tm;
       }
       break;     
       
       case SBTL_XML_IPCC_ATTR_FONTFAMILY:                             
          pt_attr_s->u.e_font_family = get_font_family_type_value(pt_attr->ps_value);       
       break;
      
       case SBTL_XML_IPCC_ATTR_TEXTALIGN:                                     
          pt_attr_s->u.e_text_align = get_text_align_type_value(pt_attr->ps_value);   
       break; 
       
       case SBTL_XML_IPCC_ATTR_VISIBILY:                                         
           pt_attr_s->u.e_visibility = get_visibility_type_value(pt_attr->ps_value);   
       break;
      
       case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:                                            
           pt_attr_s->u.e_font_weight = get_font_weight_type_value(pt_attr->ps_value);
       break;   
       
       case SBTL_XML_IPCC_ATTR_TEXT_DECORATION:                                          
           pt_attr_s->u.e_text_decoration = get_text_decoration_type_value(pt_attr->ps_value);
       break;  
         
       case SBTL_XML_IPCC_ATTR_FONT_STYLE:                                          
           pt_attr_s->u.e_font_style = get_font_style_type_value(pt_attr->ps_value);
       break;
           
       case SBTL_XML_IPCC_ATTR_WHITESPACE:                                          
           pt_attr_s->u.e_whitespace = get_white_space_type_value(pt_attr->ps_value);
           if(SBTL_TTML_ATTR_WHITESPACE_PRESERVE == pt_attr_s->u.e_whitespace)
           {
               b_is_whitspace_valid = TRUE;
           }
       break;
         
       default:                
       break;
    }     

   /* if(pt_attr->i4_type ==  SBTL_XML_IPCC_ATTR_STYLE &&
                          !pt_parse->b_is_span_attr_parse)
    {
         if (pt_attr->ps_value && pt_parse->h_style)
         {                      
            pt_cur = NULL;
            i4_ret = hash_get_obj(pt_parse->h_style,
                                 (INT8*)pt_attr->ps_value,
                                 x_strlen(pt_attr->ps_value),
                                 (VOID**)&pt_cur);
             if (i4_ret == 0 &&
                 pt_cur != NULL)
             {
                b_is_style = TRUE;
                x_sbtl_xml_iterate_all_attr(pt_cur,
                                 ipcc_iterate_attr,
                                 pv_arg,
                                 i4_depth+1);
                
                b_is_style= FALSE;
             }
         }                 
   }
  
   if(pt_attr->i4_type ==  SBTL_XML_IPCC_ATTR_REGION &&
      !pt_parse->b_is_span_attr_parse)
   {
        if (pt_attr->ps_value && pt_parse->h_region)
        {                      
           pt_cur = NULL;
           i4_ret = hash_get_obj(pt_parse->h_region,
                                (INT8*)pt_attr->ps_value,
                                x_strlen(pt_attr->ps_value),
                                (VOID**)&pt_cur);
            if (i4_ret == 0 &&
                pt_cur != NULL)
            {
               b_is_region = TRUE;
               x_sbtl_xml_iterate_all_attr(pt_cur,
                                ipcc_iterate_attr,
                                pv_arg,
                                i4_depth+1);
               
               b_is_region = FALSE;
            }
        }                 
  }*/
    
    return MMSBTLR_OK; 
}
 
BOOL  ipcc_span_string_valid(XML_T           *pt_span_node)
{
     INT32 i4_len = 0;
     if (pt_span_node)
     {
        CHAR*  ps_value = pt_span_node->ps_value;
		XML_ATTRIBUTE_T* pt_attr =  x_sbtl_xml_get_first_attr(pt_span_node);
					
        if (pt_attr && pt_attr->i4_type != SBTL_XML_IPCC_ATTR_FAKESPAN)
        {
			return TRUE;
        }
        i4_len = x_strlen(ps_value);
        if(i4_len ==  (INT32)x_strspn((ps_value),  ( CHAR*) (" \n\t\r\v")))
        {
           return FALSE;
        }
        return TRUE;
     }
     return FALSE;
}

static INT32 _ipcc_get_single_attr_list(XML_T*                  pt_span_node,
                                      SBTL_TTML_ELEMENT_P_T*  pt_p)
{            
    SBTL_TTML_ATTR_T*               pt_iter;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span_old = NULL ;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span = NULL;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span_cur = NULL;

	
    if(b_is_whitspace_valid == FALSE &&
       (FALSE == ipcc_span_string_valid(pt_span_node)))
    { 
    	return MMSBTLR_OK;
    }
    pt_span = x_mem_alloc(sizeof(SBTL_TTML_ELEMENT_SPAN_T));        
    if(NULL == pt_span)
    {
        DBG_INFO(("pt_span malloc fail!\n"));
        return MMSBTLR_INV_ARG;
    }
    pt_span_cur = SLIST_FIRST(&(pt_p->t_element_span_list));   
     
    pt_span->ps_content = pt_span_node->ps_value; 
	if (pt_span_cur == NULL)/*first span*/
	{
		SLIST_INSERT_HEAD(pt_span, &(pt_p->t_element_span_list),t_element_span_link); 
	}
	else
	{
		while(pt_span_cur)
		{
			pt_span_old = pt_span_cur;
            pt_span_cur = SLIST_NEXT(pt_span_old,t_element_span_link);
		}
		SLIST_INSERT_AFTER(pt_span,pt_span_old,t_element_span_link);
	}

    SLIST_INIT(&pt_span->t_attr_list); 
         
    SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
    {                   
        SBTL_TTML_ATTR_T* pt_new_iter = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
        if (NULL != pt_new_iter)
        {                    
            x_memcpy(pt_new_iter,pt_iter,sizeof(SBTL_TTML_ATTR_T));
            SLIST_INSERT_HEAD(pt_new_iter,&pt_span->t_attr_list,t_attr_link);
        }
    }
	
    return MMSBTLR_OK;
}
INT32 ipcc_free_attr()
{
     SBTL_TTML_ATTR_T* pt_attr_cur = NULL;
     pt_attr_cur = SLIST_FIRST(&t_attr_cur);
     while(pt_attr_cur)
     {
        SBTL_TTML_ATTR_T* pt_next = SLIST_NEXT(pt_attr_cur,t_attr_link);
        SLIST_REMOVE(pt_attr_cur,t_attr_link);
        x_mem_free(pt_attr_cur);
        pt_attr_cur = pt_next;
     } 
     return MMSBTLR_OK;
}

INT32 ipcc_load_p_node(SBTL_PAGE_INST_MNGR_T*    pt_mngr,UINT32 ui4_idx, BOOL b_next)
{
    XML_T*                        pt_p_idx_node;    
    XML_T*                        pt_span_node;
    UINT32                        ui4_p_node_idx;
    SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_private;
    UINT8                         ui1_sample_cur_idx;
    SBTL_TTML_ATTR_T*             pt_iter;
    SBTL_TTML_ELEMENT_P_T*        pt_p = NULL;
    SBTL_TTML_ELEMENT_P_T*        pt_p_cur = NULL;
    SBTL_TTML_ELEMENT_P_T*        pt_p_old = NULL;
    BOOL                          b_white  = FALSE;
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ui1_sample_cur_idx = pt_private->ui1_m4t_sample_idx;
    
    ui4_p_node_idx = pt_private->ui4_node_p_idx_cur+ui4_idx;
    pt_p_idx_node = x_sbtl_xml_get_element_byidx(pt_private->pt_node_p[ui1_sample_cur_idx],ui4_p_node_idx);
	if (pt_p_idx_node == NULL)
	{
		return MMSBTLR_NOT_FOUND;
	}
    
    SLIST_INIT(&t_attr_cur);        
    x_sbtl_xml_iterate_all_attr(pt_p_idx_node,
                     ipcc_iterate_attr,
                     (VOID*) pt_private,
                     0);
    
    pt_p = x_mem_alloc(sizeof(SBTL_TTML_ELEMENT_P_T));          
    if(NULL == pt_p) 
    {        
        return MMSBTLR_INV_ARG;
    }
    pt_p_cur = SLIST_FIRST(&t_element_p_list);   
    pt_p_old = NULL ;  
    
    if(ui4_idx == 0)
    {/*the first p insert to list*/
        SLIST_INSERT_HEAD(pt_p,&t_element_p_list,t_element_p_link);
    }
    else
    {
        do
        {
            pt_p_old = pt_p_cur;
            pt_p_cur = SLIST_NEXT(pt_p_old,t_element_p_link);
          
        }while(pt_p_cur);

        SLIST_INSERT_AFTER(pt_p,pt_p_old,t_element_p_link);
    }

    SLIST_INIT(&pt_p->t_common_attr_list);

    SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
    {   
        SBTL_TTML_ATTR_T*       pt_new_iter;
		if (b_next &&
			pt_iter->e_attr != SBTL_XML_IPCC_ATTR_BEGIN)
		{
			continue;
		}
        /*if (pt_iter->e_attr == SBTL_XML_IPCC_ATTR_BEGIN ||
            pt_iter->e_attr == SBTL_XML_IPCC_ATTR_END   ||
            pt_iter->e_attr == SBTL_XML_IPCC_ATTR_DUR)
        {
            continue;
        }*/
        pt_new_iter = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
        if(NULL != pt_new_iter)
        {
            x_memcpy(pt_new_iter,pt_iter,sizeof(SBTL_TTML_ATTR_T));
            SLIST_INSERT_HEAD(pt_new_iter,&(pt_p->t_common_attr_list),t_attr_link);
        } 
		if (b_next)
		{
			/*only add begin attr*/
			break;
		}
     } 
    
     SLIST_INIT(&pt_p->t_element_span_list);
	 
	 if (b_next)
	 {
	 	return MMSBTLR_OK;
	 }
     
     pt_span_node = x_sbtl_xml_get_first_element(SBTL_XML_ELM_SPAN,pt_p_idx_node);
    //load the unit 
     ipcc_free_attr();
     b_white = b_is_whitspace_valid;
     while(pt_span_node)
     {       
         SLIST_INIT(&t_attr_cur);
         b_is_whitspace_valid = FALSE;
         x_sbtl_xml_iterate_all_attr(pt_span_node,
                     ipcc_iterate_attr,
                     (VOID*) pt_private,
                     0);  
         if (b_is_whitspace_valid == FALSE)
         {
            b_is_whitspace_valid = b_white;
         }
         _ipcc_get_single_attr_list(pt_span_node, pt_p);
         ipcc_free_attr();
         pt_span_node = x_sbtl_xml_get_next_element(SBTL_XML_ELM_SPAN,pt_span_node);    
     }   
   
     return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
INT32 ipcc_load_string(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
  
    UINT32              ui4_p_node_idx;    
    UINT32              ui4_p_node_idx_num;    
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
       
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));
    SLIST_INIT(&t_element_p_list);    
   
    ui4_p_node_idx_num = pt_private->ui4_node_p_idx_num;
    for(ui4_p_node_idx = 0;ui4_p_node_idx < ui4_p_node_idx_num;ui4_p_node_idx++)
    {
        ipcc_load_p_node(pt_mngr,ui4_p_node_idx, FALSE);
    }
	if (pt_private->b_add_next)
	{
		ipcc_load_p_node(pt_mngr,ui4_p_node_idx, TRUE);
	}

    return MMSBTLR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT32 ipcc_timer_period(SBTL_PAGE_INST_MNGR_T*      pt_mngr)
{
    ASSERT(pt_mngr->pt_engine->t_cfg.i4_speed > 0, ("pt_mngr->pt_engine.t_cfg.i4_speed=0\n"));

    return (UINT32)((TIMER_PERIOD * 100)/pt_mngr->pt_engine->t_cfg.i4_speed);
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
INT32 ipcc_init(SBTL_PAGE_INST_MNGR_T*   pt_mngr, VOID* pv_buffer)
{
    
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_ENGINE_T*                 pt_engine;
    INT32                          i4_ret         = MMSBTLR_OK;
    HANDLE_T                       h_elm_hash     = NULL_HANDLE;
    HANDLE_T                       h_attr_hash    = NULL_HANDLE;
	UINT8                          ui1_sample_idx = 0;
    
    if(pt_mngr == NULL)
    {
        return MMSBTLR_INV_ARG;
    } 
	pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_engine  = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
	
    if(pt_engine == NULL || pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    if (h_elm_hash == NULL_HANDLE)
    {
         x_sbtl_xml_create_hash(t_elm_hash,
                                sizeof(t_elm_hash)/sizeof(SBTL_XML_HASH_INFO_T),
                                &h_elm_hash);
    }
    if (h_attr_hash == NULL_HANDLE)
    {
        x_sbtl_xml_create_hash(t_attr_hash,
                               sizeof(t_attr_hash)/sizeof(SBTL_XML_HASH_INFO_T), 
                               &h_attr_hash);
    }
    
    if(h_elm_hash  == NULL_HANDLE ||
       h_attr_hash == NULL_HANDLE)
    {
        return MMSBTLR_INV_ARG;        
    }

    pt_private->pt_parser_info = x_mem_alloc(sizeof(SBTL_XML_PARSER_INFO_T));
    if (NULL == pt_private->pt_parser_info)
    {
        return MMSBTLR_INV_ARG;        
    }
   
    pt_private->pt_parser_info->h_attr_hash = h_attr_hash;    
    pt_private->pt_parser_info->h_ele_hash= h_elm_hash;
    
    /*  get xml_parser list */    
    if(pt_engine->t_cfg.b_is_xml_m4t)
    {   
        //get the m4t subtitle content        
        pt_private->ui1_m4t_sample_idx = 0;
        i4_ret = i4_get_m4t_content(pt_mngr, pv_buffer);        
        if(i4_ret == MMSBTLR_INV_ARG)
        {   
            return i4_ret;
        }        
        pt_private->ui4_sample_num = g_m4t_idx_tbl.ui4_sample_num;
    }
    else
    {     
        pt_private->ui4_sample_num = 1;
        pt_private->pt_text_content = (UINT8*)pv_buffer;
    }     


   for(ui1_sample_idx = 0;ui1_sample_idx<pt_private->ui4_sample_num;ui1_sample_idx++)
   {    
        XML_NODE_T*            pt_node_p     = NULL;
        XML_T*                 pt_xml_tree   = NULL;
        UINT32                 ui4_data_size = 0; 
        if(ui1_sample_idx >= SAMPLE_NUM_MAX)
        {
            break;
        }
        if (pt_engine->b_is_stop_req)
        {
            break;
        }

        DBG_INFO(("===Parse sample:(%d/%d)====\n",pt_private->ui4_sample_num,ui1_sample_idx));            
        if(pt_engine->t_cfg.b_is_xml_m4t)
        {
            pt_private->pt_text_content = m4t_get_buffer(ui1_sample_idx,&ui4_data_size);                        
        }  
        else
        {
           ui4_data_size =  (UINT32)pt_engine->aui8_file_size[MM_SBTL_FEEDER_SUBTITLE];
        }
        if(NULL == pt_private->pt_text_content)
        {
            DBG_LOG_PRINT(("===m4t get NULL buffer====\n"));              
            continue;
        }
        i4_ret = x_sbtl_xml_parse_by_memory((VOID*)pt_private->pt_text_content,
                                            ui4_data_size,
                                            (VOID**)&pt_xml_tree,
                                            pt_private->pt_parser_info);
        if (MMSBTLR_OK != i4_ret)
        {   
            pt_mngr->b_parse_done = FALSE;                               
            i4_ret = MMSBTLR_INV_ARG;
            break;
        }
        
        pt_private->pt_xml_tree[ui1_sample_idx] = pt_xml_tree;
        i4_ret = x_sbtl_xml_get_elements_bytype(pt_xml_tree,
                                                &pt_node_p,
                                                SBTL_XML_ELM_PARA); 
        if(MMSBTLR_OK  == i4_ret)
        {
           UINT32 ui4_node_num = 0;
           UINT64 ui8_pts[2]   = {0,0};
           pt_private->pt_node_p[ui1_sample_idx]= pt_node_p;
           if(pt_engine->t_cfg.b_is_xml_m4t)
           {
               i4_ret = x_sbtl_xml_get_element_num(pt_node_p, &ui4_node_num);  
               if (ui4_node_num > 0)
               {
                   i4_ret = ipcc_get_xml_p_time(pt_node_p, 0,&ui8_pts[0],&ui8_pts[1]);
                   if(MMSBTLR_OK == i4_ret)
                   {
                      m4t_set_pts(ui1_sample_idx,ui8_pts[0]);
                   }
               }
           } 
        }
    }

    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
INT32 ipcc_deinit(SBTL_PAGE_INST_MNGR_T* pt_mngr)
{   
    SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_private;
    SBTL_ENGINE_T*                pt_engine;
    UINT32                        ui1_sample_idx;
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if(pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    } 

    pt_engine =pt_mngr->pt_engine;
    if(pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    } 
    
    if(NULL != pt_private->pt_parser_info)
    {
        x_sbtl_xml_free_hash(pt_private->pt_parser_info->h_attr_hash);
        x_sbtl_xml_free_hash(pt_private->pt_parser_info->h_ele_hash);
        x_memset(pt_private->pt_parser_info, 0, sizeof(SBTL_XML_PARSER_INFO_T));
        x_mem_free(pt_private->pt_parser_info);
    }
    for(ui1_sample_idx = 0;ui1_sample_idx< SAMPLE_NUM_MAX;ui1_sample_idx++)
    {   
         if(NULL !=pt_private->pt_node_p[ui1_sample_idx])
        {
            x_sbtl_xml_free_node(pt_private->pt_node_p[ui1_sample_idx]);
        }
        if(NULL !=pt_private->pt_xml_tree[ui1_sample_idx])
        {
            x_sbtl_xml_parse_free(pt_private->pt_xml_tree[ui1_sample_idx]);
        }
    }
    
    if(NULL != pt_private->pt_text_content &&
       pt_engine->t_cfg.b_is_xml_m4t == FALSE)
    {
         x_mem_free(pt_private->pt_text_content);/*m4t free full content include*/  
    }
    
    pt_private->pt_text_content = NULL;
    if(pt_engine->t_cfg.b_is_xml_m4t == TRUE)
    {        
        if (g_m4t_idx_tbl.pt_key_entry != NULL)
        {
             x_mem_free(g_m4t_idx_tbl.pt_key_entry);
             g_m4t_idx_tbl.pt_key_entry = NULL;
        }

        if (g_m4t_idx_tbl.pt_sample_entry != NULL)
        {
            x_mem_free(g_m4t_idx_tbl.pt_sample_entry);
            g_m4t_idx_tbl.pt_sample_entry = NULL;
        }
        if (g_m4t_idx_tbl.pui1_file_cntt != NULL)
        {
            x_mem_free(g_m4t_idx_tbl.pui1_file_cntt);
            g_m4t_idx_tbl.pui1_file_cntt = NULL;
        }
    }
    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
VOID ipcc_start(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    return ;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
VOID ipcc_stop(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{   
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
/*
00:00:05,892 --> 00:00:10,730
*/
BOOL ipcc_parser_verify(SBTL_ENGINE_T*   pt_engine)
{
    UINT8          ui1_tmp[IPCC_XML_MAX_HEAD_LEN] = {0};
    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
    sbtl_feeder_inpnbyte(pt_engine, 
                         IPCC_M4T_HEAD_FLAG_LEN, 
                         ui1_tmp, 
                         NULL, 
                         MM_SBTL_FEEDER_SUBTITLE);
    
    if((0x66 == ui1_tmp[4]) && (0x74 == ui1_tmp[5]) && 
       (0x79 == ui1_tmp[6]) && (0x70 == ui1_tmp[7]))
    {    
        pt_engine->t_cfg.b_is_xml_m4t = TRUE;
    }
    else
    {  
        UINT32 ui4_i = 0;
        sbtl_feeder_inpnbyte(pt_engine, 
                             IPCC_XML_MAX_HEAD_LEN - IPCC_M4T_HEAD_FLAG_LEN, 
                             ui1_tmp + IPCC_M4T_HEAD_FLAG_LEN, 
                             NULL,
                             MM_SBTL_FEEDER_SUBTITLE);
        
        ui1_tmp[IPCC_XML_MAX_HEAD_LEN-1] = 0;
        while (ui1_tmp[ui4_i])
        {
            ui4_i++;
            if (ui1_tmp[ui4_i-1] == '<')
            {
                if (x_strncmp((CHAR*)ui1_tmp + ui4_i, "?xml" , 4) == 0)
                {
                    break;
                }
            }
        }
        if (ui1_tmp[ui4_i] == 0)
        {
            return FALSE;
        }
        while (ui1_tmp[ui4_i])
        {
            ui4_i++;
            if (ui1_tmp[ui4_i-1] == '<')
            {
                if (x_strncmp((CHAR*)ui1_tmp + ui4_i, "tt" , 2) == 0)
                {
                    break;
                }
            }
        }
        if (ui1_tmp[ui4_i] == 0)
        {
            return FALSE;
        }
        
        pt_engine->t_cfg.b_is_xml_m4t = FALSE;
    }
     
    return TRUE;
}

#endif /*__KERNEL__*/


