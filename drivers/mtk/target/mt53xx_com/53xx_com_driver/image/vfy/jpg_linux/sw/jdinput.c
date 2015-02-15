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
 * $RCSfile: jdinput.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/2 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: f941da3e89b19d81d05ac7710145fd3f $ Asa
 *
 * Description:
 *         Routines to input source and calculate various quantities related to the size of the image.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "jdhuff.h"     /* Declarations shared with jdphuff.c */
#include "djpeg_sw_api.h"
#include "x_time_msrt.h"
#include "x_dbg.h"
#include "drv_img_dma.h"
#include "../jdec_debug.h"
#include "x_debug.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
extern INT32 i4JdecChkFifoStatus(JDEC_INST_T *prJdecInst);

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

INT32 i4JdecSwUpdateFifoRdPoint(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    if (prFifoInfo->u4RdPoint >= prFifoInfo->u4PreRdPoint)
    {
        if ((prFifoInfo->u4RdPoint - prFifoInfo->u4PreRdPoint) < 0x400)
        {
            return JDEC_DRV_RTN_OK;
        }
    }
    else
    {
        if (((prFifoInfo->u4FifoEnd - prFifoInfo->u4PreRdPoint) + (prFifoInfo->u4RdPoint - prFifoInfo->u4FifoStart)) < 0x400)
        {
            return JDEC_DRV_RTN_OK;
        }
    }

    if (prFifoInfo->fgEoRange == FALSE)
    {
        i4Ret = i4ImgdmaSetRdPoint(prJdecInst->u4ImgdmaId, prFifoInfo->u4RdPoint);
        VERIFY(i4Ret == IMGDMA_OK);
        prFifoInfo->u4PreRdPoint = prFifoInfo->u4RdPoint;
    }
    return JDEC_DRV_RTN_OK;
}


/*-----------------------------------------------------------------------------
 * Name:  u4Input4bytes
 *
 * Description: read 4 bytes from memory stream
 *
 * Inputs:  pt_jpg_obj  JPEG OBJECT
 *
 * Outputs: -
 *
 * Returns: 4bytes data.
 ----------------------------------------------------------------------------*/
UINT32 u4Input4bytes (JDEC_INST_T *prJdecInst)
{
    register UINT32 ui4_ret = 0;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    INT32 i4 = 0;

    while (prFifoInfo->u4ValidData < 4)
    {
        if (prFifoInfo->fgEoRange)
        {
            return 0;
        }
        x_thread_delay(5);
        i4JdecChkFifoStatus(prJdecInst);
    }
    for (i4=3; i4>=0; i4--)
    {
        ui4_ret |= (*((UINT8 *)(prFifoInfo->u4RdPoint))) << (i4 * 8);
        prFifoInfo->u4RdPoint ++;
        if (prFifoInfo->u4RdPoint >= prFifoInfo->u4FifoEnd)
        {
            prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart;
        }
    }
    prFifoInfo->u4ValidData -= 4;
    prFifoInfo->u4FileOfst += 4;
    i4JdecSwUpdateFifoRdPoint(prJdecInst);
#if 0
    //register IMG_JPG_INPUT_T *pt_input;
	PREFETCH_RING_BUFFER *pt_src_bufinfo;
    register UCHAR* buf;
    register SIZE_T z_file_size;
    register UINT32* pui4_readpos;

    //pt_input = (IMG_JPG_INPUT_T*)(&pt_jpg_obj->t_input);
	pt_src_bufinfo = &(pt_jpg_obj->t_prefetch_ring_buf);
    //buf = pt_input->u1.ps_src;
    buf = pt_src_bufinfo->puc_buf;
    z_file_size = pt_jpg_obj->ui4_JPGFileSizeInByte;
    //pui4_readpos = &(pt_input->u2.ui4_readpos);
    pui4_readpos = &(pt_src_bufinfo->ui4_head);

    if (z_file_size>= (*pui4_readpos + 4))
    {
        ui4_ret = buf[(*pui4_readpos)++] << 24;
        ui4_ret |= buf[(*pui4_readpos)++] << 16;
        ui4_ret |= buf[(*pui4_readpos)++] << 8;
        ui4_ret |= buf[(*pui4_readpos)++];
    }
#endif
    return ui4_ret;
}
/*-----------------------------------------------------------------------------
 * Name:  u2Input2bytes
 *
 * Description: read 2 bytes from memory stream
 *
 * Inputs:  pt_jpg_obj  JPEG OBJECT
 *
 * Outputs: -
 *
 * Returns: 2bytes data.
 ----------------------------------------------------------------------------*/
