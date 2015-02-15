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
 * $RCSfile: sbtl_utils.c,v $
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
#include <stdio.h>
#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif
#include "x_common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"

#include "x_rm_dev_types.h"
#include "x_stc_ctrl.h"
#include "mutil/sbtl_engine/sbtl_widget.h"

#include "u_fm.h"
#include "x_fm.h"

#ifdef INET_SUPPORT
#ifdef USE_SMART_FILE
#include "x_smart_file.h"
#else
#include "x_httpc_api.h"
#endif
#endif
#include "mutil/mm_util_input.h"
#ifdef MW_SM_DVBT_SUPPORT
#include "u_dvb_si_eng.h"
#include "tbl_mngr/dvb_si_eng/dvb_si_txt_api.h"
#endif

#include "x_rm_dev_types.h"
#include "res_mngr/rm.h"
#include "x_rm.h"
#include "x_uc_str.h"

#include "u_sm.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_drv.h"
#include "x_mm_sbtl_engine.h"
#include "mutil/sbtl_engine/sbtl_widget.h"

#else /*__KERNEL__*/
#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"

#include "../mm_util_input.h"
#include "x_uc_str.h"
#ifdef MW_SM_DVBT_SUPPORT
#include "u_dvb_si_eng.h"
#include "dvb_si_txt_api.h"
#endif

#include "sbtl_dbg.h"
#include "sbtl_struct.h"
#include "sbtl_utils.h"
#include "sbtl_register.h"
#include "sbtl_state.h"
#include "sbtl_drv.h"
#include "x_mm_sbtl_engine.h"
#include "sbtl_widget.h"

#endif /*__KERNEL__*/

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define SM_SBTL_PES_BUF_DEFAULT_SIZE        ((SIZE_T)(1024 * 8))
#define SM_SBTL_PES_BUF_INC_SIZE            ((SIZE_T)(1024))


/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/

#define SIMU_DATA_END           0xFFFFFFFF
#define SIMU_PTS_FREQ           ((UINT64)(90000))
#define SIMU_TIMER_PERIOD       100

static BOOL              b_simu_mode    = FALSE;
static PTS_T             t_simu_pts     = 0;
static HANDLE_T          h_simu_timer   = NULL_HANDLE;   
static sbtl_simu_data_cb pf_simu_cb     = NULL;
/*----------------------------For Simu mode-----------------------------------*/

static MM_SBTL_RECORD_INF*  _gpt_sbtl_rcd = NULL;


static BOOL match_special(CHAR s, CHAR c)
{
    switch(s) 
    {         
        case 'd':
            if (!IS_NUMBER(c))return FALSE;
            break;
        case 's':
            if (c != ' ')return FALSE;
            break;
        case 't':
            if (c != '\t')return FALSE;
            break;
        case 'w':
            if (!IS_WORD(c))return FALSE;
            break;
       case 'x':
            if (!IS_HEX(c))return FALSE;
            break;
        case 'W':
            if (IS_WORD(c))return FALSE;
            break;
        default:
            if (c != s)return FALSE;
            break;
    }
    return TRUE;
}

static const CHAR* match_char(const CHAR* format, CHAR c)
{
    BOOL f = FALSE;
    BOOL r = FALSE;
    while(*format && *format != ']')
    {
        if (f == FALSE)
        {
            if (*format == '\\')
            {
                format++;
                if (match_special(*format,c) == TRUE)
                {
                    if (r) return NULL;
                    f=TRUE;
                }
            }
            else if (*format == '^')
            {
                r = TRUE;
            }
            else
            {   
                if (c == *format)
                {
                    if (r)return NULL;
                    else f=TRUE;
                }
            }
        }
        format++;
    }
    if ((f || r) && *format == ']') return format;
    return NULL;
}
 
static CHAR match_single(const CHAR* format, CHAR* str,MEM_T* m,UINT16 *pos)
{
    CHAR spe = 0;
    CHAR *start = str;
    const CHAR *multi = format;
    UINT16 rec = 0;
    BOOL ignore=FALSE;
    CHAR now;
    if (*format == 'i')
    {
        format++;
        ignore=TRUE;
    }
    while(*format)
    {
        if (*str)
        {
            if (ignore && *str >= 'a' && *str <= 'z')
            {
                now = *str+'A'-'a';
            }
            else
            {
                now = *str;
            }
            if (*format == '\\')/*special char*/
            {
                format++;
                if (match_special(*format,now) == FALSE)
                {
                    if (*(format+1) && *(format+1) == '*')
                    {
                         format+=2;
                         continue;
                    }
                    else
                    {
                        return 'N';
                    }
                }
                spe = *format;
            }
            else
            {   
                if (*format == '[')
                {
                    multi = format+1;
                    spe = '[';
                    format = match_char(multi,now);
                    if ( format == NULL)
                    {
                        return 'N'; 
                    }
                    else
                    {
                        format++;
                        str++;
                        continue;
                    }
                }
                else if (*format == '.')
                {  
                }
                else if (*format == '+' || *format == '*')
                {
                    BOOL ms;
                    if (spe)
                    {
                        if (spe == '[')
                        {
                            const CHAR *cur = match_char(multi,now);
                            ms = (cur!=NULL);
                        }
                        else
                        {
                            ms = match_special(spe,now);
                        }
                    }
                    else
                    {
                        ms = (*(format-1) == now);
                    }
                    if (ms)
                    {
                        str++; 
                    }
                    else
                    {
                        format++;
                    }
                    continue;
                }
                else if (*format == '(' || *format == ')')
                {
                    if (m == NULL || m->len < rec)
                    {
                        return 'O';
                    }
                    if (*format == '(')
                    {
                        m->start[rec] = str-start;
                        rec++;
                    }
                    else
                    {
                        if (rec > 0)
                        {
                            rec--;
                            m->end[rec] = str-start-1;
                            if (m->end[rec] < m->start[rec])
                            {
                                m->end[rec] = m->start[rec];
                            }
                            rec++;
                        }
                        else
                        return 'O';
                    }
                    format++;
                    continue; 
                } 
                else
                {
                    if (now != *format)
                    {
                        if (*(format+1) == '*')
                        {
                             format+=2;
                             continue;
                        }
                        else
                        {
                            return 'N';
                        }
                    }
                    spe = 0; 
                }
            }
        }
        else
        {   
            if (*format == ')' && rec > 0)
            {
                rec--;
                m->end[rec] = str-start-1;
                if (m->end[rec] < m->start[rec])
                {
                    m->end[rec] = m->start[rec];
                }
                rec++;
                format++;
                continue;
            }
            else if (*format == '+'  || *format == '*')
            {
                format++;
                continue;
            }
            else
            {
                return '0';
            }
        }
        format++;
        str++;
    }
    if (pos)
    *pos = str-start;
    return 'Y';
}
/*
@format your regular expression:
  .match any 
  \w match a-z A-z _ and number
  \x hex
  \W match not \w
  \d match number
  \s or ' ' \t match space and tab
  lead ^ match from start
  lead i match for case not sensitive (should guarantee format string is all lowcase or uppercase)
  
modifier:
   +match one or more
   *match 0 or more
   +*both are lazy and match as many as possible .+ .*should not use
   [...] match if char contain in[ ] 
   [^...] match the char not in [] should not recursive
   (...) get the match pattern string position and put to mem if found
@str your string want to be matched
@m  if () contain in your format if found put the found position into, len should be enough
@pos return the matched str end position
example: ^123 will match 1234 not 234
        iabc  will match abc or Abc
        [\w+ ]+ could match I love China
        (\d+)\+(\d+) could match 123+345 and put 123 and 234 position 0 2 4 6 to m
*/

BOOL match_reg_exp(const CHAR* format, CHAR* str,MEM_T *m,UINT16 *pos)
{
    if (format == NULL|| str == NULL)
    {
        return FALSE;
    }
    if (pos)*pos = 0;
    if (*format == '^')
    {
        if (match_single(format+1,str,m,pos) == 'Y')
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        UINT16 i = 0;
        UINT16 j=0;
        for(;*str;str++,i++)
        {
            CHAR res = match_single(format,str,m,pos);
            if ( res == 'Y')
            {
                if (m != NULL)
                {
                    for(;j<m->len;j++)
                    {
                        m->start[j]+=i;
                        m->end[j]+=i;
                    }
                }
                if (pos)
                {
                    *pos+=i;
                }   
                return TRUE;
            }
            else if (res == 'O')
            {
                return FALSE;
            }
        }
        return FALSE;
    }
}
extern VOID sbtl_util_set_simu_mode(BOOL b_simu)
{
	b_simu_mode = b_simu;
}

extern VOID sbtl_util_set_simu_cb(sbtl_simu_data_cb pf_cb)
{
	pf_simu_cb = pf_cb;
}
INT32 sbtl_util_close_file(HANDLE_T h_file,
	                        BOOL      b_net)
{
  #ifndef __KERNEL__
    if (h_file != NULL_HANDLE)
    {
        if (b_net)
        {
        #ifdef INET_SUPPORT
            #ifdef USE_SMART_FILE
            x_smart_file_close((int)h_file);
            #else             
            x_httpc_fm_close(h_file);
            #endif
        #endif
        }
        else
        {
        #ifndef ANDROID
            x_fm_close(h_file);
        #else
            fclose((FILE*)h_file);
        #endif
        } 
        return MMSBTLR_OK;
    }
#endif
    return MMSBTLR_OK;
	
}
INT32 sbtl_util_open_file(const CHAR* ps_path,
	                        BOOL   b_net,
	                        HANDLE_T* ph_handle)
{
	INT32  i4_ret = MMSBTLR_OK;
	if (ps_path == NULL || ph_handle == NULL)
	{
		return MMSBTLR_INV_ARG;
		
	}
#ifndef __KERNEL__
	if (b_net)
	{
#ifdef INET_SUPPORT
			#ifndef USE_SMART_FILE
			i4_ret = x_httpc_fm_open_proxy_agent_timeout_cookie(
                                    NULL, 
                                    NULL, 
                                    NULL, 
                                    0,
                                    128*1024,
                                    ph_handle,
                                    NULL);
			#else
			i4_ret = x_smart_file_open((char*)ps_path, (int*)ph_handle);
			#endif
#else
		return MMSBTLR_NOT_SUPPORT;
#endif
	}
	else
	{
#ifdef ANDROID
       FILE *pt_file = NULL;
        pt_file = fopen (ps_path, "r");
        if(pt_file == NULL)
        {
            return MMSBTLR_NOT_SUPPORT;
        }
        *ph_handle = (HANDLE_T)pt_file;
#else
		i4_ret = x_fm_open( FM_ROOT_HANDLE, 
                            ps_path,
                            FM_READ_ONLY, 
                            0777,
                            FALSE, 
                            ph_handle);   
#endif
	}
#endif
	return i4_ret;
	
}

