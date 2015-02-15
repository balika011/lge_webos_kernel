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
 * $RCSfile: sbtl_xml_page_inst_mngr.c,v $
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
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"
#include "x_uc_str.h"
#include "mutil/mm_util_input.h"


#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_xml_page_inst_mngr.h"
#include "mutil/sbtl_engine/sbtl_xml_parser.h"    
#include "x_mm_sbtl_engine.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_ipcc_parser.h"
#include "x_sbtl_ipcc.h"


#define _IPCC_RET_FAIL(m) if (m != MMSBTLR_OK){return MMSBTLR_INTERNAL_ERROR;}

#define IPCC_MAX_CMDS_CNT 8
typedef struct
{
    BOOL             b_found;
    BOOL             b_white_space;
    SBTL_IPCC_ATTR_T e_attr;
    CHAR*            ps_value;
    SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_private;
}SBTL_XML_ATTR_SEARCH_T;


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
 
extern  SLIST_T(_SBTL_TTML_ELEMENT_P_T)  t_element_p_list;
extern SLIST_T(_SBTL_TTML_ATTR_T)  t_attr_cur ;
extern SLIST_T(_SBTL_TTML_ATTR_ID_T)  t_common_id_attr_list;


static UINT32 _sbtl_xml_timer_period(SBTL_PAGE_INST_MNGR_T*     pt_mngr);
static INT32 _sbtl_xml_timer_start(SBTL_PAGE_INST_MNGR_T*       pt_mngr);
static VOID _sbtl_xml_timer_stop(SBTL_PAGE_INST_MNGR_T*         pt_mngr);


static INT32 _sbtl_xml_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*  pt_mngr,
                                        PTS_T                   t_pts_current,
                                        PTS_T*                  pt_pts_start,
                                        PTS_T*                  pt_pts_end,
                                        BOOL*                   pb_found);

static INT32 _sbtl_xml_load_string(SBTL_PAGE_INST_MNGR_T*   pt_mngr);

static INT32 _sbtl_xml_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

static INT32 _sbtl_xml_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr);

static INT32 _sbtl_xml_need_dmx(SBTL_PAGE_INST_MNGR_T*     pt_mngr, 
                                 BOOL*                      pb_need);

static INT32 _sbtl_xml_handle_timing(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                                      PTS_T                     t_pts);

static INT32 _sbtl_xml_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                                  VOID*                    pv_cfg_info,
                               UINT32                   ui4_set_type);

static VOID _sbtl_xml_remove(SBTL_PAGE_INST_MNGR_T*        pt_mngr);
static INT32 _sbtl_xml_new_page(SBTL_PAGE_INST_MNGR_T*     pt_mngr);

static INT32 _sbtl_xml_data_pre_parser( SBTL_PAGE_INST_MNGR_T*        pt_mngr);

static SBTL_PAGE_INST_MNGR_T* _xml_page_mngr_constructor (SBTL_ENGINE_T*   pt_engine);
static VOID _xml_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*  pt_mngr);

static BOOL _xml_page_mngr_parser_verify(SBTL_ENGINE_T*    pt_engine,
                                          UINT8*            pt_type);
/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
static SBTL_MNGR_FCT_TBL_T t_xml_fnct_tbl = 
{
    TRUE,
    _xml_page_mngr_constructor,
    _xml_page_mngr_destructor,
    _xml_page_mngr_parser_verify
};

static SBTL_XML_FNC_TBL_T  at_fnc_tbl_list[] = 
{
    {
        MM_SBTL_TYPE_XML_IPCC,
        ipcc_seek_time_stamp,
        ipcc_load_string,
        ipcc_timer_period,
        ipcc_init,
        ipcc_deinit,
        ipcc_start,
        ipcc_stop,
        ipcc_parser_verify,
        NULL,
        NULL
    },    
    {
        MM_SBTL_TYPE_END,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    }
};


/*-----------------------------------------------------------------------------
                    Internal functions implementations
 ----------------------------------------------------------------------------*/
