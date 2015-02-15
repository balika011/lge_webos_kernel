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
/*! @file c_time_msrt.h 
 *  $RCSfile: $
 *  $Revision: #1 $
 *  $Date: 2015/02/15 $
 *  $Author: p4admin $
 *  
 *  @par Description:
 */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*! @addtogroup groupMW_TIMEMEASUREMENT
 *  @{ 
 */
/*----------------------------------------------------------------------------*/
#ifndef _FILE_NAME_H_
#define _FILE_NAME_H_    /**<        */

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "c_common.h"
#include "u_time_msrt.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/

#ifdef TIME_MEASUREMENT
#if 0
#define TIME_MSRT_SPOT( _tms_mdl_level, _name)                                \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );             \
    if ( ui2_final_lvl != TMS_LEVEL_NONE )                                    \
    {                                                                         \
        c_time_msrt_spot( _name, ui2_final_lvl, NULL );                       \
    }                                                                         \
}    /**<        */

#define TIME_MSRT_DIFF( _tms_mdl_level, _name)                                \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );             \
    if ( ui2_final_lvl != TMS_LEVEL_NONE )                                    \
    {                                                                         \
        c_time_msrt_diff( _name, ui2_final_lvl, NULL );                       \
    }                                                                         \
}    /**<        */

#define TIME_MSRT_BEGIN( _tms_mdl_level, _name)                               \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );             \
    if ( ui2_final_lvl != TMS_LEVEL_NONE )                                    \
    {                                                                         \
        c_time_msrt_begin( _name, ui2_final_lvl, NULL );                      \
    }                                                                         \
}    /**<        */

#define TIME_MSRT_END( _tms_mdl_level, _name)                                 \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );             \
    if ( ui2_final_lvl != TMS_LEVEL_NONE )                                    \
    {                                                                         \
        c_time_msrt_end( _name, ui2_final_lvl, NULL );                        \
    }                                                                         \
}    /**<        */
#endif

#define TIME_MSRT_SPOT( _tms_mdl_level, _name)    /**<This function create the SPOT TMS record.        */
#define TIME_MSRT_DIFF( _tms_mdl_level, _name)    /**<This function create the DIFF TMS record.          */
#define TIME_MSRT_BEGIN( _tms_mdl_level, _name)    /**<This function begins the BEGIN-END TMS record.         */
#define TIME_MSRT_END( _tms_mdl_level, _name)    /**<This function ends the BEGIN-END TMS record.        */

#define TIME_MSRT_SPOT_EX( _tms_mdl_level, _flag, _name, _comment)            \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    extern UINT32 time_msrt_get_flag( VOID );                                 \
    UINT32 ui4_flag = time_msrt_get_flag();                                   \
    if (ui4_flag & _flag)                                                     \
    {                                                                         \
        UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );         \
        if ( ui2_final_lvl != TMS_LEVEL_NONE )                                \
        {                                                                     \
            c_time_msrt_spot( _name, ui2_final_lvl, _comment );               \
        }                                                                     \
    }                                                                         \
}    /**<This function create the SPOT TMS record.       */

#define TIME_MSRT_DIFF_EX( _tms_mdl_level, _flag, _name, _comment)            \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    extern UINT32 time_msrt_get_flag( VOID );                                 \
    UINT32 ui4_flag = time_msrt_get_flag();                                   \
    if (ui4_flag & _flag)                                                     \
    {                                                                         \
        UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );         \
        if ( ui2_final_lvl != TMS_LEVEL_NONE )                                \
        {                                                                     \
            c_time_msrt_diff( _name, ui2_final_lvl, _comment );               \
        }                                                                     \
    }                                                                         \
}    /**<This function create the DIFF TMS record.      */

#define TIME_MSRT_BEGIN_EX( _tms_mdl_level, _flag, _name, _comment)           \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    extern UINT32 time_msrt_get_flag( VOID );                                 \
    UINT32 ui4_flag = time_msrt_get_flag();                                   \
    if (ui4_flag & _flag)                                                     \
    {                                                                         \
        UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );         \
        if ( ui2_final_lvl != TMS_LEVEL_NONE )                                \
        {                                                                     \
            c_time_msrt_begin( _name, ui2_final_lvl, _comment );              \
        }                                                                     \
    }                                                                         \
}    /**<This function begins the BEGIN-END TMS record.      */

#define TIME_MSRT_END_EX( _tms_mdl_level, _flag, _name, _comment)             \
{                                                                             \
    extern UINT16 time_msrt_get_level( UINT16 );                              \
    extern UINT32 time_msrt_get_flag( VOID );                                 \
    UINT32 ui4_flag = time_msrt_get_flag();                                   \
    if (ui4_flag & _flag)                                                     \
    {                                                                         \
        UINT16 ui2_final_lvl = time_msrt_get_level( _tms_mdl_level );         \
        if ( ui2_final_lvl != TMS_LEVEL_NONE )                                \
        {                                                                     \
            c_time_msrt_end( _name, ui2_final_lvl, _comment );                \
        }                                                                     \
        if((TMS_FLAG_POWER_OFF | TMS_FLAG_NONE) == ui4_flag)                  \
        {                                                                     \
            c_time_msrt_dump_detail(NULL);                                           \
        }                                                                     \
    }                                                                         \
}    /**< This function ends the BEGIN-END TMS record.      */

