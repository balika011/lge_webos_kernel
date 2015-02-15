/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
/*-----------------------------------------------------------------------------
 * $RCSfile: djpeg.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/14 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 5d20a52d05b772f8d75a3e9c6251a98c $ Asa
 *
 * Description: 
 *         This file contains JPEG decoding MANIPULATION FUNCTIONS 
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "jpg_config.h"
#include "jpeglib.h"
#include "jdhuff.h"
#include "djpeg_sw_api.h"
#include "../jdec_drv_scale.h"
#include "../jdec_debug.h"
//#include "x_dbg.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#ifdef USE_EXIF_ORIENTATION_TAG
#define ORIENTATION_TAG_OFST 20
#define ORIENTATION_TAG_LEN  12
#define ORIENTATION_1    1
#define ORIENTATION_2    2
#define ORIENTATION_3    3
#define ORIENTATION_4    4
#define ORIENTATION_5    5
#define ORIENTATION_6    6
#define ORIENTATION_7    7
#define ORIENTATION_8    8
#endif /* USE_EXIF_ORIENTATION_TAG */
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
extern void vInitial_fast_IDCT(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
#ifdef USE_EXIF_ORIENTATION_TAG
BYTE _pbAPP1[32] = {0xFF,0xE1,0x00,0x1E,0x45,0x78,0x69,0x66,
                    0x00,0x00,0x49,0x49,0x2A,0x00,0x08,0x00,
                    0x00,0x00,0x01,0x00,0x12,0x01,0x03,0x00,
                    0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00};

BYTE _pbAPP1BigEndian[32] = {0xFF,0xE1,0x00,0x1E,0x45,0x78,0x69,0x66,
                             0x00,0x00,0x4D,0x4D,0x2A,0x00,0x00,0x00,
                             0x00,0x08,0x00,0x01,0x01,0x12,0x00,0x03,
                             0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00};
#endif /* EXIF_ORIENTATION_TAG */
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name:  vInitial_decompress
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vInitial_decompress(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    if (pt_jpg_obj == NULL)
    {
        return;
    }
        
    x_memset(pt_jpg_obj, 0, sizeof(IMG_JPG_OBJ_T));

    if (pt_jpg_obj->pt_current_sel == NULL)
    {
        /* set default current select to primary image*/
        pt_jpg_obj->pt_current_sel = (IMG_BASIC_INFO_T*)&(pt_jpg_obj->t_primary);
    }
    
    /* initial primary and thumbnail variable*/
    
    pt_jpg_obj->t_primary.t_basic.b_first_SOS = TRUE;           
    pt_jpg_obj->t_thumbnail.t_basic.b_first_SOS = TRUE;           

    /*initial jpeg object variable*/
       
} /* vInitial_decompress() function */

/*-----------------------------------------------------------------------------
 * Name:  fgDecode_jpg
 *
 * Description: decode JPEG file.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: TRUE       success.
 *              FALSE     FAILED.  
 ----------------------------------------------------------------------------*/
BOOL fgDecode_jpg(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect)
{ 
    IMG_BASIC_INFO_T* pt_info = NULL;
    BOOL b_has_multiple_scans;
    register BOOL* pb_use_second_color_buf;
    register UINT32* pui4_OutputIMCURow;
    UINT32 ci;
    COMPONENT_INFO_T * compptr;
    INT16** ppi2_start_addr;
    UINT32 ui4_iTotalIMCURows;
    //resampling_fct pf_resampling_fct;
    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return FALSE;
    }

    b_has_multiple_scans = pt_info->b_has_multiple_scans;
    pb_use_second_color_buf = &(pt_info->b_use_second_color_buf);
    //[CJYang Resample]
    //pui4_OutputIMCURow = &(pt_jpg_obj->ui4_OutputIMCURow);
    pui4_OutputIMCURow = &(prJdecInst->u4CurrDecodeMCURow);

    /*    to check whether VDO buffered data is enough and whether VDO buffer is stalled
          1. Wait until (VDO buffered data > MIN_JPG_BUFFER_LEVEL bytes) or (the total file is read completely),
             then we start to decode one iMCU of JPG data
          2. If the VDO buffer has no input for a period of (MAX_JPG_BUFFER_STALL_TIME/0x16800) ,
              return JPG_FATAL_ERR and skip the remain of this JPG file*/
    /* Jobs to do in the beginning of every progressive scan */
    if (*pui4_OutputIMCURow == 0) 
    {
        pt_jpg_obj->b_ReachFileEnd = FALSE; 
        
        pt_jpg_obj->i2_unread_marker = 0;

        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): -.\n\r");
            vJdecLog_M("[JPG)SW] ERR: fgDecode_jpg(): -.\n\r");
            return FALSE;
        }

        if (pt_jpg_obj->b_arith_code)   
        {
            pt_jpg_obj->ui4_JPGErrorType = 1;     
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;  
            //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): pt_jpg_obj->b_arith_code %x \n\r", pt_jpg_obj->b_arith_code);
            vJdecLog_M("[JPG_SW] ERR: fgDecode_jpg(): pt_jpg_obj->b_arith_code %x \n\r", pt_jpg_obj->b_arith_code);
            return FALSE;
        }

        vInitial_decode_buffer (prJdecInst, pt_jpg_obj, pt_src_dst_rect, prJdecInst->eDstRotate, TRUE);
        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): vInitial_decode_buffer error. \n\r");
            vJdecLog_M("[JPG_SW] ERR: fgDecode_jpg(): vInitial_decode_buffer error. \n\r");
            return FALSE;
        } 

        /* Start decompressor */
        /* reset pt_info->input_iMCU_row to 0 */
        pt_jpg_obj->ui2_input_iMCU_row = 0;

        vStart_input_pass (prJdecInst, pt_jpg_obj);
        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): vStart_input_pass error. \n\r");
            vJdecLog_M("[JPG_SW] ERR: fgDecode_jpg(): vStart_input_pass error. \n\r");
            return FALSE;
        }    

        //init scale function
        i4JdecScaleInit(prJdecInst);
        i4JdecScaleSetJpegInfo(prJdecInst);

        if (b_has_multiple_scans == TRUE)
        {      
            vMultiple_scan_collect_data (prJdecInst, pt_jpg_obj, 100);
            pt_jpg_obj->b_ReachFileEnd = FALSE;
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): vMultiple_scan_collect_data error. \n\r");
                vJdecLog_M("[JPG_SW] ERR: fgDecode_jpg(): vMultiple_scan_collect_data error. \n\r");
                return FALSE;
            }
        }                               

        if (pt_info->ui2_p_scan_decode_num == 0)
        {
            *pb_use_second_color_buf = FALSE; 
            /* initial IDCT parameters */
            vInitial_fast_IDCT(prJdecInst, pt_jpg_obj); 
        }
    } /*if (pui4_OutputIMCURow == 0)*/