static VOID xml_free_one_parse(SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private)
{
    if (pt_private != NULL)
    {
         BOOL b_sample_end = TRUE;
         INT32 i4_ret;
         if (pt_private->b_is_span_attr_parse)
         {
             i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_CRNT_SAMPLE_END,
                                         (VOID *)&b_sample_end ,
                                         sizeof(BOOL));   
             if (i4_ret != MMSBTLR_OK)
             {
                DBG_ERROR(("====send data fail %d====\n", i4_ret));
             }
         }
         pt_private->ui4_p_node_num = 0;           
         pt_private->ui4_node_p_idx_cur   = 0;
         pt_private->b_is_span_attr_parse = FALSE;
         if (pt_private->h_style)
            x_sbtl_xml_free_hash(pt_private->h_style);
          if (pt_private->h_region)
            x_sbtl_xml_free_hash(pt_private->h_region);
          pt_private->h_style  = NULL_HANDLE;
          pt_private->h_region = NULL_HANDLE;
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
static INT32 _sbtl_xml_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*  pt_mngr,
                                        PTS_T                   t_pts_current,
                                        PTS_T*                  pt_pts_start,
                                        PTS_T*                  pt_pts_end,
                                        BOOL*                   pb_found)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                          i4_ret = MMSBTLR_OK;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    UINT8                          ui1_sample_idx = 0;
    SBTL_ENGINE_T*                 pt_engine;
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;    
    pt_fnc_table = pt_private->pt_fnc_table;
    
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_seek_time, ("pt_fnc_table->pf_seek_time=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_seek_time))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
    if(pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    if(pt_engine->t_cfg.b_is_xml_m4t)
    { 
        ui1_sample_idx =  ipcc_m4t_seek_sample(pt_mngr,t_pts_current);    
        
        if(ui1_sample_idx != pt_private->ui1_m4t_sample_idx)
        {       
                pt_private->ui1_m4t_sample_idx = ui1_sample_idx;
                xml_free_one_parse((SBTL_XML_PAGE_MNGR_PRIVATE_T* )pt_mngr->pv_private);
                xml_pre_get_data_info(pt_mngr);
        }
   }
   if (pt_private->ui4_p_node_num > 0)
   {
        i4_ret = pt_fnc_table->pf_seek_time(pt_mngr,
                                            t_pts_current,
                                            pt_pts_start,
                                            pt_pts_end,
                                            pb_found);
        if (pt_engine->t_cfg.b_is_xml_m4t &&
            *pb_found == FALSE)
        {
            DBG_LOG_PRINT(("Free for %d\n",pt_private->ui1_m4t_sample_idx));
            xml_free_one_parse((SBTL_XML_PAGE_MNGR_PRIVATE_T* )pt_mngr->pv_private);
        }
   }
   return i4_ret;
}
static VOID _sbtl_xml_dump_attrs(SBTL_TTML_ATTR_T* pt_attr)
{
    CHAR* ps_name = NULL;
    if (pt_attr)
    {
         if (pt_attr->e_attr == SBTL_XML_IPCC_ATTR_REGION_BGCOLOR)
         {
            ps_name = "regionbg";
         }
         else
         {
             ps_name = sbtl_xml_get_attr_name(pt_attr->e_attr);
             if (ps_name == NULL)
             {
                DBG_INFO(("Unknown attr %d\n",pt_attr->e_attr)); 
                return;
             }
         }
         switch(pt_attr->e_attr)
         {
             case SBTL_XML_IPCC_ATTR_BEGIN:
             case SBTL_XML_IPCC_ATTR_END:                
             case SBTL_XML_IPCC_ATTR_DUR: 
             case SBTL_XML_IPCC_ATTR_FONTFAMILY: 
             case SBTL_XML_IPCC_ATTR_FONT_STYLE: 
             case SBTL_XML_IPCC_ATTR_VISIBILY:  
             case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:
             case SBTL_XML_IPCC_ATTR_TEXT_DECORATION: 
             case SBTL_XML_IPCC_ATTR_TEXTALIGN:
             case SBTL_XML_IPCC_ATTR_WHITESPACE:
                 DBG_INFO(("%s[%d]=%d  \n",ps_name,pt_attr->e_attr,(INT32)pt_attr->u.e_pts_tm)); 
                 break;
             case SBTL_XML_IPCC_ATTR_ID:
             case SBTL_XML_IPCC_ATTR_REGION:
             case SBTL_XML_IPCC_ATTR_STYLE:
             case SBTL_XML_IPCC_ATTR_COLOR:
             case SBTL_XML_IPCC_ATTR_BGCOLOR:
             case SBTL_XML_IPCC_ATTR_REGION_BGCOLOR:
             case SBTL_XML_IPCC_ATTR_ROLE:
                 DBG_INFO(("%s[%d]=%s  \n",ps_name,pt_attr->e_attr,(INT32)pt_attr->u.ps_value)); 
                 break;
            default:
                break;
         }
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
static INT32 _sbtl_xml_load_string(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                           i4_ret;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    pt_fnc_table = pt_private->pt_fnc_table;
    
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_load_string, ("pt_fnc_table->pf_load_string=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_load_string))
    {
        return MMSBTLR_INV_ARG;
    }

    i4_ret = pt_fnc_table->pf_load_string(pt_mngr);
   if(MMSBTLR_OK == i4_ret)
    {
        SBTL_TTML_ELEMENT_P_T*              pt_p_list=NULL;        
        SBTL_TTML_ELEMENT_SPAN_T*           pt_span_list;
        SBTL_TTML_ATTR_T*                   pt_attr_list;
        UINT8                               ui1_p_num = 0;
        
        SLIST_FOR_EACH(pt_p_list,&t_element_p_list,t_element_p_link)
        {   
            DBG_INFO(("============Load %d <p>============\n",ui1_p_num));        
            SLIST_FOR_EACH(pt_attr_list,&(pt_p_list->t_common_attr_list),t_attr_link)
            {
                _sbtl_xml_dump_attrs(pt_attr_list);
            }
            SLIST_FOR_EACH(pt_span_list,&(pt_p_list->t_element_span_list),t_element_span_link)
            {               
                DBG_INFO(("Content:%s\n<span attrs>\n",pt_span_list->ps_content));             
                SLIST_FOR_EACH(pt_attr_list,&(pt_span_list->t_attr_list),t_attr_link)
                {                   
                    _sbtl_xml_dump_attrs(pt_attr_list);
                }                           
            } 
            ui1_p_num++;
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
static UINT32 _sbtl_xml_timer_period(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    pt_fnc_table = pt_private->pt_fnc_table;
    
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_timer_period, ("pt_fnc_table->pf_timer_period=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_timer_period))
    {
        return 200;
    }

    return pt_fnc_table->pf_timer_period(pt_mngr);
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
static VOID _sbtl_timer_cb_fct(HANDLE_T     pt_tm_handle,
                               VOID*        pv_tag)
{
    SBTL_PAGE_INST_MNGR_T*  pt_mngr;    
    MM_SBTL_HDLR_MSG_T      t_msg;
    if (NULL == pv_tag)
    {
        return;
    }
    pt_mngr = (SBTL_PAGE_INST_MNGR_T*)pv_tag;
    mm_sbtl_hdlr_send_timer_msg(t_msg, (VOID*)pt_mngr);
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
static INT32 _sbtl_xml_timer_start(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
    INT32                           i4_ret;
   
    if ((NULL == pt_mngr) || (NULL == pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (pt_private->h_timer)
    {
        x_timer_delete(pt_private->h_timer);
        pt_private->h_timer = NULL_HANDLE;
    }

    i4_ret = x_timer_create(&pt_private->h_timer);
    if (i4_ret != OSR_OK)
    {
        return MMSBTLR_CORE;
    }

    i4_ret = x_timer_start(pt_private->h_timer,
                           _sbtl_xml_timer_period(pt_mngr),
                           X_TIMER_FLAG_REPEAT,
                           _sbtl_timer_cb_fct,
                           pt_mngr);
                    
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
static VOID _sbtl_xml_timer_stop(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
    
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return;
    }

    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (pt_private->h_timer)
    {
        x_timer_delete(pt_private->h_timer);
        pt_private->h_timer = NULL_HANDLE;
    }
}

static INT32 _sbtl_xml_attr_iter(XML_ATTRIBUTE_T*   pt_attr,
                                    VOID*              pv_arg,
                                    INT32              i4_depth)
{
    SBTL_XML_ATTR_SEARCH_T* pt_s =(SBTL_XML_ATTR_SEARCH_T*)pv_arg;
    
    if (pt_attr && pt_s)
    {
        if (pt_attr->i4_type == pt_s->e_attr)
        {
            pt_s->b_found = TRUE;
            pt_s->ps_value = pt_attr->ps_value;
        }
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_WHITESPACE)
        {
            if (x_strcmp(pt_attr->ps_value, "preserve") == 0)
            {
                pt_s->b_white_space = TRUE;
            }
        }

        if ((pt_attr->i4_type == SBTL_XML_IPCC_ATTR_STYLE ||
            pt_attr->i4_type ==  SBTL_XML_IPCC_ATTR_REGION)
            && pt_s->pt_private)
        {
            SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_parse = pt_s->pt_private;
            XML_T*                        pt_cur = NULL;
            HANDLE_T                      h_handle = pt_parse->h_style;
            INT32                         i4_ret;
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_REGION)
            {
                h_handle = pt_parse->h_region;
            }
            if (h_handle)
            {
                i4_ret = hash_get_obj(h_handle,
                                     (INT8*)pt_attr->ps_value,
                                     x_strlen(pt_attr->ps_value),
                                     (VOID**)&pt_cur);
                if(i4_ret == 0 && pt_cur && i4_depth < 4)
                {
                     x_sbtl_xml_iterate_all_attr(pt_cur,
                                     _sbtl_xml_attr_iter,
                                     pv_arg,
                                     i4_depth+1);
                    
                }
            }  
        }     
    }    
    return MMSBTLR_OK; 
}

static BOOL valid_string(CHAR* ps_str)
{
    if (ps_str)
    {
        while (*ps_str)
        {
            if (*ps_str != ' ' &&
                *ps_str != '\t' &&
                *ps_str != '\r' &&
                *ps_str != '\n')
            {
                return TRUE;
            }
            ps_str++;
        }
    }
    return FALSE;
}

static VOID _sbtl_xml_id2reg(XML_NODE_T*  pt_node,UINT16 ui2_num)
{
     UINT16 ui2_i = 0;
     XML_T* pt_p = NULL;
     if (pt_node == NULL)
     {
        return;
     }
     for (ui2_i = 0; ui2_i < ui2_num; ui2_i++)
     {
        pt_p = x_sbtl_xml_get_element_byidx(pt_node, ui2_i);
        if (pt_p)
        {
             XML_ATTRIBUTE_T* pt_attr_rep = NULL;
             XML_ATTRIBUTE_T* pt_attr = NULL;
             pt_attr = x_sbtl_xml_get_first_attr(pt_p);
             while(pt_attr)
             {
                if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_REGION)
                {
                    pt_attr_rep = pt_attr;
                    break;
                }
                if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_ID && pt_attr_rep == NULL)
                {
                    pt_attr_rep = pt_attr;
                }
                pt_attr = x_sbtl_xml_get_next_attr(pt_attr);
             }
             if (pt_attr_rep)
             {
                pt_attr_rep->i4_type = SBTL_XML_IPCC_ATTR_REGION;
             }
        }
        else
        {
            break;
        }
     }    
}

static CHAR* _sbtl_xml_get_attr_extent(SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_private,UINT8* pui1_num)
{
    SBTL_XML_ATTR_SEARCH_T t_s;
    XML_T*                 pt_p;
    XML_T*                 pt_span;
    UINT8                  ui1_idx = pt_private->ui1_m4t_sample_idx;
    CHAR*                  ps_value;
    if (ui1_idx < SAMPLE_NUM_MAX)
    {
        x_memset(&t_s, 0, sizeof(SBTL_XML_ATTR_SEARCH_T));
        t_s.e_attr = SBTL_XML_IPCC_ATTR_FONT_EXTENT;
        t_s.pt_private = pt_private;
        pt_p = x_sbtl_xml_get_element_byidx(pt_private->pt_node_p[ui1_idx],0);
        if (pt_p)
        {
            x_sbtl_xml_iterate_all_attr(pt_p,
                                 _sbtl_xml_attr_iter,
                                 &t_s,
                                 0);
            if (pui1_num)
            {
                *pui1_num = 1;
                pt_span = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_SPAN,pt_p);
                
                while(NULL != pt_span)
                { 
					XML_ATTRIBUTE_T* pt_attr =  x_sbtl_xml_get_first_attr(pt_span);
                    ps_value = pt_span->ps_value;
					
                    if (pt_attr && pt_attr->i4_type == SBTL_XML_IPCC_ATTR_FAKESPAN)
                    {
                        if (t_s.b_white_space == FALSE && (valid_string(ps_value) == FALSE))
                        {
                            pt_span = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_SPAN,pt_span);
                            continue;
                        }  
                    }
                    
                    while(ps_value)
                    {
                        ps_value = x_strchr(ps_value,'\n');
                        if (ps_value)
                        {
                            (*pui1_num)++;
                            ps_value++;
                        }
                        else
                        {
                            break;
                        }
                    }
                    pt_span = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_SPAN,pt_span);
                } 
            }
          
            if (pui1_num) DBG_INFO(("Find lines %d\n",*pui1_num));
            if (t_s.b_found)
            {
                DBG_INFO(("Find Extent %s\n",t_s.ps_value));
                return t_s.ps_value;
            }
        } 
    }
    return NULL; 
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
static INT32 _sbtl_xml_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T* pt_private;
    SBTL_XML_FNC_TBL_T*           pt_fnc_table;              
    SBTL_ENGINE_T*                pt_engine;         
    INT32                         i4_ret = MMSBTLR_OK;
    
    if ((NULL == pt_mngr) || 
        (NULL == pt_mngr->pv_private) ||
        (NULL == pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    if (pt_private->b_wait_cfg == FALSE)
    {
        return MMSBTLR_OK;
    }
    
    pt_fnc_table = pt_private->pt_fnc_table;

    if ((NULL == pt_fnc_table) || (NULL == pt_fnc_table->pf_start))
    {
        return MMSBTLR_INV_ARG;
    } 

    if(pt_engine->b_is_stop_req)
    {
        return MMSBTLR_OK;
    }

    
     //x_sema_lock(pt_private->h_sem, X_SEMA_OPTION_WAIT);
    
     if ( pt_mngr->b_started == FALSE && pt_mngr->b_parse_done)
     {
         i4_ret = xml_pre_get_data_info(pt_mngr);
         if (MMSBTLR_OK != i4_ret)
         {       
             DBG_ERROR(("===xml_pre_get_data_info %d ===\n",i4_ret));
             return i4_ret;
         } 
         pt_fnc_table->pf_start(pt_mngr);
     
         _sbtl_xml_timer_start(pt_mngr);
         
         pt_mngr->b_started = TRUE;
     }
    
    // x_sema_unlock(pt_private->h_sem);   

    
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
static INT32 _sbtl_xml_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*      pt_private;
    SBTL_XML_FNC_TBL_T*                pt_fnc_table;
    INT32                              i4_ret = MMSBTLR_OK;

    if ((NULL == pt_mngr) || (NULL == pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    pt_fnc_table = pt_private->pt_fnc_table;

    if ((NULL == pt_fnc_table) || (NULL == pt_fnc_table->pf_stop))
    {
        return MMSBTLR_INV_ARG;
    }
    DBG_LOG_PRINT(("===_sbtl_xml_stop start==\n"));
    if (pt_private->b_wait_cfg)
    {
        if (pt_mngr->b_started)
        {
            _sbtl_xml_timer_stop(pt_mngr);
            pt_fnc_table->pf_stop(pt_mngr);
        }
        if (pt_private->b_is_span_attr_parse)
        {
            _sbtl_xml_remove(pt_mngr);
            xml_free_one_parse(pt_private); 
        }
        if (pt_private->b_wgt_open)
        {
            i4_ret = x_sbtl_ipcc_close_fct();
            DBG_LOG_PRINT(("===x_sbtl_ipcc_close_fct ret %d===\n",i4_ret));
        }
    }
    pt_private->b_wgt_open = FALSE;
    pt_mngr->b_started = FALSE;
    pt_private->e_state = XML_STATE_NONE; 
    pt_private->b_page_exist = FALSE;
    //pt_mngr->pt_engine->b_is_stop_req = FALSE;
    pt_private->b_wait_cfg = FALSE;
    DBG_LOG_PRINT(("===_sbtl_xml_stop end==\n"));
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
static INT32 _sbtl_xml_need_dmx(SBTL_PAGE_INST_MNGR_T*      pt_mngr, 
                                 BOOL*                       pb_need)
{   
    *pb_need = FALSE;
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
static INT32 _sbtl_xml_handle_timing(SBTL_PAGE_INST_MNGR_T*     pt_mngr,
                                      PTS_T                      t_pts)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*      pt_private;
    INT32                              i4_ret = 0;    
    SBTL_ENGINE_T*                     pt_engine;         
    
    if ((NULL == pt_mngr) || 
        (NULL == pt_mngr->pv_private) || 
        (NULL == pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;    
    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;  

    if(!pt_mngr->b_parse_done)
    {
        DBG_INFO(("===XML_PARSER Fail!===\n"));
        return MMSBTLR_INV_ARG;
    }

    if(pt_engine->b_is_stop_req)
    {        
        DBG_INFO(("===handingtiming receive stop req!===\n"));
        return MMSBTLR_OK;
    }
    if (pt_private->b_page_exist)
    {        
        DBG_INFO(("Data exists time is %lld\n",t_pts));
        if (t_pts < pt_private->t_pts_s)
        {
            pt_private->e_state = XML_STATE_DATA_RDY;
            return MMSBTLR_OK;
        }
        else if (t_pts >= pt_private->t_pts_s && t_pts <= pt_private->t_pts_e)
        {   
            if (pt_private->e_state != XML_STATE_DISPLAY)
            {
				if (pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
				{
	                DBG_INFO(("begin to send data to ipcc!\n"));   
	                i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_DATA,(VOID *)&t_element_p_list , sizeof(SBTL_TTML_DATA_T));
	                if(MMSBTLR_OK != i4_ret)
	                {                    
	                    DBG_INFO(("===send data fail return (%d)====\n",i4_ret));
	                }
					pt_private->e_state = XML_STATE_DISPLAY;
				}   
           }
		   if (pt_private->t_pts_e >= 0xFFFFFFFF)
		   {
		   		/*we try to skip last for ever ones*/
				UINT8 ui1_sample_idx = pt_private->ui1_m4t_sample_idx;
				if(pt_engine->t_cfg.b_is_xml_m4t)
    			{ 
        			pt_private->ui1_m4t_sample_idx =  ipcc_m4t_seek_sample(pt_mngr,t_pts);
					if (ui1_sample_idx != pt_private->ui1_m4t_sample_idx)
					{
						_sbtl_xml_remove(pt_mngr);
						DBG_INFO(("Remove m4t forever %lld\n",t_pts));
       					_sbtl_xml_new_page(pt_mngr);
					}
				 }
		   }
        }
        else if (pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
        {   
            DBG_INFO(("Call remove\n"));
            _sbtl_xml_remove(pt_mngr);
        }
    }
    else
    {            
       DBG_INFO(("New page at %lld\n",t_pts));
       _sbtl_xml_new_page(pt_mngr);
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
static INT32 _sbtl_xml_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                              VOID*                    pv_cfg_info,
                              UINT32                   ui4_set_type)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    INT32                          i4_ret;        
    SM_CCH_DTVCC_ATTRIBS_INFO_T*   pt_sbtl_cc_info;

    if (pt_mngr == NULL ||
        (pt_mngr->pv_private == NULL) ||
        (pv_cfg_info == NULL && ui4_set_type != EVN_MM_SBTL_SET_TYPE_PARSE_DONE))
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_fnc_table = pt_private->pt_fnc_table;
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    pt_sbtl_cc_info = &pt_mngr->pt_engine->t_cfg.t_sbtl_cc_info;
    switch (ui4_set_type)
    {
        case EVN_MM_SBTL_SET_TYPE_PARSE_DONE:
        {
            DBG_INFO(("[IPCC] parse done now\n"));
            i4_ret = pt_mngr->pf_start(pt_mngr);
        }
        break;
        case EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO:
        if (pt_private->b_wait_cfg == FALSE)
        {
            DBG_LOG_PRINT(("[IPCC] delay to start now\n"));
            pt_private->b_wait_cfg = TRUE;
            i4_ret = pt_mngr->pf_start(pt_mngr);
            
            return i4_ret;
        }
        else/*stop and replay......*/
        {
            return MMSBTLR_OK;
        }
        break;
    
        case EVN_MM_SBTL_SET_CC_ATTR_INFO:
        {
            i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_ATTRIBS,
                                        (VOID *) pt_sbtl_cc_info, 
                                         sizeof(SM_CCH_DTVCC_ATTRIBS_INFO_T));
            DBG_INFO(("x_sbtl_ipcc_set_fct ret %d\n",i4_ret));
            UNUSED(i4_ret);
        }
        break;
        
        case EVN_MM_SBTL_SET_SHOW_HIDE:
        {
            BOOL b_hide = (*(BOOL*)pv_cfg_info)? FALSE:TRUE;
            PTS_T t_pts = 0;
            if (pt_mngr->pt_engine->t_cfg.b_hide == b_hide)
            {
                break;
            }
            pt_mngr->pt_engine->t_cfg.b_hide = b_hide;
            if (b_hide)//IPCC hide
            {
                if (pt_private->e_state == XML_STATE_DISPLAY)
                {
                    _sbtl_xml_remove(pt_mngr);
                }
            }
            else
            {
                i4_ret = sbtl_stc_pts(pt_mngr->pt_engine,
                                      &t_pts);
                if (i4_ret == MMSBTLR_OK)
                {
                    _sbtl_xml_handle_timing(pt_mngr,t_pts);
                }
            }
            break;
            
        }
        
        default:
            DBG_INFO(("cannot find the correspound set_type !\n"));
        break;
    }

    if (NULL==pt_fnc_table)
    {
        return MMSBTLR_OK;
    }

    if (pt_fnc_table->pf_config)
    {
        pt_fnc_table->pf_config(pt_mngr, ui4_set_type);
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
   INT32 _sbtl_xml_data_pre_parser(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{   
    SBTL_XML_PAGE_MNGR_PRIVATE_T*    pt_private = NULL;
    SBTL_ENGINE_T*                   pt_engine  = NULL;         
    INT32                            i4_ret = MMSBTLR_OK;    
    MEDIA_FEEDER_T*                  pt_feeder = NULL;
    copybytes_fct                    pf_copybytes = NULL;
    UINT32                           ui4_file_size;
    UINT32                           ui4_readed;
    VOID*                            pv_buffer = NULL;
    
    if( NULL == pt_mngr)
    {            
        return MMSBTLR_INV_ARG;
    } 
    
    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;   
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if(NULL == pt_private || pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    } 
    
    pt_feeder = pt_engine->t_cfg.pt_feeder[MM_SBTL_FEEDER_SUBTITLE];
    if (pt_feeder == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
    if (pf_copybytes == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
     
    do 
    {  
        sbtl_feeder_init(pt_mngr->pt_engine, MM_SBTL_FEEDER_SUBTITLE);
        ui4_file_size = (UINT32)pt_engine->aui8_file_size[MM_SBTL_FEEDER_SUBTITLE];
        if (ui4_file_size == 0)
        {
            break;
        }
        DBG_LOG_PRINT(("===IPCC filesize %d===\n",ui4_file_size));
        pv_buffer = x_mem_alloc(ui4_file_size);
        if (NULL == pv_buffer)
        {
            break;
        }
        ui4_readed = 0;
        while(ui4_readed < ui4_file_size)
        {
            UINT32 ui4_size = 4*1024;
            
            if(pt_engine->b_is_stop_req == TRUE)
            {            
                DBG_LOG_PRINT(("===_stop when init readed:%d ====\n", ui4_readed)); 
                i4_ret = MMSBTLR_ALREADY_STOPPED;            
                break;
            }
            
            if (ui4_readed + ui4_size > ui4_file_size)
            {
                ui4_size = ui4_file_size-ui4_readed;
            }
            i4_ret = pf_copybytes(pt_feeder, 
                             pv_buffer+ui4_readed, 
                             ui4_size, 
                             ui4_size,
                             &pt_engine->aui4_read_count[MM_SBTL_FEEDER_SUBTITLE]);
            if (i4_ret != MMSBTLR_OK)
            {
                break;
            }
            ui4_readed += ui4_size;
            
        }
        if (i4_ret != MMSBTLR_OK)
        {
            if (pv_buffer)
            {
                x_mem_free(pv_buffer);
            }
            break;
        }
        
        i4_ret = pt_private->pt_fnc_table->pf_init(pt_mngr, pv_buffer);         
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_INFO(("xml test file init fail!\n"));            
            i4_ret = MMSBTLR_INV_ARG;            
            break;
        }      
                
        pt_mngr->b_parse_done = TRUE;
        
       x_sbtl_engine_set(pt_engine, 
            EVN_MM_SBTL_SET_TYPE_PARSE_DONE, 
            NULL,
            0);
         
     } while(0);
	 x_sema_unlock(pt_private->h_sem);
     
    return i4_ret;
}


INT32 xml_pre_get_com_id_data_info(SBTL_XML_PAGE_MNGR_PRIVATE_T*       pt_private,
                                            XML_NODE_T*                  pt_node,
                                            SBTL_XML_ELEM_T              e_ele_type )
                                                                                       
{
    SBTL_TTML_ATTR_T*       pt_iter = NULL;
    SBTL_TTML_ATTR_T*       pt_iter_attr_list = NULL;
    INT32                   i4_ret;
    UINT32                  ui4_node_num = 0;
    UINT8                   ui1_id_idx = 0;
    SBTL_TTML_ATTR_ID_T*    pt_com_id_list = NULL;
    XML_T*                  pt_idx_node = NULL;

    SBTL_IPCC_ATTR_T         e_attr_type = SBTL_XML_IPCC_ATTR_REGION;
   
    if(SBTL_XML_ELM_STYLE == e_ele_type)
    {
        e_attr_type = SBTL_XML_IPCC_ATTR_STYLE;        
    }
    else if(SBTL_XML_ELM_REGION== e_ele_type)
    {        
        e_attr_type = SBTL_XML_IPCC_ATTR_REGION;        
    }
    
    i4_ret = x_sbtl_xml_get_element_num(pt_node, &ui4_node_num);  

    for( ; ui1_id_idx < ui4_node_num; ui1_id_idx++)
    {       
           BOOL   b_id = FALSE;
           pt_idx_node = x_sbtl_xml_get_element_byidx(pt_node,ui1_id_idx);
                 
           SLIST_INIT(&t_attr_cur);  
           pt_private->pt_attr_parent = pt_idx_node;
           x_sbtl_xml_iterate_all_attr(pt_idx_node,
                                       ipcc_iterate_attr,
                                       (VOID*) pt_private,
                                        0);
           pt_private->pt_attr_parent = NULL;
           if(SLIST_IS_EMPTY(&t_attr_cur))
           {
              return MMSBTLR_NOT_FOUND;
           }
           pt_com_id_list = x_mem_alloc(sizeof(SBTL_TTML_ATTR_ID_T));
           if (pt_com_id_list != NULL)
           {
               pt_com_id_list->e_attr_type = e_attr_type;
               pt_com_id_list->ps_attr_id_value = NULL;
                       
               SLIST_INIT(&pt_com_id_list->t_attr_list);
               DBG_INFO(("===Load Common type:%d===\n",e_ele_type));
               SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
               {   
                   _sbtl_xml_dump_attrs(pt_iter);
                   if(SBTL_XML_IPCC_ATTR_ID != pt_iter->e_attr)
                   {                        
                      pt_iter_attr_list = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
                       if(NULL != pt_iter_attr_list)
                       {
                           x_memcpy(pt_iter_attr_list,pt_iter,sizeof(SBTL_TTML_ATTR_T));
                           SLIST_INSERT_HEAD(pt_iter_attr_list,&(pt_com_id_list->t_attr_list),t_attr_link);
                       }          
                   }
                   else
                   {
                        if (b_id == FALSE)
                        {   
                            pt_com_id_list->ps_attr_id_value = pt_iter->u.ps_value;
                            SLIST_INSERT_HEAD(pt_com_id_list,&t_common_id_attr_list,t_attr_id_link);
                            b_id = TRUE;
                        }
                   }
               }
           }
           ipcc_free_attr();
        }
        return i4_ret;
}
/*
<head>
  <styling>
    <style  styleattrs>
  </styling>
  <layout>
    <region regionattrs>
  </layout>
*/

INT32 xml_pre_get_data_info(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{            
    SBTL_XML_PAGE_MNGR_PRIVATE_T*    pt_private;
    SBTL_ENGINE_T*                   pt_engine;                                                       
    XML_T*                           pt_tree_top = NULL;
    INT32                            i4_ret = MMSBTLR_OK;
    XML_NODE_T*                      pt_node_p = NULL;
    XML_T*                           pt_parent = NULL;        
    XML_T*                           pt_cur    = NULL;        
    UINT32                           ui4_node_num = 0;
    UINT8                            ui1_sample_idx = 0;
    XML_NODE_T*                      pt_node_style = NULL;
    XML_NODE_T*                      pt_node_region = NULL;
    SBTL_TTML_ATTR_ID_T*             pt_com_id_attr_list = t_common_id_attr_list.pt_head;  
    HANDLE_T                         h_canvas;               
    CHAR*                            ps_aspect_ratio = "16:9"; // default
    SBTL_XML_COMMAND_T               t_cmds[IPCC_MAX_CMDS_CNT];
    UINT8                            ui1_idx =0;
    UINT8                            ui1_lines = 1;

    if (NULL == pt_mngr)
    {
        return MMSBTLR_INV_ARG ;
    }
    
    pt_engine = (SBTL_ENGINE_T*)pt_mngr->pt_engine;
    if (pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    if (NULL == pt_private)
    {
        return MMSBTLR_INV_ARG;
    }       
    pt_private->b_is_span_attr_parse = FALSE;
    ui1_sample_idx = pt_private->ui1_m4t_sample_idx;
    pt_tree_top = pt_private->pt_xml_tree[ui1_sample_idx] ;     
    pt_node_p = pt_private->pt_node_p[ui1_sample_idx];
    if(pt_tree_top == NULL || pt_node_p == NULL)
    {            
        return MMSBTLR_INV_ARG;
    }
     
    i4_ret = x_sbtl_xml_get_element_num(pt_node_p, &ui4_node_num);
    if(MMSBTLR_OK  == i4_ret && ui4_node_num > 0)
    {
       pt_private->ui4_p_node_num = ui4_node_num;           
       pt_private->ui4_node_p_idx_cur   = 0;
    }
    else
    {                   
          return MMSBTLR_INV_ARG;
    }
   
    pt_parent = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_HEAD,pt_tree_top);
    if(pt_parent == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }
    
    pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_STYLING,pt_parent);
    if(NULL !=pt_cur)
    {     
        i4_ret = x_sbtl_xml_create_ref_hash(pt_cur,
                          (INT32)SBTL_XML_ELM_STYLE,
                          (INT32)SBTL_XML_IPCC_ATTR_ID,
                          &pt_private->h_style);
        pt_node_style = NULL;

        i4_ret = x_sbtl_xml_get_elements_bytype(pt_tree_top,
                                                &pt_node_style,
                                                SBTL_XML_ELM_STYLE);
      
        
    }
    
    pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_LAYOUT,pt_parent);
    if(NULL !=pt_cur)
    {
        i4_ret = x_sbtl_xml_create_ref_hash(pt_cur,
                              (INT32)SBTL_XML_ELM_REGION,
                              (INT32)SBTL_XML_IPCC_ATTR_ID,
                              &pt_private->h_region);
         pt_node_region = NULL;
         i4_ret = x_sbtl_xml_get_elements_bytype(pt_tree_top,
                                                &pt_node_region,
                                                SBTL_XML_ELM_REGION);
    } 
    _sbtl_xml_id2reg(pt_node_p, ui4_node_num);//change <p id='a'> to <p region='a'>
    if (pt_mngr->b_started == FALSE)
    {
        h_canvas = pt_engine->t_cfg.h_gl_plane;
        if (h_canvas == NULL_HANDLE)
        {
            return MMSBTLR_INTERNAL_ERROR;
        }
        
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_ASPECT_RATIO;
        t_cmds[ui1_idx++].u.ps_text = ps_aspect_ratio;
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_FIRST_P_EXTENT;
        t_cmds[ui1_idx++].u.ps_text = _sbtl_xml_get_attr_extent(pt_private,&ui1_lines);
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_FIRST_P_LINES;
        t_cmds[ui1_idx++].u.ui1_data = ui1_lines;
       
        if (pt_engine->t_cfg.t_display_info.b_display_info)
        {
            GL_RECT_T* pt_rect = &pt_engine->t_cfg.t_display_info.t_rect_refer;
            UINT16 ui2_width = (UINT16)(pt_rect->i4_right-pt_rect->i4_left);
            UINT16 ui2_height = (UINT16)(pt_rect->i4_bottom-pt_rect->i4_top);
            
            t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION;
            t_cmds[ui1_idx++].u.ui4_data = ((UINT32)ui2_width<<16)+ui2_height;
            DBG_LOG_PRINT(("Get width %d height %d vid height %d ",
                ui2_width,ui2_height,pt_engine->t_cfg.t_display_info.ui4_src_height));
            t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_TYPE;
            if (pt_engine->t_cfg.t_display_info.ui4_src_height >= 720)
            {
                t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_HD;
            }
            else
            {
                t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_SD;
            }
        }
        else
        {
            t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION;
            t_cmds[ui1_idx++].u.ui4_data = (1920<<16)+1080;
            t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_TYPE;
            t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_HD;
        }
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_FILE_TYPE;
        if (pt_engine->t_cfg.b_is_xml_m4t)
        {
            t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_FILE_TYPE_SENVU;
        }
        else
        {
            t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_FILE_TYPE_CRACKLE;
        }
		
		t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_FILE_MODE;
		t_cmds[ui1_idx].u.ui4_data = (UINT32)SBTL_XML_FILE_MODE_PRESERVED;

		pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_INFO,pt_parent);
		if (pt_cur)
		{
		    XML_ATTRIBUTE_T*     pt_attr = NULL;
		    pt_attr = x_sbtl_xml_get_first_attr(pt_cur);
		    while(pt_attr)
		    {
		        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_MODE)
		        {
					if (x_strcasecmp(pt_attr->ps_value, "enhanced") == 0)
					{
						t_cmds[ui1_idx].u.ui4_data = (UINT32)SBTL_XML_FILE_MODE_ENHANCED;
					}
		        }
		        pt_attr = x_sbtl_xml_get_next_attr(pt_attr);
		    }
		}
		
		DBG_LOG_PRINT(("mode %d\n",t_cmds[ui1_idx].u.ui4_data));
		ui1_idx++;
		
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_END;
        
        i4_ret = x_sbtl_ipcc_open_fct( h_canvas, (VOID*)&t_cmds);
		DBG_LOG_PRINT(("===x_sbtl_ipcc_open_fct return %d===\n", i4_ret));
        if(MMSBTLR_OK != i4_ret)
        {       
            return i4_ret;
        }  
    }
	
    pt_private->b_wgt_open = TRUE;
	
    SLIST_INIT(&t_common_id_attr_list);
	do
	{
	    if (pt_node_style != NULL)
	    {
	        i4_ret = xml_pre_get_com_id_data_info(pt_private,pt_node_style,SBTL_XML_ELM_STYLE);
            if (MMSBTLR_OK != i4_ret)
            {
                break;
            }
	        
	    }
	    
	    if (pt_node_region != NULL)
	    {
	        i4_ret = xml_pre_get_com_id_data_info(pt_private,pt_node_region,SBTL_XML_ELM_REGION);
	        if (MMSBTLR_OK != i4_ret)
            {
                break;
            }
	    }  
	    DBG_LOG_PRINT(("Get data end for %d\n", pt_private->ui1_m4t_sample_idx));

	    pt_private->b_is_span_attr_parse = TRUE;
	    i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_GLOBAL_STYLE,
	                                 (VOID *)&t_common_id_attr_list ,
	                                 sizeof(SBTL_TTML_ATTR_ID_LIST_T));
	    if(MMSBTLR_OK != i4_ret)
	    {
	        DBG_ERROR(("x_sbtl_ipcc_set_fct() SBTL_IPCC_SET_TYPE_GLOBAL_STYLE FAIL! ret:%d\n", i4_ret));
	    }
	}while(0);

    while(pt_com_id_attr_list  != NULL) 
    {                                
        SBTL_TTML_ATTR_ID_T*       pt_id_list_cur = NULL;
        SBTL_TTML_ATTR_T*          pt_attr_list = NULL;
        pt_id_list_cur = pt_com_id_attr_list;
        pt_attr_list = (pt_com_id_attr_list->t_attr_list).pt_head;
        while(pt_attr_list != NULL)
        {                         
            SBTL_TTML_ATTR_T*  pt_attr_list_cur = pt_attr_list;
            
            SLIST_REMOVE(pt_attr_list,t_attr_link);                                                   
            x_mem_free(pt_attr_list_cur);         
            
            pt_attr_list = (pt_com_id_attr_list->t_attr_list).pt_head;
            if(pt_attr_list == NULL) break;                        
        }

        SLIST_REMOVE(pt_com_id_attr_list,t_attr_id_link);   
        pt_com_id_attr_list = t_common_id_attr_list.pt_head;                
        x_mem_free(pt_id_list_cur);   
        if(pt_com_id_attr_list == NULL) break;
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
static INT32 _sbtl_xml_get(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                            UINT32                   ui4_get_type,
                            VOID*                    pv_get_info,
                            SIZE_T                   z_get_info_len)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    
    if ((pt_mngr == NULL) ||
        (pt_mngr->pv_private == NULL))
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_fnc_table = pt_private->pt_fnc_table;
    if (NULL == pt_fnc_table)
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
    
    if (pt_fnc_table->pf_get_info)
    {
        pt_fnc_table->pf_get_info(pt_mngr, ui4_get_type, pv_get_info, z_get_info_len);
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
 static VOID _sbtl_xml_remove(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*   pt_private;
    SBTL_TTML_ELEMENT_P_T*          pt_p_list=NULL;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span_list;
    SBTL_TTML_ATTR_T*               pt_attr_list;
    SBTL_TTML_ATTR_T*               pt_com_attr_list;
    INT32                           i4_ret;
    BOOL    b_is_remove = FALSE;
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (!pt_private->b_page_exist)
    {
        return;
    }
    
    if (pt_private->e_state == XML_STATE_DISPLAY)
    {          
        pt_p_list = t_element_p_list.pt_head;     
        while(pt_p_list != NULL)
        {    
            SBTL_TTML_ELEMENT_P_T*       pt_p_list_cur=NULL;          
            pt_p_list_cur = pt_p_list;
            b_is_remove = FALSE;
            pt_com_attr_list = (pt_p_list->t_common_attr_list).pt_head;
            while(pt_com_attr_list != NULL)
            {                         
                SBTL_TTML_ATTR_T*        pt_attr_list_cur = pt_com_attr_list;
                if(b_is_remove == FALSE && pt_attr_list_cur->e_attr == SBTL_XML_IPCC_ATTR_REGION)
                {                        
                    i4_ret = x_sbtl_ipcc_remove(pt_attr_list_cur->u.ps_value);
                    DBG_INFO(("Remove region %s ret %d\n",pt_attr_list_cur->u.ps_value,i4_ret));
                    b_is_remove = TRUE;
                }
                    
                SLIST_REMOVE(pt_com_attr_list,t_attr_link);                          
                x_mem_free(pt_attr_list_cur);         
                pt_com_attr_list = (pt_p_list->t_common_attr_list).pt_head;                       
            }
            if (b_is_remove == FALSE)
            {
                i4_ret = x_sbtl_ipcc_remove("undefine");
                DBG_INFO(("Remove default region ret %d\n",i4_ret));
            }
            
            pt_span_list = pt_p_list->t_element_span_list.pt_head; 
            while(pt_span_list  != NULL) 
            {                                
                SBTL_TTML_ELEMENT_SPAN_T*       pt_span_list_cur = NULL;
                pt_span_list_cur = pt_span_list;
                pt_attr_list = (pt_span_list->t_attr_list).pt_head;
                while(pt_attr_list != NULL)
                {                         
                    SBTL_TTML_ATTR_T*        pt_attr_list_cur = pt_attr_list;
                    SLIST_REMOVE(pt_attr_list,t_attr_link);                       
                    x_mem_free(pt_attr_list_cur);         
                    pt_attr_list = (pt_span_list->t_attr_list).pt_head;
                    if(pt_attr_list == NULL) break;                        
                }

                SLIST_REMOVE(pt_span_list,t_element_span_link);   
                pt_span_list = pt_p_list->t_element_span_list.pt_head;                
                x_mem_free(pt_span_list_cur);   
            }
            
            SLIST_REMOVE(pt_p_list,t_element_p_link);                  
            x_mem_free(pt_p_list_cur);
            
            pt_p_list = t_element_p_list.pt_head;     
            /*if(--pt_private->ui4_node_p_idx_num == 0)
            {
                break;
            }*/
        }
        pt_private->ui4_node_p_idx_num = 0;
   }

    pt_private->e_state = XML_STATE_NONE;
    pt_private->b_page_exist = FALSE;
    UNUSED(i4_ret);
    return;
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
static INT32 _sbtl_xml_new_page(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    BOOL                            b_found = FALSE;
    PTS_T                           t_pts_current;
    INT32                           i4_ret;
    PTS_T                           t_pts_start;
    PTS_T                           t_pts_end;
    STC_CTRL_TYPE_T                 t_stc_type;

    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private) || (NULL==pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }

    t_stc_type = STC_CTRL_START;
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (!pt_mngr->b_started)
    {
        return MMSBTLR_OK;
    }

    if (pt_private->b_page_exist)
    {
        return MMSBTLR_OK;
    }

    i4_ret = sbtl_stc_type(pt_mngr->pt_engine, &t_stc_type);
    DBG_INFO(("[MM SBTL]t_stc_type:%d \n",t_stc_type));
    /*if((t_stc_type == STC_CTRL_START) && (i4_ret == MMSBTLR_OK))*/
    {
        i4_ret = sbtl_stc_pts(pt_mngr->pt_engine, &t_pts_current);

        if (i4_ret != MMSBTLR_OK)
        {
            ASSERT(0, ("Get STC Error!!\n"));
        }

        _sbtl_xml_seek_time_stamp(pt_mngr,
                                   t_pts_current,
                                   &t_pts_start,
                                   &t_pts_end,
                                   &b_found);
    }

    if (b_found)
    {
        pt_private->t_pts_s = t_pts_start;
        pt_private->t_pts_e = t_pts_end;
        
        DBG_INFO(("===Find pts %lld [%lld, %lld]===\n",
                  t_pts_current, pt_private->t_pts_s, pt_private->t_pts_e));

        i4_ret = _sbtl_xml_load_string(pt_mngr);
        
        if (i4_ret == MMSBTLR_OK)
        {        
            if (t_pts_current < t_pts_start && t_pts_current <= t_pts_end)
            {
                pt_private->e_state = XML_STATE_DATA_RDY;
            }
            else
            {                
				if (pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
				{
	                DBG_INFO(("===begin to send data to ipcc!====\n"));
	                i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_DATA,(VOID *)&t_element_p_list , sizeof(SBTL_TTML_DATA_T));
	                if(MMSBTLR_OK != i4_ret)
	                {                    
	                    DBG_INFO(("===Send data fail return (%d)====\n",i4_ret));
	                }
					pt_private->e_state = XML_STATE_DISPLAY;
				}
            }                
             pt_private->b_page_exist = TRUE;
        }   
        else
        {
            DBG_INFO(("====_sbtl_xml_load_string FAIL!====\n"));
        }      
    }
    else
    {
        pt_private->e_state = XML_STATE_NONE;
        return MMSBTLR_OK;
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
static SBTL_PAGE_INST_MNGR_T* _xml_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine)
{
    SBTL_PAGE_INST_MNGR_T*              pt_mngr;
    SBTL_XML_PAGE_MNGR_PRIVATE_T*      pt_private;    
    MM_SBTL_PARSE_HDLR_MSG_T              t_parser_msg;
    UINT8                               ui1_idx;
    INT32                       i4_ret = 0;
    ASSERT(pt_engine, ("_xml_page_mngr_constructor pt_engine=NULL\n"));
    if (NULL==pt_engine)
    {
        return NULL;
    }

    do
    {
        pt_mngr = (SBTL_PAGE_INST_MNGR_T*)x_mem_alloc(sizeof(SBTL_PAGE_INST_MNGR_T));
        if (NULL == pt_mngr)
        {
            break;
        }
        x_memset(pt_mngr, 0, sizeof(SBTL_PAGE_INST_MNGR_T));

        pt_mngr->pf_start = _sbtl_xml_start;
        pt_mngr->pf_stop = _sbtl_xml_stop;
        pt_mngr->pf_cfg = _sbtl_xml_config;
        pt_mngr->pf_handle_timing = _sbtl_xml_handle_timing;
        pt_mngr->pf_need_dmx = _sbtl_xml_need_dmx;
        pt_mngr->pf_new_page = _sbtl_xml_new_page;
        pt_mngr->pf_get = _sbtl_xml_get;
        pt_mngr->pf_pre_parser = _sbtl_xml_data_pre_parser;

        pt_mngr->pt_engine = pt_engine;
        
        pt_mngr->pv_private = x_mem_alloc(sizeof(SBTL_XML_PAGE_MNGR_PRIVATE_T));
        
        pt_mngr->b_parse_done = FALSE;      
       
        if (NULL == pt_mngr->pv_private)
        {
            break;
        }
        x_memset(pt_mngr->pv_private, 0, sizeof(SBTL_XML_PAGE_MNGR_PRIVATE_T));

        pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

        i4_ret = x_sema_create(&pt_private->h_sem, 
                           X_SEMA_TYPE_MUTEX, 
                           X_SEMA_STATE_LOCK);
        if (i4_ret != OSR_OK)
        {
           break;
        }
        
        for(ui1_idx=0; ; ui1_idx++)
        {
            if (ui1_idx >= (UINT8)(sizeof(at_fnc_tbl_list)/sizeof(at_fnc_tbl_list[0])))
            {
                break;
            }
            
            if (MM_SBTL_TYPE_END == at_fnc_tbl_list[ui1_idx].e_sbtl_xml_type)
            {
                break;
            }

            if(at_fnc_tbl_list[ui1_idx].e_sbtl_xml_type == pt_engine->t_cfg.e_sbtl_type)
            {
                pt_private->pt_fnc_table = &at_fnc_tbl_list[ui1_idx];
                sbtl_feeder_init(pt_mngr->pt_engine, MM_SBTL_FEEDER_SUBTITLE);
               
                pt_engine->t_cfg.e_encoding = MM_SBTL_ENCODE_UTF8;
                pt_private->b_is_span_attr_parse = FALSE;  
				pt_mngr->b_parse_done = FALSE;
                mm_sbtl_parser_hdlr_send_parser_msg(t_parser_msg,pt_mngr);
            
                return pt_mngr;
            }
        }

    }while(0);

    if (pt_mngr)
    {
        if (pt_mngr->pv_private)
        {
            x_mem_free(pt_mngr->pv_private);
        }
        x_mem_free(pt_mngr);
    }
    return NULL;
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
static VOID _xml_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    if (NULL == pt_mngr)
    {
        return;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (NULL == pt_private)
    {
        return;
    }
	if (pt_private->h_sem != NULL_HANDLE)/*wait for parse done*/
	{
		x_sema_lock(pt_private->h_sem, X_SEMA_OPTION_WAIT);
    	x_sema_unlock(pt_private->h_sem);  
	}

    if (pt_private->pt_fnc_table)
    {
        if (pt_private->pt_fnc_table->pf_deinit)
        {
            pt_private->pt_fnc_table->pf_deinit(pt_mngr);
        }
    }
    if (pt_private->h_sem != NULL_HANDLE)
    {
        x_sema_delete(pt_private->h_sem);
    }
    pt_private->h_sem = NULL_HANDLE;
    x_mem_free(pt_private);
    pt_mngr->pt_engine->pt_page_mngr = NULL;    
    x_mem_free(pt_mngr);
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
static BOOL _xml_page_mngr_parser_verify(SBTL_ENGINE_T*    pt_engine,
                                          UINT8*            pt_type)

{
    UINT8   ui1_idx;
    
    if ((NULL==pt_engine) || (NULL==pt_type))
    {
        return FALSE;
    }
    
    sbtl_feeder_init(pt_engine, MM_SBTL_FEEDER_SUBTITLE);
    
    for(ui1_idx=0; ; ui1_idx++)
    {
        if (ui1_idx >= (UINT8)(sizeof(at_fnc_tbl_list)/sizeof(at_fnc_tbl_list[0])))
        {
            break;
        }

        if (MM_SBTL_TYPE_END == at_fnc_tbl_list[ui1_idx].e_sbtl_xml_type)
        {
            break;
        }
        
        if (*pt_type == MM_SBTL_TYPE_XML)
        {
            if(at_fnc_tbl_list[ui1_idx].pf_verify(pt_engine))
            {
                *pt_type = at_fnc_tbl_list[ui1_idx].e_sbtl_xml_type;
                return TRUE;
            }
        }
        else
        {
            if (*pt_type == at_fnc_tbl_list[ui1_idx].e_sbtl_xml_type)
            {
                if(at_fnc_tbl_list[ui1_idx].pf_verify(pt_engine))
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
        }
    }
    
    return FALSE;
}

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
VOID x_sbtl_xml_init(VOID)
{
    sbtl_register(MM_SBTL_TYPE_XML,
                  &t_xml_fnct_tbl);
    
    sbtl_register(MM_SBTL_TYPE_XML_IPCC,
                  &t_xml_fnct_tbl);    
}
#endif