/*-----------------------------------------------------------------------------
                    functions declarations
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
static BOOL _sbtl_dmx_pes_nfy_fct (VOID*            pv_nfy_tag,
                                   DEMUX_COND_T     e_nfy_cond,
                                   UINT32           ui4_data_1,
                                   UINT32           ui4_data_2,
                                   UINT32           ui4_data_3
                            #ifndef MW_CLASSIC_DMX_NFY
                                   ,
                                   UINT64           ui8_ext_data_1,
                                   UINT64           ui8_ext_data_2,
                                   UINT32           ui4_ext_data_3
                            #endif
                                   )
{
    MM_SBTL_HDLR_MSG_T  t_msg;
    SBTL_ENGINE_T*      pt_engine;
    
    pt_engine = (SBTL_ENGINE_T*) pv_nfy_tag;
    if (e_nfy_cond == DEMUX_COND_OVERFLOW)
    {
        return TRUE;
    }
    
    if (e_nfy_cond == DEMUX_COND_DATA_REC)
    {
        if (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_DIVX ||
			pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_TS_PGS)
        {
            mm_sbtl_hdlr_send_dmx_msg(t_msg, 
                                      pt_engine, 
                                      e_nfy_cond, 
                                      ui4_data_1, 
                                      ui4_data_2, 
                                      ui4_data_3, 
                                      0, 
                                      0, 
                                      0);
        }
        else if (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MKV_VOBSUB ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MKV_SRT    ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MKV_SSA    ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_PS_VOBSUB  ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MP4_BLURAY ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MP4_VOBSUB ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_XML_MSS    ||
                 pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MKV_PGS)
        {
            MKV_PACK_T* pt_mkv_pack;

            if (ui4_data_1 == 0)
            {
                return FALSE;
            }
            
            pt_mkv_pack = (MKV_PACK_T*)ui4_data_1;
            mm_sbtl_hdlr_send_dmx_msg(t_msg, 
                                      pt_engine, 
                                      e_nfy_cond, 
                                      pt_mkv_pack->ui4_data_1, 
                                      pt_mkv_pack->ui4_data_2, 
                                      pt_mkv_pack->ui4_data_3, 
                                      pt_mkv_pack->ui4_pts, 
                                      pt_mkv_pack->ui4_duration, 
                                      0);
        }
        else
        {
            return FALSE;
        }
        return TRUE;
    }
    
    return FALSE;
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
INT32 sbtl_dmx_open(SBTL_ENGINE_T*      pt_engine,
                    HANDLE_T            h_source)
{
	
    SBTL_DMX_PES_FMT_T   e_pes_fmt;
	
	if (b_simu_mode)
	{
    	return MMSBTLR_OK;
	}

    switch (pt_engine->t_cfg.e_sbtl_type)
    {
    case MM_SBTL_TYPE_DIVX:
	case MM_SBTL_TYPE_TS_PGS:
        e_pes_fmt = SBTL_DMX_PES_DIVX;
        break;
        
    case MM_SBTL_TYPE_PS_VOBSUB:
        e_pes_fmt = SBTL_DMX_PES_PS;
        break;
        
    case MM_SBTL_TYPE_MKV_VOBSUB:
    case MM_SBTL_TYPE_MKV_SRT:
    case MM_SBTL_TYPE_MKV_SSA:
    case MM_SBTL_TYPE_MP4_BLURAY:
    case MM_SBTL_TYPE_MP4_VOBSUB:
    case MM_SBTL_TYPE_XML_MSS:
	case MM_SBTL_TYPE_MKV_PGS:
        e_pes_fmt = SBTL_DMX_PES_MKV;
        break;

    default:
        return MMSBTLR_NOT_SUPPORT;
    }
    
    return sbtl_dmx_open_drv_ex(pt_engine->t_cfg.e_sbtl_type, h_source, &pt_engine->h_dmx, e_pes_fmt);
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
INT32 sbtl_dmx_close(SBTL_ENGINE_T*    pt_engine)
{
	if (b_simu_mode)
	{
		return MMSBTLR_OK;
	}

    sbtl_dmx_close_drv(pt_engine->h_dmx);
    pt_engine->h_dmx = NULL_HANDLE;
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
static VOID _simu_timer_cb_fct(HANDLE_T     pt_tm_handle,
                                    VOID*        pv_tag)
{
    t_simu_pts += ((SIMU_TIMER_PERIOD * SIMU_PTS_FREQ)/1000);
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
static INT32 _simu_timer_start(VOID)
{
    INT32   i4_ret;

    if (h_simu_timer == NULL_HANDLE)
    {
		i4_ret = x_timer_create(&h_simu_timer);
	    if (i4_ret != OSR_OK)
	    {
	        return MMSBTLR_CORE;
	    }
    }
    i4_ret = x_timer_start(h_simu_timer,
                           SIMU_TIMER_PERIOD,
                           X_TIMER_FLAG_REPEAT,
                           _simu_timer_cb_fct,
                           (VOID*)h_simu_timer);
                    
    if (i4_ret != OSR_OK)
    {
        return MMSBTLR_CORE;
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
static VOID _simu_timer_stop(VOID)
{
    if (h_simu_timer)
    {
        x_timer_delete(h_simu_timer);
        h_simu_timer = NULL_HANDLE;
        t_simu_pts   = 0;
    }
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
INT32 sbtl_dmx_enable(SBTL_ENGINE_T*   pt_engine)
{
    if (b_simu_mode)
    {
		return MMSBTLR_OK;
	}
    if (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_PS_VOBSUB ||
		pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_TS_PGS)
    {
        return sbtl_dmx_enable_drv_ex(pt_engine->t_cfg.e_sbtl_type,
			                       pt_engine->h_dmx,
                                   pt_engine->t_cfg.t_subtitle_ps.ui2_strm_id,
                                   _sbtl_dmx_pes_nfy_fct,
                                   (VOID*)pt_engine);

    }
	else if(pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MP4_BLURAY ||
             pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_MP4_VOBSUB)
    {
         return sbtl_dmx_enable_drv_ex(pt_engine->t_cfg.e_sbtl_type,
			                       pt_engine->h_dmx,
                                   pt_engine->t_cfg.t_subtitle_mp4.ui4_stream_id,
                                   _sbtl_dmx_pes_nfy_fct,
                                   (VOID*)pt_engine);
    }
    else
    {
        return sbtl_dmx_enable_drv_ex(pt_engine->t_cfg.e_sbtl_type,
			                       pt_engine->h_dmx,
                                   pt_engine->t_cfg.t_subtitle_avi.ui4_stream_id,
                                   _sbtl_dmx_pes_nfy_fct,
                                   (VOID*)pt_engine);
    }
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
INT32 sbtl_dmx_disable(SBTL_ENGINE_T*   pt_engine)
{
	if (b_simu_mode)
   {
	   return MMSBTLR_OK;
   }

   return sbtl_dmx_disable_drv(pt_engine->h_dmx);
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
INT32 sbtl_stc_open(SBTL_ENGINE_T*     pt_engine)
{
	if (b_simu_mode)
	{
		 return _simu_timer_start();
	}

    return sbtl_stc_open_drv(&(pt_engine->h_stc), 
                             pt_engine->t_cfg.s_dest_grp_name);
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
INT32 sbtl_stc_close(SBTL_ENGINE_T*    pt_engine)
{
	if (b_simu_mode)
	{
		_simu_timer_stop();
		return MMSBTLR_OK;
	}

    sbtl_stc_close_drv(pt_engine->h_stc);
    pt_engine->h_stc = NULL_HANDLE;
    return MMSBTLR_OK;
}

static INT32 sbtl_query_simu_data(
	   SBTL_ENGINE_T*      pt_engine,
	   PTS_T   t_cur)
{
	MKV_PACK_T          t_pack = {0};
	MM_SBTL_HDLR_MSG_T  t_msg = {0};
	BOOL                b_has_data = FALSE;
	INT32               i4_ret = 0;
	if (pf_simu_cb)
	{
		i4_ret = pf_simu_cb (SBTL_SIMU_DATA_ASK,
			                t_cur,
			                &t_pack,
			                &b_has_data);
		if (b_has_data)
		{
			MKV_PACK_T          t_first_pack = {0};
			if (sbtl_dmx_data_que_query(pt_engine,
		                             &t_first_pack.ui4_data_1,
		                             &t_first_pack.ui4_data_2,
		                             &t_first_pack.ui4_data_3,
		                             &t_first_pack.ui4_pts,
		                             &t_first_pack.ui4_duration)
		                             &&
		         t_first_pack.ui4_data_3 == t_pack.ui4_data_3)
			{
				return MMSBTLR_OK;
			}
			mm_sbtl_hdlr_send_dmx_msg(t_msg, 
                                      pt_engine, 
                                      DEMUX_COND_DATA_REC, 
                                      t_pack.ui4_data_1, 
                                      t_pack.ui4_data_2, 
                                      t_pack.ui4_data_3, 
                                      t_pack.ui4_pts, 
                                      t_pack.ui4_duration, 
                                      0);
		}
		return i4_ret;
	}
	else
	{
		return MMSBTLR_OK; 
	}
	
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
INT32 sbtl_stc_pts(SBTL_ENGINE_T*      pt_engine,
                   PTS_T*              pt_pts)
{
    if (b_simu_mode)
    {
		if (pt_pts) *pt_pts = t_simu_pts;
		sbtl_query_simu_data( pt_engine, t_simu_pts);
		return MMSBTLR_OK;
	}
    if(pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_LRC ||
       pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_KRK_KING ||
       pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_MP3_KRK)
    {
        if(pt_engine->t_cfg.pf_pts_cb != NULL && pt_engine->t_cfg.pv_pts_cb_tag != NULL)
        {
            if(pt_engine->t_cfg.pf_pts_cb(pt_engine->t_cfg.pv_pts_cb_tag, pt_pts))
            {
                *pt_pts =  (*pt_pts)*90000;
                return MMSBTLR_OK;
            }
            else
            {
                return sbtl_stc_pts_drv(pt_engine->h_stc, pt_pts);
            }
        }
        else
        {
            return sbtl_stc_pts_drv(pt_engine->h_stc, pt_pts);
        }
    }
    else
    {
        return sbtl_stc_pts_drv(pt_engine->h_stc, pt_pts);
    }
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
INT32 sbtl_stc_type(SBTL_ENGINE_T*      pt_engine,
                      STC_CTRL_TYPE_T*    pt_type)
{
	if (pt_engine == NULL || pt_type == NULL)
	{
		return MMSBTLR_INV_ARG;
	}
    if (b_simu_mode)
    {
		if (h_simu_timer != NULL_HANDLE)
		{ 
			*pt_type = STC_CTRL_START;
		}
		else
		{
			*pt_type = STC_CTRL_STOP;
		}
		return MMSBTLR_OK;
    }
    return sbtl_stc_type_drv(pt_engine->h_stc, pt_type);
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
INT32 sbtl_dmx_retrive_data(SBTL_ENGINE_T*     pt_engine,
                            UINT32             ui4_tag,
                            UINT32             ui4_length,
                            UINT32             ui4_serial_num,
                            UINT8*             aui1_buf)
{

    if (b_simu_mode)
	{
		 if (pf_simu_cb)
		 {
		 	MKV_PACK_T          t_pack = {0};
			t_pack.ui4_data_1 = ui4_tag;
			t_pack.ui4_data_2 = ui4_length;
			t_pack.ui4_data_3 = ui4_serial_num;
			
		 	return pf_simu_cb(SBTL_SIMU_DATA_CPY,
			                0,
			                &t_pack,
			                aui1_buf);
		 }
		return MMSBTLR_OK;
	}
   
    return sbtl_dmx_retrive_data_drv(pt_engine->h_dmx,
                                     ui4_tag,
                                     ui4_length,
                                     ui4_serial_num,
                                     aui1_buf);
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
INT32 sbtl_dmx_release_data(SBTL_ENGINE_T*     pt_engine,
                            UINT32             ui4_tag,
                            UINT32             ui4_length,
                            UINT32             ui4_serial_num)
{
	if (b_simu_mode)
	{
		 if (pf_simu_cb)
		 {
		 	MKV_PACK_T          t_pack = {0};
			t_pack.ui4_data_1 = ui4_tag;
			t_pack.ui4_data_2 = ui4_length;
			t_pack.ui4_data_3 = ui4_serial_num;
			
		 	return pf_simu_cb(SBTL_SIMU_DATA_RLS,
			                0,
			                &t_pack,
			                NULL);
		 }
		return MMSBTLR_OK;
	}

    return sbtl_dmx_release_data_drv(pt_engine->h_dmx,
                                     ui4_tag,
                                     ui4_length,
                                     ui4_serial_num);
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
INT32 sbtl_dmx_data_que_open(SBTL_ENGINE_T*  pt_engine)
{
    pt_engine->t_data_que.ui2_first_entry_idx = 0;
    pt_engine->t_data_que.ui2_last_entry_idx = 0;
    pt_engine->t_data_que.ui2_used_entry_cnt = 0;
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
INT32 sbtl_dmx_data_que_close(SBTL_ENGINE_T*   pt_engine)
{
    BOOL                            b_found;
    UINT32                          ui4_data_1;
    UINT32                          ui4_data_2;
    UINT32                          ui4_data_3;
    UINT32                          ui4_pts;
    UINT32                          ui4_duration;
    PTS_T                           t_stc;
    
    b_found = FALSE;
    ui4_data_1 = 0;
    ui4_data_2 = 0;
    ui4_data_3 = 0;
    
    while(1)
    {
        b_found = sbtl_dmx_data_que_retrieve(pt_engine,
                                             &ui4_data_1,
                                             &ui4_data_2,
                                             &ui4_data_3,
                                             &ui4_pts,
                                             &ui4_duration,
                                             &t_stc);
        if (b_found)
        {
            sbtl_dmx_release_data(pt_engine, ui4_data_1, ui4_data_2, ui4_data_3);
        }
        else
        {
            break;
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
INT32 sbtl_dmx_data_que_append(SBTL_ENGINE_T*       pt_engine,
                               UINT32               ui4_data_1,
                               UINT32               ui4_data_2,
                               UINT32               ui4_data_3,
                               UINT32               ui4_pts,
                               UINT32               ui4_duration,
                               PTS_T                t_stc)
{
    SBTL_DMX_DATA_QUE_T*    pt_data_que;
    
    pt_data_que = &(pt_engine->t_data_que);

    if (SBTL_DMX_DATA_QUE_MAX_SIZE == pt_data_que->ui2_used_entry_cnt)
    {
        if (SBTL_COND_STARTED == pt_engine->t_state.ui4_curr_state)
        {
            UINT32  ui4_data_que_1;
            UINT32  ui4_data_que_2;
            UINT32  ui4_data_que_3;
            UINT32  ui4_pts;
            UINT32  ui4_duration;
            PTS_T   t_stc_que;
            BOOL    b_get;
            
            /* something is wrong, drop frame ??? */
            /*mm_sbtl_hdlr_send_state_msg(t_msg, pt_engine, EVN_MM_SBTL_DRV_ERR);*/
            b_get = sbtl_dmx_data_que_query(pt_engine,
                                            &ui4_data_que_1,
                                            &ui4_data_que_2,
                                            &ui4_data_que_3,
                                            &ui4_pts,
                                            &ui4_duration);
            if (b_get)
            {
                sbtl_dmx_data_que_retrieve(pt_engine,
                                           &ui4_data_que_1,
                                           &ui4_data_que_2,
                                           &ui4_data_que_3,
                                           &ui4_pts,
                                           &ui4_duration, 
                                           &t_stc_que);
                sbtl_dmx_release_data(pt_engine, ui4_data_que_1, ui4_data_que_2, ui4_data_que_2);
            }
            else
            {
                sbtl_dmx_release_data(pt_engine, ui4_data_1, ui4_data_2, ui4_data_3);
                return (MMSBTLR_OK);
            }
        }
        else
        {
            sbtl_dmx_release_data(pt_engine, ui4_data_1, ui4_data_2, ui4_data_3);
            return (MMSBTLR_OK);
        }
    }

    if (pt_data_que->ui2_last_entry_idx >= SBTL_DMX_DATA_QUE_MAX_SIZE)
    {
        ASSERT(0, ("sbtl_dmx_data_que_append out of boundary\n"));
        pt_data_que->ui2_first_entry_idx = 0;
        pt_data_que->ui2_last_entry_idx = 0;
        pt_data_que->ui2_used_entry_cnt = 0;

        return MMSBTLR_OK;
    }

    /* append data */
    pt_data_que->aui4_data1[pt_data_que->ui2_last_entry_idx] = ui4_data_1;
    pt_data_que->aui4_data2[pt_data_que->ui2_last_entry_idx] = ui4_data_2;
    pt_data_que->aui4_data3[pt_data_que->ui2_last_entry_idx] = ui4_data_3;
    pt_data_que->aui4_pts[pt_data_que->ui2_last_entry_idx] = ui4_pts;
    pt_data_que->aui4_duration[pt_data_que->ui2_last_entry_idx] = ui4_duration;

    pt_data_que->at_stc[pt_data_que->ui2_last_entry_idx] = t_stc;
    
    pt_data_que->ui2_last_entry_idx = (UINT16)((pt_data_que->ui2_last_entry_idx + 1) % 
        SBTL_DMX_DATA_QUE_MAX_SIZE);

    pt_data_que->ui2_used_entry_cnt++;


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
BOOL sbtl_dmx_data_que_retrieve(SBTL_ENGINE_T*      pt_engine,
                                UINT32*             pui4_data_1,
                                UINT32*             pui4_data_2,
                                UINT32*             pui4_data_3,
                                UINT32*             pui4_pts,
                                UINT32*             pui4_duration,
                                PTS_T*              pt_stc)
{
    SBTL_DMX_DATA_QUE_T*            pt_data_que;

    pt_data_que = &(pt_engine->t_data_que);

    /* No entry */
    if (0 == pt_data_que->ui2_used_entry_cnt)
    {
        return FALSE;
    }

    if (pt_data_que->ui2_first_entry_idx >= SBTL_DMX_DATA_QUE_MAX_SIZE)
    {
        ASSERT(0, ("sbtl_dmx_data_que_retrieve out of boundary\n"));
        pt_data_que->ui2_first_entry_idx = 0;
        pt_data_que->ui2_last_entry_idx = 0;
        pt_data_que->ui2_used_entry_cnt = 0;

        return FALSE;
    }

    *pui4_data_1 = pt_data_que->aui4_data1[pt_data_que->ui2_first_entry_idx];
    *pui4_data_2 = pt_data_que->aui4_data2[pt_data_que->ui2_first_entry_idx];
    *pui4_data_3 = pt_data_que->aui4_data3[pt_data_que->ui2_first_entry_idx];
    *pui4_pts = pt_data_que->aui4_pts[pt_data_que->ui2_first_entry_idx];
    *pui4_duration = pt_data_que->aui4_duration[pt_data_que->ui2_first_entry_idx];

    *pt_stc = pt_data_que->at_stc[pt_data_que->ui2_first_entry_idx];
    
    pt_data_que->ui2_first_entry_idx = 
        (UINT16)((pt_data_que->ui2_first_entry_idx + 1) % SBTL_DMX_DATA_QUE_MAX_SIZE);
    
    pt_data_que->ui2_used_entry_cnt --;

    return TRUE;
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
BOOL sbtl_dmx_data_que_query(SBTL_ENGINE_T*     pt_engine,
                             UINT32*            pui4_data_1,
                             UINT32*            pui4_data_2,
                             UINT32*            pui4_data_3,
                             UINT32*            pui4_pts,
                             UINT32*            pui4_duration)

{
    SBTL_DMX_DATA_QUE_T*            pt_data_que;

    pt_data_que = &(pt_engine->t_data_que);

    /* No entry */
    if (0 == pt_data_que->ui2_used_entry_cnt)
    {
        return FALSE;
    }

    if (pt_data_que->ui2_first_entry_idx >= SBTL_DMX_DATA_QUE_MAX_SIZE)
    {
        ASSERT(0, ("sbtl_dmx_data_que_query out of boundary\n"));
        pt_data_que->ui2_first_entry_idx = 0;
        pt_data_que->ui2_last_entry_idx = 0;
        pt_data_que->ui2_used_entry_cnt = 0;

        return FALSE;
    }
    
    *pui4_data_1 = pt_data_que->aui4_data1[pt_data_que->ui2_first_entry_idx];
    *pui4_data_2 = pt_data_que->aui4_data2[pt_data_que->ui2_first_entry_idx];
    *pui4_data_3 = pt_data_que->aui4_data3[pt_data_que->ui2_first_entry_idx];
    *pui4_pts = pt_data_que->aui4_pts[pt_data_que->ui2_first_entry_idx];
    *pui4_duration = pt_data_que->aui4_duration[pt_data_que->ui2_first_entry_idx];

    return TRUE;
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
INT32 sbtl_feeder_init(SBTL_ENGINE_T*          pt_engine,
                        MM_SBTL_FEEDER_TYPE_T   e_type)
{
    MEDIA_FEEDER_T* pt_feeder;
    setpos_fct      pf_set_pos;
    getpos_fct      pf_get_pos;
    UINT64          ui8_pos_bgn;
    UINT64          ui8_pos_end;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }
    
    pt_feeder = pt_engine->t_cfg.pt_feeder[e_type];
        
    ASSERT(pt_feeder, ("pt_feeder=NULL\n"));

    pf_get_pos = pt_feeder->t_feeder_be_fct_tbl.pf_get_pos;
    pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
    pf_set_pos(pt_feeder, 0, MEDIA_SEEK_END);
    pf_get_pos(pt_feeder, &ui8_pos_end);
    pf_set_pos(pt_feeder, 0, MEDIA_SEEK_BGN);
    pf_get_pos(pt_feeder, &ui8_pos_bgn);

    pt_engine->aui8_file_size[e_type] = ui8_pos_end-ui8_pos_bgn;
    pt_engine->aui4_read_count[e_type] = 0;
    pt_engine->aui8_pos[e_type] = 0;
    pt_engine->aui8_cache_pos[e_type] = 0;

    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, e_type);
    
    return MMSBTLR_OK;
}

