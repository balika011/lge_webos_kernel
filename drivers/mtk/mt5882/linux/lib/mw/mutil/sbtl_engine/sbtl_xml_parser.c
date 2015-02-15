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
 * RCSfile: sbtl_xml_parser.c
 * Revision:  V1.0
 * Date: 2012/09/06
 * Author:Lingbin.bu 
 *
 * Description: 
 *         This file contains the implementation of the ttml(xml) file parser function
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include head files
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
#include "xml/xml_debug.h"
#include "xml/xml.h"
#include "xml/xml_list.h"
#include "sbtl_xml_parser.h"
#include "hash/hash_api.h"





/*-----------------------------------------------------------------------------
                    functions declarations
-----------------------------------------------------------------------------*/

static BOOL     _sbtl_xml_skip(CHAR *ps_ele, HANDLE_T   h_hash);

static VOID     _sbtl_xml_normalize_char (CHAR *ps_str);

static INT32    _sbtl_xml_isspace (INT32  i4_char);

static INT32    _sbtl_xml_deque_push (XML_USER_DATA_T *pt_user_data, XML_T *pt_ele);

static XML_T*   _sbtl_xml_deque_pop (XML_USER_DATA_T *pt_user_data);

static VOID     _sbtl_xml_add_str(CHAR **pps_orignal,const CHAR *ps_add,int i4_len);

static VOID     _sbtl_xml_parse_start_handle(VOID*                         pv_data,
                                                   const ssXML_Char *            ps_element,
                                                   const ssXML_Char **           pps_attribute);

static VOID     _sbtl_xml_parse_end_handle(VOID*                          pv_data,
                                                  const ssXML_Char *             ps_element);

static VOID     _sbtl_xml_parse_character_handle(VOID*                          pv_data,
                                                        const ssXML_Char *              ps_value,
                                                        int                           i4_len);

static VOID     _sbtl_xml_parser_init_handle(VOID *pv_user_data,VOID *pv_user_private_info);

static VOID     _sbtl_xml_parser_done_handle(VOID *pv_user_data,VOID *pv_user_private_info);


static INT32    _sbtl_xml_parse_begin(XML_USER_PRIVARE_INFO_T *pt_user_private_info,
                                           SBTL_XML_USER_ARG_T *pt_user_arg); 

static INT32    _sbtl_xml_parse(CHAR*                       ps_mem_data,
                                   UINT32                       ui4_mem_leng,
                                   XML_T **                     ppt_xml_info,
                                   XML_USER_PRIVARE_INFO_T*     pt_private_info);

static INT32    _sbtl_xml_parse_end(); 

static XML_ATTRIBUTE_T* _sbtl_xml_add_attribute(XML_T *             pt_xml_ele,
                                                    const CHAR *        ps_attr_name,
                                                    INT32               i4_type,
                                                    const CHAR *        ps_attr_value);

static XML_T*   _sbtl_xml_new_element (const CHAR             *ps_ele_name,
                                            INT32                    i4_type,
                                            const CHAR              *ps_ele_value);

static XML_T *  _sbtl_xml_add_element (XML_T                    *pt_xml_ele, 
                                            const CHAR                  *ps_ele_name,
                                            INT32                       i4_type, 
                                            const CHAR                 *ps_ele_value);

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

