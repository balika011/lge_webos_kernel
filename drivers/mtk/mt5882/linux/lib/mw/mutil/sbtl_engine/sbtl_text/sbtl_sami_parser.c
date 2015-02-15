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
 * $RCSfile: sbtl_sami_parser.c,v $
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
#include "x_common.h"
#include "inc/common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_widget.h"
#include "mutil/sbtl_engine/sbtl_text/sbtl_text_page_inst_mngr.h"
#include "mutil/sbtl_engine/sbtl_text/sbtl_sami_parser.h"
#include "u_mm_sbtl_engine.h"



#else /*__KERNEL__*/
#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"


#include "../sbtl_dbg.h"
#include "../sbtl_struct.h"
#include "../sbtl_utils.h"
#include "../sbtl_register.h"
#include "../sbtl_state.h"
#include "../sbtl_widget.h"
#include "sbtl_text_page_inst_mngr.h"
#include "sbtl_sami_parser.h"
#include "u_mm_sbtl_engine.h"

#endif /*__KERNEL__*/



/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define CARRIAGE_RETURN             0x0d
#define LINE_FEED                   0x0a
#define CHAR_TAB                    0x09
#define CHAR_SPACE                  0x20

#define PTS_FREQ        ((UINT64)(90000))
#define TIMER_PERIOD    500
#define CONTAINER_LEN   16
#define SAMI_CC_SZ      3
#define CONTENT_SIZE    512

#define SBTL_CLR_NAME_MAX_LEN     12
#define MAX_TOKEN_LENGTH          (4*1024)
#define CHECK_DATA_LENGTH         (2*1024)
#define CHECK_TIME_OUT            500


typedef struct _SAMI_PRIVATE_T
{
    UINT16          w2s_text[CONTENT_SIZE];
    UINT8           s_text_buf[CONTENT_SIZE];
    CHAR            s_curr_class[CONTAINER_LEN];
    UINT8           ui1_total_stream_count;
    UINT8           ui1_stream_idx;
    SMAI_CC_INF_T   at_stream_info[SAMI_CC_SZ];
    UINT8           ui1_clr_stat;
	GL_COLOR_T      t_color;
	CHAR*           ps_token_buf;
}SAMI_PRIVATE_T;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
#define SAMI_ID "^i<SAMI>"
#define SAMI_SYNC_START "^i<SYNC *START *= *(\\d+)"
#define SAMI_END_TIME "iEND *= *(\\d+)"

#define SAMI_SYNC_END   "^i</SYNC>"
#define SAMI_HEAD "^i<HEAD>"
#define SAMI_BODY "^i<BODY>"
#define SAMI_STYLE_START "^i<STYLE"

#define SAMI_STYLE_END "^i</STYLE>"
#define SAMI_P_CLASS "^i<P *CLASS *= *(\\w+)"
#define SAMI_P "^i<P>"
#define SAMI_P_END "^i</P>"
#define SAMI_BR "^i<BR"
#define SAMI_FONT_COLOR "^i<FONT *COLOR *= *(\\w+)>"
#define SAMI_LATIN_CHAR "^i&#0*X(\\x+);"

typedef enum
{
    SBTL_SEARCH_BEGIN,
    SBTL_SEARCH_MIDDLE
} SBTL_SEARCH_TYPE_T;

typedef struct
{
	CHAR* ps_name;
	GL_COLOR_T t_color;
}SAMI_COLOR_T;

#define SAMI_COLOR_CNT  7
#define SAMI_MAX_PTS (0xFFFFFFFF)


static SAMI_COLOR_T at_smi_clr_map[SAMI_COLOR_CNT] =
{
	{"trans",      {0,     {   0 }, {   0 }, {   0 } }},
	{"white",      {255,   {  255}, {  255}, {  255} }},
	{"yellow",     {255,   {  255}, {  255}, {    0} }},
	{"blue",       {255,   {   0 }, {   0 }, {  255} }},
    {"lightgreen", {255,   {  128}, {  255}, {  128} }},
    {"black",      {255,   {   0 }, {   0 }, {   0 } }},
    {"violet",     {255,   {  128}, {   0 }, {  255} }}
};

/*-----------------------------------------------------------------------------
                    Regular expression support
 ----------------------------------------------------------------------------*/