#if 0
     pf_resampling_fct = pt_jpg_obj->pf_resampling_fct;
     if (pf_resampling_fct == NULL)
    {
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            UTIL_Printf("[IMG] ERR: pf_resampling_fct == NULL \n\r");       
            return FALSE;
    }
#endif
    /*****************************************/
    /*  Process one iMCU row loop body begin */
    /*****************************************/  
    if (!b_has_multiple_scans)    
    {
        /* clear_iMCU_row_buf function */ 
        ppi2_start_addr = pt_jpg_obj->ppui2_iMCU_row_start_addr;
        //[CJYang Resample]
        //ui4_iTotalIMCURows = pt_info->ui4_TotalIMCURows;
        ui4_iTotalIMCURows = prJdecInst->u4MCURowNum;

        if (pt_info->ui2_num_components > MAX_COMPS_IN_SCAN)
        {
            ASSERT(0);
        }
        for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++) 
        {          
            x_memset(*(ppi2_start_addr + (ci * ui4_iTotalIMCURows)), 0, pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * DCTSIZE2 * INSIZE);
        }        
        vConsume_data (prJdecInst, pt_jpg_obj, TRUE);    
    }  

    if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
    {
        //UTIL_Printf("[IMG] ERR: fgDecode_jpg(): Process one iMCU row loop body begin\n\r");
        vJdecLog_M("[JPG_SW] ERR: fgDecode_jpg(): Process one iMCU row loop body begin\n\r");
        return FALSE;
    }   

    if ((b_has_multiple_scans == FALSE) || 
        ((b_has_multiple_scans == TRUE) && (*pui4_OutputIMCURow >= pt_info->ui4_begin_decode_iMCU_row) && (*pui4_OutputIMCURow < (pt_info->ui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num))))
    {
        vDecompress_iMCU_data (prJdecInst, pt_jpg_obj, pt_src_dst_rect);

        if (*pb_use_second_color_buf == TRUE)
        {
            *pb_use_second_color_buf = FALSE;
        }    
        else
        {
            *pb_use_second_color_buf = TRUE;  
        }
        /* 執行 upsampling 在上一個解好的 IDCT iMCU row */
        /* 只有在pt_jpg_obj->_iOutputIMCURow == 0的時候，才不做bilinear_resampling，之
        後每解一個iMCU rows槫
        ，之後就做前一個iMCU row 的resampling 動作*/
        //[CJYang Resample]
        //if (pt_jpg_obj->ui4_OutputIMCURow != 0) 
        {
            //TMS_BEGIN(AS_IMG_RESAMPLE);	//[CJYang]
#if 0
		    pf_resampling_fct( 
                                        pt_jpg_obj, 
                                        pt_src_dst_rect, 
                                        e_ratate_type, 
                                        pt_lock_info,
                                        pt_jpg_obj->aapui1_color_buf[Y_INDEX][*pb_use_second_color_buf],
                                        pt_jpg_obj->aapui1_color_buf[CB_INDEX][*pb_use_second_color_buf],
                                        pt_jpg_obj->aapui1_color_buf[CR_INDEX][*pb_use_second_color_buf],
                                        pt_jpg_obj->aapui1_color_buf[Y_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                        pt_jpg_obj->aapui1_color_buf[CB_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                        pt_jpg_obj->aapui1_color_buf[CR_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                        h_surf_signal);
#else
            i4JdecScaleSetNextPartialBuf(prJdecInst);
            i4JdecScaleFire(prJdecInst);
            if (i4JdecScaleWaitFinish(prJdecInst) != JDEC_DRV_RTN_OK)
            {
                return FALSE;
            }
#endif
            //TMS_END(AS_IMG_RESAMPLE);
        }

    }
    *pui4_OutputIMCURow = *pui4_OutputIMCURow + 1;  

    /* When not using the simplest bilinear resampling algorithm, 
    we have to perform additional resampling at last pt_jpg_obj->ui4_OutputIMCURow */
    //[CJYang Resample]
    //if ( (*pui4_OutputIMCURow >= pt_info->ui4_TotalIMCURows) && (pt_info->b_last_p_scan_decode == TRUE) ) 
    if ( (*pui4_OutputIMCURow >= prJdecInst->u4MCURowNum) && (pt_info->b_last_p_scan_decode == TRUE) ) 
    {
        if (*pb_use_second_color_buf == TRUE)
        {
            *pb_use_second_color_buf = FALSE;
        }    
        else
        {
            *pb_use_second_color_buf = TRUE;  
        }
        //TMS_BEGIN(AS_IMG_RESAMPLE);	//[CJYang] need to check
        /* last resampling process */
#if 0
        pf_resampling_fct( 
                                    pt_jpg_obj, 
                                    pt_src_dst_rect, 
                                    e_ratate_type, 
                                    pt_lock_info,
                                    pt_jpg_obj->aapui1_color_buf[Y_INDEX][*pb_use_second_color_buf],
                                    pt_jpg_obj->aapui1_color_buf[CB_INDEX][*pb_use_second_color_buf],
                                    pt_jpg_obj->aapui1_color_buf[CR_INDEX][*pb_use_second_color_buf],
                                    pt_jpg_obj->aapui1_color_buf[Y_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                    pt_jpg_obj->aapui1_color_buf[CB_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                    pt_jpg_obj->aapui1_color_buf[CR_INDEX][((*pb_use_second_color_buf == 1)? 0 : 1)],
                                    h_surf_signal);
#endif
        //TMS_END(AS_IMG_RESAMPLE);	//[CJYang] need to check
    }
    /***************************************/
    /*  Process one iMCU row loop body end */
    /***************************************/
    return TRUE;
}/* fgDecode_jpg() function */