UINT16 u2Input2bytes (JDEC_INST_T *prJdecInst)
{
    register UINT16 ui2_ret = 0;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    INT32 i4 = 0;

    while (prFifoInfo->u4ValidData < 2)
    {
        if (prFifoInfo->fgEoRange)
        {
            return 0;
        }
        x_thread_delay(5);
        i4JdecChkFifoStatus(prJdecInst);
    }
    for (i4=1; i4>=0; i4--)
    {
        ui2_ret |= (*((UINT8 *)(prFifoInfo->u4RdPoint))) << (i4 * 8);
        prFifoInfo->u4RdPoint ++;
        if (prFifoInfo->u4RdPoint >= prFifoInfo->u4FifoEnd)
        {
            prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart;
        }
    }
    prFifoInfo->u4ValidData -= 2;
    prFifoInfo->u4FileOfst += 2;
    i4JdecSwUpdateFifoRdPoint(prJdecInst);
#if 0
    //register IMG_JPG_INPUT_T *pt_input;
	PREFETCH_RING_BUFFER *pt_src_bufinfo;
    register UCHAR* buf;
    register SIZE_T z_file_size;
    register UINT32* pui4_readpos;

    //pt_input = (IMG_JPG_INPUT_T*)(&pt_jpg_obj->t_input);
	pt_src_bufinfo = &(pt_jpg_obj->t_prefetch_ring_buf);
    //buf = pt_input->u1.ps_src;
    buf = pt_src_bufinfo->puc_buf;
    z_file_size = pt_jpg_obj->ui4_JPGFileSizeInByte;
    //pui4_readpos = &(pt_input->u2.ui4_readpos);
    pui4_readpos = &(pt_src_bufinfo->ui4_head);

    //if (z_file_size >= (pt_input->u2.ui4_readpos + 2))
    if (z_file_size >= (pt_src_bufinfo->ui4_head + 2))
    {
        ui2_ret = buf[(*pui4_readpos)++] << 8;
        ui2_ret |= buf[(*pui4_readpos)++];
    }
#endif
    return ui2_ret;
}
/*-----------------------------------------------------------------------------
 * Name:  u1Input1byte
 *
 * Description: read 1 bytes from memory stream
 *
 * Inputs:  pt_jpg_obj  JPEG OBJECT
 *
 * Outputs: -
 *
 * Returns: 1byte data.
 ----------------------------------------------------------------------------*/
UINT8 u1Input1byte (JDEC_INST_T *prJdecInst)
{
#if 0
    register UCHAR* buf;
    register UINT32* pui4_readpos;
    //register IMG_JPG_INPUT_T *pt_input;
	PREFETCH_RING_BUFFER *pt_src_bufinfo;

    //pt_input = (IMG_JPG_INPUT_T*)(&pt_jpg_obj->t_input);
	pt_src_bufinfo = &(pt_jpg_obj->t_prefetch_ring_buf);
    //pui4_readpos = &(pt_input->u2.ui4_readpos);
    pui4_readpos = &(pt_src_bufinfo->ui4_head);

    if (pt_jpg_obj->ui4_JPGFileSizeInByte >= (*pui4_readpos + 1))
    {
        //buf = pt_input->u1.ps_src;
        buf = pt_src_bufinfo->puc_buf;
        return buf[(*pui4_readpos)++];
    }
#else
    UINT8 ui1_ret = 0;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    while (prFifoInfo->u4ValidData < 1)
    {
        if (prFifoInfo->fgEoRange)
        {
            return 0;
        }
        x_thread_delay(5);
        i4JdecChkFifoStatus(prJdecInst);
    }
    ui1_ret |= *((UINT8 *)(prFifoInfo->u4RdPoint));
    prFifoInfo->u4RdPoint ++;
    if (prFifoInfo->u4RdPoint >= prFifoInfo->u4FifoEnd)
    {
        prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart;
    }
    prFifoInfo->u4ValidData -= 1;
    prFifoInfo->u4FileOfst += 1;
    i4JdecSwUpdateFifoRdPoint(prJdecInst);

    return ui1_ret;
#endif
}