static INT32 _sami_apply_color(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SAMI_PRIVATE_T*                    pt_this;
    WGL_COLOR_INFO_T                   t_color;
	SBTL_TEXT_PAGE_MNGR_PRIVATE_T*     pt_private;
	INT32                              i4_ret = MMSBTLR_OK;
	if (pt_mngr == NULL || pt_mngr->pv_private == NULL)
	{
		return MMSBTLR_INV_ARG;
	}
	pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
	if (pt_private == NULL || pt_private->pv_data == NULL)
	{
		return MMSBTLR_INV_ARG;
	}
	 
    pt_this = (SAMI_PRIVATE_T*)pt_private->pv_data;
	 
	if (pt_this->ui1_clr_stat == 2)
	{
		x_memset(&t_color, 0, sizeof(WGL_COLOR_INFO_T));
		t_color.e_type = MM_SBTL_DFT_FG_TYPE;
        x_memcpy ((VOID *)&t_color.u_color_data.t_standard.t_enable, 
                 & pt_this->t_color, 
                 sizeof(GL_COLOR_T));

        x_memcpy ((VOID *)&t_color.u_color_data.t_standard.t_disable, 
                   & pt_this->t_color,  
                   sizeof(GL_COLOR_T));

        x_memcpy ((VOID *)&t_color.u_color_data.t_standard.t_highlight, 
                   & pt_this->t_color, 
                   sizeof(GL_COLOR_T));
		i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                         SBTL_WGL_CMD_GL_SET_COLOR,
                         (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                         (VOID*)&t_color);
		
	}
	if (pt_this->ui1_clr_stat == 1)
	{
		i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_fg_color_info);
		pt_this->ui1_clr_stat = 0;
	}
    return  i4_ret;
}

INT32 read_one_char(SBTL_ENGINE_T *pt_engine, UINT16 *pui2_data, BOOL b_UTF16)
{
    INT32 i4_ret;
    UINT8 ui1_data;
    if (b_UTF16)
    {
        i4_ret = sbtl_feeder_inp2byte(pt_engine,
                                      pui2_data, 
                                      MM_SBTL_FEEDER_SUBTITLE);
    }
    else
    {
        i4_ret = sbtl_feeder_inp1byte(pt_engine,
                                      &ui1_data, 
                                      MM_SBTL_FEEDER_SUBTITLE);
    }
    if (i4_ret != MMSBTLR_OK)
    {
         return MMSBTLR_EOT;
    }
    if (b_UTF16 == FALSE)
    {
        *pui2_data = ui1_data;
    }
    return MMSBTLR_OK;
    
}

static INT32 sami_read_util(
	         SBTL_ENGINE_T* pt_engine,
             BOOL           b_UTF16,
             CHAR*          s_buf,
             CHAR           c_find,
             UINT16         ui2_len)
{
    UINT16 ui2_data;
    UINT16 ui2_idx;
	if (s_buf == NULL)
	{
		return MMSBTLR_INV_ARG;
	}
    for (ui2_idx = 0; ui2_idx < ui2_len; ui2_idx++)
    {
        if (read_one_char(pt_engine, &ui2_data, b_UTF16) != MMSBTLR_OK)/*error return directly*/
        {
            return MMSBTLR_EOT;
        }
        if (ui2_data > 0xff)
        {
			s_buf[ui2_idx] = 0;
			return (INT32)ui2_data;
        }
        s_buf[ui2_idx] = (CHAR)ui2_data;
        if (s_buf[ui2_idx] == c_find)
        {
			s_buf[ui2_idx+1] = 0;
			DBG_INFO(("Find Token %s\n",s_buf));
			if (c_find == '>')
	        {
				if (s_buf[0] == '!' || (s_buf[0] == '-' && s_buf[1] == '-'))
	            break;
	        }
            return MMSBTLR_OK;
        }
    }
	s_buf[ui2_len-1] = 0;
    return MMSBTLR_NOT_FOUND;
}


void printc(CHAR*c, UINT16 s, UINT16 e)
{
    for (;s<=e;s++)
    {
        DBG_INFO(("%c",*(c+(int)s)));
    }
    DBG_INFO(("\n"));
}

