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
 /*----------------------------------------------------------------------------*/
/*! @addtogroup groupMW_STRM_MNGR_MM_SBTL_HDLR
*  @{ 
*/
/*----------------------------------------------------------------------------*/
#ifndef __KERNEL__



/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif

#include "x_sbtl_ipcc_util.h"
#include "_sbtl_ipcc_common.h"




/*-----------------------------------------------------------------------------
                    function define
-----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Function: x_sbtl_ipcc_str_split
 *
 * Description:
 *      This API is used to divide ps_str by ps_rgx.
 * Inputs:
 *      ps_rgx        - String separator
 *      ps_str        - String which will be splited
 *      i4_len        - The length of ps_rslt, if ps_rslt is a pointer of array, then i4_len will be the length of array  
 *
 * Outputs:  
 *      ps_rslt       - Save the result after split
 *      ps_rest_str   - Save the rest str after split. The rest str will not save if ps_rest_str is NULL
 *
 * Returns:  
 *      IPCCR_OK              - Routine successful.
 *      IPCCR_INV_ARG         - Invalid para.
 *      IPCCR_NOT_FOUND       - ps_rgx was not found in ps_str
 *---------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_str_split(CHAR* ps_rgx, CHAR* ps_str, CHAR** ps_rslt, INT32 i4_len, CHAR* ps_rest_str)
{
    /* Check arguments */
    if(!ps_rgx)
    {
        DBG_ERROR(("{IPCC} ERR: ps_rgx is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }


    if(!ps_str)
    {
        DBG_ERROR(("{IPCC} ERR: ps_str is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(!ps_rslt)
    {
        DBG_ERROR(("{IPCC} ERR: ps_rslt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(i4_len <= 0)
    {
        DBG_ERROR(("{IPCC} ERR: i4_len is %d!   <%d, %s, %s>\n\r", i4_len, __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }    
    
    INT32 i4_ret = IPCCR_OK;
    CHAR* ps_token = NULL;
    
    CHAR* ps_tmp_str = (CHAR*)x_mem_alloc(sizeof(CHAR) * IPCC_TTS_ORIGIN_LENGTH);
    if(NULL == ps_tmp_str)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset(ps_tmp_str, 0, (SIZE_T)IPCC_TTS_ORIGIN_LENGTH);
    x_strncpy(ps_tmp_str, ps_str, (SIZE_T)(IPCC_TTS_ORIGIN_LENGTH - 1));

    CHAR* ps_tmp = ps_tmp_str;
    
    INT32 i4_i = 0;

    while(i4_i < i4_len)
    {
        ps_token = x_strtok(ps_tmp, (const CHAR*)ps_rgx, &ps_tmp, 0);

        if((NULL != ps_token) && (0 != x_strcmp((const CHAR*)ps_str, ( const CHAR*)ps_token)))
        {
            x_strcpy(ps_rslt[i4_i], (const CHAR*)ps_token);
        }
        else
        {
            DBG_ERROR(("{IPCC} ERR: Split string failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            x_mem_free(ps_tmp_str);
            ps_tmp_str = NULL;
            i4_ret = IPCCR_NOT_FOUND;
            return i4_ret;
        }
        ++i4_i;
    }

    if(NULL != ps_rest_str)
    {
        x_strcpy(ps_rest_str, (const CHAR*)ps_tmp);
    }

    x_mem_free(ps_tmp_str);
    ps_tmp_str = NULL;
    ps_tmp = NULL;

    return IPCCR_OK;
    
}






/*----------------------------------------------------------------------------
 * Function: x_sbtl_ipcc_str_to_float
 *
 * Description:
 *      This API is used to divide ps_str by ps_rgx.
 * Inputs:
 *      ps_src_str     - String which will be splited
 *
 * Outputs:  pf_rslt       - Save the result after convert
 *
 * Returns:  
 *      IPCCR_OK              - Routine successful.
 *      IPCCR_INV_ARG         - Invalid para.
 *      IPCCR_NOT_FOUND       - ps_rgx was not found in ps_str
 *---------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_str_to_float(CHAR* ps_src_str, FLOAT* pf_rslt)
{
    /* Check arguments */
    if(!ps_src_str)
    {
        DBG_ERROR(("{IPCC} ERR: ps_src_str is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    INT32 i4_ret = IPCCR_OK;
    INT32 i4_integer = 0;
    INT32 i4_decimal = 0;
    FLOAT f_tmp_rslt = 0.0;

    CHAR* ps_str = (CHAR*)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
    if(NULL == ps_str)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset(ps_str, 0, (SIZE_T)(IPCC_TTS_ORIGIN_LENGTH/2));
    x_strncpy(ps_str, (const CHAR*)ps_src_str, (SIZE_T)(IPCC_TTS_ORIGIN_LENGTH/2 - 1));

    
    CHAR* ps_integer = (CHAR*)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
    if(NULL == ps_integer)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        x_mem_free(ps_str);
        return IPCCR_INSUFFICIENT_SIZE;
    }

    CHAR* ps_decimal = (CHAR*)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
    if(NULL == ps_decimal)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        x_mem_free(ps_str);
        x_mem_free(ps_integer);
        return IPCCR_INSUFFICIENT_SIZE;
    }

    /* Step1. Split ps_src_str by ps_rgx and get the integer part */
    CHAR* ps_rgx = ".";
    i4_ret = x_sbtl_ipcc_str_split(ps_rgx, ps_str, &ps_integer, 1, ps_decimal);
    if(IPCCR_OK != i4_ret)
    {
        x_mem_free(ps_str);
        x_mem_free(ps_integer);
        x_mem_free(ps_decimal);
        return IPCCR_INV_ARG;
    }

    i4_integer = (INT32)x_strtoull((const CHAR*)ps_integer, NULL, 10);

    /* Step2. Process the decimal part */
    i4_decimal = (INT32)x_strtoull((const CHAR*)ps_decimal, NULL, 10);
    SIZE_T z_len = x_strlen((const CHAR *)ps_decimal);
    UINT8 ui1_i = 0;
    f_tmp_rslt = (FLOAT)i4_decimal;
    for(ui1_i = 0; ui1_i < z_len; ++ui1_i)
    {
        f_tmp_rslt =  f_tmp_rslt / 10.0;
    }
  
    
    /* Step3. Combine the integer part and decimal part */
    f_tmp_rslt = (FLOAT)i4_integer + f_tmp_rslt;
    *pf_rslt = f_tmp_rslt;
    
    
    return IPCCR_OK;
}





/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_util_detect_length_unit
 *
 * Description: This API is used to detect the unit of length, 
 *              including tts:origin, tts:extent.
 *
 * Inputs:  ps_str                  - String which will be detected
 *          pe_origin_type          - The detected origin type
 *
 * Outputs: pe_origin_type         - The detected origin type
 *
 * Returns: IPCCR_OK          - Success
 *          IPCCR_INV_ARG     - Invalid para
 *          IPCCR_NOT_FOUND   - Not matched
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_util_detect_length_unit(CHAR* ps_str, TTML_LENGTH_UNIT_TYPE* pe_origin_type)
{
    /* Check arguments */
    if(!ps_str)
    {
        DBG_ERROR(("{IPCC} ERR: ps_str is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(!pe_origin_type)
    {
        DBG_ERROR(("{IPCC} ERR: pe_length_unit is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    
    INT32 i4_ret = IPCCR_OK;
    CHAR* ps_match[IPCC_LENGTH_UNIT_COUNT] = {"px", "%", "c"};
    UINT8 ui1_index = 0;
    BOOL b_flag = FALSE;

    for(ui1_index = 0; ui1_index < IPCC_LENGTH_UNIT_COUNT; ++ui1_index)
    {
        if(NULL != x_strstr((const CHAR*)ps_str, (const CHAR*)ps_match[ui1_index]))
        {
            *pe_origin_type = (TTML_LENGTH_UNIT_TYPE)(ui1_index + 1);
            b_flag = TRUE;
            
            break;
        }
    }

    if(FALSE == b_flag)
    {
        i4_ret = IPCCR_NOT_FOUND;
    }
    
    return i4_ret;
    
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_util_abstract_value
 *
 * Description: This API is used to abstract value from string
 *
 * Inputs:  ps_string           - Source string
 *          pe_length_unit      - Length unit
 *          
 * Outputs: ps_row                  - row
 *          ps_col                  - col
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_util_abstract_value(CHAR* ps_string, 
                                        TTML_LENGTH_UNIT_TYPE* pe_length_unit,
                                        CHAR* ps_row,
                                        CHAR* ps_col
                                        )
{
    /* Check arguments */  
    if(!ps_string)
    {
        DBG_ERROR(("{IPCC} ERR: ps_string is NULL!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(!pe_length_unit)
    {
        DBG_ERROR(("{IPCC} ERR: pe_length_unit is NULL!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }    

    if(!ps_row)
    {
        DBG_ERROR(("{IPCC} ERR: ps_row is NULL!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(!ps_col)
    {
        DBG_ERROR(("{IPCC} ERR: ps_col is NULL!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

   

    INT32 i4_ret = IPCCR_OK;
    i4_ret = x_sbtl_ipcc_util_detect_length_unit(ps_string, pe_length_unit);
    if(IPCCR_OK != i4_ret)
    {
        return i4_ret;
    }

    CHAR* ps_origin = (CHAR*)x_mem_alloc(sizeof(CHAR) * IPCC_TTS_ORIGIN_LENGTH);
    if(NULL == ps_origin)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_strcpy(ps_origin, ps_string);

    CHAR* ps_whitespace =" \t\f\r\v\n";
    CHAR* ps_px = "px";
    CHAR* ps_percentage = "%";
    CHAR* ps_rslt[2];
    UINT8 ui1_i = 0;
    
    for(ui1_i = 0; ui1_i < 2; ++ui1_i)
    {
        ps_rslt[ui1_i] = (CHAR*)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
        if(NULL == ps_rslt[ui1_i])
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            x_mem_free(ps_origin);
            ps_origin = NULL;
            return IPCCR_INSUFFICIENT_SIZE;
        }
    }

    /* Parse tts:origin by white space, and save the result  */
    i4_ret = x_sbtl_ipcc_str_split(ps_whitespace, ps_origin, ps_rslt, 2, NULL);
    if(IPCCR_OK != i4_ret)
    {
        x_mem_free(ps_origin);  ps_origin = NULL;
        x_mem_free(ps_rslt[0]); ps_rslt[0] = NULL;
        x_mem_free(ps_rslt[1]); ps_rslt[0] = NULL;
        return IPCCR_INV_ARG;
    }

    /* Parse each result */
    switch(*pe_length_unit)
    {
        case TTML_LENGTH_UNIT_TYPE_PX:
            i4_ret = x_sbtl_ipcc_str_split(ps_px, ps_rslt[0], &ps_col, 1, NULL);
            if(IPCCR_OK == i4_ret)
            {
                i4_ret = x_sbtl_ipcc_str_split(ps_px, ps_rslt[1], &ps_row, 1, NULL);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                    x_mem_free(ps_origin);  ps_origin = NULL;
                    x_mem_free(ps_rslt[0]); ps_rslt[0] = NULL;
                    x_mem_free(ps_rslt[1]); ps_rslt[0] = NULL;
                    return i4_ret;
                }
            }
            else
            {
                DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                x_mem_free(ps_origin);  ps_origin = NULL;
                x_mem_free(ps_rslt[0]); ps_rslt[0] = NULL;
                x_mem_free(ps_rslt[1]); ps_rslt[0] = NULL;
                return i4_ret;
            }

            break;

#if 0
        case TTML_LENGTH_UNIT_TYPE_C:
            i4_ret = _sbtl_ipcc_parse_str_split(ps_c, ps_rslt[0], &ps_col, 1);
            if(IPCCR_OK == i4_ret)
            {
                i4_ret = _sbtl_ipcc_parse_str_split(ps_c, ps_rslt[1], &ps_row, 1);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                    return i4_ret;
                }
            }
            else
            {
                DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                return i4_ret;
            }
            break;
#endif

        case TTML_LENGTH_UNIT_TYPE_PRCNT:
            i4_ret = x_sbtl_ipcc_str_split(ps_percentage, ps_rslt[0], &ps_col, 1, NULL);
            if(IPCCR_OK == i4_ret)
            {
                i4_ret = x_sbtl_ipcc_str_split(ps_percentage, ps_rslt[1], &ps_row, 1, NULL);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                    x_mem_free(ps_origin);  ps_origin = NULL;
                    x_mem_free(ps_rslt[0]); ps_rslt[0] = NULL;
                    x_mem_free(ps_rslt[1]); ps_rslt[0] = NULL;
                    return i4_ret;
                }
            }
            else
            {
                DBG_ERROR(("{IPCC} ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                x_mem_free(ps_origin);  ps_origin = NULL;
                x_mem_free(ps_rslt[0]); ps_rslt[0] = NULL;
                x_mem_free(ps_rslt[1]); ps_rslt[0] = NULL;
                return i4_ret;
            }

            break;


    default:
        DBG_INFO(("{IPCC} INFO: Not support this length unit!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        break;
    }

    x_mem_free(ps_origin);
    ps_origin = NULL;
    
    for(ui1_i = 0; ui1_i < 2; ++ui1_i)
    {
        x_mem_free(ps_rslt[ui1_i]);
        ps_rslt[ui1_i] = NULL;
    }

    return i4_ret;
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_util_calculate_row_count
 *
 * Description: This API is used to calculate the row count of the specify text
 *
 * Inputs:  ps_text                 - Source text
 *          
 * Outputs: pi4_row_count           - row count
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_util_calculate_row_count(const CHAR* ps_text, INT32* pi4_row_count)
{
    /* Check arguments */
    if(!ps_text || !pi4_row_count)
    {
        DBG_ERROR(("{IPCC} ERR: ps_text or pi4_row_count is NULL!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    INT32 i4_line_feed = 0;
    INT32 i4_index = 0;

    while('\0' != *ps_text)
    {
        if('\n' == *ps_text)
        {
            ++i4_line_feed;
            i4_index = 0;
        }
        else
        {
            ++i4_index;
        }

        ++ps_text;        
    }

    if(i4_index > 0)
    {
        ++i4_line_feed;
    }

    *pi4_row_count = i4_line_feed;

    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_util_is_space
 *
 * Description: This API is used to detect the current character is whitespace or not
 *
 * Inputs:  c_char                 - Current character
 *
 * Returns: TRUE              
 *          FLASE         
 *
 ----------------------------------------------------------------------------*/
BOOL x_sbtl_ipcc_util_is_space (CHAR  c_char)
{
    return ((c_char == ' ')        /* space */
            || (c_char == '\f')    /* form feed */
            || (c_char == '\n')    /* new line */
            || (c_char == '\r')    /* carriage return */
            || (c_char == '\t')    /* horizontal tab */
            || (c_char == '\v'));  /* vertical tab */
}






#endif
/*----------------------------------------------------------------------------*/
/*! @} */   /* end of  groupMW_STRM_MNGR_MM_SBTL_HDLR*/
/*----------------------------------------------------------------------------*/