/*-----------------------------------------------------------------------------
 * Name:  fgSkipbytes
 *
 * Description: skip bytes from memory stream
 *
 * Inputs:  pt_jpg_obj      JPEG OBJECT
 *          ui4_num_bytes   number of bytes
 *
 * Outputs: -
 *
 * Returns: TRUE success.
 *          FALSE Fail.
 ----------------------------------------------------------------------------*/
BOOL fgSkipbytes (JDEC_INST_T *prJdecInst, UINT32 ui4_num_bytes)
{
#if 0
    //register IMG_JPG_INPUT_T *pt_input;
	PREFETCH_RING_BUFFER *pt_src_bufinfo;
    register SIZE_T z_file_size;
    register UINT32* pui4_readpos;

    //pt_input = (IMG_JPG_INPUT_T*)(&pt_jpg_obj->t_input);
	pt_src_bufinfo = &(pt_jpg_obj->t_prefetch_ring_buf);
    //pui4_readpos = &(pt_input->u2.ui4_readpos);
    pui4_readpos = &(pt_src_bufinfo->ui4_head);
    z_file_size = pt_jpg_obj->ui4_JPGFileSizeInByte;

    if (z_file_size >= (*pui4_readpos + ui4_num_bytes))
    {
        *pui4_readpos += ui4_num_bytes;
        return TRUE;
    }
#else
    UINT32 u4SkipBytes = ui4_num_bytes;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    while (u4SkipBytes != 0)
    {
        UINT32 u4ValidData = prFifoInfo->u4ValidData;
        while (prFifoInfo->u4ValidData < 1)
        {
            if (prFifoInfo->fgEoRange)
            {
                return 0;
            }
            x_thread_delay(5);
            i4JdecChkFifoStatus(prJdecInst);
        }
        u4ValidData = prFifoInfo->u4ValidData;
        if (u4ValidData > u4SkipBytes)
        {
            if ((prFifoInfo->u4RdPoint + u4SkipBytes) < prFifoInfo->u4FifoEnd)
            {
                prFifoInfo->u4RdPoint += u4SkipBytes;
            }
            else
            {
                prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart + ((prFifoInfo->u4RdPoint + u4SkipBytes) - prFifoInfo->u4FifoEnd);
            }
            prFifoInfo->u4ValidData -= u4SkipBytes;
            u4SkipBytes = 0;
        }
        else
        {
            if ((prFifoInfo->u4RdPoint + u4ValidData) < prFifoInfo->u4FifoEnd)
            {
                prFifoInfo->u4RdPoint += u4ValidData;
            }
            else
            {
                prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart + ((prFifoInfo->u4RdPoint + u4ValidData) - prFifoInfo->u4FifoEnd);
            }
            prFifoInfo->u4ValidData -= u4ValidData;
            u4SkipBytes -= u4ValidData;
        }
        i4JdecSwUpdateFifoRdPoint(prJdecInst);
    }

    prFifoInfo->u4FileOfst += ui4_num_bytes;
    return TRUE;
#endif
}
/*-----------------------------------------------------------------------------
 * Name:  vFirst_SOS_setup
 *
 * Description: Called once, when first SOS marker is reached
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vFirst_SOS_setup (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    register INT16 ci;
    register COMPONENT_INFO_T *compptr;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    /* Compute maximum sampling factors; check factor validity */
    pt_info->ui1_max_h_samp_factor = 1;
    pt_info->ui1_max_v_samp_factor = 1;

    for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
    {
        if (compptr->i2_h_samp_factor <= 0
            || compptr->i2_h_samp_factor > MAX_SAMP_FACTOR
            || compptr->i2_v_samp_factor <= 0
            || compptr->i2_v_samp_factor > MAX_SAMP_FACTOR)
        {
            //UTIL_Printf("[IMG] Bogus sampling factors\n");
            vJdecLog_L("[JPG_SW] Bogus sampling factors\n");

            if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vFirst_SOS_setup(): Bogus sampling factors.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vFirst_SOS_setup(): Bogus sampling factors.\n\r");
                pt_jpg_obj->ui4_JPGErrorType = 7;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            }
            /* Error handler:
             set to nearest value */
            if (compptr->i2_h_samp_factor <= 0)
                compptr->i2_h_samp_factor = 0;
            else /* (compptr->h_samp_factor>MAX_SAMP_FACTOR) */
                compptr->i2_h_samp_factor = MAX_SAMP_FACTOR;

            if (compptr->i2_v_samp_factor <= 0)
                compptr->i2_v_samp_factor = 0;
            else /* compptr->v_samp_factor>MAX_SAMP_FACTOR */
                compptr->i2_v_samp_factor = MAX_SAMP_FACTOR;
        }
        pt_info->ui1_max_h_samp_factor = MAX(pt_info->ui1_max_h_samp_factor, compptr->i2_h_samp_factor);
        pt_info->ui1_max_v_samp_factor = MAX(pt_info->ui1_max_v_samp_factor, compptr->i2_v_samp_factor);
    }

    /* We initialize DCT_scaled_size and min_DCT_scaled_size to DCTSIZE.
    * In the full decompressor, this will be overridden by jdmaster.c;
    * but in the transcoder, jdmaster.c is not used, so we must do it here.
    */

    /* Compute dimensions of components */
    for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components;  ci++, compptr++)
    {
        /* Size in DCT blocks */
        compptr->ui2_width_in_blocks = (UINT16)((SCALE_UP_CEIL((((UINT32)pt_info->ui2_image_width * (UINT32)compptr->i2_h_samp_factor)<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_h_samp_factor * DCTSIZE)))>>UP_SCALE_Q);
        compptr->ui2_height_in_blocks = (UINT16)((SCALE_UP_CEIL((((UINT32)pt_info->ui2_image_height * (UINT32)compptr->i2_v_samp_factor)<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_v_samp_factor * DCTSIZE)))>>UP_SCALE_Q);

        /* Mark component needed, until color conversion says otherwise */
        compptr->b_component_needed = TRUE;
    }

    /* Compute number of fully interleaved MCU rows. */
    //[CJYang Resample]
    //pt_info->ui4_TotalIMCURows = (INT32)((SCALE_UP_CEIL(((UINT32)pt_info->ui2_image_height<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_v_samp_factor*DCTSIZE)))>>UP_SCALE_Q);
    prJdecInst->u4MCURowNum = (INT32)((SCALE_UP_CEIL(((UINT32)pt_info->ui2_image_height<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_v_samp_factor*DCTSIZE)))>>UP_SCALE_Q);

    /* Decide whether file contains multiple scans */
    if (pt_info->ui1_comps_in_scan < pt_info->ui2_num_components
        || pt_info->b_progressive_mode)
    {
        pt_info->b_has_multiple_scans = TRUE;
    }
    else
    {
        pt_info->b_has_multiple_scans = FALSE;
    }

}