static INT32 sami_fill_class(SAMI_PRIVATE_T* pt_sami_private, CHAR* s_buf)
{

	CHAR*       ps_dot = s_buf;
	CHAR*       ps_pair;
	UINT16      ui2_len;
	UINT8       ui1_stream_idx;
	UINT16      ui2_pos;
	UINT16      ui2_start[2];
    UINT16      ui2_end[2];
    MEM_T       t_mem;
	
	t_mem.start    = ui2_start;
    t_mem.end      = ui2_end;
    t_mem.len      = 2;
	ui1_stream_idx =  pt_sami_private->ui1_total_stream_count;
	if (ui1_stream_idx >= SAMI_CC_SZ)
	{
		return MMSBTLR_EOT;
	}

	while ((ps_dot = x_strchr(ps_dot, '.')) != NULL)
	{
		ps_dot++;
		if (match_reg_exp("^(\\w+) *\\{([^\\}]+)\\}", ps_dot, &t_mem, &ui2_pos))
		{
			ui2_len = t_mem.end[0]-t_mem.start[0]+1;
			if (ui2_len >= CONTAINER_LEN-1)
			{
				ui2_len = CONTAINER_LEN-1;
			}
			x_memcpy(pt_sami_private->at_stream_info[ui1_stream_idx].s_class_name,
					ps_dot+t_mem.start[0],
					ui2_len);
			pt_sami_private->at_stream_info[ui1_stream_idx].s_class_name[ui2_len] = 0;
			pt_sami_private->at_stream_info[ui1_stream_idx].ui1_name_len = (UINT8)ui2_len;
			
			DBG_INFO(("Get Class name: %s\n", pt_sami_private->at_stream_info[ui1_stream_idx].s_class_name));
			
			ps_dot[t_mem.end[1]+1] = 0;
			
			ps_pair = ps_dot + t_mem.start[1];
			ps_dot += ui2_pos;

			while(match_reg_exp("(\\w+): *([^;]+);", ps_pair, &t_mem, &ui2_pos))
            {
				if (match_reg_exp("^iLANG", ps_pair+t_mem.start[0], NULL, NULL))
				{
					if (t_mem.end[1] - t_mem.start[1] >= 1)
					{
						x_memcpy(pt_sami_private->at_stream_info[ui1_stream_idx].aui1_lang,
							     ps_pair+t_mem.start[1],
							     2);
					}
				}
                printc(ps_pair,t_mem.start[0],t_mem.end[0]);
                printc(ps_pair,t_mem.start[1],t_mem.end[1]);
                ps_pair += ui2_pos;
            }
            ui1_stream_idx++;
            pt_sami_private->ui1_total_stream_count = ui1_stream_idx;
		}
	}
	return MMSBTLR_OK;
}

static VOID _sami_class_info(SBTL_PAGE_INST_MNGR_T*     pt_mngr, BOOL b_UTF16)
{
    INT32                               i4_ret;
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;
    SBTL_ENGINE_T*                      pt_engine;
    
    CHAR*                               s_buf;
    UINT32                              ui4_timer_start;
    BOOL                                b_token;
    UINT16                              ui2_state;
    UINT16                              ui2_read_cnt;
    UINT16                              ui2_data;
    
    if (pt_mngr == NULL ||
		pt_mngr->pt_engine == NULL ||
		pt_mngr->pv_private == NULL)
    {
		return;
    }
    pt_engine = pt_mngr->pt_engine;
    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
	if (pt_sami_private == NULL || pt_sami_private->ps_token_buf == NULL)
	{
		return;
	}
	s_buf = pt_sami_private->ps_token_buf;
    pt_sami_private->ui1_total_stream_count = 0;
    ui2_state      = 0;
    ui2_read_cnt   = 0;
    ui4_timer_start= x_os_get_sys_tick() * x_os_get_sys_tick_period();
    
    while (1)
    {
        b_token = FALSE;
        i4_ret = read_one_char(pt_engine, &ui2_data, b_UTF16);
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        ui2_read_cnt++;
        if (ui2_read_cnt > CHECK_DATA_LENGTH)/*timeout*/
        {
            ui2_read_cnt = 0;
            if (x_os_get_sys_tick()* x_os_get_sys_tick_period()-ui4_timer_start > CHECK_TIME_OUT)
            {
                DBG_INFO(("get_info Read timeout\n"));
                break;
            }
        }
        
        if (ui2_data == '<')
        {
            s_buf[0] = '<';
            i4_ret = sami_read_util(pt_engine, b_UTF16, s_buf+1, '>', MAX_TOKEN_LENGTH-1);
            if (i4_ret == MMSBTLR_EOT)
            {
                break;
            }
            ui2_read_cnt += MAX_TOKEN_LENGTH;
            if (i4_ret == MMSBTLR_OK)
            {
                b_token = TRUE;
            }
            else
            {
				if (ui2_state != 2)
				{
					continue; 
				}
            }
        }
        
        switch (ui2_state)
        {
            case 0:
            if (b_token == FALSE) break;
            if (match_reg_exp(SAMI_HEAD, s_buf, NULL, NULL))/*got head*/
            {
               ui2_state = 1;
			   DBG_INFO(("class Step 1...\n"));
               break;
            }
            if (match_reg_exp(SAMI_BODY, s_buf, NULL, NULL))/*get body without head*/
            {
                pt_sami_private->ui1_total_stream_count = 1;
                pt_sami_private->at_stream_info[0].aui1_lang[0] = 0xFF;
                pt_sami_private->at_stream_info[0].aui1_lang[1] = 0xFF;
                return;
            }
            break;
            
            case 1:
            if (b_token == FALSE) break;
            if (match_reg_exp(SAMI_STYLE_START, s_buf, NULL, NULL))
            {
                ui2_state = 2;
				DBG_INFO(("class Step 2...\n"));
            }
            if (match_reg_exp(SAMI_BODY, s_buf, NULL, NULL))/*get body no style*/
            {
                pt_sami_private->ui1_total_stream_count = 1;
                pt_sami_private->at_stream_info[0].aui1_lang[0] = 0;
                pt_sami_private->at_stream_info[0].aui1_lang[1] = 0;
                return;
            }
            break;

            case 2:
            if (b_token)
            {
                if ( match_reg_exp(SAMI_STYLE_END, s_buf, NULL, NULL))
                {
                    if (pt_sami_private->ui1_total_stream_count == 0)
                    {
                        pt_sami_private->ui1_total_stream_count = 1;
                        pt_sami_private->at_stream_info[0].aui1_lang[0] = 0;
                        pt_sami_private->at_stream_info[0].aui1_lang[1] = 0;
                    }
                    return;
                }             
            }
			if (ui2_data == '<')
			{
				DBG_INFO(("find class info...\n"));
				i4_ret = sami_fill_class(pt_sami_private, s_buf);
				if (pt_sami_private->ui1_total_stream_count  >= SAMI_CC_SZ)
			    {
			        return;
			    }
			}
            break;
            
            default:
                return; 
        }
    }
}