UINT8* sbtl_feeder_get_data_addr(
    SBTL_ENGINE_T*                  pt_engine,
    MM_SBTL_FEEDER_TYPE_T           e_type)
{
    UINT64          ui8_offset;

    ui8_offset = pt_engine->aui8_pos[e_type] - pt_engine->aui8_cache_pos[e_type];
    return &(pt_engine->aui1_cache_buf[e_type][ui8_offset]); 
}

INT32 sbtl_feeder_get_file_size(
    SBTL_ENGINE_T*            pt_engine,
    UINT64*                   pui8_offset,
    MM_SBTL_FEEDER_TYPE_T     e_type)
{
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    *pui8_offset = pt_engine->aui8_file_size[e_type];
    return MMSBTLR_OK;
}

INT32 sbtl_feeder_get_cache_endpos(
    SBTL_ENGINE_T*            pt_engine,
    UINT64*                   pui8_offset,
    MM_SBTL_FEEDER_TYPE_T     e_type)
{
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    *pui8_offset = pt_engine->aui8_cache_pos[e_type] + pt_engine->aui4_read_count[e_type];
    return MMSBTLR_OK;
}

INT32 sbtl_feeder_render_new_cache(
    SBTL_ENGINE_T*                  pt_engine,
    MM_SBTL_FEEDER_TYPE_T           e_type)
{
    UINT64          ui8_pos;
    copybytes_fct   pf_copybytes;
    setpos_fct      pf_set_pos;
    MEDIA_FEEDER_T* pt_feeder;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    pt_feeder = pt_engine->t_cfg.pt_feeder[e_type];
    ASSERT(pt_feeder, ("pt_feeder=NULL\n"));
    
    pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
    pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
    
    sbtl_feeder_getpos(pt_engine, &ui8_pos, e_type);
    
    if (ui8_pos+1 > pt_engine->aui8_file_size[e_type])
    {
        return MMSBTLR_EOT;
    }

    pf_set_pos(pt_feeder, (INT64)pt_engine->aui8_pos[e_type], MEDIA_SEEK_BGN);
    pf_copybytes(pt_feeder, 
                 pt_engine->aui1_cache_buf[e_type], 
                 FEEDER_CACHE_SIZE, 
                 FEEDER_CACHE_SIZE,
                 &pt_engine->aui4_read_count[e_type]);

    pt_engine->aui8_cache_pos[e_type] = pt_engine->aui8_pos[e_type];
    
    return MMSBTLR_OK;
}