/*-----------------------------------------------------------------------------
 * Name:  vPer_scan_setup
 *
 * Description: Do computations that are needed before processing a JPEG scan
 *              cinfo->pt_info->ui1_comps_in_scan and cinfo->pt_info->apt_cur_comp_info[] were set from SOS marker
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vPer_scan_setup (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT16 ci, mcublks, tmp;
    COMPONENT_INFO_T *compptr;
    IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    if (pt_info->ui1_comps_in_scan == 1)
    {
        /* Noninterleaved (single-component) scan */
        compptr = pt_info->apt_cur_comp_info[0];

        /* Overall image size in MCUs */
        pt_info->ui2_MCUs_per_row = compptr->ui2_width_in_blocks;

        /* For noninterleaved scan, always one block per MCU */
        compptr->i2_MCU_width = 1;
        compptr->i2_MCU_height = 1;
        compptr->i2_MCU_blocks = 1;

        /* For noninterleaved scans, it is convenient to define last_row_height
        * as the number of block rows present in the last iMCU row.
        */
        tmp = (INT16) (compptr->ui2_height_in_blocks % compptr->i2_v_samp_factor);
        if (tmp == 0) tmp = compptr->i2_v_samp_factor;
            compptr->i2_last_row_height = tmp;

        /* Prepare array describing MCU composition */
        pt_info->ui2_blocks_in_MCU = 1;
        pt_info->aui2_MCU_membership[0] = 0;
    }
    else
    {
        /* Interleaved (multi-component) scan */
        if (pt_info->ui1_comps_in_scan > MAX_COMPS_IN_SCAN)
        {

            //UTIL_Printf("[IMG] Too many color components: %d, max %d\n", pt_info->ui1_comps_in_scan, MAX_COMPS_IN_SCAN);
            vJdecLog_M("[JPG_SW] Too many color components: %d, max %d\n", pt_info->ui1_comps_in_scan, MAX_COMPS_IN_SCAN);

            pt_jpg_obj->ui4_JPGErrorType = 8+0x100;
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            //UTIL_Printf("[IMG] ERR: vPer_scan_setup(): Too many color components.\n\r");
            vJdecLog_M("[JPG_SW] ERR: vPer_scan_setup(): Too many color components.\n\r");
            /* Error handler:
               immediately return to caller */
            return;
        }

        /* Overall image size in MCUs */
        pt_info->ui2_MCUs_per_row = (UINT16)((SCALE_UP_CEIL(((UINT32)pt_info->ui2_image_width<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_h_samp_factor*DCTSIZE)))>>UP_SCALE_Q);

        pt_info->ui2_blocks_in_MCU = 0;

        for (ci = 0; ci < pt_info->ui1_comps_in_scan; ci++)
        {
            compptr = pt_info->apt_cur_comp_info[ci];
            /* Sampling factors give # of blocks of component in each MCU */
            compptr->i2_MCU_width = compptr->i2_h_samp_factor;
            compptr->i2_MCU_height = compptr->i2_v_samp_factor;
            compptr->i2_MCU_blocks = compptr->i2_MCU_width * compptr->i2_MCU_height;
            /* Figure number of non-dummy blocks in last MCU column & row */
            tmp = (INT16) (compptr->ui2_height_in_blocks % compptr->i2_MCU_height);
            
            prJdecInst->au4CompScanCount[compptr->i2_component_index] += 1;

            if (tmp == 0)
            {
                tmp = compptr->i2_MCU_height;
            }

            compptr->i2_last_row_height = tmp;
            /* Prepare array describing MCU composition */
            mcublks = compptr->i2_MCU_blocks;

            if (pt_info->ui2_blocks_in_MCU + mcublks > D_MAX_BLOCKS_IN_MCU)
            {

                //UTIL_Printf("[IMG] Sampling factors too large for interleaved scan\n");
                vJdecLog_M("[JPG_SW] Sampling factors too large for interleaved scan\n");

                pt_jpg_obj->ui4_JPGErrorType = 9;
                pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
                //UTIL_Printf("[IMG] ERR: vPer_scan_setup(): Sampling factors too large for interleaved scan.\n\r");
                vJdecLog_M("[JPG_SW] ERR: vPer_scan_setup(): Sampling factors too large for interleaved scan.\n\r");
                /* Error handler:
                   immediately return to caller */
                return;
            }

            while (mcublks-- > 0) /* safe while*/
            {
                pt_info->aui2_MCU_membership[(pt_info->ui2_blocks_in_MCU)++] = ci;
                /* ci 是componemt 的 index
                 pt_info->ui2_blocks_in_MCU 會跨 componrent 累加
                 mcublk (compptr->MCU_blocks) 則是目前component 的 block 個數 */
                 if (pt_info->ui2_blocks_in_MCU > (D_MAX_BLOCKS_IN_MCU - 1))
                 {
                    ASSERT(0);
                    break;
                 }
            }
        }
    }
}