static INT32 _sami_get_num(CHAR* ps_c, UINT8 ui1_b)
{
    #ifndef __KERNEL__
    return (INT32)x_strtoll(ps_c, NULL, ui1_b);
    #else
    INT32 i4_n = 0;
    while (IS_HEX(*ps_c))
    {
        i4_n *= ui1_b;
        if (*ps_c >= 'A' && *ps_c <= 'F')
        {
            i4_n += *ps_c - 'A' + 10;
        }
        else if (*ps_c >= 'a' && *ps_c <= 'f')
        {
            i4_n += *ps_c - 'a' + 10;
        }
        else
        {
            i4_n += *ps_c - '0';
        }
        ps_c++;
    }
    return i4_n;
    #endif
}

static INT32 _sami_parse_special_chars(CHAR* ps_buf)
{
	if (ps_buf && ps_buf[0] == '&')
	{
        MEM_T         t_mem;
		UINT16        ui2_start[2];
        UINT16        ui2_end[2];
        t_mem.start = ui2_start;
        t_mem.end = ui2_end;
        t_mem.len = 2;
                
		if (x_strcmp(ps_buf, "&nbsp;") == 0)
		{
			return ' ';
		}
		else if (x_strcmp(ps_buf, "&lt;") == 0)
		{
			return '<';
		}
		
		else if (x_strcmp(ps_buf, "&gt;") == 0)
		{
			return '>';
		}
		else if (x_strcmp(ps_buf, "&amp;") == 0)
		{
			return '&';
		}
		else if (x_strcmp(ps_buf, "&quot;") == 0)
		{
			return '&';
		}
		else if (match_reg_exp(SAMI_LATIN_CHAR, ps_buf, &t_mem, NULL))
		{
			return _sami_get_num(ps_buf+t_mem.start[0], 16);
		}
	}
	return MMSBTLR_NOT_FOUND;
}


static BOOL sami_match_sync(CHAR* s_buf, PTS_T* pt_pts_start, PTS_T* pt_pts_end)
{
	UINT32 ui4_n;
	UINT16                              ui2_start[2];
    UINT16                              ui2_end[2];
    MEM_T                               t_mem;
	t_mem.start = ui2_start;
    t_mem.end = ui2_end;
    t_mem.len = 2;
	
	if (match_reg_exp(SAMI_SYNC_START, s_buf, &t_mem, NULL))
    {
		ui4_n = (UINT32)_sami_get_num(s_buf+t_mem.start[0], 10);
		*pt_pts_start = ui4_n*(PTS_FREQ/1000);
		*pt_pts_end   = SAMI_MAX_PTS;
		if (match_reg_exp(SAMI_END_TIME, s_buf, &t_mem, NULL))
		{
		 	ui4_n = (UINT32)_sami_get_num(s_buf+t_mem.start[0],10);
			*pt_pts_end = ui4_n*(PTS_FREQ/1000);;
		}
		return TRUE;
    }
	return FALSE;
	 
}