INT32 sbtl_feeder_inpnbyte_p(SBTL_ENGINE_T*          pt_engine,
                                     UINT32                  ui4_len,
                                     UINT8**                 ppui1_buf,
                                     UINT32*                 pui4_done_bytes,
                                     MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT64          ui8_pos;
    UINT64          ui8_read_end,ui8_cache_end;
    UINT64          ui8_offset = 0;
    UINT32          ui4_read_bytes = 0;
    INT32           i4_ret = MMSBTLR_OK;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    if(pt_engine->aui4_read_count[e_type] == 0)
    {
       i4_ret = sbtl_feeder_render_new_cache(pt_engine,e_type);
       if(i4_ret != MMSBTLR_OK)
       {
            if(pui4_done_bytes != NULL)
            {
                *pui4_done_bytes = 0;
            }
            return MMSBTLR_EOT;
       }
    }
    
    ui8_pos = pt_engine->aui8_pos[e_type];
    ui8_read_end = ui8_pos + (UINT64)ui4_len;
    ui8_cache_end = pt_engine->aui8_cache_pos[e_type] + pt_engine->aui4_read_count[e_type];

    if(ui8_read_end <= ui8_cache_end)
    { /*The read data is in the cache buffer.*/
        ui8_offset = pt_engine->aui8_pos[e_type] - pt_engine->aui8_cache_pos[e_type];
       (*ppui1_buf) = &(pt_engine->aui1_cache_buf[e_type][ui8_offset]); 
        ui4_read_bytes = ui4_len;
    }
    else if(ui8_cache_end <= pt_engine->aui8_file_size[e_type])
    {  
        /*The part of read data range is out of cache.Get new cache page from current position.*/
       i4_ret = sbtl_feeder_render_new_cache(pt_engine,e_type);
       if(i4_ret != MMSBTLR_OK)
       {
            ui4_read_bytes = 0;
            i4_ret = MMSBTLR_EOT;
       }
       else if(ui4_len <= pt_engine->aui4_read_count[e_type])
       {
            (*ppui1_buf) = &(pt_engine->aui1_cache_buf[e_type][0]); 
            ui4_read_bytes = ui4_len;
       }         
       else
       {
            (*ppui1_buf) = &(pt_engine->aui1_cache_buf[e_type][0]); 
            ui4_read_bytes = pt_engine->aui4_read_count[e_type];
            i4_ret = MMSBTLR_EOT;
       }
    }
    
    pt_engine->aui8_pos[e_type] += ui4_read_bytes;
    if(pui4_done_bytes != NULL)
    {
        *pui4_done_bytes = ui4_read_bytes;
    }

    return i4_ret;
}

INT32 sbtl_feeder_inpnbyte_addr_p(
    SBTL_ENGINE_T*                  pt_engine,
    UINT64                          ui8_start_pos,
    UINT32                          ui4_len,
    UINT8**                         ppui1_buf,
    UINT32*                         pui4_done_bytes,
    MM_SBTL_FEEDER_TYPE_T           e_type)
{
    INT32                           i4_ret = MMSBTLR_OK;

    ASSERT(pt_engine, ("pt_feeder=NULL\n"));
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }
    
    if (ui8_start_pos > pt_engine->aui8_file_size[e_type])
    {
        if(pui4_done_bytes != NULL)
        {
            *pui4_done_bytes = 0;
        }
        return MMSBTLR_EOT;
    }

    sbtl_feeder_setpos(pt_engine, (INT64)ui8_start_pos, MEDIA_SEEK_BGN, e_type);

    i4_ret = sbtl_feeder_inpnbyte_p(pt_engine,ui4_len,ppui1_buf,pui4_done_bytes,e_type);
    
    return i4_ret;
}