extern BOOL g_b_debug_on;

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_add_str
 *
 * Description: add src to the end of str
 *
 * Inputs: -src: the string need to add ; 
 *            -i4_len:the length of string need to add
 *
 * Outputs: -str:the result of str merge
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_add_str(CHAR **str,const CHAR *src,int i4_len)
{
    UINT32              ui4_len = 0;
    if (*str) 
    {
        ui4_len = x_strlen (*str);
    }
  
    /* Allocate memory for the string.  */
    if (! *str) 
    {
        *str = (CHAR *) x_mem_alloc (i4_len+1);
    } 
    else 
    {
       *str
        = (CHAR *) x_mem_realloc (*str, ui4_len+i4_len+1);
    }
    /* Copy string.  */
    if (*str) 
    {
        x_strncpy ((*str) + ui4_len, (CHAR *) src, (SIZE_T)i4_len);
        (*str)[ui4_len+i4_len]='\0';
    }
    
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_deque_push
 *
 * Description: push the element to stack
 *
 * Inputs: -pt_ele: the element to push
 *            -pt_user_data:the user data structure
 *
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 *              -SBTL_XMLR_OUT_OF_MEMORY if allocte out of memory
 ----------------------------------------------------------------------------*/
static INT32  _sbtl_xml_deque_push (XML_USER_DATA_T *pt_user_data, XML_T *pt_ele)
{
    XML_DEQUE_T  *pt_new_deque;

    XML_ABORT(pt_user_data, ("pt_user_data = %p\n", pt_user_data));
    XML_ASSERT(pt_ele, ("pt_ele = %p\n", pt_ele));

    pt_new_deque = (XML_DEQUE_T *) x_mem_alloc (sizeof (XML_DEQUE_T));
    if (! pt_new_deque) 
    {
        return SBTL_XMLR_OUT_OF_MEMORY;
    }
    x_memset(pt_new_deque , 0, sizeof(XML_DEQUE_T));
    pt_new_deque->pt_xml_element = pt_ele;
    pt_new_deque->pt_prev = pt_user_data->pt_deque;
    pt_user_data->pt_deque = pt_new_deque;
  
    return SBTL_XMLR_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_deque_pop
 *
 * Description: pop the element from stack
 *
 * Inputs: -pt_user_data:the user data structure
 *
 * Outputs:
 *
 * Returns: -the element of pop from the stack
 ----------------------------------------------------------------------------*/
static XML_T * _sbtl_xml_deque_pop (XML_USER_DATA_T *pt_user_data)
{
    XML_DEQUE_T  *pt_bottom;
    XML_T        *pt_ele = NULL;

    XML_ABORT(pt_user_data, ("pt_user_data = %p\n", pt_user_data));
  
    pt_bottom = pt_user_data->pt_deque;
    if (pt_bottom) 
    {
        pt_user_data->pt_deque = pt_bottom->pt_prev;
        pt_ele = pt_bottom->pt_xml_element;
        x_mem_free (pt_bottom);
    }
    return  pt_ele;
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_isspace
 *
 * Description: jugement the character whether is white space
 *
 * Inputs: -i4_char:the chracter need to juge
 *
 * Outputs:
 *
 * Returns: -TRUE if the charater is white space
 *              -FALSE if the charater is not white space
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_isspace (INT32  i4_char)
{
    return (i4_char ==  ' ' ||
        i4_char ==  '\f'||
        i4_char ==  '\n'||
        i4_char ==  '\r'||
        i4_char ==  '\t'|| 
        i4_char ==  '\v');
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_normalize_char
 *
 * Description: move the white space of the string at the begin and the end
 *                  ex:"    /nabc   \t\r  "----->"abc"
 *
 * Inputs: -ps_str:the string need to process
 *
 * Outputs:-ps_str:the string after process
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_normalize_char (CHAR *ps_str)
{
    INT32  i4_start;
    INT32  i4_end;
    INT32  i4_total;

    XML_ABORT(ps_str, ("...."));
  
    i4_end = (INT32)x_strlen (ps_str);
  
    while (i4_end > 0 && _sbtl_xml_isspace (ps_str[i4_end - 1])) 
    {
        ps_str[--i4_end] = '\0';
    }
    /*x_strspn: Find the first sub-string*/
    i4_start = (INT32)x_strspn (ps_str, (CHAR *) " \n\r\t\v");
    i4_total = i4_end - i4_start;
    x_memmove (ps_str, &ps_str[i4_start], (SIZE_T)i4_total);
    ps_str[i4_total] = '\0';
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_skip
 *
 * Description: jugement whether the element or attribute need to skip
 *
 * Inputs: -ps_ele:the string of element/attribute name
 *            -h_hash:the hash table for skip jugement
 *
 * Outputs:-ps_str:the string after process
 *
 * Returns: -TRUE:if the ps_ele is not in hash table
 *              -FALSE:if the ps_ele is in hash table
 ----------------------------------------------------------------------------*/
static BOOL _sbtl_xml_skip(CHAR *ps_ele, HANDLE_T   h_hash)
{
    BOOL        b_is_skip = TRUE;
    CHAR*       ps_attr = NULL;
    CHAR*       ps_low_case = x_str_tolower(ps_ele);

    if(x_handle_valid(h_hash) != TRUE)
    {
        DBG_ERROR(("The hash handle is valid!\n"));
        return b_is_skip;
    }
    ps_attr = x_strchr(ps_low_case,':');
    if(ps_attr == NULL)
    {
        ps_attr = ps_low_case;
    }
    else
    {
        ps_attr = ps_attr + 1;
    }
    if(hash_is_key_exist(h_hash,(const INT8*)ps_attr,x_strlen(ps_attr)))
    {
        b_is_skip = FALSE;
    }
    return b_is_skip;
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse_start_handle
 *
 * Description: the fuction register to xml_parser,do something when meet the start element
 *                   ex:<p......>
 *
 * Inputs: -pv_data:the user data structure
 *            -ps_element:the start element name   ex:p
 *            -pps_attribute:the attribute of the element, each atribute include name+value
 *
 * Outputs:
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID  _sbtl_xml_parse_start_handle(VOID*                         pv_data,
                                             const ssXML_Char *            ps_element,
                                             const ssXML_Char **           pps_attribute)
{
    XML_USER_DATA_T*            pt_user_data = (XML_USER_DATA_T *) pv_data;
    INT32                       i4_idx = 0;
    INT32                       i4_ret;   
    CHAR                        m[]="\n";
    SBTL_XML_USER_ARG_T*        pt_user_arg = (SBTL_XML_USER_ARG_T*)pt_user_data->pv_user_private;
    CHAR*                       ps_ele_low = NULL;
    CHAR*                       ps_ele = NULL;
    INT32                       i4_ele_type;
  
    pt_user_arg->ui2_depth++;
    if(pt_user_arg->b_is_skip)
    {
        return;
    }
    if (_sbtl_xml_skip((CHAR*)ps_element,pt_user_arg->h_ele_hash))
    {
        pt_user_arg->b_is_skip= TRUE;
        pt_user_arg->ui2_skip_depth= pt_user_arg->ui2_depth-1;
        return;
    }
    XML_ABORT(pv_data, ("pv_data = %p\n", pv_data));
    XML_ASSERT(ps_element, ("ps_element = %p\n", ps_element));

    ps_ele_low = x_str_tolower((CHAR *) ps_element);
    ps_ele = x_strchr(ps_ele_low,':');
    if(ps_ele == NULL)
    {
        ps_ele = ps_ele_low;
    }
    else
    {
        ps_ele = ps_ele + 1;
    }
    i4_ret = hash_get_obj(pt_user_arg->h_ele_hash,(const INT8*)ps_ele,x_strlen(ps_ele),(VOID**)&i4_ele_type);
    if(i4_ret != HASH_OK)
    {
        DBG_ERROR(("Can not get hash value form key=%s\n,i4_ret=%d", ps_ele, i4_ret));
    }
    
    if (! pt_user_data->pt_doc) 
    {
        pt_user_data->pt_doc = pt_user_data->pt_xml_current = _sbtl_xml_new_element((CHAR *) ps_element,i4_ele_type, NULL);
        if (!pt_user_data->pt_doc) 
        {
            pt_user_data->i4_error = SBTL_XMLR_OUT_OF_MEMORY;
            return;
        }
    } 
    else 
    {
        if (i4_ele_type == SBTL_XML_ELM_BR)
        {
 
            XML_T   *pt_cur = NULL;
            if (pt_user_data->pt_xml_current->i4_type==(INT32)SBTL_XML_ELM_SPAN)
            {
                pt_cur = pt_user_data->pt_xml_current;
                _sbtl_xml_add_str(&pt_cur->ps_value,m,1);
            }
            else if (pt_user_data->pt_xml_current->i4_type==(INT32)SBTL_XML_ELM_PARA)
            {
                if (xml_ListEmpty(&pt_user_data->pt_xml_current->child_ele_list))
                {
                    _sbtl_xml_parse_start_handle (pv_data, "span", NULL);
                    _sbtl_xml_parse_character_handle (pv_data, m, 1);
                    _sbtl_xml_parse_end_handle (pv_data, "span"); 
                }
                else
                {
                    pt_cur = xml_ListTail(&pt_user_data->pt_xml_current->child_ele_list);
                    if (pt_cur && (pt_cur->i4_type==(INT32)SBTL_XML_ELM_SPAN))
                    {
                        _sbtl_xml_add_str(&pt_cur->ps_value,m,1);
                    }  
                }
            
            }
          
            return;
        }
        
        i4_ret = _sbtl_xml_deque_push (pt_user_data, pt_user_data->pt_xml_current);
        if (i4_ret != SBTL_XMLR_SUCCESS)
        {
            pt_user_data->i4_error = i4_ret;
            return;
        }
        pt_user_data->pt_xml_current = _sbtl_xml_add_element(pt_user_data->pt_xml_current,(CHAR *)ps_element,i4_ele_type, NULL);
        if (! pt_user_data->pt_xml_current) 
        {
            pt_user_data->i4_error = SBTL_XMLR_OUT_OF_MEMORY;
            return;
        }
    }
  
    /* Attributes.  */
    if(pps_attribute != NULL)
    {
        BOOL        b_is_attr_skip = FALSE;
        INT32       i4_type;
        CHAR*       ps_attr = NULL;
        CHAR*       ps_low_case = NULL;
        for (i4_idx = 0; pps_attribute[i4_idx]; i4_idx += 2) 
        {
            b_is_attr_skip = _sbtl_xml_skip((CHAR *) pps_attribute[i4_idx],pt_user_arg->h_attr_hash);
            if(b_is_attr_skip)
            {
                continue;
            }
            XML_ATTRIBUTE_T  *pt_new_attr;
            ps_low_case = x_str_tolower((CHAR *) pps_attribute[i4_idx]);
            ps_attr = x_strchr(ps_low_case,':');
            if(ps_attr == NULL)
            {
                ps_attr = ps_low_case;
            }
            else
            {
                ps_attr = ps_attr + 1;
            }
            i4_ret = hash_get_obj(pt_user_arg->h_attr_hash,(const INT8*)ps_attr,x_strlen(ps_attr),(VOID**)&i4_type);
            if(i4_ret != HASH_OK)
            {
                DBG_ERROR(("Can not get hash value form key=%s\n,i4_ret=%d", ps_attr, i4_ret));
            }
            pt_new_attr = _sbtl_xml_add_attribute(pt_user_data->pt_xml_current, (CHAR *) pps_attribute[i4_idx], i4_type ,(CHAR *) pps_attribute[i4_idx + 1]);
            if (! pt_new_attr) 
            {
                pt_user_data->i4_error = SBTL_XMLR_OUT_OF_MEMORY;
                return;
            }
        }
    }
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse_end_handle
 *
 * Description: the fuction register to xml_parser,do something when meet the end element
 *                   ex:</p>
 *
 * Inputs: -pv_data:the user data structure
 *            -ps_element:the end element name   ex:p
 *
 * Outputs:
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID  _sbtl_xml_parse_end_handle(VOID*                          pv_data,
                                        const ssXML_Char *             ps_element)
{
    XML_USER_DATA_T*            pt_user_data = (XML_USER_DATA_T *) pv_data;
    SBTL_XML_USER_ARG_T*        pt_user_arg = (SBTL_XML_USER_ARG_T*)pt_user_data->pv_user_private;
    CHAR*                       ps_ele = NULL;
  
    pt_user_arg->ui2_depth--;
    if(pt_user_arg->b_is_skip)
    {
        if (pt_user_arg->ui2_skip_depth == pt_user_arg->ui2_depth)
        {
            pt_user_arg->b_is_skip = FALSE;
        }
        return;
    }
    XML_ABORT(pv_data, ("pv_data = %p\n", pv_data));
    XML_ASSERT(ps_element, ("ps_element = %p\n", ps_element));
  
    ps_ele = x_strchr((CHAR*)ps_element, ':');
    if(ps_ele == NULL)
    {
        ps_ele = (CHAR*)ps_element;
    }
    else
    {
        ps_ele = ps_ele + 1;
    }
    //BR should not push and pop
    if (x_strcasecmp(ps_ele, "br") == 0)
    {
        return;
    }
   
    if (pt_user_data->pt_xml_current && pt_user_data->pt_xml_current->ps_value &&
        pt_user_data->pt_xml_current->i4_type != (INT32)SBTL_XML_ELM_SPAN) 
    {
        _sbtl_xml_normalize_char (pt_user_data->pt_xml_current->ps_value);
    }
    pt_user_data->pt_xml_current = _sbtl_xml_deque_pop (pt_user_data);
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse_character_handle
 *
 * Description: the fuction register to xml_parser,do something when meet the value of element
 *                   ex:<p.........>abc</p>  the ps_value of i4_len is "abc"-->the value of element p
 *
 * Inputs: -pv_data:the user data structure
 *            -ps_value:the string from the begin of value of element ex:"abc</p>"
 *            -i4_len:the len of value belong to the elemnt ex:3
 *
 * Outputs:
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID  _sbtl_xml_parse_character_handle(VOID*                          pv_data,
                                              const ssXML_Char *              ps_value,
                                              int                           i4_len)
{
    XML_USER_DATA_T  *pt_user_data = (XML_USER_DATA_T *) pv_data;
    BOOL             b_add_span = FALSE;
    BOOL             b_is_space = FALSE;
    SBTL_XML_USER_ARG_T*        pt_user_arg = (SBTL_XML_USER_ARG_T*)pt_user_data->pv_user_private;
    XML_ABORT(pv_data, ("pv_data = %p\n", pv_data));
    XML_ASSERT(ps_value, ("ps_value = %p\n", ps_value));
    XML_ASSERT((i4_len >= 0), ("len = %d\n",i4_len));
  
    /* Current XML node must be there.  */
    if (! pt_user_data->pt_xml_current) 
    {
        return;
    }
    if(pt_user_arg->b_is_skip)
    {
        return;
    }
    /* Skip the CDATA payloads.  */
    if (XML_CHECK_FLAG (pt_user_data->i4_flags, XML_FLAG_CDATA)) 
    {
        return;
    }
    XML_T   *pt_cur = NULL;
    pt_cur = pt_user_data->pt_xml_current;
    if (pt_cur->i4_type == (INT32)SBTL_XML_ELM_PARA)
    {
        if (xml_ListEmpty(&pt_cur->child_ele_list))
        {
            b_add_span = TRUE;
        }
        else
        {
			XML_ATTRIBUTE_T* pt_attr = NULL;
            pt_cur = xml_ListTail(&pt_user_data->pt_xml_current->child_ele_list);
			pt_attr = pt_cur->attrlist.head;
			// fake span add to fake? or last is not fake new
			if (pt_attr != NULL && x_strlen(pt_attr->ps_value) == 0)
			{
				//fake span
			}
            else
		    {
                if(i4_len ==(INT32)x_strspn (ps_value, (CHAR *) " \n\r\t\v"))
                {        
                    b_is_space = TRUE;    
                }
                if(!b_is_space && xml_ListEmpty(&pt_cur->attrlist))//has content last no attr
                {
                }
                else
                {
                    b_add_span = TRUE;
                }
            }
        }
    }

    if (b_add_span)
    {
		static CHAR* ps_fake_span[] = {"fakespan", NULL, NULL,NULL};
        _sbtl_xml_parse_start_handle (pv_data, "span", (const ssXML_Char**)ps_fake_span);
        pt_cur = pt_user_data->pt_xml_current;
    }
    if (ps_value)
    {
        _sbtl_xml_add_str(&pt_cur->ps_value,ps_value,i4_len);
    }
    if (b_add_span)
    {
        _sbtl_xml_parse_end_handle (pv_data, "span");
    }
    
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parser_init_handle
 *
 * Description: set some private user data to user data for xml_parser
 *
 * Inputs: -pv_user_data:the user data structure for xml_parser
 *            -pv_user_private_info:the private user date defined by user
 *
 * Outputs:
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_parser_init_handle(VOID *pv_user_data,VOID *pv_user_private_info)
{
    if(pv_user_data == NULL)
    {
        return;
    }
    XML_USER_DATA_T * pt_user_data = (XML_USER_DATA_T *)pv_user_data;
    SBTL_XML_USER_ARG_T * pt_user_private_info = (SBTL_XML_USER_ARG_T *)pv_user_private_info;
    pt_user_data->pv_user_private = pt_user_private_info;
}

/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parser_done_handle
 *
 * Description: Reserved now (used for free some resource when parser done)
 *
 * Inputs: -pv_user_data:the user data structure for xml_parser
 *            -pv_user_private_info:the private user date defined by user
 *
 * Outputs:
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_parser_done_handle(VOID *pv_user_data,VOID *pv_user_private_info)
{

}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse_begin
 *
 * Description: register user function and user concrete private data to private user data 
 *
 * Inputs: -pt_user_private_info:the user private data structure for user data structure
 *            -pt_user_arg:the structure keep user concrete private data
 *
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 *              -SBTL_XMLR_INV_ARG if agrument is invalid
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_parse_begin(XML_USER_PRIVARE_INFO_T *pt_user_private_info,SBTL_XML_USER_ARG_T *pt_user_arg)
{
    if(pt_user_private_info == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }
    pt_user_private_info ->pf_start_func = _sbtl_xml_parse_start_handle;
    pt_user_private_info ->pf_character_func = _sbtl_xml_parse_character_handle;
    pt_user_private_info ->pf_end_func = _sbtl_xml_parse_end_handle;
    pt_user_private_info ->pf_init_func = _sbtl_xml_parser_init_handle;
    pt_user_private_info ->pf_done_func = _sbtl_xml_parser_done_handle;
    pt_user_private_info ->pv_user_set_info= pt_user_arg;
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse
 *
 * Description: call the xml_parser to paser xml 
 *
 * Inputs: -ps_mem_data:the buffer save xml file data (must be completely xml file data)
 *            -ui4_mem_leng:the length of the memory buffer
 *            -pt_user_private_info:the user private data structure for user data structure
 *
 * Outputs:-ppt_xml_info:the result of parser, saved as a tree structure
 *
 * Returns: -XML_SUCCESS if sucess
 *              -SBTL_XMLR_FAIL if parser fail
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_parse(CHAR*                       ps_mem_data,
                                UINT32                       ui4_mem_leng,
                                XML_T**                      ppt_xml_info,
                                XML_USER_PRIVARE_INFO_T*     pt_private_info)
{
    INT32       i4_ret = SBTL_XMLR_SUCCESS;
    i4_ret = x_xml_user_parse_buffer(ps_mem_data,ui4_mem_leng,ppt_xml_info,pt_private_info);
    if(i4_ret != XML_SUCCESS)
    {
        DBG_ERROR(("x_xml_user_parse_buffer fail i4_ret=%d\n", i4_ret));
        i4_ret = SBTL_XMLR_FAIL;
    }
    return i4_ret;
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_parse_end
 *
 * Description: Reserved (do someting when call parser done)
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 ----------------------------------------------------------------------------*/
static INT32    _sbtl_xml_parse_end()

{
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_parse_by_memory
 *
 * Description: for other module, parser xml file
 *
 * Inputs: -pv_mem_data:the buffer save xml file data (must be completely xml file data)
 *            -ui4_mem_leng:the length of the memory buffer
 *            -pt_info:the structure include ele_hash and attr_hash for skip
 *
 * Outputs:-ppv_xml_info:the result of parser, saved as a tree structure
 *
 * Returns: -XML_SUCCESS if sucess
 *              -SBTL_XMLR_INV_ARG if argument is invalid
 *              -SBTL_XMLR_FAIL if fail
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_xml_parse_by_memory(VOID*                       pv_mem_data, 
                                         UINT32                       ui4_mem_leng, 
                                         VOID**                       ppv_xml_info,
                                         SBTL_XML_PARSER_INFO_T*      pt_info)
{
    INT32                           i4_ret = SBTL_XMLR_SUCCESS;
    CHAR*                           ps_buff = NULL;
    XML_USER_PRIVARE_INFO_T         t_private_info;
    SBTL_XML_USER_ARG_T             t_user_arg;
    XML_T**                         ppt_xml_info = (XML_T**)ppv_xml_info;

    do
    {
        if(pt_info == NULL || pv_mem_data == NULL)
        {
            i4_ret = SBTL_XMLR_INV_ARG;
            break;
        }
        DBG_LOG_PRINT(("x_sbtl_xml_parse_by_memory, The buffer size is %d\n", ui4_mem_leng));
        x_memset (&t_user_arg, 0, sizeof (SBTL_XML_USER_ARG_T));
        x_memset (&t_private_info, 0, sizeof (XML_USER_PRIVARE_INFO_T));
        
        t_user_arg.h_attr_hash = pt_info->h_attr_hash;
        t_user_arg.h_ele_hash = pt_info->h_ele_hash;
        i4_ret = _sbtl_xml_parse_begin(&t_private_info,&t_user_arg);
        if(i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_ERROR(("_sbtl_xml_parse_begin fail\n"));
            i4_ret = SBTL_XMLR_FAIL;
            break;
        }

        ps_buff = (CHAR*)pv_mem_data;

        i4_ret = _sbtl_xml_parse(ps_buff,ui4_mem_leng,ppt_xml_info,&t_private_info);
        if(i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_ERROR(("_sbtl_xml_parse fail\n"));
            i4_ret = SBTL_XMLR_FAIL;
            break;
        }
        #ifdef CLI_LVL_ALL
        if(g_b_debug_on)
        {
            sbtl_xml_print_func((XML_T*)(*ppt_xml_info),0);
        }
        #endif
        i4_ret = _sbtl_xml_parse_end();
        if(i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_ERROR(("_sbtl_xml_parse_end fail\n"));
            i4_ret = SBTL_XMLR_FAIL;
            break;
        }
    }while(0);
    return i4_ret;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_parse_free
 *
 * Description:free the xml parser result
 *
 * Inputs:-pv_xml_info:the xml parser result
 *
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_xml_parse_free(VOID*         pv_xml_info)
{
    XML_T*      pt_xml_info = (XML_T*)pv_xml_info;
    if(pt_xml_info != NULL)
    {
        x_xml_free(pt_xml_info);
    }
    pt_xml_info = NULL;
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_create_hash
 *
 * Description:for other module, create hash table according hash info
 *
 * Inputs:-pt_hash_info:the hash info include key and value
 *           -z_size:the size of hash table size
 *              
 * Outputs:-ph_hash:the hash handle created
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 *              -SBTL_XMLR_FAIL if fail
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_xml_create_hash(SBTL_XML_HASH_INFO_T*   pt_hash_info,SIZE_T z_size, HANDLE_T* ph_hash)
{
    INT32    i4_ret = SBTL_XMLR_SUCCESS;
    if (pt_hash_info == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }
    do
    {
        i4_ret = hash_create( z_size,ph_hash);
        if (i4_ret == HR_OK)
        {
            SIZE_T z_i = 0;
            for (z_i = 0;z_i<z_size;z_i++)
            {
                i4_ret = hash_add_obj(*ph_hash,
                                      (const INT8*)pt_hash_info[z_i].ps_key,
                                       x_strlen(pt_hash_info[z_i].ps_key),
                                      (VOID*)pt_hash_info[z_i].i4_value,
                                       NULL);
                if(i4_ret != HR_OK) 
                {
                    break;
                }
                    
            }
            if (z_i != z_size)
            {
                hash_delete(*ph_hash,NULL);
                *ph_hash = NULL_HANDLE;
                ph_hash = NULL;
                i4_ret = SBTL_XMLR_FAIL;
                break; 
            }
        }
    }while(0);
    return i4_ret; 
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_add_attribute
 *
 * Description:add one attribute for element
 *
 * Inputs:-pt_xml_ele:the element which the attibute belong to
 *           -ps_attr_name:the attibute name
 *           -i4_type:the attibute type
 *           -ps_attr_value:the attibute value
 *              
 * Outputs:
 *
 * Returns: -the new attribute object created
 ----------------------------------------------------------------------------*/
static XML_ATTRIBUTE_T* _sbtl_xml_add_attribute(XML_T *             pt_xml_ele,
                                                    const CHAR *        ps_attr_name,
                                                    INT32               i4_type,
                                                    const CHAR *        ps_attr_value)
{
    XML_ATTRIBUTE_T *pt_new_attr;

    XML_ASSERT(pt_xml_ele, ("pt_xml_ele = %p\n", pt_xml_ele));
    XML_ASSERT(ps_attr_name, ("ps_attr_name = %p\n", ps_attr_name));
  
    if (! pt_xml_ele) 
    {
        return NULL;
    }
  
    pt_new_attr = (XML_ATTRIBUTE_T *) x_mem_alloc (sizeof (XML_ATTRIBUTE_T));
    if (pt_new_attr) 
    {
        x_memset(pt_new_attr, 0, sizeof(XML_ATTRIBUTE_T));
        if(!g_b_debug_on)
        {
            pt_new_attr->ps_name = NULL;
        }
        else
        {
            pt_new_attr->ps_name = x_strdup (ps_attr_name);
        }
        pt_new_attr->ps_value = x_strdup (ps_attr_value != NULL ? ps_attr_value : (CHAR *)"");
        pt_new_attr->i4_type = i4_type;

        if (!pt_new_attr->ps_value) 
        {
            goto failed;
        }
    }
    else
    {
        return NULL;
    }
    if (pt_new_attr) 
    {   
        xml_ListAdd (&pt_xml_ele->attrlist, pt_new_attr);
    }
    return pt_new_attr;

failed:
    DBG_ERROR(("ps_value in _sbtl_xml_add_attribute is NULL\n"));
    if (pt_new_attr) 
    {
        if (pt_new_attr->ps_name) 
        {
            x_mem_free(pt_new_attr->ps_name);
        }
        if (pt_new_attr->ps_value) 
        {
            x_mem_free(pt_new_attr->ps_value);
        }
        x_mem_free(pt_new_attr);
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_free_hash
 *
 * Description:for other module , free hash table
 *
 * Inputs:-h_hash:the handle of hash table need to free
 *              
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if sucess
 *              -SBTL_XMLR_FAIL if fail
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_xml_free_hash(HANDLE_T      h_hash)
{
    INT32           i4_ret = SBTL_XMLR_SUCCESS;
    do
    {
        if(x_handle_valid(h_hash) != TRUE)
        {
            DBG_ERROR(("The handle to free is valid\n"));
            i4_ret = SBTL_XMLR_FAIL;
        }
        hash_delete(h_hash,NULL);
    }while(0);
    
    return i4_ret;
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_new_element
 *
 * Description:new one element
 *
 * Inputs:-ps_ele_name:the element name
 *           -i4_type:the element type
 *           -ps_ele_value:the element value
 *              
 * Outputs:
 *
 * Returns: -the new element object created
 ----------------------------------------------------------------------------*/
static XML_T * _sbtl_xml_new_element (const CHAR *ps_ele_name,INT32  i4_type, const CHAR *ps_ele_value)
{
    XML_T *pt_new_ele;

    XML_ABORT(ps_ele_name, ("ps_ele_name = %p\n", ps_ele_name));

    pt_new_ele = (XML_T *) x_mem_alloc (sizeof (XML_T));
    if (pt_new_ele) 
    {
        x_memset(pt_new_ele, 0, sizeof(XML_T));
        pt_new_ele->pt_parent = NULL;
        if(!g_b_debug_on)
        {
            pt_new_ele->ps_name = NULL;   
        }
        else
        {
            pt_new_ele->ps_name = x_strdup (ps_ele_name);
        }
        pt_new_ele->i4_type = i4_type;
        pt_new_ele->ps_value = x_strdup (ps_ele_value ? ps_ele_value : (CHAR *) "");
        if (!pt_new_ele->ps_value) 
        {
            goto failed;
        }
    }
    else
    {
        return NULL;  
    }      
  
  /*init list*/
    xml_ListInit(&pt_new_ele->child_ele_list);
    xml_ListInit(&pt_new_ele->attrlist);
    return pt_new_ele;

failed:
    if (pt_new_ele) 
    {
        if (pt_new_ele->ps_name) 
        {
            x_mem_free(pt_new_ele->ps_name);
        }
        if (pt_new_ele->ps_value) 
        {
            x_mem_free(pt_new_ele->ps_value);
        }
        x_mem_free(pt_new_ele);
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
 * Name: _sbtl_xml_add_element
 *
 * Description:add one element
 *
 * Inputs:-pt_xml_ele:the parent element of the new element
 *           -ps_ele_name:the element name
 *           -i4_type:the element type
 *           -ps_ele_value:the element value
 *              
 * Outputs:
 *
 * Returns: -the new element object need to add
 ----------------------------------------------------------------------------*/
static XML_T * _sbtl_xml_add_element (XML_T *pt_xml_ele, const CHAR *ps_ele_name,INT32  i4_type, const CHAR *ps_ele_value)
{
    XML_T *pt_new_ele;

    XML_ABORT(pt_xml_ele, ("pt_xml_ele = %p\n", pt_xml_ele));
    XML_ASSERT(ps_ele_name, ("ps_ele_name = %p\n", ps_ele_name));
  
    pt_new_ele = _sbtl_xml_new_element (ps_ele_name,i4_type, ps_ele_value);
    if (pt_new_ele) 
    {
        pt_new_ele->pt_parent = pt_xml_ele;
        xml_ListAdd(&pt_xml_ele->child_ele_list, pt_new_ele);
    }
    return pt_new_ele;
}

/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_first_element
 *
 * Description:get the first element of given type form parent element's child-list
 *
 * Inputs:-i4_ele_type:the element type want to get
 *           -pt_parent:the parent element
 *              
 * Outputs:
 *
 * Returns: -the element gotten
 ----------------------------------------------------------------------------*/
XML_T*   x_sbtl_xml_get_first_element(INT32 i4_ele_type, XML_T *pt_parent)
{
    XML_T*      pt_xml_ele = NULL;
    if(pt_parent == NULL)
    {
        return NULL;
    }
    xml_ListLoop (&pt_parent->child_ele_list, pt_xml_ele) 
    {
        if (i4_ele_type == pt_xml_ele->i4_type) 
        {
            return pt_xml_ele;
        }
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_tail_element
 *
 * Description:get the last element of given type form parent element's child-list
 *
 * Inputs:-i4_ele_type:the element type want to get
 *           -pt_parent:the parent element
 *              
 * Outputs:
 *
 * Returns: -the element gotten
 ----------------------------------------------------------------------------*/
XML_T*   x_sbtl_xml_get_tail_element(INT32 i4_ele_type, XML_T *pt_parent)
{
    XML_T*      pt_xml_ele = NULL;
    if(pt_parent == NULL)
    {
        return NULL;
    }
    for( pt_xml_ele=(&pt_parent->child_ele_list)->tail;pt_xml_ele; pt_xml_ele = pt_xml_ele->pt_prev)
    {
        if (i4_ele_type == pt_xml_ele->i4_type) 
        {
            return pt_xml_ele;
        }
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_next_element
 *
 * Description:get the next element of given type form sibling element
 *
 * Inputs:-i4_ele_type:the element type want to get
 *           -pt_sibling:the sibling element
 *              
 * Outputs:
 *
 * Returns: -the element gotten
 ----------------------------------------------------------------------------*/
XML_T*   x_sbtl_xml_get_next_element(INT32 i4_ele_type, XML_T *pt_sibling)
{
    XML_T*      pt_xml_ele = NULL;
    if(pt_sibling == NULL)
    {
        return NULL;
    }
    for( pt_xml_ele= pt_sibling->pt_next;pt_xml_ele; pt_xml_ele = pt_xml_ele->pt_next)
    {
        if (i4_ele_type == pt_xml_ele->i4_type) 
        {
            return pt_xml_ele;
        }
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_first_attr
 *
 * Description:get the first attribute of given element
 *
 * Inputs:-pt_element:the element which you want get attribute from
 *              
 * Outputs:
 *
 * Returns: -the attribute gotten
 ----------------------------------------------------------------------------*/
XML_ATTRIBUTE_T*   x_sbtl_xml_get_first_attr( XML_T *pt_element)
{
    XML_ATTRIBUTE_T* pt_attr = NULL;
    if(pt_element == NULL)
    {
        return NULL;
    }
    pt_attr = pt_element->attrlist.head;

    return pt_attr;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_attr_value_bytype
 *
 * Description:get the attribute vlaue maked by type of given element
 *
 * Inputs:-i4_attr_type:the attribute type you wanted
 *           -pt_element:the element which you want get attribute from
 *              
 * Outputs:
 *
 * Returns: -the attribute value
 ----------------------------------------------------------------------------*/
CHAR*    x_sbtl_xml_get_attr_value_bytype( INT32 i4_attr_type,XML_T *pt_element)
{
    XML_ATTRIBUTE_T*  pt_attr = NULL;
    if(pt_element == NULL)
    {
        return NULL;
    }
    for( pt_attr=(&pt_element->attrlist)->head;pt_attr; pt_attr = pt_attr->pt_next)
    {
        if (i4_attr_type == pt_attr->i4_type) 
        {
            return pt_attr->ps_value;
        }
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_next_attr
 *
 * Description:get the next attibute form sibling element
 *
 * Inputs: -pt_sibling:the sibling attibute
 *              
 * Outputs:
 *
 * Returns: -the attibute gotten
 ----------------------------------------------------------------------------*/
XML_ATTRIBUTE_T*   x_sbtl_xml_get_next_attr( XML_ATTRIBUTE_T *pt_sibling)
{
    XML_ATTRIBUTE_T* pt_attr = NULL;
    if(pt_sibling == NULL)
    {
        return NULL;
    }
    pt_attr = pt_sibling->pt_next;

    return pt_attr;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_create_ref_hash
 *
 * Description:create reference hash table (key is id and value is object address referenct to the id )
 *
 * Inputs: -pt_parent:the parent of the element which is need created reference hash table
 *            -i4_ele_type:the element type which need to create reference hash table (the according element address as value)       
 *            -i4_attr_type:the attribute value of the type as key when create reference hash table (the according attribute value as key)
 * Outputs: -ph_ref_hash:the handle of  hash created
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 *              -SBTL_XMLR_FAIL if fail
 *              -SBTL_XMLR_INV_ARG if argument is invalid
 ----------------------------------------------------------------------------*/
INT32    x_sbtl_xml_create_ref_hash(XML_T*        pt_parent,
                                          INT32         i4_ele_type,
                                          INT32         i4_attr_type, 
                                          HANDLE_T*     ph_ref_hash)
{
    INT32           i4_ret = SBTL_XMLR_SUCCESS;
    XML_T*          pt_cur = NULL;
    UINT16          ui2_num = 0;
    CHAR*           ps_value = NULL;
    
    if (pt_parent == NULL) 
    {
        return SBTL_XMLR_INV_ARG;
    }
    pt_cur = x_sbtl_xml_get_first_element(i4_ele_type,pt_parent);
    
    while(pt_cur)
    {
        ps_value = x_sbtl_xml_get_attr_value_bytype(i4_attr_type,pt_cur);
        if (ps_value)
        {
            ui2_num++;
        }
        pt_cur = x_sbtl_xml_get_next_element(i4_ele_type,pt_cur);
    }
    if (ui2_num == 0)
    {
        return SBTL_XMLR_FAIL;
    }
    do
    {
        i4_ret = hash_create( (SIZE_T)ui2_num,ph_ref_hash );
        if(i4_ret != HR_OK)
        {
            i4_ret = SBTL_XMLR_FAIL;
            break;
        }
        pt_cur = x_sbtl_xml_get_first_element(i4_ele_type,pt_parent);
        while(pt_cur)
        {
            ps_value = x_sbtl_xml_get_attr_value_bytype(i4_attr_type,pt_cur);
            if (ps_value)
            {
                i4_ret = hash_add_obj(*ph_ref_hash,
                                      (const INT8*)ps_value,
                                       x_strlen(ps_value),
                                      (VOID*)pt_cur,
                                       NULL);
                ui2_num--;
            }
            pt_cur = x_sbtl_xml_get_next_element(i4_ele_type,pt_cur);
        }
        if (ui2_num > 0)
        {
            hash_delete(*ph_ref_hash,NULL);
            *ph_ref_hash = NULL_HANDLE;
            ph_ref_hash =NULL;
            i4_ret = SBTL_XMLR_FAIL;
            break;
        }
    }while(0);
    
    return i4_ret; 
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_iterate_all_attr
 *
 * Description:handle all attribute of element given for caller
 *
 * Inputs: -pt_cur:the element need to process
 *            -pf_attr_handler:the function register by caller,the function do the caller wanted when meet a attribute
 *            -pv_arg:the argument of caller register function
 *            -i4_depth:the recusive depth
 *              
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 ----------------------------------------------------------------------------*/
INT32    x_sbtl_xml_iterate_all_attr(XML_T*                      pt_cur,
                                        sbtl_xml_attr_handler        pf_attr_handler,
                                        VOID*                        pv_arg,
                                        INT32                        i4_depth)
{
    XML_ATTRIBUTE_T*     pt_attr = NULL;
    INT32                i4_ret = SBTL_XMLR_SUCCESS;
    pt_attr = x_sbtl_xml_get_first_attr(pt_cur);
    while(pt_attr)
    {
        i4_ret = pf_attr_handler(pt_attr,pv_arg,i4_depth);
        if (i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_WARN(("iterate error %d\n",i4_ret));
        }
        pt_attr = x_sbtl_xml_get_next_attr(pt_attr);
        
    }
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_elements_bytype
 *
 * Description:get all elements by type in xml_parser tree
 *
 * Inputs: -pt_xml_info:the xml_parser result tree
 *            -i4_ele_type:the element' type wanted
 *              
 * Outputs:-ppt_xml_node:the output list contains the element wanted
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 *              -SBTL_XMLR_INV_ARG if argument invalid
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_xml_get_elements_bytype(
                                            XML_T*              pt_xml_info,
                                            XML_NODE_T**        ppt_xml_node,
                                            INT32               i4_ele_type)
{
    if (pt_xml_info == NULL || ppt_xml_node == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }

    *ppt_xml_node = x_xml_user_get_elements_by_type(pt_xml_info, i4_ele_type);
    
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_element_byidx
 *
 * Description:get the element by index in element list
 *
 * Inputs: -pt_xml_node:the element list
 *            -ui4_idx:the index
 *              
 * Outputs:
 *
 * Returns: -the element gotten
 ----------------------------------------------------------------------------*/
XML_T*  x_sbtl_xml_get_element_byidx(
                                            XML_NODE_T*         pt_xml_node,
                                            UINT32              ui4_idx)
{
    UINT32          ui4_num = 0;
    XML_T*          pt_ele_need = NULL;
    if(pt_xml_node == NULL)
    {
        return NULL;
    }       
    ui4_num = XML_NodeLength(pt_xml_node);

    if (ui4_idx > ui4_num)
    {
        return NULL;
    }
    pt_ele_need = XML_NodeItem(pt_xml_node, ui4_idx);

    return pt_ele_need;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_element_num
 *
 * Description:get the number of element list
 *
 * Inputs: -pt_xml_node:the element list
 *              
 * Outputs:-pui4_idx:the number of element list
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 *              -SBTL_XMLR_INV_ARG if argument invalid
 ----------------------------------------------------------------------------*/
INT32    x_sbtl_xml_get_element_num(
                                            XML_NODE_T*         pt_xml_node,
                                            UINT32*             pui4_idx)
{
    if(pt_xml_node == NULL || pui4_idx == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }
    *pui4_idx = XML_NodeLength(pt_xml_node);
    return SBTL_XMLR_SUCCESS;
}
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_free_node
 *
 * Description:free the element list
 *
 * Inputs: -pt_xml_node:the element list
 *              
 * Outputs:
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 *              -SBTL_XMLR_INV_ARG if argument invalid
 ----------------------------------------------------------------------------*/
INT32    x_sbtl_xml_free_node(XML_NODE_T*         pt_xml_node)
{
    if(pt_xml_node == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }
    x_xml_free_node(pt_xml_node);

    pt_xml_node = NULL;
    return SBTL_XMLR_SUCCESS;
}

/*-----------------------------------------------------------------------------
                                    The functions below is common for all users
                                     like a mutil (but now not use now)
-------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name: x_sbtl_xml_get_timecode
 *
 * Description:get timecode according type and string
 *
 * Inputs: -ps_timecode:the time string
 *            -pui1_time_type:the time type
 *                                      :SBTL_XML_TYPE_UNKOWN
 *                                      :SBTL_XML_TYPE_FULL
 *                                      :SBTL_XML_TYPE_OFFSET
 *              
 * Outputs:-pui4_time:the time in ms
 *
 * Returns: -SBTL_XMLR_SUCCESS if success
 *              -SBTL_XMLR_INV_ARG if argument invalid
 *              -SBTL_XMLR_NOT_SUPPORT if the time_type is not support
 ----------------------------------------------------------------------------*/
INT32   x_sbtl_xml_get_timecode(CHAR*   ps_timecode, UINT32*    pui4_time, UINT8*   pui1_time_type)
{

        
    UINT32          ui4_time = 0;
    UINT16          ui2_temp = 0;
    UINT16          ui2_point_cnt = 0; 
    UINT16          ui2_spe_cnt = 0;
    CHAR*           ps_metric = NULL;
    
    if (ps_timecode == NULL || pui4_time == NULL || pui1_time_type == NULL)
    {
        return SBTL_XMLR_INV_ARG;
    }
    *pui1_time_type = SBTL_XML_TYPE_UNKOWN;
    while(*ps_timecode)
    {
        if (*ps_timecode >= '0' && *ps_timecode <= '9')
        {
            ui2_temp = ui2_temp*10 +(*ps_timecode -'0');
            ps_metric = ps_timecode + 1;
        }
        if (*ps_timecode == ':')
        {
            ui4_time=ui4_time*60+ui2_temp;
            ui2_temp = 0;
            ui2_spe_cnt++;
        }
        if(*ps_timecode == '.')
        {
            ui4_time=ui4_time*60+ui2_temp;
            ui2_temp = 0;
            ui2_point_cnt++;
        }
        ps_timecode++;
    }  
    /*00:00:00.00*/
    if (ui2_spe_cnt == 2 && ui2_point_cnt == 1)
    {
        ui4_time = ui4_time*60*1000 + ui2_temp;
        *pui1_time_type = SBTL_XML_TYPE_FULL;
    }
    /*00:00:00:00*/
    else if (ui2_spe_cnt == 3)
    {
        ui4_time = (ui4_time*1000+ui2_temp*10);
        *pui1_time_type = SBTL_XML_TYPE_FULL;
    }
    /*00(default is second)  || 00(hours/minutes/seconds/milliseconds/frames/ticks)*/
    else if(ui2_spe_cnt == 0)
    {
		UINT32 ui4_m_len = ps_metric? x_strlen(ps_metric):0;
        *pui1_time_type = SBTL_XML_TYPE_OFFSET;
        switch(ui4_m_len)
        {
            /*00(default is second) */
            case 0:
                ui4_time = ui2_temp * 1000;
                break;
            /*00(hours/minutes/seconds/milliseconds/frames/ticks)*/
            case 1:
                if(*ps_metric == 'h')
                {
                    ui4_time = ui2_temp * 60 * 60 * 1000;
                }
                if(*ps_metric == 'm')
                {
                    ui4_time = ui2_temp * 60 * 1000;
                }
                if(*ps_metric == 's')
                {
                    ui4_time = ui2_temp * 1000;
                }    
                if(*ps_metric == 'f' || *ps_metric == 't')
                {
                    ui4_time = ui2_temp * 10;
                }
                break;
            case 2:
                ui4_time = ui2_temp;
                break;
            default:
                return   SBTL_XMLR_NOT_SUPPORT;
        }
        
    }
    /*Not support now*/
    else
    {
        return SBTL_XMLR_NOT_SUPPORT;
    }
    *pui4_time = ui4_time;
    
    return SBTL_XMLR_SUCCESS;
}

#endif /*__KERNEL__*/

