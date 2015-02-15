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
 * RCSfile: sbtl_xml_parser.h
 * Revision:  V1.0
 * Date: 2012/09/06
 * Author:Lingbin.bu 
 *
 * Description: 
 *         This file contains the implementation of the ttml(xml) file parser function
 *---------------------------------------------------------------------------*/
#ifndef __SBTL_XML_PAESER_H_
#define __SBTL_XML_PAESER_H_
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
#include "hash/hash_api.h"



/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

typedef enum _SBTL_XML_TIME_TYPE_T
{
    SBTL_XML_TYPE_FULL,
    SBTL_XML_TYPE_OFFSET,
    SBTL_XML_TYPE_UNKOWN
}SBTL_XML_TIME_TYPE_T;

typedef enum _SBTL_XML_ELEM_T
{
    SBTL_XML_ELM_TT,
    SBTL_XML_ELM_HEAD,
    SBTL_XML_ELM_METADATA,
    SBTL_XML_ELM_TITLE,
    SBTL_XML_ELM_DESC,
    SBTL_XML_ELM_COPYRIGHT,
    SBTL_XML_ELM_STYLING,
    SBTL_XML_ELM_STYLE,
    SBTL_XML_ELM_LAYOUT,
    SBTL_XML_ELM_REGION,
    SBTL_XML_ELM_BODY,
    SBTL_XML_ELM_DIV,
    SBTL_XML_ELM_PARA,
    SBTL_XML_ELM_SPAN,
    SBTL_XML_ELM_BR,
    SBTL_XML_ELM_INFO,
    SBTL_XML_ELM_IMAGE
}SBTL_XML_ELEM_T;
typedef struct _SBTL_XML_PARSER_INFO_T
{
    HANDLE_T                h_ele_hash;
    HANDLE_T                h_attr_hash;
} SBTL_XML_PARSER_INFO_T;

typedef enum _SBTL_XML_RESULT_CODE_E
{ 
    SBTL_XMLR_SUCCESS = 0,
    SBTL_XMLR_FAIL = -1,
    SBTL_XMLR_INV_ARG = -2,
    SBTL_XMLR_OUT_OF_MEMORY = -3,
    SBTL_XMLR_NOT_SUPPORT = -4,
} SBTL_XML_RESULT_CODE_E;


typedef struct _SBTL_XML_USER_AGR_T
{
    UINT16                  ui2_depth;
    UINT16                  ui2_skip_depth;
    BOOL                    b_is_skip;
    HANDLE_T                h_ele_hash;
    HANDLE_T                h_attr_hash;
} SBTL_XML_USER_ARG_T;

typedef struct _SBTL_XML_HASH_INFO_T
{
    CHAR*   ps_key;
    INT32   i4_value;
}SBTL_XML_HASH_INFO_T;


typedef INT32 (*sbtl_xml_attr_handler)(
                                     XML_ATTRIBUTE_T*   pt_attr,
                                     VOID*              pv_arg,
                                     INT32              i4_depth);

/*-----------------------------------------------------------------------------
                    Data declarations
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    functions declarations
-----------------------------------------------------------------------------*/

extern INT32    x_sbtl_xml_parse_by_memory(VOID*                       pv_mem_data, 
                                                UINT32                       ui4_mem_leng, 
                                                VOID**                       ppv_xml_info,
                                                SBTL_XML_PARSER_INFO_T*      pt_cmd);

extern INT32    x_sbtl_xml_parse_free(VOID*         pv_xml_info);

extern INT32    x_sbtl_xml_create_hash(SBTL_XML_HASH_INFO_T*   pt_hash_info,
                                            SIZE_T                  z_size,
                                            HANDLE_T*               ph_hash);

extern INT32    x_sbtl_xml_free_hash(HANDLE_T      h_hash);




extern XML_T*   x_sbtl_xml_get_first_element(INT32 i4_ele_type, XML_T *pt_parent);

extern XML_T*   x_sbtl_xml_get_tail_element(INT32 i4_ele_type, XML_T *pt_parent);

extern XML_T*   x_sbtl_xml_get_next_element(INT32 i4_ele_type, XML_T *pt_sibling);

extern XML_ATTRIBUTE_T*   x_sbtl_xml_get_first_attr( XML_T *pt_element);

extern CHAR*    x_sbtl_xml_get_attr_value_bytype( INT32 i4_attr_type,XML_T *pt_element);

extern XML_ATTRIBUTE_T*   x_sbtl_xml_get_next_attr( XML_ATTRIBUTE_T *pt_sibling);

extern INT32    x_sbtl_xml_create_ref_hash(XML_T* pt_parent,
                                                INT32 i4_ele_type,
                                                INT32 i4_attr_type, 
                                                HANDLE_T* ph_ref_hash);

extern INT32    x_sbtl_xml_iterate_all_attr(XML_T*                       pt_cur,
                                               sbtl_xml_attr_handler        pf_attr_handler,
                                               VOID*                        pv_arg,
                                               INT32                        i4_depth);

extern INT32    x_sbtl_xml_get_elements_bytype(
                                            XML_T*              pt_xml_info,
                                            XML_NODE_T**        pt_xml_node,
                                            INT32               i4_ele_type);
extern XML_T*   x_sbtl_xml_get_element_byidx(
                                            XML_NODE_T*         pt_xml_node,
                                            UINT32              ui4_idx);
extern INT32    x_sbtl_xml_get_element_num(
                                            XML_NODE_T*         pt_xml_node,
                                            UINT32*             pui4_idx);
extern INT32    x_sbtl_xml_free_node(XML_NODE_T*         pt_xml_node);


extern INT32    x_sbtl_xml_get_timecode(CHAR*           ps_timecode, 
                                              UINT32*         pui4_time, 
                                              UINT8*          pui1_time_type);

#endif /*__KERNEL__*/
#endif