#else
#define TIME_MSRT_SPOT( _tms_mdl_level, _name)    /**<This function create the SPOT TMS record.          */
#define TIME_MSRT_DIFF( _tms_mdl_level, _name)    /**<This function create the DIFF TMS record.          */
#define TIME_MSRT_BEGIN( _tms_mdl_level, _name)    /**<This function begins the BEGIN-END TMS record.         */
#define TIME_MSRT_END( _tms_mdl_level, _name)    /**<This function ends the BEGIN-END TMS record.      */
#define TIME_MSRT_SPOT_EX( _tms_mdl_level, _flag, _name, _comment)    /**<This function create the SPOT TMS record.          */
#define TIME_MSRT_DIFF_EX( _tms_mdl_level, _flag, _name, _comment)    /**<This function create the DIFF TMS record.          */
#define TIME_MSRT_BEGIN_EX( _tms_mdl_level, _flag, _name, _comment)    /**<This function begins the BEGIN-END TMS record.        */
#define TIME_MSRT_END_EX( _tms_mdl_level, _flag, _name, _comment)    /**<This function ends the BEGIN-END TMS record.       */
#endif /* TIME_MEASUREMENT */


/*-----------------------------------------------------------------------------
                    functions declarations
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
/*! @brief   c_time_msrt_spot
 *               This function create the SPOT TMS record.
 *  @param [in] s_name      Name of the SPOT TMS record.
 *  @param [in] ui2_tms_lvl The TMS level of the record.
 *  @param [in] s_comment  String of comment
 *  @note
 *  @see
 *  @return Return the error code.
 *  @retval TMSR_FAIL                 Success.
 *  @retval TMSR_FAIL                 Fail.
 *  @retval TMSR_INSUFFICIENT_MEMORY  Fail.
 */
/*------------------------------------------------------------------*/
extern INT32 c_time_msrt_spot( 
    const CHAR*           s_name,
    UINT16                ui2_tms_lvl,
    const CHAR*           s_comment );

/*------------------------------------------------------------------*/
/*! @brief   c_time_msrt_diff
 *               This function create the diff TMS record.
 *  @param [in] s_name      Name of the diff TMS record.
 *  @param [in] ui2_tms_lvl The TMS level of the record.
 *  @param [in] s_comment  String of comment
 *  @note
 *  @see
 *  @return Return the error code.
 *  @retval TMSR_FAIL                 Success.
 *  @retval TMSR_FAIL                 Fail.
 *  @retval TMSR_INSUFFICIENT_MEMORY  Fail.
 */
/*------------------------------------------------------------------*/

extern INT32 c_time_msrt_diff(
    const CHAR*           s_name,
    UINT16                ui2_tms_lvl,
    const CHAR*           s_comment );


/*------------------------------------------------------------------*/
/*! @brief  c_time_msrt_begin
 *              This function begins the BEGIN-END TMS record.
 *  @param [in] s_name      Name of the SPOT TMS record.
 *  @param [in] ui2_tms_lvl The TMS level of the record.
 *  @param [in] s_comment  String of comment
 *  @note
 *  @see
 *  @return Return the error code.
 *  @retval TMSR_FAIL                 Success.
 *  @retval TMSR_FAIL                 Fail.
 *  @retval TMSR_INSUFFICIENT_MEMORY  Fail.
 */
/*------------------------------------------------------------------*/
extern INT32 c_time_msrt_begin( 
    const CHAR*           s_name,
    UINT16                ui2_tms_lvl,
    const CHAR*           s_comment );
 
/*------------------------------------------------------------------*/
/*! @brief   c_time_msrt_end
 *               This function ends the BEGIN-END TMS record.
 *  @param [in] s_name      Name of the SPOT TMS record.
 *  @param [in] ui2_tms_lvl The TMS level of the record.
 *  @param [in] s_comment  String of comment
 *  @note
 *  @see
 *  @return Reutrn the error code.
 *  @retval TMSR_FAIL                 Success.
 *  @retval TMSR_FAIL                 Fail.
 *  @retval TMSR_INSUFFICIENT_MEMORY  Fail.
 */
/*------------------------------------------------------------------*/
extern INT32 c_time_msrt_end( 
    const CHAR*           s_name,
    UINT16                ui2_tms_lvl,
    const CHAR*           s_comment );
 

/*------------------------------------------------------------------*/
/*! @brief  c_time_msrt_dump
 *             This function dumps all records which matchs the name specified
 *              by caller.
 *  @param [in] s_name Caller specified record name to be dump. Set it to NULL to 
 *                              dump all.
 */
/*------------------------------------------------------------------*/
extern VOID c_time_msrt_dump( const CHAR* s_name );


/*------------------------------------------------------------------*/
/*! @brief   c_time_msrt_dump_detail
 *              This function dumps details of all records which matchs the 
 *              name specified by caller.
 *  @param [in] s_name Caller specified record name to be dump. Set it to NULL to 
 *                              dump all.
 */
/*------------------------------------------------------------------*/
extern VOID c_time_msrt_dump_detail( const CHAR* s_name );


/*------------------------------------------------------------------*/
/*! @brief   c_time_msrt_clean
 *              This function clean up all records which matchs the 
 *              name specified by caller.
 *  @param [in] s_name Caller specified record name to be clean. Set it to NULL to 
 *                              clean all.
 */
/*------------------------------------------------------------------*/
extern VOID c_time_msrt_clean( const CHAR* s_name );

#endif /* _FILE_NAME_H_ */
/*----------------------------------------------------------------------------*/
/*! @} */   /* end of  groupMW_TIMEMEASUREMENT*/
/*----------------------------------------------------------------------------*/