static BOOL sami_match_para(
	          SAMI_PRIVATE_T* pt_sami_private,
	          CHAR*           s_buf)
{

	UINT16 ui2_start[2];
    UINT16 ui2_end[2];
    MEM_T  t_mem;
	UINT16 ui2_len;
	t_mem.start = ui2_start;
    t_mem.end = ui2_end;
    t_mem.len = 2;
	
	if ( match_reg_exp(SAMI_P, s_buf, NULL, NULL))
	{
		return TRUE;
	}
    if (match_reg_exp(SAMI_P_CLASS, s_buf, &t_mem, NULL))
    { 
		if (pt_sami_private->ui1_total_stream_count == 1)
	    {
	        return TRUE;
	    }
		ui2_len = t_mem.end[0]-t_mem.start[0]+1;
        if (ui2_len >= CONTAINER_LEN-1)
        {
            ui2_len = CONTAINER_LEN-1;
        }
		x_memcpy(pt_sami_private->s_curr_class, s_buf+t_mem.start[0], ui2_len);
		pt_sami_private->s_curr_class[ui2_len] = 0;
		DBG_INFO(("Check class:%s\n", pt_sami_private->s_curr_class));;

	    if (pt_sami_private->ui1_stream_idx >= SAMI_CC_SZ)
	    {
	        return FALSE;
	    }
	    
	   if (x_strcmp((const CHAR *)pt_sami_private->s_curr_class, 
	                 (const CHAR *)pt_sami_private->at_stream_info[pt_sami_private->ui1_stream_idx].s_class_name
	                 ) == 0 )
	   {
	   		return TRUE;
	   }
    }
    return FALSE;
}

static BOOL sami_match_color(
	          SAMI_PRIVATE_T* pt_sami_private,
	          CHAR*           s_buf)
{

	UINT16 ui2_start[2];
    UINT16 ui2_end[2];
    MEM_T  t_mem;
	t_mem.start = ui2_start;
    t_mem.end = ui2_end;
    t_mem.len = 2;

	if (match_reg_exp(SAMI_FONT_COLOR, s_buf, &t_mem, NULL))
    {
		UINT8  ui1_idx = 0;
        s_buf[t_mem.end[0]+1] = 0;
	    for (ui1_idx = 0; ui1_idx<SAMI_COLOR_CNT; ui1_idx++)
	    {
			#ifndef __KERNEL__ 	
			if (x_strcasecmp(s_buf+t_mem.start[0], at_smi_clr_map[ui1_idx].ps_name) == 0)
		 	#endif
			{
				x_memcpy(&pt_sami_private->t_color, 
						 &(at_smi_clr_map[ui1_idx].t_color), 
						 sizeof(GL_COLOR_T));
				pt_sami_private->ui1_clr_stat = 2;
				break;
		 	}
	     }
		return TRUE;
    }
	return FALSE;
}

static BOOL sami_valid_str(UINT8* s_str)
{
    if (s_str)
    {
        while (*s_str)
        {
            if (x_strchr(" \t\n\r", *s_str) == NULL)
            {
                return TRUE;
            }
            s_str++;
        }
    }
    return FALSE;
}


