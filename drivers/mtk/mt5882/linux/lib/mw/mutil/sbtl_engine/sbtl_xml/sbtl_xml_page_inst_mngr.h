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
 * $RCSfile: sbtl_xml_page_inst_mngr.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description: 
 *         
 *         
 *---------------------------------------------------------------------------*/

#ifndef _SBTL_XML_PAGE_INST_MNGR_H_
#define _SBTL_XML_PAGE_INST_MNGR_H_
#ifndef __KERNEL__
#include "mutil/sbtl_engine/sbtl_xml_parser.h"    
/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/
     
typedef INT32 (*sbtl_seek_time_stamp_fnc)(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                                          PTS_T                     t_pts_current,
                                          PTS_T*                    pt_pts_start,
                                          PTS_T*                    pt_pts_end,
                                          BOOL*                     pb_found);

typedef INT32 (*sbtl_load_string_fnc)(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

typedef UINT32 (*sbtl_timer_period_fnc)(SBTL_PAGE_INST_MNGR_T*  pt_mngr);

typedef INT32 (*sbtl_init_fnc)(SBTL_PAGE_INST_MNGR_T*   pt_mngr, VOID* pv_buffer);

typedef INT32 (*sbtl_deinit_fnc)(SBTL_PAGE_INST_MNGR_T* pt_mngr);

typedef BOOL (*xml_parser_verify_fnc)(SBTL_ENGINE_T*   pt_engine);

typedef VOID (*xml_parser_start)(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

typedef VOID (*xml_parser_stop)(SBTL_PAGE_INST_MNGR_T*     pt_mngr);

typedef VOID (*xml_config)( SBTL_PAGE_INST_MNGR_T*     pt_mngr,
                             UINT32                     ui4_set_type);

typedef INT32 (*xml_get_info)( SBTL_PAGE_INST_MNGR_T*        pt_mngr,
                                UINT32                        ui4_get_type,
                                VOID*                         pv_get_info,
                                SIZE_T                        z_get_info_len);

typedef INT32 (*xml_data_pre_parser)( SBTL_PAGE_INST_MNGR_T*        pt_mngr);
extern INT32 xml_pre_get_data_info(SBTL_PAGE_INST_MNGR_T*   pt_mngr);


#define     SAMPLE_NUM_MAX           255

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ATTR_FONT_WEIGHT
 *  @brief  SBTL_TTML_ATTR_FONT_WEIGHT
 *  @code
 *  typedef enum _SBTL_TTML_ATTR_FONT_WEIGHT
 *  {
 *      SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,       
 *      SBTL_TTML_ATTR_FONT_WEGHT_BOLD
 *      
 *  } SBTL_TTML_ATTR_FONT_WEIGHT;
 *  @endcode
 *   
 *  @li@c SBTL_TTML_ATTR_FONT_WEGHT_NORMAL                             - Normal
 *  @li@c SBTL_TTML_ATTR_FONT_WEGHT_BOLD                                  -Bold
 */
/*----------------------------------------------------------------------------*/

typedef enum _SBTL_TTML_ATTR_FONT_WEIGHT
{
    SBTL_TTML_ATTR_FONT_WEGHT_NORMAL = 0,
    SBTL_TTML_ATTR_FONT_WEGHT_BOLD
}SBTL_TTML_ATTR_FONT_WEIGHT;

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ATTR_FONT_STYLE
 *  @brief  SBTL_TTML_ATTR_FONT_STYLE
 *  @code
 *  typedef enum _SBTL_TTML_ATTR_FONT_STYLE
 *  {       
 *      SBTL_TTML_ATTR_FONT_STYLE_NORMAL = 0,
 *      SBTL_TTML_ATTR_FONT_STYLE_ITALIC
 *  } SBTL_TTML_ATTR_FONT_STYLE;
 *  @endcode
 *   
 *  @li@c TTML_FONT_STYLE_NORMAL                          -Normal
 *  @li@c TTML_FONT_STYLE_ITALIC                             - Italic 
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_FONT_STYLE
{
    SBTL_TTML_ATTR_FONT_STYLE_NORMAL = 0,
    SBTL_TTML_ATTR_FONT_STYLE_ITALIC,
}SBTL_TTML_ATTR_FONT_STYLE;

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ATTR_TEXT_DECORATION
 *  @brief  SBTL_TTML_ATTR_TEXT_DECORATION
 *  @code
 *  typedef enum _SBTL_TTML_ATTR_TEXT_DECORATION
 *  {
 *      SBTL_TTML_ATTR_TEXT_DECORATION_NONE   = 0,
 *      SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE
 *  }SBTL_TTML_ATTR_TEXT_DECORATION;
 *  @endcode
 *   
 *  @li@c SBTL_TTML_ATTR_TEXT_DECORATION_NONE                              - None
 *  @li@c SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE                     -  Underline
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_TEXT_DECORATION
{
    SBTL_TTML_ATTR_TEXT_DECORATION_NONE   = 0,
    SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE
}SBTL_TTML_ATTR_TEXT_DECORATION;

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ATTR_TEXT_ALIGIN
 *  @brief  SBTL_TTML_ATTR_TEXT_ALIGIN
 *  @code
 *  typedef enum _SBTL_TTML_ATTR_TEXT_ALIGIN
 *  { 
 *      SBTL_TTML_ATTR_TEXT_ALIGIN_START = 0,    
 *      SBTL_TTML_ATTR_TEXT_ALIGIN_END,
 *      SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
 *      SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT,
 *      SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER
 *  } SBTL_TTML_ATTR_TEXT_ALIGIN;
 *  @endcode
 *   
 *  @li@c SBTL_TTML_ATTR_TEXT_ALIGIN_START                      - Start
 *  @li@c SBTL_TTML_ATTR_TEXT_ALIGIN_END                          -End
 *  @li@c SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT                         -Left
 *  @li@c SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT                       -Right
 *  @li@c SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER                    -Center
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_TEXT_ALIGIN
{
    SBTL_TTML_ATTR_TEXT_ALIGIN_START = 0,
    SBTL_TTML_ATTR_TEXT_ALIGIN_END,
    SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
    SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT,
    SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER
}SBTL_TTML_ATTR_TEXT_ALIGIN;

/*----------------------------------------------------------------------------*/
/*! @enum   TTML_VISIBILITY
 *  @brief  TTML_VISIBILITY
 *  @code
 *  typedef enum _TTML_VISIBILITY
 *  {
 *      TTML_VISIBILITY_VISIBLE,
 *      TTML_VISIBILITY_HIDDEN
 *  } TTML_VISIBILITY;
 *  @endcode
 *   
 *  @li@c TTML_VISIBILITY_VISIBLE                      - visible
 *  @li@c TTML_VISIBILITY_HIDDEN                      - hidden
 *
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_VISIBILITY
{
    SBTL_TTML_ATTR_VISIBILITY_VISIBLE = 0,
    SBTL_TTML_ATTR_VISIBILITY_HIDDEN
}SBTL_TTML_ATTR_VISIBILITY;

/*----------------------------------------------------------------------------*/
/*! @enum   TTML_WHITESAPCE
 *  @brief  TTML_WHITESAPCE
 *  @code
 *  typedef enum _TTML_WHITESAPCE
 *  {
 *      SBTL_TTML_ATTR_WHITESPACE_DEFAULT,
 *      SBTL_TTML_ATTR_WHITESPACE_PRESERVE
 *  } TTML_WHITESAPCE;
 *  @endcode
 *   
 *  @li@c TTML_WHITESAPCE_DEFULT                      - default
 *  @li@c TTML_WHITESAPCE_PRESERVE                      - preserve
 *
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_WHITESAPCE
{
    SBTL_TTML_ATTR_WHITESPACE_DEFAULT = 0,
    SBTL_TTML_ATTR_WHITESPACE_PRESERVE
}SBTL_TTML_ATTR_WHITESAPCE;

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ATTR_FONT_FAMILY
 *  @brief  SBTL_TTML_ATTR_FONT_FAMILY
 *  @code
 *  typedef enum _SBTL_TTML_ATTR_FONT_FAMILY
 *  {
 *      SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT         = 0,
 *      SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF,
 *      SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF,
 *      SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF,
 *      SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF,
 *      SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD,
 *      SBTL_TTML_ATTR_FONT_FAMILY_CURDIVE_TBD,
 *      SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD

 *  } SBTL_TTML_ATTR_FONT_FAMILY;
 *  @endcode
 *   
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT                                    - default
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF                - monospace
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF             - proportional
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF             - monospaceSansSerif
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF         - proportionalSansSerif
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD                    - casual
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD                   - cursive
 *  @li@c SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD                     - samllcaps
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ATTR_FONT_FAMILY
{
    SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT         = 0,
    SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF,
    SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF,
    SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF,
    SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF,
    SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD,
    SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD,
    SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD 
}SBTL_TTML_ATTR_FONT_FAMILY;

/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_TTML_ENCODING
 *  @brief  SBTL_TTML_ENCODING
 *  @code
 *  typedef enum _SBTL_TTML_ENCODING
 *  {
 *      SBTL_TTML_ENCODING_UTF8 = 0
 *  } SBTL_TTML_ENCODING;
 *  @endcode
 *   
 *  @li@c SBTL_TTML_ENCODING_UTF8                      - utf-8
 *
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_TTML_ENCODING
{
    SBTL_TTML_ENCODING_UTF8 = 0
}SBTL_TTML_ENCODING;

typedef enum _SBTL_IPCC_ATTR_T
{   
    SBTL_XML_IPCC_ATTR_ID,
    SBTL_XML_IPCC_ATTR_STYLE,
    SBTL_XML_IPCC_ATTR_REGION,
    
    SBTL_XML_IPCC_ATTR_BEGIN,
    SBTL_XML_IPCC_ATTR_END,
    SBTL_XML_IPCC_ATTR_DUR,
    SBTL_XML_IPCC_ATTR_FONTSIZE,                       //font size
    SBTL_XML_IPCC_ATTR_COLOR,                             //font color    
    SBTL_XML_IPCC_ATTR_BGCOLOR,                 //backgroundColor
    SBTL_XML_IPCC_ATTR_FONTFAMILY,             //font family
    SBTL_XML_IPCC_ATTR_FONT_WEIGHT,                //font weight
    SBTL_XML_IPCC_ATTR_FONT_STYLE,                 //font style
    SBTL_XML_IPCC_ATTR_ORIGIN,                          //origin
    SBTL_XML_IPCC_ATTR_TEXTALIGN,                  //text_align
    SBTL_XML_IPCC_ATTR_VISIBILY,                     //visibility  
    SBTL_XML_IPCC_ATTR_ECODING,                    //encoding
    SBTL_XML_IPCC_ATTR_FONT_EXTENT,
    SBTL_XML_IPCC_ATTR_CELL_RESOLUTION,  //cellResolution
    SBTL_XML_IPCC_ATTR_TEXT_DECORATION,  //textDecoration
    
    SBTL_XML_IPCC_ATTR_REGION_BGCOLOR,                //regionbackgroundColor
    SBTL_XML_IPCC_ATTR_WINDOW_OPACITY,  //tts:opacity
    SBTL_XML_IPCC_ATTR_TEXT_OUTLINE,    //tts:textOutline
    SBTL_XML_IPCC_ATTR_WRAP_OPTION,      //tts:warpOption
    SBTL_XML_IPCC_ATTR_WHITESPACE,           //tts:whiteSpace    
    SBTL_XML_IPCC_ATTR_LINEHEIGHT,           //tts:lineHeight
    SBTL_XML_IPCC_ATTR_ROLE,             //ttm:role
    SBTL_XML_IPCC_ATTR_MODE,
    SBTL_XML_IPCC_ATTR_BGIMG,
    SBTL_XML_IPCC_ATTR_DISP_MODE,
    SBTL_XML_IPCC_ATTR_FAKESPAN,
    SBTL_XML_IPCC_ATTR_IMAGE_TYPE
}SBTL_IPCC_ATTR_T;

/*
typedef struct _AttrList_XML_T
{   
     Attr_TYPE_T   e_type;
    union
    {        
       CHAR    *ps_value;
       UINT64   ui8_time_value; 
       ATTR_TEXT_ALIGN_T          e_text_align;
       ATTR_FONT_WEIGHT_T         e_font_weight;
       ATTR_FONT_STYLE_T              e_font_style; 
       ATTR_VISIBILITY_T                  e_visibility;
    }u;    
    struct  _AttrList_XML_T     *pt_brother_atrt_list;
}AttrList_XML_T;


typedef struct _SBTL_XML_T
{   
    struct _SBTL_XML_T      *pt_child_list;
    CHAR                *ps_content;
    struct  _AttrList_XML_T  *pt_attr_list;
    struct _SBTL_XML_T      *pt_brother_list;
}SBTL_XML_T;
*/
    
/*-----------------------------------------------------------------------------
                     struct
-----------------------------------------------------------------------------*/
/*      The following structs is used for sbtl engine.       */
/*------------------------------------------------------------------*/
/*! @struct SBTL_TTML_ATTR_T
 *  @brief This struct contains the attr of <span>.
 *  @code
 *  typedef struct _SBTL_TTML_ATTR_T
 *  {
 *      SBTL_IPCC_ATTR_T e_attr;
 *      union
 *      {
 *          CHAR*                           ps_value;
 *          SBTL_TTML_ATTR_TEXT_ALIGIN      e_text_align;
 *          SBTL_TTML_ATTR_FONT_STYLE       e_font_style;
 *          SBTL_TTML_ATTR_FONT_WEIGHT      e_font_weight;
 *          SBTL_TTML_ATTR_FONT_FAMILY      e_font_family;
 *          SBTL_TTML_ATTR_TEXT_DECORATION  e_text_decoration;
 *      }u;
 *        SLIST_ENTRY_T(_SBTL_TTML_ATTR_T)   t_attr_link;
 *  }SBTL_TTML_ATTR_T;
 *  @endcode
 *  @li@c  ps_value                                 -except following attributes, eg, tts:color, tts:backgroundColor, tts:fontSize, tts:origin, tts:cellResolution
 *  @li@c  e_text_align                            -tts:textAlign, the value can refer to SBTL_TTML_ATTR_TEXT_ALIGIN
 *  @li@c  e_font_style                            -tts:fontStyle, the value can refer to SBTL_TTML_ATTR_FONT_STYLE
 *  @li@c  e_font_weight                         -tts:fontWeight, the value can refer to SBTL_TTML_ATTR_FONT_WEIGHT
 *  @li@c  e_font_family                          -tts:fontFamily, the value can refer to SBTL_TTML_ATTR_FONT_FAMILY
 *  @li@c  e_text_decoration                   -tts:textDecoration, the value can refer to SBTL_TTML_ATTR_TEXT_DECORATION
 *  @li@c  t_attr_link                               - the pointer of attr SLIST
 */
/*------------------------------------------------------------------*/
typedef struct _SBTL_TTML_ATTR_T
{
    SBTL_IPCC_ATTR_T e_attr;
    union
    {
        CHAR*                           ps_value;
        SBTL_TTML_ATTR_TEXT_ALIGIN      e_text_align;
        SBTL_TTML_ATTR_FONT_STYLE       e_font_style;
        SBTL_TTML_ATTR_FONT_WEIGHT      e_font_weight;
        SBTL_TTML_ATTR_FONT_FAMILY      e_font_family;
        SBTL_TTML_ATTR_TEXT_DECORATION  e_text_decoration;
        SBTL_TTML_ATTR_VISIBILITY       e_visibility;            
        SBTL_TTML_ATTR_WHITESAPCE       e_whitespace;
        PTS_T                           e_pts_tm;
    }u;
    SLIST_ENTRY_T(_SBTL_TTML_ATTR_T)    t_attr_link;
    
}SBTL_TTML_ATTR_T;

/*------------------------------------------------------------------*/
/*! @struct SBTL_TTML_ELEMENT_SPAN_T
 *  @brief This struct contains the  <span> list.
 *  @code
 *  typedef struct _SBTL_TTML_ELEMENT_SPAN_T
 *  {
 *      SLIST_T(_SBTL_TTML_ATTR_T)              t_attr_list;
 *      SLIST_ENTRY_T(_SBTL_TTML_ELEMENT_P_T)   t_element_span_link;
 *      CHAR*                                   ps_content;
 *  }SBTL_TTML_ELEMENT_SPAN_T;

 *  @endcode
 *  @li@c  t_attr_list                              -the entry of attr list
 *  @li@c  t_element_span_link              -the pointer of <span> list
 *  @li@c  ps_content                            -the text which need to show
 */
/*------------------------------------------------------------------*/
typedef struct _SBTL_TTML_ELEMENT_SPAN_T
{
    SLIST_ENTRY_T(_SBTL_TTML_ELEMENT_SPAN_T)   t_element_span_link;
    SLIST_T(_SBTL_TTML_ATTR_T)              t_attr_list;
    CHAR*                                   ps_content;
}SBTL_TTML_ELEMENT_SPAN_T;


/*------------------------------------------------------------------*/
/*! @struct SBTL_TTML_ELEMENT_P_T
 *  @brief This struct contains the  <p> list.
 *  @code
 *  typedef struct _SBTL_TTML_ELEMENT_P_T
 *  {
 *      SLIST_ENTRY_T(_SBTL_TTML_ELEMENT_P_T)   t_element_p_link;
 *      SLIST_T(_SBTL_TTML_ATTR_T)              t_common_attr_list;
 *      SLIST_T(_SBTL_TTML_ELEMENT_SPAN_T)         t_element_span_list;
 *  }SBTL_TTML_ELEMENT_P_T;
 *  @endcode
 *  @li@c  t_element_p_link                   -the pointer of <p> list     
 *  @li@c  t_common_attr_list                   -the entry of <common_attr> list
 *  @li@c  t_element_span_list               -the entry of <span> list
 */
/*------------------------------------------------------------------*/
typedef struct _SBTL_TTML_ELEMENT_P_T
{
    SLIST_ENTRY_T(_SBTL_TTML_ELEMENT_P_T)   t_element_p_link;        
    SLIST_T(_SBTL_TTML_ATTR_T)              t_common_attr_list;
    SLIST_T(_SBTL_TTML_ELEMENT_SPAN_T)      t_element_span_list;
}SBTL_TTML_ELEMENT_P_T;

/*------------------------------------------------------------------*/
/*! @struct SBTL_TTML_DATA_T
 *  @brief This struct contains the  <p> list.
 *  @code
 *  typedef struct _SBTL_TTML_DATA_T
 *  {
 *      SLIST_T(_SBTL_TTML_ELEMENT_P_T) t_element_p_list;
 *  }SBTL_TTML_DATA_T;
 *  @endcode
 *  @li@c  t_element_p_list               -the entry of <p> list
 */
/*------------------------------------------------------------------*/
typedef struct _SBTL_TTML_DATA_T
{
    SLIST_T(_SBTL_TTML_ELEMENT_P_T) t_element_p_list;
}SBTL_TTML_DATA_T;

/*------------------------------------------------------------------*/
   /*! @struct SBTL_TTML_ELEMENT_SPAN_T
    *  @brief This struct contains the  <id> list.such as region,style...
    *  @code
    *  typedef struct _SBTL_TTML_ELEMENT_SPAN_T
    *  {
    *      SBTL_IPCC_ATTR_T             e_attr;
    *      CHAR*                                   ps_attr_id_value;  
    *      SLIST_ENTRY_T(_SBTL_TTML_ATTR_ID_T)   t_attr_id_link;
    *      SLIST_T(_SBTL_TTML_ATTR_T)              t_attr_list;
    *  }SBTL_TTML_ELEMENT_SPAN_T;
   
    *  @endcode
    *  @li@c  e_attr                              -the entry of attr_id list
    *  @li@c  ps_attr_id_value                     -the value of the attr_id
    *  @li@c  t_attr_id_link              -the pointer of <attr_id> list
    *  @li@c  t_attr_list                       -the entry of attr list
    */
   /*------------------------------------------------------------------*/
   typedef struct _SBTL_TTML_ATTR_ID_T
   {        
       SBTL_IPCC_ATTR_T             e_attr_type;
       CHAR*                                   ps_attr_id_value;       
       SLIST_ENTRY_T(_SBTL_TTML_ATTR_ID_T)   t_attr_id_link;
       SLIST_T(_SBTL_TTML_ATTR_T)              t_attr_list;      
   }SBTL_TTML_ATTR_ID_T;

/*------------------------------------------------------------------*/
    /*! @struct SBTL_TTML_ATTR_ID_LIST_T
     *  @brief This struct contains the  <attr_id> list.
     *  @code
     *  typedef struct _SBTL_TTML_ATTR_ID_LIST_T
     *  {
     *     SLIST_T(_SBTL_TTML_ATTR_ID_T)   t_attr_id_list;     
     *  }SBTL_TTML_ATTR_ID_LIST_T;;
     *  @endcode
     *  @li@c  t_attr_id_list               -the entry of <attr_id_list> list
     */
/*------------------------------------------------------------------*/
    
typedef struct _SBTL_TTML_ATTR_ID_LIST_T
{
    SLIST_T(_SBTL_TTML_ATTR_ID_T)   t_attr_id_list;
}SBTL_TTML_ATTR_ID_LIST_T;

typedef enum
{
    XML_STATE_NONE,
    XML_STATE_DATA_RDY,
    XML_STATE_DISPLAY,
    XML_STATE_END
} XML_PAGE_STATE_T;


typedef struct _SBTL_XML_FNC_TBL_T
{
    MM_SBTL_TYPE_T              e_sbtl_xml_type;
    sbtl_seek_time_stamp_fnc    pf_seek_time;
    sbtl_load_string_fnc        pf_load_string;
    sbtl_timer_period_fnc       pf_timer_period;
    sbtl_init_fnc               pf_init;
    sbtl_deinit_fnc             pf_deinit;
    xml_parser_start           pf_start;
    xml_parser_stop            pf_stop;
    xml_parser_verify_fnc      pf_verify;
    xml_config                 pf_config;
    xml_get_info               pf_get_info;
}SBTL_XML_FNC_TBL_T;

typedef struct _SBTL_XML_PAGE_MNGR_PRIVATE_T
{
    BOOL                            b_page_exist;
    PTS_T                           t_pts_s;
    PTS_T                           t_pts_e;
    
    XML_PAGE_STATE_T                e_state;
    SBTL_XML_FNC_TBL_T*             pt_fnc_table;

    XML_T*                          pt_xml_tree[SAMPLE_NUM_MAX];
    XML_NODE_T*                     pt_node_p[SAMPLE_NUM_MAX];
    UINT32                          ui4_node_p_idx_cur;   
    UINT32                          ui4_node_p_idx_num;
    UINT32                          ui4_p_node_num;
	BOOL                            b_add_next;//add extra info load next
    
    HANDLE_T                        h_sem;
    
    HANDLE_T                        h_timer;
    SBTL_XML_PARSER_INFO_T*         pt_parser_info;
    UINT8*                          pt_text_content;
    UINT8                           ui1_m4t_sample_idx;
    UINT32                          ui4_sample_num;
    XML_T*                          pt_attr_parent;
    HANDLE_T                        h_style;
    HANDLE_T                        h_region;
    SBTL_TTML_ATTR_T *              pt_ttml_attr;
	BOOL                            b_wgt_open;
    BOOL                            b_is_span_attr_parse;
    BOOL                            b_wait_cfg;
    VOID*                           pv_data;
}SBTL_XML_PAGE_MNGR_PRIVATE_T;


/* The following is used to set parameter to sbtl_cc */

typedef UINT32 SBTL_XML_CMD_CODE_T;    /**<        */

#define SBTL_XML_CMD_CODE_END               ((SBTL_XML_CMD_CODE_T) 0)    /**<        */
#define SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG  ((SBTL_XML_CMD_CODE_T) 0x80000000)    /**<        */


/**This is to set the aspect ratio to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_ASPECT_RATIO

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_ASPECT_RATIO    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 0))  


/**This is to set the tts:extent of first <p> to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_FIRST_P_EXTENT

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_FIRST_P_EXTENT    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 1))  


/**This is to set the text of first <p> to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_FIRST_P_TEXT

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_FIRST_P_LINES    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 2))  


/**This is to set the video to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_SET_VIDEO_TYPE

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_SET_VIDEO_TYPE    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 3)) 


/**This is to set the video resolution to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 4)) 


/**This is to set the xml file type to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_SET_FILE_TYPE

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_SET_FILE_TYPE    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 5)) 


/**This is to set the xml file mode to sbtl_cc .The command is using SBTL_XML_COMMAND_T struct.

Input arguments:

e_code      -Set to SBTL_XML_CMD_CODE_SET_FILE_MODE

h_obj       -Contains the handle of the video stream

Returns:

MMSBTLR_OK      -The command is successful.
*/
#define SBTL_XML_CMD_CODE_SET_FILE_MODE    ((SBTL_XML_CMD_CODE_T)(SBTL_XML_CMD_CODE_PRIMARY_BIT_FLAG | 6)) 




/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_XML_VIDEO_TYPE
 *  @brief  SBTL_XML_VIDEO_TYPE
 *  @code
 *  typedef enum _SBTL_XML_VIDEO_TYPE
 *  {
 *      SBTL_XML_VIDEO_TYPE_HD = 0,       
 *      SBTL_XML_VIDEO_TYPE_SD
 *      
 *  } SBTL_XML_VIDEO_TYPE;
 *  @endcode
 *   
 *  @li@c SBTL_XML_VIDEO_TYPE_HD                             - HD
 *  @li@c SBTL_XML_VIDEO_TYPE_SD                             - SD
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_XML_VIDEO_TYPE
{
    SBTL_XML_VIDEO_TYPE_HD = 0,
    SBTL_XML_VIDEO_TYPE_SD
}SBTL_XML_VIDEO_TYPE;


/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_XML_FILE_TYPE
 *  @brief  SBTL_XML_FILE_TYPE
 *  @code
 *  typedef enum _SBTL_XML_FILE_TYPE
 *  {
 *      SBTL_XML_FILE_TYPE_SENVU = 0,       
 *      SBTL_XML_FILE_TYPE_CRACKLE
 *      
 *  } SBTL_XML_FILE_TYPE;
 *  @endcode
 *   
 *  @li@c SBTL_XML_FILE_TYPE_SENVU                 - m4t type
 *  @li@c SBTL_XML_FILE_TYPE_CRACKLE               - Other type, except m4t
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_XML_FILE_TYPE
{
    SBTL_XML_FILE_TYPE_SENVU = 0,
    SBTL_XML_FILE_TYPE_CRACKLE
}SBTL_XML_FILE_TYPE;



/*----------------------------------------------------------------------------*/
/*! @enum   SBTL_XML_FILE_MODE
 *  @brief  SBTL_XML_FILE_MODE
 *  @code
 *  typedef enum _SBTL_XML_FILE_MODE
 *  {
 *      SBTL_XML_FILE_MODE_PRESERVED = 0,       
 *      SBTL_XML_FILE_MODE_ENHANCED
 *      
 *  } SBTL_XML_FILE_MODE;
 *  @endcode
 *   
 *  @li@c SBTL_XML_FILE_MODE_PRESERVED              - Preserved
 *  @li@c SBTL_XML_FILE_MODE_ENHANCED               - Enhanced
 */
/*----------------------------------------------------------------------------*/
typedef enum _SBTL_XML_FILE_MODE
{
    SBTL_XML_FILE_MODE_PRESERVED = 0,
    SBTL_XML_FILE_MODE_ENHANCED
}SBTL_XML_FILE_MODE;



/*------------------------------------------------------------------*/
/*! @struct  SBTL_XML_COMMAND_T
 *  @brief  For client to set command
 *  @code
 *  typedef struct
 *  {
 *      SBTL_XML_CMD_CODE_T  e_code;
 *      union
 *      {
 *          BOOL       b_boolean;
 *          UINT8      ui1_data;
 *          UINT16     ui2_data;
 *          UINT32     ui4_data;
 *          HANDLE_T   h_obj;
 *          CHAR*      ps_text;
 *          VOID*      pv_data;
 *      } u;
 *  } SM_COMMAND_T;
 *  @endcode
 *  @li@c  e_code                                            -References to the type of command 
 *  @li@c  b_boolean                                         - the data type of command is BOOL
 *  @li@c  ui1_data                                          -the data type of command is  UINT8
 *  @li@c  ui2_data                                          -the data type of command is UINT16
 *  @li@c  ui4_data                                          -the data type of command is UINT32
 *  @li@c  h_obj                                             -the data type of command is HANDL_T
 *  @li@c  ps_text                                           -the data type of command is CHAR*
 *  @li@c  pv_data                                           -the data type of command is VOID*
 */
/*------------------------------------------------------------------*/
typedef struct _SBTL_XML_COMMAND_T
{
    SBTL_XML_CMD_CODE_T  e_code;

    union
    {
        BOOL       b_boolean;
        UINT8      ui1_data;
        UINT16     ui2_data;
        UINT32     ui4_data;
        HANDLE_T   h_obj;
        CHAR*      ps_text;
        VOID*      pv_data;
    } u;
} SBTL_XML_COMMAND_T;

/*-----------------------------------------------------------------------------
                    functions declarations
-----------------------------------------------------------------------------*/
//#endif
#endif

#endif /* _SBTL_XML_PAGE_INST_MNGR_H_ */