/*-----------------------------------------------------------------------------
 * Name:  vLatch_quant_tables
 *
 * Description: Save away a copy of the Q-table referenced by each component present
 *              in the current scan, unless already saved during a prior scan.
 *
 *              In a multiple-scan JPEG file, the encoder could assign different components
 *              the same Q-table slot number, but change table definitions between scans
 *              so that each component uses a different Q-table.  (The IJG encoder is not
 *              currently capable of doing this, but other encoders might.)  Since we want
 *              to be able to dequantize all the components at the end of the file, this
 *              means that we have to save away the table actually used for each component.
 *              We do this by copying the table at the start of the first scan containing
 *              the component.
 *              The JPEG spec prohibits the encoder from changing the contents of a Q-table
 *              slot between scans of a component using that slot.  If the encoder does so
 *              anyway, this decoder will simply use the Q-table values that were current
 *              at the start of the first scan for the component.
 *
 *              The decompressor output side looks only at the saved quant tables,
 *              not at the current Q-table slots.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vLatch_quant_tables (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT16 ci, qtblno, i;
    COMPONENT_INFO_T *compptr;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT16* pui2_quantval;
    UINT32 ui4_qtblno_DCTSIZE2;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pui2_quantval = (UINT16*) pt_info->aaui2_quantval;

    if (pt_info->ui1_comps_in_scan > MAX_COMPS_IN_SCAN)
    {
        ASSERT(0);
    }
    for (ci = 0; ci < pt_info->ui1_comps_in_scan; ci++)
    {
        compptr = pt_info->apt_cur_comp_info[ci];
        /* No work if we already saved Q-table for this component */
        if (compptr->b_quant_table_saved == TRUE)
            continue;
        /* Make sure specified quantization table is present */
        qtblno = compptr->i2_quant_tbl_no;

        if (qtblno < 0 || qtblno >= NUM_QUANT_TBLS)
        {

            //UTIL_Printf("[IMG] Quantization table 0x%02x was not defined\n", qtblno);
            vJdecLog_L("[JPG_SW] Quantization table 0x%02x was not defined\n", qtblno);

            if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vLatch_quant_tables(): Quantization table was not defined.\n\r");
                vJdecLog_L("[JPG_SW] Quantization table 0x%02x was not defined\n", qtblno);
                pt_jpg_obj->ui4_JPGErrorType = 10;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            }
            /* Error handler:
               set qtblno = NUM_QUANT_TBLS, i.e. least used table*/
            qtblno = NUM_QUANT_TBLS;
        }

        ui4_qtblno_DCTSIZE2 = qtblno * DCTSIZE2;

        /* OK, save away the quantization table */
        for (i = 0; i < DCTSIZE2; i++)
        {
            compptr->aui2_quant_table[i] =  *((UINT16*)(pui2_quantval) + (ui4_qtblno_DCTSIZE2) + i);
        }

        compptr->b_quant_table_saved = TRUE;
    }
}

/*-----------------------------------------------------------------------------
 * Name:  vStart_input_pass
 *
 * Description: Initialize the input modules to read a scan of compressed data.
 *              The first call to this is done by jdmaster.c after initializing
 *              the entire decompressor (during jpeg_start_decompress).
 *              Subsequent calls come from i2Consume_markers, below.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vStart_input_pass (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    vPer_scan_setup (prJdecInst, pt_jpg_obj);

    if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
    {
        //UTIL_Printf("[IMG] ERR: vStart_input_pass(): vPer_scan_setup error.\n\r");
        vJdecLog_L("[JPG_SW] ERR: vStart_input_pass(): vPer_scan_setup error.\n\r");
        return;
    }

    vLatch_quant_tables (prJdecInst, pt_jpg_obj);

    if (pt_info->b_progressive_mode == TRUE)
        vStart_pass_phuff_decoder (prJdecInst, pt_jpg_obj);
    else
        vStart_pass_huff_decoder (prJdecInst, pt_jpg_obj);

    pt_jpg_obj->ui2_input_iMCU_row = 0;
    vStart_iMCU_row (prJdecInst, pt_jpg_obj);
    pt_jpg_obj->b_consume_markers = FALSE;
}