INT32 sami_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                           PTS_T                    t_pts_current,
                           PTS_T*                   pt_pts_start,
                           PTS_T*                   pt_pts_end,
                           BOOL*                    pb_found)
{
    INT32                               i4_ret;
    SBTL_ENGINE_T*                      pt_engine;
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;
   
    UINT16                              ui2_data;
    UINT16                              ui2_state = 0;
    UINT16                              ui2_cont_idx = 0;
    BOOL                                b_token;
    CHAR*                               s_buf;
    BOOL                                b_UTF16;
    BOOL                                b_end_wr = FALSE;
    UINT32                              ui4_timer_start;
    UINT16                              ui2_read_cnt;
	
    UINT64                              ui8_offset = 0;
	BOOL                                b_join_next = FALSE;
	BOOL                                b_quit = FALSE;
	PTS_T                               t_pts_s = 0;
	PTS_T                               t_pts_e = 0;
    
    if (pt_mngr == NULL ||
		pt_mngr->pt_engine == NULL ||
		pt_mngr->pv_private == NULL)
    {
		return MMSBTLR_INV_ARG;
    }
    pt_engine = pt_mngr->pt_engine;
    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
	if (pt_sami_private == NULL || pt_sami_private->ps_token_buf == NULL)
	{
		return MMSBTLR_INV_ARG;
	}
	s_buf = pt_sami_private->ps_token_buf;
    b_UTF16 = pt_private->b_UTF16;
    ui4_timer_start = x_os_get_sys_tick();
    ui2_read_cnt = 0;
    *pb_found = FALSE;
	*pt_pts_end = SAMI_MAX_PTS;
	
    //we always load data but not parse
    while(!b_quit)
    {
        b_token = FALSE;
        i4_ret = read_one_char(pt_engine, &ui2_data, b_UTF16);
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        ui2_read_cnt++;
        if (ui2_read_cnt > CHECK_DATA_LENGTH)/*timeout*/
        {
            ui2_read_cnt = 0;
            if ((x_os_get_sys_tick()-ui4_timer_start)*x_os_get_sys_tick_period() > CHECK_TIME_OUT)
            {
                DBG_INFO(("Read timeout\n"));
                break;
            }
        }
        
        if (ui2_data == '<')
        {
            s_buf[0] = '<';
            sbtl_feeder_getpos(pt_engine, &ui8_offset, MM_SBTL_FEEDER_SUBTITLE);
            ui8_offset--;
            if (b_UTF16) ui8_offset--;
            i4_ret = sami_read_util(pt_engine, b_UTF16, s_buf+1, '>', MAX_TOKEN_LENGTH-1);
            if (i4_ret == MMSBTLR_EOT)
            {
                break;
            }
            ui2_read_cnt += CHECK_DATA_LENGTH;
            if (i4_ret == MMSBTLR_OK)
            {
                b_token = TRUE;
            }
            else
            {
                continue; 
            }
        }
        if (b_token == FALSE && ui2_state != 2 &&  ui2_state != 3)/*get content need it*/
        {
            continue;
        }
        
        switch(ui2_state)
        {
            case 0:/*step 0 find <SYNC START>*/
            if (!b_token)
                break;
			if (sami_match_sync(s_buf, pt_pts_start, pt_pts_end) &&
				*pt_pts_end > t_pts_current)
			{
				ui2_state = 1; 
				DBG_INFO(("Step 1...\n"));
			}
            break;
            
            case 1:/*step 1 find <P> */
            if (!b_token)break;
			
			if (sami_match_para(pt_sami_private, s_buf))
			{
				ui2_state = 2;
				if (pt_sami_private->ui1_clr_stat == 2)
				{
					pt_sami_private->ui1_clr_stat = 1;
				}
                DBG_INFO(("Step 2...\n"));
			}
            break;
            
            case 2:/*step 2 skip unwanted char*/
            if(x_strchr("\r\n \t", (CHAR)ui2_data))
            {
                break;
            }
            else//no break 
            {
				if (b_join_next == FALSE)
				{
                	ui2_cont_idx = 0;
				}
                ui2_state = 3;
                b_end_wr  = FALSE;
				*pb_found = TRUE;
            }
			
            case 3:/*step 3 parse content, go to step1 if not found yet*/
			if (b_token)
			{
				if (match_reg_exp(SAMI_BR, s_buf, NULL, NULL))
                {
                    ui2_data = LINE_FEED;
                }
				else
				{
	                if (sami_match_color(pt_sami_private, s_buf))
	                {
	                }
	                else if(match_reg_exp(SAMI_SYNC_END, s_buf, NULL, NULL) ||
	                        match_reg_exp(SAMI_P_END,    s_buf, NULL, NULL) ||
	                        match_reg_exp("^i<P +",      s_buf, NULL, NULL))
	                {
	                    DBG_INFO(("Find para end...\n"));
						if (*pt_pts_end != SAMI_MAX_PTS) 
						{
							b_quit = TRUE;
						}
	                    b_end_wr = TRUE;
	                }
	                else if (sami_match_sync(s_buf, &t_pts_s, &t_pts_e))
	                {
	                    if (t_pts_current < t_pts_s)/*for next parse*/
	                    {
	                        if (t_pts_s - *pt_pts_start <= PTS_FREQ/3)
	                        {
								b_join_next = TRUE;
								ui2_state = 1;
	                            break;/*parse for content contiue*/
	                        }
							sbtl_feeder_setpos(pt_engine, 
											   (INT64)ui8_offset,
	                                            MEDIA_SEEK_BGN,
	                                            MM_SBTL_FEEDER_SUBTITLE);
							if (*pt_pts_end == SAMI_MAX_PTS)
							{
		                    	*pt_pts_end = t_pts_s;
							}
							b_quit = TRUE;
							break;
	                    }
	                    else
	                    {
	                        *pt_pts_start = t_pts_s;
							*pt_pts_end   = t_pts_e;
	                        ui2_state = 1;
	                        DBG_INFO(("Still less\n"));
	                    } 
	                }
					break;
				}
            } 
            else
            {
                if (ui2_data == CARRIAGE_RETURN || ui2_data  == LINE_FEED)
                {
                    break;/*skip return*/
                }
            }
            if (!b_end_wr && ui2_cont_idx < CONTENT_SIZE-1)
            {
				if (ui2_data == '&')
				{
					s_buf[0] = '&';
					i4_ret = sami_read_util(pt_engine, b_UTF16, s_buf+1, ';', MAX_TOKEN_LENGTH-1);
					if (i4_ret != MMSBTLR_OK)
		            {
		                break;
		            }
					i4_ret = _sami_parse_special_chars(s_buf);
					if (i4_ret < 0)
					{
						break;
					}
					ui2_data = (UINT16)i4_ret;
				}
                if (b_UTF16)
                {
                     pt_sami_private->w2s_text[ui2_cont_idx++] = ui2_data;
                }
                else
                {
                     pt_sami_private->s_text_buf[ui2_cont_idx++] = (UINT8)ui2_data;
                }
            }
            break; 
        }     
    }

	if (*pb_found)
	{
		if (ui2_cont_idx < CONTENT_SIZE-1)
		{
		    if (b_UTF16)
		    {
		        pt_sami_private->w2s_text[ui2_cont_idx] = 0;
				x_uc_w2s_strncpy(pt_private->w2s_text, 
					             pt_sami_private->w2s_text, 
					             MM_SBTL_CACHE_MAX);
		    }
		    else
		    {
		        pt_sami_private->s_text_buf[ui2_cont_idx] = 0;
                if (sami_valid_str(pt_sami_private->s_text_buf))
                {
				    sbtl_trans_to_w2s_ex(pt_mngr->pt_engine->t_cfg.e_encoding,
									 pt_sami_private->s_text_buf,
									 CONTENT_SIZE,
									 pt_private->w2s_text);
                }
                else
                {
                    pt_private->w2s_text[0] = pt_private->w2s_text[1] = 0;
                }
		    }
		}
	    _sami_apply_color(pt_mngr);
		
	    if (mm_sbtl_get_dbg_level() >= 5)
	    {
	        UINT16 ui2_i=0;
			DBG_INFO(("Fetch string length %d\n,", ui2_cont_idx));
	        for (ui2_i=0; ui2_i < MM_SBTL_CACHE_MAX; ui2_i++)
	        {
	            if (pt_private->w2s_text[ui2_i] > 0x7f)
	            {
	                DBG_INFO(("%X ",pt_private->w2s_text[ui2_i]));
	                continue;
	            }
	            if (pt_private->w2s_text[ui2_i] == 0)break;
	            DBG_INFO(("%c",pt_private->w2s_text[ui2_i]));
	        }
    	}
		return MMSBTLR_OK;
	}

    return MMSBTLR_NOT_FOUND;
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
INT32 sami_load_string(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{             
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
UINT32 sami_timer_period(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
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
INT32 sami_init(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;
    
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    
    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));
    
    pt_private->pv_data = x_mem_alloc(sizeof(SAMI_PRIVATE_T));
    
    if (NULL == pt_private->pv_data)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
	pt_sami_private->ps_token_buf = x_mem_alloc(MAX_TOKEN_LENGTH);
	if (pt_sami_private->ps_token_buf == NULL)
	{
		return MMSBTLR_INSUFFICIENT_MEMORY;
	}
    pt_sami_private->ui1_stream_idx = (UINT8)pt_mngr->pt_engine->t_cfg.ui4_stream_id;
    
    _sami_class_info(pt_mngr, pt_private->b_UTF16);
	if (pt_sami_private->ui1_stream_idx+1 > pt_sami_private->ui1_total_stream_count)
	{
		pt_sami_private->ui1_stream_idx = 0;
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
INT32 sami_deinit(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));

    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));

    if (pt_private->pv_data)
    {
		SAMI_PRIVATE_T*  t_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
		if (t_sami_private->ps_token_buf)
		{
			x_mem_free(t_sami_private->ps_token_buf);
			t_sami_private->ps_token_buf = NULL;
		}
        x_mem_free(pt_private->pv_data);
        pt_private->pv_data = NULL;
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
VOID sami_start(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;


    ASSERT(pt_mngr, ("pt_mngr=NULL\n"))
    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));
    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
    ASSERT(pt_sami_private, ("pt_sami_private=NULL\n"));
    
    x_memset(pt_sami_private->w2s_text,  0, CONTENT_SIZE*sizeof(UINT16));
    x_memset(pt_sami_private->s_text_buf,0, CONTENT_SIZE*sizeof(UINT8));
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
VOID sami_stop(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
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
VOID sami_config(SBTL_PAGE_INST_MNGR_T*     pt_mngr,
                 UINT32                     ui4_set_type)
{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;
    
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));

    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));
    
    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;
    ASSERT(pt_sami_private, ("pt_sami_private=NULL\n"));

    switch (ui4_set_type)
    {
    case EVN_MM_SBTL_SET_STREAM_INDEX:
        pt_sami_private->ui1_stream_idx = (UINT8)pt_mngr->pt_engine->t_cfg.ui4_stream_id;
        break;
        
    default:
        break;
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
INT32 sami_get_info( SBTL_PAGE_INST_MNGR_T*        pt_mngr,
                     UINT32                        ui4_get_type,
                     VOID*                         pv_get_info,
                     SIZE_T                        z_get_info_len)

{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*      pt_private;
    SAMI_PRIVATE_T*                     pt_sami_private;
    SAMI_STREAM_INFO_T*                 pt_stream_info;
    
    if (pv_get_info == NULL ||
		pt_mngr == NULL     || 
		pt_mngr->pv_private == NULL)
    {
		return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
	if (pt_private->pv_data == NULL)
	{
		return MMSBTLR_INV_ARG;
	}

    pt_sami_private = (SAMI_PRIVATE_T*)pt_private->pv_data;

    switch (ui4_get_type)
    {
    case EVN_MM_SBTL_GET_STREAM_INFO:
        if (z_get_info_len < sizeof (SAMI_STREAM_INFO_T))
        {
            pv_get_info = NULL;
        }
        else
        {
            pt_stream_info = (SAMI_STREAM_INFO_T*)pv_get_info;
            pt_stream_info->ui1_stream_idx = pt_sami_private->ui1_stream_idx;
            pt_stream_info->ui1_total_stream_count = pt_sami_private->ui1_total_stream_count;
            x_memcpy(pt_stream_info->at_stream_info, pt_sami_private->at_stream_info, sizeof(SMAI_CC_INF_T)*SAMI_CC_SZ);
        }
        break;
    case EVN_MM_SBTL_GET_BASIC_INFO:
		if (z_get_info_len == sizeof(MM_SBTL_BASIC_INFO_T))
		{
			MM_SBTL_BASIC_INFO_T*  pt_info  = (MM_SBTL_BASIC_INFO_T*)pv_get_info;
			MM_SBTL_TRACK_INFO*    pt_track = NULL;
			UINT8                  ui1_i    = 0;
			UINT8                  ui1_num  = pt_sami_private->ui1_total_stream_count;
			SMAI_CC_INF_T*         pt_smi_info = pt_sami_private->at_stream_info;
			if (ui1_num > SAMI_CC_SZ) ui1_num = SAMI_CC_SZ;
			pt_info->pt_track_info = NULL;
			if (ui1_num > 0)
			{
				pt_track = (MM_SBTL_TRACK_INFO*)x_mem_alloc(ui1_num*sizeof(MM_SBTL_TRACK_INFO));
				if (pt_track == NULL)
				{
					return MMSBTLR_INSUFFICIENT_MEMORY;
				}
				pt_info->pt_track_info = pt_track;
				x_memset(pt_track, 0, ui1_num*sizeof(MM_SBTL_TRACK_INFO));
				for (ui1_i = 0; ui1_i < ui1_num; ui1_i++)
				{
					pt_track->ps_title = NULL;
					if (pt_smi_info->s_class_name[0])
					{
                        #ifndef __KERNEL__
						pt_track->ps_title = x_strdup((const CHAR*)pt_smi_info->s_class_name);
                        #endif
					}
					x_memcpy(pt_track->s_lang, pt_smi_info->aui1_lang, 2*sizeof(CHAR));
					if (ui1_i < ui1_num - 1)
					{
						pt_track->pt_next_info = (pt_track+1);
					}
					pt_smi_info++;
					pt_track++;
				}
			}
		}
		break;
        
    default:
        break;
    }
    
    return MMSBTLR_OK;
}

BOOL sami_parser_verify(SBTL_ENGINE_T*  pt_engine)
{
   
    INT32   i4_ret;
    BOOL    b_UTF16 = FALSE;
	BOOL    b_found = FALSE;
    CHAR    s_buf[256];
    UINT16  ui2_idx =  0;
    UINT16  ui2_data = 0;
    
    ASSERT(pt_engine, ("pt_engine=NULL\n"));
    
	sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
	i4_ret = read_one_char(pt_engine, &ui2_data, FALSE);
	if (ui2_data == 0xFF)
	{
		i4_ret = read_one_char(pt_engine, &ui2_data, FALSE);
		if (ui2_data == 0xFE)
		{
			b_UTF16 = TRUE;
		}
	}
	
    while (ui2_idx < MM_SBTL_PARSE_MAX_LENGTH)
    {
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        if (ui2_data == (UINT16)'<')
        {
            s_buf[0] = '<';
            i4_ret = sami_read_util(pt_engine, b_UTF16, s_buf+1, '>', 255);
            if (i4_ret == MMSBTLR_OK)
            {
                b_found = match_reg_exp(SAMI_ID, s_buf, NULL, NULL);
                break; 
            }
        }
		i4_ret = read_one_char(pt_engine, &ui2_data, b_UTF16);
        ui2_idx++;
    }
	
    return b_found;
}