UINT8 sbtl_tag_fmt_check(
    UINT8                           ui1_fmt_cnt,
    SBTL_TAG_FORMAT_T*              pt_tag_inf,
    UINT8*                          pui1_buf,
    UINT8*                          pui1_fmt_len,
    UINT8*                          pui1_fmt_idx,
    UINT32                          ui4_read_len,
    BOOL                            b_UTF16)
{
    UINT8                           ui1_fmt;
    UINT8                           ui1_i = 0;
    UINT8                           ui1_j = 0;
    UINT8                           ui1_pos = 0;
    UINT8                           ui1_count;
    UINT16                          ui2_tmp1,ui2_tmp2;
    UINT16*                         pui2_buf;
    INT32                           i4_ret = SBTL_TIME_TAG_NOT_FOUND;

    if((pt_tag_inf == NULL) || (pui1_buf == NULL))
    {
        return SBTL_TIME_TAG_NOT_FOUND;
    }

    pui2_buf = (UINT16*)pui1_buf;
    
    for(ui1_fmt = 0; ui1_fmt < ui1_fmt_cnt; ui1_fmt ++)
    {
        if(ui4_read_len < pt_tag_inf[ui1_fmt].ui1_fmt_len)
        {
            i4_ret = SBTL_TIME_TAG_NOT_FOUND;
            continue;
        }
        ui1_count = 0;
        ui1_j = 0;
        for(ui1_i = 0; ui1_i < pt_tag_inf[ui1_fmt].ui1_fmt_len; ui1_i ++)
        {
            if(b_UTF16 == TRUE)
            {
                ui2_tmp2 = (UINT16)pui2_buf[ui1_i];
            }
            else
            {
                ui2_tmp2 = (UINT16)pui1_buf[ui1_i];
            }
            if(ui1_j >= pt_tag_inf[ui1_fmt].ui1_len)
            {
                ui1_j = pt_tag_inf[ui1_fmt].ui1_len - 1;
            }
            if(ui1_j >= SBTL_TIME_FORMAT_LEN)
            {
                break;
            }
            ui1_pos = pt_tag_inf[ui1_fmt].ui1_content_pos[ui1_j];
            if(ui1_i == ui1_pos)
            {
                ui2_tmp1 = (UINT16)pt_tag_inf[ui1_fmt].s_content[ui1_j];
                ui1_j ++;
                
                if(ui2_tmp1 == ui2_tmp2)
                {
                    if(b_UTF16 == TRUE)
                    {
                        ui1_count += 2;
                    }
                    else
                    {
                        ui1_count ++;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                if(pt_tag_inf[ui1_fmt].b_check_num)
                {
                    if(ui2_tmp2 >= '0' && ui2_tmp2 <='9')
                    {
                        if(b_UTF16 == TRUE)
                        {
                            ui1_count += 2;
                        }
                        else
                        {
                            ui1_count ++;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                     if(b_UTF16 == TRUE)
                    {
                        ui1_count += 2;
                    }
                    else
                    {
                        ui1_count ++;
                    }
                }
            }
        }

        if(ui1_count >= pt_tag_inf[ui1_fmt].ui1_fmt_len)
        {
            if(pui1_fmt_idx != NULL)
            {
                *pui1_fmt_idx = ui1_fmt;
            }

            if (pui1_fmt_len != NULL)
            {
                *pui1_fmt_len = pt_tag_inf[ui1_fmt].ui1_fmt_len;
            }
            return SBTL_TIME_TAG_FOUND;
        }
    }

    return i4_ret;
}

UINT32 sbtl_get_tag_fmt_max_len(
    UINT8                           ui1_fmt_cnt,
    SBTL_TAG_FORMAT_T*              pt_tag_inf)
{
    UINT8                           ui1_max_len = 0;
    UINT8                           ui1_i;
    
    if(pt_tag_inf == NULL)
    {
        return 0;
    }   
    
    for(ui1_i = 0; ui1_i < ui1_fmt_cnt; ui1_i ++)
    {
        if(pt_tag_inf[ui1_i].ui1_fmt_len > ui1_max_len)
        {
            ui1_max_len = pt_tag_inf[ui1_i].ui1_fmt_len;
        }
    }
    
    return (UINT32)ui1_max_len;
}

UINT32 sbtl_get_tag_fmt_min_len(
    UINT8                           ui1_fmt_cnt,
    SBTL_TAG_FORMAT_T*              pt_tag_inf)
{
    UINT8                           ui1_min_len = 0xff;
    UINT8                           ui1_i;
    
    if(pt_tag_inf == NULL)
    {
        return 0;
    }   
    
    for(ui1_i = 0; ui1_i < ui1_fmt_cnt; ui1_i ++)
    {
        if(pt_tag_inf[ui1_i].ui1_fmt_len < ui1_min_len)
        {
            ui1_min_len = pt_tag_inf[ui1_i].ui1_fmt_len;
        }
    }
    
    return (UINT32)ui1_min_len;
}

INT32 sbtl_tag_inf_seek(
    SBTL_ENGINE_T*                  pt_engine,
    UINT64                          ui8_start_pos,
    UINT8                           ui1_fmt_cnt,
    SBTL_TAG_FORMAT_T*              pt_tag_inf,
    MM_SBTL_FEEDER_TYPE_T           e_type,
    BOOL                            b_found_total,
    UINT8**                         ppui1_buf,
    UINT64*                         pui8_seek_len,
    UINT16*                         pui2_count,
    UINT8*                          pui1_fmt_idx,
    BOOL                            b_UTF16)
{
    INT32                           i4_ret = MMSBTLR_OK;
    UINT32                          ui4_read_len;
    UINT32                          ui4_leave_len;
    UINT8*                          pui1_buf_s = 0;
    UINT64                          ui8_pos = 0;
    UINT8                           ui1_ret;
    UINT8                           ui1_fmt_len;
    //UINT32                          ui4_fmt_max;
    UINT32                          ui4_fmt_min;
    #if 0
    UINT8                           at_name[20];
    #endif

    ASSERT(pt_engine, ("pt_feeder=NULL\n"));
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        return MMSBTLR_INV_ARG;
    }

    if ((ui1_fmt_cnt == 0) || (pt_tag_inf == NULL) || (pui2_count == NULL) ||
        ((b_found_total == FALSE) && ((ppui1_buf == NULL) || (pui8_seek_len == NULL))) )
    {
        return MMSBTLR_INV_ARG;
    }

    //ui4_fmt_max = sbtl_get_tag_fmt_max_len(ui1_fmt_cnt, pt_tag_inf);
    ui4_fmt_min = sbtl_get_tag_fmt_min_len(ui1_fmt_cnt, pt_tag_inf);
    *pui2_count = 0;
    ui8_pos = ui8_start_pos;
    
    while(1)
    {
        if(b_UTF16 == TRUE)
        {
            ui8_pos = (ui8_pos >> 1) << 1;
        }
        
        sbtl_feeder_inpnbyte_addr_p(pt_engine,ui8_pos,FEEDER_CACHE_SIZE,&pui1_buf_s,&ui4_read_len,e_type);

        if ((ui4_read_len < ui4_fmt_min) || (pui1_buf_s == 0))
        {
            if(pui8_seek_len != NULL)
            {
            *pui8_seek_len = ui4_read_len;
            }

            i4_ret = MMSBTLR_EOT;
            break;
        }
    
        /*Seek time tag in one cache range buffer.*/
        ui4_leave_len = ui4_read_len;
        while(ui4_leave_len > 0)
        {
            if(ui4_leave_len < ui4_fmt_min)
            {
                break;
            }
            
            /*Check the time format.*/
            ui1_ret = sbtl_tag_fmt_check(ui1_fmt_cnt,
                                            pt_tag_inf,
                                            pui1_buf_s,
                                            &ui1_fmt_len,
                                            pui1_fmt_idx,
                                            ui4_leave_len,
                                            b_UTF16);

            if (ui1_ret == SBTL_TIME_TAG_OUT)
            {
                /*This cache buffer is seek in the end.*/
                break;
            }
            else if (ui1_ret == SBTL_TIME_TAG_FOUND)
            {          
                (*pui2_count) ++;
                if((b_found_total == FALSE) && ((*pui2_count) > 0))
                {
                    break;
                }
                pui1_buf_s += ui1_fmt_len;
                ui4_leave_len -= ui1_fmt_len;
            }
            else if(b_UTF16 == TRUE) 
            {
                pui1_buf_s += 2;
                ui4_leave_len -= 2;
            }                
            else
            {
                pui1_buf_s ++;
                ui4_leave_len --;
            }
        }

        /*if (((*pui2_count) > 0) && (b_found_total == FALSE))
        {
            ui8_pos = ui8_pos + ui4_read_len - ui4_leave_len;
            sbtl_feeder_inpnbyte_addr_p(pt_engine,ui8_pos,ui1_fmt_len,ppui1_buf,&ui4_read_len,e_type);
            *pui8_seek_len = ui8_pos + ui1_fmt_len - ui8_start_pos;
            break;
        }
        else
        {
            if((*pui2_count) == 0 && ui4_read_len < FEEDER_CACHE_SIZE)
            {
                *pui8_seek_len = ui4_read_len;
                i4_ret = MMSBTLR_EOT;
                break;
            }
        }*/
        if(b_found_total == FALSE)
        {
            if((*pui2_count) > 0)
            {
                ui8_pos = ui8_pos + ui4_read_len - ui4_leave_len;
                sbtl_feeder_inpnbyte_addr_p(pt_engine,ui8_pos,ui1_fmt_len,ppui1_buf,&ui4_read_len,e_type);
                *pui8_seek_len = ui8_pos + ui1_fmt_len - ui8_start_pos;
                break;
            }
            else
            {
                if(ui4_read_len < FEEDER_CACHE_SIZE)
                {
                    *pui8_seek_len = ui4_read_len;
                    i4_ret = MMSBTLR_EOT;
                    break;
                }
            }
        }
        else
        {
            if(ui4_read_len < FEEDER_CACHE_SIZE)
            {
                break;
            }
        }

        ui8_pos = ui8_pos + ui4_read_len - ui4_leave_len;
    }

    return i4_ret;
}

INT32 sbtl_seek_tag_inf(
    SBTL_ENGINE_T*                  pt_engine,
    UINT64                          ui8_start_pos,
    SBTL_TAG_FORMAT_T*              pt_start_tag,
    SBTL_TAG_FORMAT_T*              pt_end_tag,
    UINT8**                         ppui1_buf,
    UINT64*                         pui8_offset,
    UINT64*                         pui8_len,
    MM_SBTL_FEEDER_TYPE_T           e_type,
    BOOL                            b_UTF16)
{
    UINT8*                              pui1_buf = NULL; 
    UINT64                              ui8_seek_len;
    UINT64                              ui8_pos = 0;
    UINT16                              ui2_found_cnt;
    UINT8                               ui1_fmt; 
    UINT32                              ui4_done;

    ASSERT(pt_engine, ("pt_feeder=NULL\n"));
    if ((e_type >= MM_SBTL_FEEDER_MAX) || (pt_start_tag == NULL) || (pt_end_tag == NULL) ||
        (pt_start_tag->ui1_fmt_len == 0) || (pt_end_tag->ui1_fmt_len == 0))
    {
        return MMSBTLR_INV_ARG;
    }

    ui2_found_cnt = 0;
    ui8_seek_len = 0;
    ui1_fmt = 0;
    sbtl_tag_inf_seek(pt_engine,
           ui8_start_pos,
           1,
           pt_start_tag,
           e_type,
           FALSE,
           &pui1_buf,
           &ui8_seek_len,
           &ui2_found_cnt,
           &ui1_fmt,
           b_UTF16); 
    
    if(ui2_found_cnt == 0)
    {
        return MMSBTLR_NOT_FOUND;
    }
    
    ui8_pos = ui8_start_pos + ui8_seek_len;
    sbtl_tag_inf_seek(pt_engine,
           ui8_pos,
           1,
           pt_end_tag,
           e_type,
           FALSE,
           &pui1_buf,
           &ui8_seek_len,
           &ui2_found_cnt,
           &ui1_fmt,
           b_UTF16); 
    
    if(ui2_found_cnt == 0)
    {
        return MMSBTLR_NOT_FOUND;
    }

    ui4_done = 0;
    sbtl_feeder_inpnbyte_addr_p(pt_engine,ui8_pos,(UINT32)ui8_seek_len,&pui1_buf,&ui4_done,e_type);  
    if(ui4_done == 0)
    {
        return MMSBTLR_NOT_FOUND;
    }

    if(pui8_offset != NULL)
    {
        *pui8_offset = ui8_pos;
    }
    
    if(pui8_len != NULL)
    {
        *pui8_len = ui4_done - pt_end_tag->ui1_fmt_len;
    }

    if(ppui1_buf != NULL)
    {
        *ppui1_buf = pui1_buf;
    }
    
    return MMSBTLR_OK;
}

BOOL sbtl_check_tag_inf_by_addr(
    SBTL_ENGINE_T*                  pt_engine,
    UINT64                          ui8_start_pos,
    UINT8                           ui1_fmt_cnt,
    SBTL_TAG_FORMAT_T*              pt_tag_inf,
    MM_SBTL_FEEDER_TYPE_T           e_type,
    UINT8**                         ppui1_buf,
    UINT8*                          pui1_fmt_idx,
    BOOL                            b_UTF16)
{
    UINT8                           ui1_fmt;
    UINT8                           ui1_count;
    UINT8                           ui1_i;
    UINT8                           ui1_j = 0;
    UINT8                           ui1_pos;
    UINT8*                          pui1_buf;
    UINT16*                         pui2_buf;
    UINT32                          ui4_read_len;
    UINT16                          ui2_tmp1, ui2_tmp2;
    
    ASSERT(pt_engine, ("pt_feeder=NULL\n"));
    ASSERT((e_type < MM_SBTL_FEEDER_MAX), ("e_type is invalid\n"));

    if ((ui1_fmt_cnt == 0) || (pt_tag_inf == NULL))
    {
        return FALSE;
    }

    for(ui1_fmt = 0; ui1_fmt < ui1_fmt_cnt; ui1_fmt ++)
    {
        ui1_count = 0;
        ui1_j = 0;
        sbtl_feeder_inpnbyte_addr_p(pt_engine,
                                    ui8_start_pos,
                                    (UINT32)pt_tag_inf[ui1_fmt].ui1_fmt_len,
                                    &pui1_buf,
                                    &ui4_read_len,
                                    e_type);
        pui2_buf = (UINT16*)pui1_buf;
        if(ui4_read_len >= (UINT32)pt_tag_inf[ui1_fmt].ui1_fmt_len)
        {
            for(ui1_i = 0; ui1_i < pt_tag_inf[ui1_fmt].ui1_fmt_len; ui1_i ++)
            {              
                if(b_UTF16)
                {
                    ui2_tmp2 = pui2_buf[ui1_i];
                }
                else
                {
                    ui2_tmp2 = (UINT16)pui1_buf[ui1_i];
                }
                if(ui1_j >= pt_tag_inf[ui1_fmt].ui1_len)
                {
                    ui1_j = pt_tag_inf[ui1_fmt].ui1_len - 1;
                }
                if(ui1_j >= SBTL_TIME_FORMAT_LEN)
                {
                    break;
                }
                ui1_pos = pt_tag_inf[ui1_fmt].ui1_content_pos[ui1_j];
                if(ui1_i == ui1_pos)
                {
                    ui2_tmp1 = (UINT16)pt_tag_inf[ui1_fmt].s_content[ui1_j];
                    ui1_j ++;
                    
                    if(ui2_tmp1 == ui2_tmp2)
                    {
                        if(b_UTF16 == TRUE)
                        {
                            ui1_count += 2;
                        }
                        else
                        {
                            ui1_count ++;
                        }

                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if(pt_tag_inf[ui1_fmt].b_check_num)
                    {
                        if(ui2_tmp2 >= '0' && ui2_tmp2 <='9')
                        {
                            if(b_UTF16 == TRUE)
                            {
                                ui1_count += 2;
                            }
                            else
                            {
                                ui1_count ++;
                            }

                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        if(b_UTF16 == TRUE)
                        {
                            ui1_count += 2;
                        }
                        else
                        {
                            ui1_count ++;
                        }

                    }
                }
                if(ui1_count >= pt_tag_inf[ui1_fmt].ui1_fmt_len)
                {
                    if(pui1_fmt_idx != NULL)
                    {
                        *pui1_fmt_idx = ui1_fmt;
                    }

                    if(ppui1_buf != NULL)
                    {
                        *ppui1_buf = pui1_buf;
                    }
                    return TRUE;
                }
            }
            
        }

    }
    return FALSE;
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
INT32 sbtl_feeder_inp1byte(SBTL_ENGINE_T*          pt_engine,
                           UINT8*                  pui1_data,
                           MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT64          ui8_pos;
    copybytes_fct   pf_copybytes;
    setpos_fct      pf_set_pos;
    MEDIA_FEEDER_T* pt_feeder;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }
    
    pt_feeder = pt_engine->t_cfg.pt_feeder[e_type];
    ASSERT(pt_feeder, ("pt_feeder=NULL\n"));
    
    pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
    pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
    
    sbtl_feeder_getpos(pt_engine, &ui8_pos, e_type);
    
    if (ui8_pos+1 > pt_engine->aui8_file_size[e_type])
    {
        return MMSBTLR_EOT;
    }

    if (0 == pt_engine->aui4_read_count[e_type]
        || pt_engine->aui8_pos[e_type] < pt_engine->aui8_cache_pos[e_type]
        || pt_engine->aui8_pos[e_type]+1 > pt_engine->aui8_cache_pos[e_type]+pt_engine->aui4_read_count[e_type])
    {
        pf_set_pos(pt_feeder, (INT64)pt_engine->aui8_pos[e_type], MEDIA_SEEK_BGN);
        pf_copybytes(pt_feeder, 
                     pt_engine->aui1_cache_buf[e_type], 
                     FEEDER_CACHE_SIZE, 
                     FEEDER_CACHE_SIZE,
                     &pt_engine->aui4_read_count[e_type]);
        
        pt_engine->aui8_cache_pos[e_type] = pt_engine->aui8_pos[e_type];
    }

    *pui1_data = pt_engine->aui1_cache_buf[e_type][pt_engine->aui8_pos[e_type]-pt_engine->aui8_cache_pos[e_type]];
    pt_engine->aui8_pos[e_type]++;
    
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
INT32 sbtl_feeder_inp2byte(SBTL_ENGINE_T*          pt_engine,
                           UINT16*                 pui2_data,
                           MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT8           ui1_idx;
    UINT8           ui1_tmp;
    INT32           i4_ret;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        ASSERT(0, ("wrong feeder type\n"));
    }

    *pui2_data = 0;
    for (ui1_idx=0; ui1_idx<2; ui1_idx++)
    {
        i4_ret = sbtl_feeder_inp1byte(pt_engine, &ui1_tmp, e_type);
        if (i4_ret != MMSBTLR_OK)
        {
            return i4_ret;
        }
        #ifndef SBTL_LITTLE_EDIAN
        *pui2_data = (((*pui2_data)<<8)|ui1_tmp);
        #else
        *pui2_data += (ui1_tmp)<<(ui1_idx*8);
        #endif
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
INT32 _sbtl_feeder_inp3byte(SBTL_ENGINE_T*          pt_engine,
                            UINT32*                 pui4_data,
                            MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT8           ui1_idx;
    UINT8           ui1_tmp;
    INT32           i4_ret;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        ASSERT(0, ("wrong feeder type\n"));
    }

    *pui4_data = 0;
    
    for (ui1_idx=0; ui1_idx<3; ui1_idx++)
    {
        i4_ret = sbtl_feeder_inp1byte(pt_engine, &ui1_tmp, e_type);
        if (i4_ret != MMSBTLR_OK)
        {
            return i4_ret;
        }
        
        #ifndef SBTL_LITTLE_EDIAN
            *pui4_data = (((*pui4_data)<<8)|ui1_tmp);
        #else
            *pui4_data += (ui1_tmp)<<(ui1_idx*8);
        #endif
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
INT32 sbtl_feeder_inp4byte(SBTL_ENGINE_T*          pt_engine,
                           UINT32*                 pui4_data,
                           MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT8           ui1_idx;
    UINT8           ui1_tmp;
    INT32           i4_ret;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        ASSERT(0, ("wrong feeder type\n"));
    }
    *pui4_data = 0;
    for (ui1_idx=0; ui1_idx<4; ui1_idx++)
    {
        i4_ret = sbtl_feeder_inp1byte(pt_engine, &ui1_tmp, e_type);
        if (i4_ret != MMSBTLR_OK)
        {
            return i4_ret;
        }
        
        #ifndef SBTL_LITTLE_EDIAN
            *pui4_data = (((*pui4_data)<<8)|ui1_tmp);
        #else
            *pui4_data += (ui1_tmp)<<(ui1_idx*8);
        #endif
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
INT32 sbtl_feeder_inpnbyte(SBTL_ENGINE_T*          pt_engine,
                           UINT32                  ui4_len,
                           UINT8*                  aui_buf,
                           UINT32*                 pui4_done_bytes,
                           MM_SBTL_FEEDER_TYPE_T   e_type)
{
    UINT32          ui4_idx;
    UINT8           ui1_tmp;
    INT32           i4_ret;
    
    i4_ret = MMSBTLR_OK;
    
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        ASSERT(0, ("wrong feeder type\n"));
    }

    if (pui4_done_bytes)
    {
        *pui4_done_bytes = 0;
    }
    
    for (ui4_idx=0; ui4_idx<ui4_len; ui4_idx++)
    {
        i4_ret = sbtl_feeder_inp1byte(pt_engine, &ui1_tmp, e_type);
        if (i4_ret != MMSBTLR_OK)
        {
            return i4_ret;
        }

        if (pui4_done_bytes)
        {
            *pui4_done_bytes = *pui4_done_bytes + 1;
        }
        
        aui_buf[ui4_idx] = ui1_tmp;
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
INT32 sbtl_feeder_setpos(SBTL_ENGINE_T*            pt_engine,
                         INT64                     i8_offset, 
                         UINT8                     ui1_whence,
                         MM_SBTL_FEEDER_TYPE_T     e_type)
{ 
    INT64           i8_pos;
    setpos_fct      pf_set_pos;
    MEDIA_FEEDER_T* pt_feeder;

    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    pt_feeder = pt_engine->t_cfg.pt_feeder[e_type];
    ASSERT(pt_feeder, ("pt_feeder=NULL\n"));
        
    pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;

    switch (ui1_whence)
    {
        case MEDIA_SEEK_BGN:
            i8_pos = i8_offset;
            break;
            
        case MEDIA_SEEK_CUR: 
            i8_pos = (INT64)(pt_engine->aui8_pos[e_type]);
            i8_pos = i8_pos + i8_offset;
            break;
            
        case MEDIA_SEEK_END:
            i8_pos = (INT64)(pt_engine->aui8_file_size[e_type]);
            i8_pos = i8_pos + i8_offset;
            break;
            
        default:
            return MMSBTLR_INV_ARG;
    }
    
    if (i8_pos > (INT64)(pt_engine->aui8_file_size[e_type]))
    {
        return MMSBTLR_EOT;
    }
    if (i8_pos < 0)
    {
        return MMSBTLR_EOT;
    }

    pt_engine->aui8_pos[e_type] = (UINT64)i8_pos;
    
    if (pt_engine->aui8_pos[e_type] < pt_engine->aui8_cache_pos[e_type]
        || pt_engine->aui8_pos[e_type]+1 > pt_engine->aui8_cache_pos[e_type]+pt_engine->aui4_read_count[e_type])
    {
        pt_engine->aui4_read_count[e_type] = 0;
        pf_set_pos(pt_feeder, i8_pos, MEDIA_SEEK_BGN);
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
INT32 sbtl_feeder_getpos(SBTL_ENGINE_T*            pt_engine,
                         UINT64*                   pui8_offset,
                         MM_SBTL_FEEDER_TYPE_T     e_type)
{
    if (e_type >= MM_SBTL_FEEDER_MAX)
    {
        //ASSERT(0, ("wrong feeder type\n"));
        return MMSBTLR_INV_ARG;
    }

    *pui8_offset = pt_engine->aui8_pos[e_type];
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
UINT32 sbtl_asci2decimal(UINT8*    aui1_ary,
                         UINT8     ui1_round)
{
    UINT32  ui4_result = 0;
    UINT32  ui4_multiply = 1;
    while(ui1_round)
    {
        ui1_round -= 1;
        ui4_result += (aui1_ary[ui1_round]-'0')*ui4_multiply;
        ui4_multiply *= 10;
    }

    return ui4_result;
}

UINT32 sbtl_wasci2decimal(UINT16*     aui2_ary,
                          UINT8       ui1_round)
{
    UINT32  ui4_result = 0;
    UINT32  ui4_multiply = 1;
    while(ui1_round)
    {
        ui1_round -= 1;
        ui4_result += (aui2_ary[ui1_round]-'0')*ui4_multiply;
        ui4_multiply *= 10;
    }

    return ui4_result;
}


UINT32 sbtl_get_utf16_string_len(UINT16* pui2_buf, UINT32 ui4_max_len)
{
    UINT32              ui4_len;

    ui4_len = 0;
    
    while(1)
    {
        if(ui4_len >= ui4_max_len)
        {
            break;
        }

        if(pui2_buf[ui4_len] == 0)
        {
            break;
        }
        else
        {
            ui4_len ++;
        }
    }
    
    return ui4_len;
}

INT32 sbtl_trans_to_w2s(MM_SBTL_LANG_T          e_language,
                            UINT16                  ui2_data_in,
                            UINT16*                 pui2_data_out)
{
    UINT32          ui4_offset = 0;
    UINT32          ui4_count = 2;

    if(pui2_data_out == NULL || e_language >= MM_SBTL_LANG_END)
    {
        return MMSBTLR_INV_ARG;
    }
    
    switch(e_language)
    {
        case MM_SBTL_LANG_ASCI:
            sbtl_multi_byte_to_unicode(&ui2_data_in, 2, &ui4_offset, MUL_BYTE_CODEPAGE1252, pui2_data_out, &ui4_count);
            break;

        case MM_SBTL_LANG_UNICODE:
            *pui2_data_out = ui2_data_in;
            break;

        case MM_SBTL_LANG_BIG5:
            sbtl_multi_byte_to_unicode(&ui2_data_in, 2, &ui4_offset, MUL_BYTE_BIG5, pui2_data_out, &ui4_count);
            break;

        case MM_SBTL_LANG_BG2312:
            sbtl_multi_byte_to_unicode(&ui2_data_in, 2, &ui4_offset, MUL_BYTE_GB2312, pui2_data_out, &ui4_count);
            break;

        default:
            break;
    }

    return MMSBTLR_OK;
}


INT32 sbtl_trans_to_w2s_ex(MM_SBTL_ENCODING_T          e_encoding,
                            UINT8*                  pui1_src,
                            UINT32                  ui4_len,
                            UINT16*                 pui2_data_out)
{
    UINT16*         pui2_buf;
    UINT32          ui4_i;
    UINT32          ui4_offset = 0;
    UINT32          ui4_count = ui4_len;

    if(pui1_src == NULL || pui2_data_out == NULL || e_encoding >= MM_SBTL_ENCODE_END)
    {
        return MMSBTLR_INV_ARG;
    }

    pui2_buf = (UINT16*)pui1_src;
    if(e_encoding == MM_SBTL_ENCODE_UTF16)
    {
        ui4_count = ui4_count >> 1;
    }
    
    switch(e_encoding)
    {
         case MM_SBTL_ENCODE_UTF8:
            x_uc_ps_to_w2s((CHAR*)pui1_src,(UTF16_T*)pui2_data_out, ui4_len);
            break;

        case MM_SBTL_ENCODE_UTF16:
            for(ui4_i = 0; ui4_i < ui4_count;ui4_i ++)
            {
                *pui2_data_out = *pui2_buf;
                pui2_buf++;
                pui2_data_out++;
            }            
            break;

        case MM_SBTL_ENCODE_BIG5:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_BIG5, pui2_data_out, &ui4_count);
            break;
            
    	case MM_SBTL_ENCODE_GB18030:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_GB18030, pui2_data_out, &ui4_count);
            break;

        case MM_SBTL_ENCODE_GB2312:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_GB2312, pui2_data_out, &ui4_count);
            break;
        case MM_SBTL_ENCODE_CODEPAGE1250:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1250, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1251:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1251, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_ASCI:    
        case MM_SBTL_ENCODE_CODEPAGE1252:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1252, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1253:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1253, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1254:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1254, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1255:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1255, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1256:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1256, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1257:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1257, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_CODEPAGE1258:
            sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_CODEPAGE1258, pui2_data_out, &ui4_count);
            break;
            
        case MM_SBTL_ENCODE_ISO8859_2:
	     sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_2, pui2_data_out, &ui4_count);
             break;
        case MM_SBTL_ENCODE_ISO8859_5:
	     sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_5, pui2_data_out, &ui4_count);
             break;
        case MM_SBTL_ENCODE_ISO8859_7:
	     sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_7, pui2_data_out, &ui4_count);
             break;
        case MM_SBTL_ENCODE_ISO8859_8:
	     sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_8, pui2_data_out, &ui4_count);
             break;
        case MM_SBTL_ENCODE_ISO8859_9:
	     sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_9, pui2_data_out, &ui4_count);
             break;
        case MM_SBTL_ENCODE_ISO8859_16:
             sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE__ISO8859_16, pui2_data_out, &ui4_count);
             break;
	    case MM_SBTL_ENCODE_KOI8_R:
             sbtl_multi_byte_to_unicode(pui2_buf, ui4_len, &ui4_offset, MUL_BYTE_KOI8_R, pui2_data_out, &ui4_count);
             break;

        default:
            break;
    }

    return MMSBTLR_OK;
}

INT32 sbtl_record_init(VOID)
{
    _gpt_sbtl_rcd = (MM_SBTL_RECORD_INF*)x_mem_alloc(sizeof(MM_SBTL_RECORD_INF));
    if(_gpt_sbtl_rcd == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    else
    {
        x_memset(_gpt_sbtl_rcd, 0, sizeof(MM_SBTL_RECORD_INF));
    }

    _gpt_sbtl_rcd->aui1_rcd_mask[MM_SBTL_INF_RCD_VIDEO] = MM_SBTL_INF_MSK_NONE;
    _gpt_sbtl_rcd->aui1_rcd_mask[MM_SBTL_INF_RCD_AUDIO] = MM_SBTL_INF_MSK_NONE;
    return MMSBTLR_OK;
}

INT32 sbtl_record_config(SBTL_ENGINE_T*  pt_engine,
	                         UINT32          ui4_type,
	                         VOID*           pv_info)
{
	if (pt_engine != NULL && pv_info != NULL && _gpt_sbtl_rcd != NULL)
	{
		UINT8 ui1_idx = MM_SBTL_INF_RCD_VIDEO;
		if((pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_LRC) ||
       	   (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_KRK_KING) ||
           (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_MP3_KRK))
	    {
			ui1_idx = MM_SBTL_INF_RCD_AUDIO;
		}
		switch (ui4_type)
		{
			case MM_SBTL_INF_MSK_FONT_INF:
				x_memcpy(&_gpt_sbtl_rcd->at_font_info[ui1_idx], pv_info, sizeof(WGL_FONT_INFO_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_FONT_INF;
				//DBG_LOG_PRINT(("[sbtl util] store font size: %d \n",_gpt_sbtl_rcd->at_font_info[ui1_idx].ui1_custom_size));
				break;
				
			case MM_SBTL_INF_MSK_ENCODING:
				x_memcpy(&_gpt_sbtl_rcd->e_coding[ui1_idx], pv_info, sizeof(MM_SBTL_ENCODING_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_ENCODING;
				break;
				
			case MM_SBTL_INF_MSK_TM_OFST:
			    x_memcpy(&_gpt_sbtl_rcd->t_tm_ofst[ui1_idx], pv_info, sizeof(MM_SBTL_TM_OFST_INF));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_TM_OFST;
				break;
				
			case MM_SBTL_INF_MSK_OSD_OFST:
				x_memcpy(&_gpt_sbtl_rcd->t_osd_ofst[ui1_idx], pv_info, sizeof(GL_POINT_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_OSD_OFST;
				break;
				
			case MM_SBTL_INF_MSK_FG_COLOR:
				x_memcpy(&_gpt_sbtl_rcd->t_fg_color[ui1_idx], pv_info, sizeof(WGL_COLOR_INFO_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_FG_COLOR;
				break;	
				
			case MM_SBTL_INF_MSK_BG_COLOR:
				x_memcpy(&_gpt_sbtl_rcd->t_bg_color[ui1_idx], pv_info, sizeof(WGL_COLOR_INFO_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_BG_COLOR;
				break;
				
			case MM_SBTL_INF_MSK_EDGE_COLOR:
				x_memcpy(&_gpt_sbtl_rcd->t_edge_color[ui1_idx], pv_info, sizeof(WGL_COLOR_INFO_T));
				_gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx] |= MM_SBTL_INF_MSK_EDGE_COLOR;
				break;

			case MM_SBTL_INF_MSK_PAGE_INF:
		    default:
			return MMSBTLR_INV_ARG;
		}
		return MMSBTLR_OK;
	}
	return MMSBTLR_INV_ARG;
	
}



INT32 sbtl_check_record_inf(
    SBTL_ENGINE_T*                   pt_engine, UINT32 ui4_check_mask)
{
    UINT8  ui1_idx = MM_SBTL_INF_RCD_VIDEO;
	UINT32 ui4_mask;
	
    if(pt_engine == NULL || _gpt_sbtl_rcd == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
	
	if((pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_LRC) ||
   	   (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_KRK_KING) ||
       (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_LYRIC_MP3_KRK))
    {
		ui1_idx = MM_SBTL_INF_RCD_AUDIO;
	}
	
    ui4_mask = _gpt_sbtl_rcd->aui1_rcd_mask[ui1_idx];
    ui4_mask &= ui4_check_mask;
	if (ui4_mask == 0)
	{
		return MMSBTLR_NOT_SUPPORT;
	}
    
	
	if (ui4_mask & MM_SBTL_INF_MSK_FONT_INF)
	{
		x_memcpy(&pt_engine->t_cfg.t_font_info,
			    &_gpt_sbtl_rcd->at_font_info[ui1_idx], 
				sizeof(WGL_FONT_INFO_T));
		//DBG_LOG_PRINT(("[sbtl util]  restore font size: %d \n",pt_engine->t_cfg.t_font_info.ui1_custom_size));
	}

	if (ui4_mask & MM_SBTL_INF_MSK_ENCODING)
	{
		x_memcpy(&pt_engine->t_cfg.e_encoding, 
				&_gpt_sbtl_rcd->e_coding[ui1_idx], 
				sizeof(MM_SBTL_ENCODING_T));
	}

	if (ui4_mask & MM_SBTL_INF_MSK_TM_OFST)
	{
		x_memcpy(&pt_engine->t_cfg.t_time_ofst,
				&_gpt_sbtl_rcd->t_tm_ofst[ui1_idx], 
				sizeof(MM_SBTL_TM_OFST_INF));
	}

	if (ui4_mask & MM_SBTL_INF_MSK_OSD_OFST)
	{
		x_memcpy(&pt_engine->t_cfg.t_pos_adjust, 
				&_gpt_sbtl_rcd->t_osd_ofst[ui1_idx], 
				sizeof(GL_POINT_T));
	}

	if (ui4_mask & MM_SBTL_INF_MSK_BG_COLOR)
	{
		x_memcpy(&pt_engine->t_cfg.t_font_bg_color, 
				&_gpt_sbtl_rcd->t_bg_color[ui1_idx], 
				sizeof(WGL_COLOR_INFO_T));
	}
	if (ui4_mask & MM_SBTL_INF_MSK_EDGE_COLOR)
	{
		x_memcpy(&pt_engine->t_cfg.t_edge_color_info,
			    &_gpt_sbtl_rcd->t_edge_color[ui1_idx],
			    sizeof(WGL_COLOR_INFO_T));
	}			

	if (ui4_mask & MM_SBTL_INF_MSK_FG_COLOR)
	{
		x_memcpy(&pt_engine->t_cfg.t_fg_color_info, 
				&_gpt_sbtl_rcd->t_fg_color[ui1_idx],
				sizeof(WGL_COLOR_INFO_T));
	}
    return MMSBTLR_OK;
}

BOOL sbtl_cmp_string(
    UINT16*             pui2_buf,
    UINT16              ui2_len,
    CHAR*               s_string)
{
    UINT16              ui2_str_len;
    UINT16              ui2_i;

    if(pui2_buf == NULL || ui2_len == 0)
    {
        return FALSE;
    }
    
    ui2_str_len = (UINT16)x_strlen((const CHAR *)s_string);
    if(ui2_str_len != ui2_len)
    {
        return FALSE;
    }

    for(ui2_i = 0; ui2_i < ui2_len; ui2_i++)
    {
        if(pui2_buf[ui2_i] != (UINT16)s_string[ui2_i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

INT32 sbtl_util_fcopybytes_fct(VOID*       pv_obj, 
                                       VOID*       pv_buf, 
                                       SIZE_T      z_buf_leng, 
                                       SIZE_T      z_copy_size,
                                       UINT32*     pui4_size)
{
#ifndef __KERNEL__
    UINT32              ui4_read;
    INT32               i4_ret;
    BOOL                b_buf_not_enough;
    SIZE_T              z_copy;
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fcopybytes_fct pv_obj=NULL\n"));
    ASSERT(pv_buf, ("_sbtl_util_fcopybytes_fct pv_buf=NULL\n"));
    ASSERT(pui4_size, ("_sbtl_util_fcopybytes_fct pui4_size=NULL\n"));
    
    if ((NULL==pv_obj) || (NULL==pv_buf) || (NULL==pui4_size))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
    
    if (z_buf_leng < z_copy_size)
    {
        z_copy = z_buf_leng;
        b_buf_not_enough = TRUE;
    }
    else
    {
        z_copy = z_copy_size;
        b_buf_not_enough = FALSE;
    }
#ifndef ANDROID
    i4_ret = x_fm_read(pt_obj->h_feeder, pv_buf, z_copy, &ui4_read);
    *pui4_size = ui4_read;
    if (i4_ret == FMR_OK)
    {
        if(ui4_read < z_copy)
        {
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        else
        {
            if(b_buf_not_enough)
            {
                return MMSBTLR_INSUFFICIENT_MEMORY;
            }
            else
            {
                return MMSBTLR_OK;
            }
        }
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    i4_ret = fread(pv_buf, 1, z_copy, (FILE*)pt_obj->h_feeder);
    if (i4_ret < 0)
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
    else if(i4_ret > 0)
    {
        ui4_read = i4_ret;
        *pui4_size = ui4_read;
        if(ui4_read < z_copy)
        {
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        return MMSBTLR_OK;
    }
    else
    {
        ui4_read = 0;
        return IMTK_PB_CB_ERROR_CODE_EOF;
    }
    
#endif
#else
    return MMSBTLR_OK;
#endif
}


INT32 sbtl_util_finput1bytes_fct(VOID* pv_obj, UINT8* pui1_data)
{
#ifndef __KERNEL__


    UINT8               ui1_data;
    UINT32              ui4_read;
    INT32               i4_ret;
    MEDIA_FEEDER_T*     pt_obj;

    
    ASSERT(pv_obj, ("_sbtl_util_finput1bytes_fct pv_obj=NULL\n"));
    ASSERT(pui1_data, ("_sbtl_util_finput1bytes_fct pui1_data=NULL\n"));

    if ((NULL==pv_obj) || (NULL==pui1_data))
    {
        return MMSBTLR_INV_ARG;
    }
    
    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
    
#ifndef ANDROID
    i4_ret = x_fm_read(pt_obj->h_feeder, (VOID*)&ui1_data, sizeof(ui1_data), &ui4_read);
    *pui1_data = ui1_data;
    if (i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    i4_ret = fread((VOID*)&ui1_data, 1, sizeof(ui1_data), (FILE*)pt_obj->h_feeder);
    if (i4_ret < 0)
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
    else if(i4_ret > 0)
    {
        ui4_read = i4_ret;
        if(ui4_read < sizeof(ui1_data))
        {
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        return MMSBTLR_OK;
    }
    else
    {
        ui4_read = 0;
        return IMTK_PB_CB_ERROR_CODE_EOF;
    }
    
#endif
#else
        return MMSBTLR_OK;
#endif

}

INT32 sbtl_util_fsetpos_fct(VOID* pv_obj, INT64 i8_offset, UINT8 ui1_whence)
{
#ifndef __KERNEL__
    INT32               i4_ret;
    UINT64              ui8_cur_pos;
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fsetpos_fct pv_obj=NULL\n"));

    if (NULL==pv_obj)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
#ifndef ANDROID
    i4_ret = x_fm_lseek(pt_obj->h_feeder, i8_offset, ui1_whence, &ui8_cur_pos);
    if(i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    switch (ui1_whence)
    {
    case MEDIA_SEEK_BGN:
        ui1_whence = SEEK_SET;
        break;
    case MEDIA_SEEK_CUR:
        ui1_whence = SEEK_CUR;
        break;
    case MEDIA_SEEK_END:
        ui1_whence = SEEK_END;
        break;
    default:
        return MMSBTLR_INTERNAL_ERROR;
    }
    i4_ret = fseek((FILE*)pt_obj->h_feeder, i8_offset, ui1_whence);
    ui8_cur_pos = ftell((FILE*)pt_obj->h_feeder);
    if(i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    /*else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }*/
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#endif

#else
        return MMSBTLR_OK;
#endif

}

INT32 sbtl_util_fgetpos_fct(VOID* pv_obj, UINT64* pui8_cur_pos)
{
#ifndef __KERNEL__
    UINT64              ui8_cur_pos = 0;
    INT32               i4_ret      = 0;
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fgetpos_fct pv_obj=NULL\n"));
    ASSERT(pui8_cur_pos, ("_sbtl_util_fgetpos_fct pui8_cur_pos=NULL\n"));

    if ((NULL==pv_obj) || (NULL==pui8_cur_pos))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
#ifndef ANDROID
    i4_ret = x_fm_lseek(pt_obj->h_feeder, 0, FM_SEEK_CUR, &ui8_cur_pos);
#else
    ui8_cur_pos = ftell((FILE*)pt_obj->h_feeder);
#endif
    *pui8_cur_pos = ui8_cur_pos;
    
    if(i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    return MMSBTLR_OK;
#endif
}


#ifdef INET_SUPPORT
INT32 sbtl_util_http_fcopybytes_fct(VOID*       pv_obj, 
                                    VOID*       pv_buf, 
                                    SIZE_T      z_buf_leng, 
                                    SIZE_T      z_copy_size,
                                    UINT32*     pui4_size)
{
#ifndef __KERNEL__


    UINT32              ui4_read;
    INT32               i4_ret;
    BOOL                b_buf_not_enough;
    SIZE_T              z_copy;
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fcopybytes_fct pv_obj=NULL\n"));
    ASSERT(pv_buf, ("_sbtl_util_fcopybytes_fct pv_buf=NULL\n"));
    ASSERT(pui4_size, ("_sbtl_util_fcopybytes_fct pui4_size=NULL\n"));
    
    if ((NULL==pv_obj) || (NULL==pv_buf) || (NULL==pui4_size))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
    
    if (z_buf_leng < z_copy_size)
    {
        z_copy = z_buf_leng;
        b_buf_not_enough = TRUE;
    }
    else
    {
        z_copy = z_copy_size;
        b_buf_not_enough = FALSE;
    }
#ifdef USE_SMART_FILE
    i4_ret = x_smart_file_read(pt_obj->h_feeder, (CHAR*)pv_buf, z_copy, (int*)&ui4_read);
#else
    i4_ret = x_httpc_fm_read(pt_obj->h_feeder, pv_buf, z_copy, &ui4_read);
#endif
    *pui4_size = ui4_read;
    if (i4_ret == FMR_OK)
    {
        if(ui4_read < z_copy)
        {
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        else
        {
            if(b_buf_not_enough)
            {
                return MMSBTLR_INSUFFICIENT_MEMORY;
            }
            else
            {
                return MMSBTLR_OK;
            }
        }
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    return MMSBTLR_OK;
#endif
}


INT32 sbtl_util_http_finput1bytes_fct(VOID* pv_obj, UINT8* pui1_data)
{
#ifndef __KERNEL__


    UINT8               ui1_data = 0;
    UINT32              ui4_read = 0;
    INT32               i4_ret;
    MEDIA_FEEDER_T*     pt_obj;

    
    ASSERT(pv_obj, ("_sbtl_util_finput1bytes_fct pv_obj=NULL\n"));
    ASSERT(pui1_data, ("_sbtl_util_finput1bytes_fct pui1_data=NULL\n"));

    if ((NULL==pv_obj) || (NULL==pui1_data))
    {
        return MMSBTLR_INV_ARG;
    }
    
    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
#ifdef USE_SMART_FILE
    i4_ret = x_smart_file_read(pt_obj->h_feeder, (CHAR*)&ui1_data, sizeof(ui1_data), (int*)&ui4_read);
#else
    i4_ret = x_httpc_fm_read(pt_obj->h_feeder, (VOID*)&ui1_data, sizeof(ui1_data), &ui4_read);
#endif
    *pui1_data = ui1_data;
    if (i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
        return MMSBTLR_OK;
#endif

}

INT32 sbtl_util_http_fsetpos_fct(VOID* pv_obj, INT64 i8_offset, UINT8 ui1_whence)
{
#ifndef __KERNEL__


    INT32               i4_ret;
#ifdef USE_SMART_FILE
    INT64              i8_cur_pos;
#else
    UINT64             ui8_cur_pos;
#endif
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fsetpos_fct pv_obj=NULL\n"));

    if (NULL==pv_obj)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
#ifdef USE_SMART_FILE
    switch(ui1_whence)
    {   
    case MEDIA_SEEK_BGN:
        break;
    case MEDIA_SEEK_CUR:
        i8_offset += x_smart_file_offset(pt_obj->h_feeder);
        break;
    case MEDIA_SEEK_END:
    {
        INT64 i8_filesize = x_smart_file_size(pt_obj->h_feeder);
        if(i8_filesize != -1 && i8_filesize != 0)
        {
            i8_offset += i8_filesize;
        }
        else
        {
            ASSERT(pv_obj, ("_sbtl_util_fsetpos_fct can't support seek without file size\n"));
        }
        break;
    } 
    default:
        return MMSBTLR_INV_ARG;
        break;
    }
    i4_ret = x_smart_file_seek((INT32)pt_obj->h_feeder, i8_offset, &i8_cur_pos);
#else
    i4_ret = x_httpc_fm_lseek(pt_obj->h_feeder, i8_offset, ui1_whence, &ui8_cur_pos);
#endif
    if(i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
        return MMSBTLR_OK;
#endif

}

INT32 sbtl_util_http_fgetpos_fct(VOID* pv_obj, UINT64* pui8_cur_pos)
{
#ifndef __KERNEL__
#ifndef USE_SMART_FILE
    UINT64              ui8_cur_pos;
    INT32               i4_ret;
#endif
    MEDIA_FEEDER_T*     pt_obj;

    ASSERT(pv_obj, ("_sbtl_util_fgetpos_fct pv_obj=NULL\n"));
    ASSERT(pui8_cur_pos, ("_sbtl_util_fgetpos_fct pui8_cur_pos=NULL\n"));

    if ((NULL==pv_obj) || (NULL==pui8_cur_pos))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_obj = (MEDIA_FEEDER_T*)pv_obj;
#ifdef USE_SMART_FILE
    *pui8_cur_pos = x_smart_file_offset(pt_obj->h_feeder);
    
    if(*pui8_cur_pos >= 0 && *pui8_cur_pos < x_smart_file_size(pt_obj->h_feeder))
    {
        return MMSBTLR_OK;
    }
    else if (*pui8_cur_pos >= x_smart_file_size(pt_obj->h_feeder))
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#else
    i4_ret = x_httpc_fm_lseek(pt_obj->h_feeder, 0, FM_SEEK_CUR, &ui8_cur_pos);
    *pui8_cur_pos = ui8_cur_pos;
    
    if(i4_ret == FMR_OK)
    {
        return MMSBTLR_OK;
    }
    else if (i4_ret == FMR_EOF)
    {
        return MMSBTLR_INSUFFICIENT_SIZE;
    }
    else
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
#endif
    
#else
    return MMSBTLR_OK;
#endif
}
#endif

INT32 x_sbtl_util_fcopybytes_fct(VOID*       pv_obj, 
                                    VOID*       pv_buf, 
                                    SIZE_T      z_buf_leng, 
                                    SIZE_T      z_copy_size,
                                    UINT32*     pui4_size)
{
    return sbtl_util_fcopybytes_fct(pv_obj,pv_buf,z_buf_leng,z_copy_size,pui4_size);
}


INT32 x_sbtl_util_finput1bytes_fct(VOID* pv_obj, UINT8* pui1_data)
{
    return sbtl_util_finput1bytes_fct(pv_obj, pui1_data);
}

INT32 x_sbtl_util_fsetpos_fct(VOID* pv_obj, INT64 i8_offset, UINT8 ui1_whence)
{
    return sbtl_util_fsetpos_fct(pv_obj, i8_offset, ui1_whence);
}

INT32 x_sbtl_util_fgetpos_fct(VOID* pv_obj, UINT64* pui8_cur_pos)
{
    return sbtl_util_fgetpos_fct(pv_obj, pui8_cur_pos);
}

INT32 sbtl_util_skip_effect(UINT8* pui1_data, UINT32 ui4_len)
{
    UINT16 ui2_idx        = 0;
    BOOL   b_is_effect    = FALSE;
    UINT16 ui2_tok_s      = 0;
    UINT16 ui2_end        = 0;
    UINT16 ui2_skip       = 0;
    UINT16 ui2_match      = 0;
    
    if (pui1_data == NULL || ui4_len == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    //currently not support subtitle ass effect just skip it
    //search for {content}  content leading is \ and only contain: word (,)
    
    for (ui2_idx = 0; ui2_idx < ui4_len; ui2_idx++)
    {
        UINT8 ui1_cur = pui1_data[ui2_idx];
        if (ui1_cur == 0)
        {
            ui2_end = ui2_idx;
            break;
        }
        if (ui1_cur == '{')
        {
            b_is_effect = TRUE;
            ui2_tok_s   = ui2_idx;
        }
        else if (b_is_effect)
        {
            if (ui2_idx-ui2_tok_s == 1 && ui1_cur != '\\')
            {
                b_is_effect = FALSE;
            }
            else if (ui1_cur == '}')
            {
                b_is_effect = FALSE;
                pui1_data[ui2_tok_s] = 0;
                pui1_data[ui2_idx]   = 0;
                ui2_match++;
            }
            else if (
                ui1_cur == '('  ||
                ui1_cur == ')'  ||
                ui1_cur == ','  ||
                ui1_cur == ' '  ||
                ui1_cur == '\\' ||
                ui1_cur == '&' ||
                (ui1_cur >= '0' && ui1_cur <= '9') ||
                (ui1_cur >= 'a' && ui1_cur <= 'z') ||
                (ui1_cur >= 'A' && ui1_cur <= 'Z'))
            {
                continue;
            }
            else
            {
                b_is_effect = FALSE;
            }
        }
    }
    if (ui2_match == 0)
    {
        return MMSBTLR_OK;
    }
    
    b_is_effect = FALSE;
    
    for (ui2_idx = 0; ui2_idx < ui2_end; ui2_idx++)
    {
        if (pui1_data[ui2_idx] == 0)
        {
            ui2_skip++;
            if (b_is_effect)
            {
                b_is_effect = FALSE;
            }
            else
            {
                b_is_effect = TRUE;
            }
        }
        else
        {
            if (b_is_effect)
            {
                ui2_skip++;
            }
            else
            {
                pui1_data[ui2_idx-ui2_skip] = pui1_data[ui2_idx];
            }
        }
    }
    pui1_data[ui2_end-ui2_skip] = 0;
   // DBG_LOG_PRINT(("skip string is:%s len:%d\n", pui1_data, ui2_end-ui2_skip));
    if (ui2_end-ui2_skip < ui4_len-1)
    {
         pui1_data[ui2_end-ui2_skip+1] = 0;
    }   
    return MMSBTLR_OK;
}
