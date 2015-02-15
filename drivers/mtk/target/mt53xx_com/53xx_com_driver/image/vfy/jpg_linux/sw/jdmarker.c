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
 * $RCSfile: jdmarker.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/2 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 67b89514219cd77f253224c24cd184a2 $ Asa
 *
 * Description:
 *         This file contains the Routines to process JPEG markers
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
#include "../jdec_debug.h"
#include "../jdec_com_common.h"
//#include "x_rtos.h"

//#include "sys_config.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define REORDER4(dwValue)  ((dwValue>>24)|((dwValue>>8)&0xFF00)|((dwValue<<8)&0xFF0000)|((dwValue<<24)&0xFF000000))
#define REORDER2(dwValue)  ((dwValue>>8) | ((dwValue<<8)&0xFF00))
#define IFD_TAG_SIZE 12
#define LITTLE_ENDIAN_ID  0x4949
#define BIG_ENDIAN_ID     0x4D4D
#define EXIF_ID_CODE      0x45786966
#define APPN_DATA_LEN 14  /* Must be the largest of the above!! */

#define EXIF_ARTIST_TAG                 (0x013B)
#define EXIF_DATE_TIME_TAG              (0x0132)
#define EXIF_DESCRIPTION_TAG            (0x010E)
#define EXIF_MAKE_TAG                   (0x010F)
#define EXIF_MODEL_TAG                  (0x0110)
#define EXIF_ORIENTATION_TAG            (0x0112)

/* - JPEG marker codes */
typedef enum {
  M_SW_SOF0  = 0xc0,
  M_SW_SOF1  = 0xc1,
  M_SW_SOF2  = 0xc2,
  M_SW_SOF3  = 0xc3,

  M_SW_SOF5  = 0xc5,
  M_SW_SOF6  = 0xc6,
  M_SW_SOF7  = 0xc7,

  M_SW_JPG   = 0xc8,
  M_SW_SOF9  = 0xc9,
  M_SW_SOF10 = 0xca,
  M_SW_SOF11 = 0xcb,

  M_SW_SOF13 = 0xcd,
  M_SW_SOF14 = 0xce,
  M_SW_SOF15 = 0xcf,

  M_SW_DHT   = 0xc4,

  M_SW_DAC   = 0xcc,

  M_SW_RST0  = 0xd0,
  M_SW_RST1  = 0xd1,
  M_SW_RST2  = 0xd2,
  M_SW_RST3  = 0xd3,
  M_SW_RST4  = 0xd4,
  M_SW_RST5  = 0xd5,
  M_SW_RST6  = 0xd6,
  M_SW_RST7  = 0xd7,

  M_SW_SOI   = 0xd8,
  M_SW_EOI   = 0xd9,
  M_SW_SOS   = 0xda,
  M_SW_DQT   = 0xdb,
  M_SW_DNL   = 0xdc,
  M_SW_DRI   = 0xdd,
  M_SW_DHP   = 0xde,
  M_SW_EXP   = 0xdf,

  M_SW_APP0  = 0xe0,
  M_SW_APP1  = 0xe1,
  M_SW_APP2  = 0xe2,
  M_SW_APP3  = 0xe3,
  M_SW_APP4  = 0xe4,
  M_SW_APP5  = 0xe5,
  M_SW_APP6  = 0xe6,
  M_SW_APP7  = 0xe7,
  M_SW_APP8  = 0xe8,
  M_SW_APP9  = 0xe9,
  M_SW_APP10 = 0xea,
  M_SW_APP11 = 0xeb,
  M_SW_APP12 = 0xec,
  M_SW_APP13 = 0xed,
  M_SW_APP14 = 0xee,
  M_SW_APP15 = 0xef,

  M_SW_JPG0  = 0xf0,
  M_SW_JPG13 = 0xfd,
  M_SW_COM   = 0xfe,

  M_SW_TEM   = 0x01,

  M_SW_ERROR = 0x100
} JPEG_MARKER;

/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
extern const INT16 jdec_natural_order[DCTSIZE2+16];
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
VOID vSet_orientation(UINT32 ui4_orientation, IMG_ROTATE_T *pe_rotation);
VOID vCopy_exif_string(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T *pt_jpg_obj, UINT32 ui4_count, UINT32 ui4_offset, UINT32 ui4_tiff_start_pos, CHAR **pps_out);

extern UINT32 u4Input4bytes (JDEC_INST_T *prJdecInst);
extern UINT16 u2Input2bytes (JDEC_INST_T *prJdecInst);
extern UINT8 u1Input1byte (JDEC_INST_T *prJdecInst);
extern BOOL fgSkipbytes (JDEC_INST_T *prJdecInst, UINT32 ui4_num_bytes);
extern INT32 i4JdecSwUpdateFifoRdPoint(JDEC_INST_T *prJdecInst);


#define INPUT_BYTE(obj, out)    out = u1Input1byte(obj);
#define INPUT_2BYTES(obj, out)  out = u2Input2bytes(obj);
#define INPUT_4BYTES(obj, out)  out = u4Input4bytes(obj);
#define SKIP_BYTES(obj, num)    fgSkipbytes(obj, num);

/*-----------------------------------------------------------------------------
 * Name:  vSet_InputPos
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
VOID vSet_InputPos (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 ui4_pos)
{
#if 0 //[CJYang]
    setpos_fct fp_setpos_fct = NULL;

    if (ui4_pos < pt_jpg_obj->ui4_JPGFileSizeInByte)
    {
        fp_setpos_fct = pt_jpg_obj->t_input.fp_setpos_fct;
        if (fp_setpos_fct)
        {
            fp_setpos_fct(pt_jpg_obj, ui4_pos);
            pt_jpg_obj->ui4_JPGReadByteNum = ui4_pos;
            return;
        }
    }
    else
    {
        pt_jpg_obj->b_ReachFileEnd = TRUE;
    }
#else
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    if (ui4_pos < (prFifoInfo->u4FileOfst - 64))
    {
        //re-transfer
        //ASSERT(0);  //[CJYang]
    }
    else if (ui4_pos <= prFifoInfo->u4FileOfst)
    {
        //move read point
        UINT32 u4Shift = prFifoInfo->u4FileOfst - ui4_pos;
        prFifoInfo->u4FileOfst = ui4_pos;
        prFifoInfo->u4RdPoint -= u4Shift;
        if (prFifoInfo->u4RdPoint < prFifoInfo->u4FifoStart)
        {
            prFifoInfo->u4RdPoint = prFifoInfo->u4FifoEnd - (prFifoInfo->u4FifoStart - prFifoInfo->u4RdPoint);
        }
    }
    else if (ui4_pos > prFifoInfo->u4FileOfst)
    {
        UINT32 u4Shift = ui4_pos - prFifoInfo->u4FileOfst;
        SKIP_BYTES(prJdecInst, u4Shift);
    }
    i4JdecSwUpdateFifoRdPoint(prJdecInst);
#endif
    return;
}
/*-----------------------------------------------------------------------------
 * Name:  u4Get_InputPos
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT32 u4Get_InputPos (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
#if 0 //[CJYang]
    UINT32 ui4_ret = 0;
    getpos_fct fp_getpos_fct = 0;

    if (pt_jpg_obj->ui4_JPGReadByteNum < pt_jpg_obj->ui4_JPGFileSizeInByte)
    {
        fp_getpos_fct = pt_jpg_obj->t_input.fp_getpos_fct;
        if (fp_getpos_fct)
        {
            fp_getpos_fct(pt_jpg_obj, &ui4_ret);
            return ui4_ret;
        }
    }
    else
    {
        pt_jpg_obj->b_ReachFileEnd = TRUE;
    }
#else
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    return prFifoInfo->u4FileOfst;
#endif
}
/*-----------------------------------------------------------------------------
 * Name:  vSkip_input_data
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vSkip_input_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 num_bytes)
{
#if 0 //[CJYang]
    skipbytes_fct fp_skipbytes_fct = 0;

    if (pt_jpg_obj->ui4_JPGReadByteNum < pt_jpg_obj->ui4_JPGFileSizeInByte)
    {
        fp_skipbytes_fct = pt_jpg_obj->t_input.fp_skipbytes_fct;
        if (fp_skipbytes_fct)
        {
            if (fp_skipbytes_fct(pt_jpg_obj, num_bytes) == TRUE)
            {
                pt_jpg_obj->ui4_JPGReadByteNum += num_bytes;
                return;
            }
        }
    }
    else
    {
        pt_jpg_obj->b_ReachFileEnd = TRUE;
    }
#endif
    SKIP_BYTES(prJdecInst, num_bytes);
    return;
}


/*
 * This is the default resync_to_restart method for data source managers
 * to use if they don't have any better approach.  Some data source managers
 * may be able to back up, or may have additional knowledge about the data
 * which permits a more intelligent recovery strategy; such managers would
 * presumably supply their own resync method.
 *
 * vRead_restart_marker calls resync_to_restart if it finds a marker other than
 * the restart marker it was expecting.  (This code is *not* used unless
 * a nonzero restart interval has been declared.)  cinfo->pt_info->unread_marker is
 * the marker code actually found (might be anything, except 0 or FF).
 * The desired restart marker number (0..7) is passed as a parameter.
 * This routine is supposed to apply whatever error recovery strategy seems
 * appropriate in order to position the input stream to the next data segment.
 * Note that cinfo->pt_info->unread_marker is treated as a marker appearing before
 * the current data-source input point; usually it should be reset to zero
 * before returning.
 * Returns FALSE if suspension is required.
 *
 * This implementation is substantially constrained by wanting to treat the
 * input as a data stream; this means we can't back up.  Therefore, we have
 * only the following actions to work with:
 *   1. Simply discard the marker and let the entropy decoder resume at next
 *      byte of file.
 *   2. Read forward until we find another marker, discarding intervening
 *      data.  (In theory we could look ahead within the current bufferload,
 *      without having to discard data if we don't find the desired marker.
 *      This idea is not implemented here, in part because it makes behavior
 *      dependent on buffer size and chance buffer-boundary positions.)
 *   3. Leave the marker unread (by failing to zero cinfo->pt_info->unread_marker).
 *      This will cause the entropy decoder to process an empty data segment,
 *      inserting dummy zeroes, and then we will reprocess the marker.
 *
 * #2 is appropriate if we think the desired marker lies ahead, while #3 is
 * appropriate if the found marker is a future restart marker (indicating
 * that we have missed the desired restart marker, probably because it got
 * corrupted).
 * We apply #2 or #3 if the found marker is a restart marker no more than
 * two counts behind or ahead of the expected one.  We also apply #2 if the
 * found marker is not a legal JPEG marker code (it's certainly bogus data).
 * If the found marker is a restart marker more than 2 counts away, we do #1
 * (too much risk that the marker is erroneous; with luck we will be able to
 * resync at some future point).
 * For any valid non-restart JPEG marker, we apply #3.  This keeps us from
 * overrunning the end of a scan.  An implementation limited to single-scan
 * files might find it better to apply #2 for markers other than EOI, since
 * any other marker would have to be bogus data in that case.
 */
/*-----------------------------------------------------------------------------
 * Name:  vJpeg_resync_to_restart
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vJpeg_resync_to_restart (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    register INT16 marker;
    INT16 action = 1;
    register UINT32 i;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 ui4_next_restart_num;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    marker = pt_jpg_obj->i2_unread_marker;
    /* Always put up a warning. */
    vJdecLog_L("[JPG_SW] Corrupt JPEG data: found marker 0x%02x instead of RST%d\n", marker, pt_info->ui1_next_restart_num);

    if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
    {
        vJdecLog_M("[JPG_SW] ERR: vJpeg_resync_to_restart(): Corrupt JPEG data: found marker 0x%02x instead of RST%d\n", marker, pt_info->ui1_next_restart_num);
        pt_jpg_obj->ui4_JPGErrorType = 11;
        pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
    }
    /* Error handler:
       As follows*/

    /* Outer loop handles repeated decision after scanning forward. */
    for (i = 0; i < BIG_LOOP; i++)
    {
        if (marker < (INT16) M_SW_SOF0)
            action = 2;   /* invalid marker */
        else if (marker < (INT16) M_SW_RST0 || marker > (INT16) M_SW_RST7)
            action = 3;   /* valid non-restart marker */
        else
        {
            ui4_next_restart_num = pt_info->ui1_next_restart_num;

            if (marker == (INT16)( M_SW_RST0 + ((ui4_next_restart_num+1) & 7)) ||
                marker == (INT16)( M_SW_RST0 + ((ui4_next_restart_num+2) & 7)))
                action = 3;   /* one of the next two expected restarts */
            else if (marker == (INT16)( M_SW_RST0 + ((ui4_next_restart_num-1) & 7)) ||
                marker == (INT16)( M_SW_RST0 + ((ui4_next_restart_num-2) & 7)))
                action = 2;   /* a prior restart, so advance */
            else
                action = 1;   /* pt_info->ui1_next_restart_num restart or too far away */
        }

        vJdecLog_L("[IMG] At marker 0x%02x, recovery action %d \n", marker, action);

        switch (action)
        {
        case 1:
          /* Discard marker and let entropy decoder resume processing. */
          pt_jpg_obj->i2_unread_marker = 0;
          break;
        case 2:
            /* Scan to the next marker, and repeat the decision loop. */
            vNext_marker (prJdecInst, pt_jpg_obj);
            marker = pt_jpg_obj->i2_unread_marker;
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                vJdecLog_M("[JPG_SW] ERR: vJpeg_resync_to_restart(): Scan to the next marker, and repeat the decision loop. \n\r");
                return;
            }
            break;
        case 3:
            /* Return without advancing past this marker. */
            /* Entropy decoder will be forced to process an empty segment. */
            break;
        }
    } /* end loop */
}

/*
 * Routines to process JPEG markers.
 *
 * Entry condition: JPEG marker itself has been read and its code saved
 *   in cinfo->pt_info->unread_marker; input restart point is just after the marker.
 *
 * Exit: if return TRUE, have read and processed any parameters, and have
 *   updated the restart point to point after the parameters.
 *   If return FALSE, was forced to suspend before reaching end of
 *   marker parameters; restart point has not been moved.  Same routine
 *   will be called again after application supplies more input data.
 *
 * This approach to suspension assumes that all of a marker's parameters
 * can fit into a single input bufferload.  This should hold for "normal"
 * markers.  Some COM/APPn markers might have large parameter segments
 * that might not fit.  If we are simply dropping such a marker, we use
 * vSkip_input_data to get past it, and thereby put the problem on the
 * source manager's shoulders.  If we are saving the marker's contents
 * into memory, we use a slightly different convention: when forced to
 * suspend, the marker processor updates the restart point to the end of
 * what it's consumed (ie, the end of the buffer) before returning FALSE.
 * On resumption, cinfo->pt_info->unread_marker still contains the marker code,
 * but the data source will point to the next chunk of marker data.
 * The marker processor must retain internal state to deal with this.
 *
 * Note that we don't bother to avoid duplicate trace messages if a
 * suspension occurs within marker parameters.  Other side effects
 * require more care.
 */

/*-----------------------------------------------------------------------------
 * Name:  vGet_soi
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_soi (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    if (pt_jpg_obj->b_saw_SOI)
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_soi(): Invalid JPEG file structure: two SOI markers. \n\r");
        /* - Invalid JPEG file structure: two SOI markers.*/
        pt_jpg_obj->ui4_JPGErrorType = 12;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: Immediately return to caller*/
        return;
    }
    pt_jpg_obj->b_saw_SOI = TRUE;
}

/*-----------------------------------------------------------------------------
 * Name:  vGet_sof
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_sof (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT16 c, ci;
    COMPONENT_INFO_T * compptr;
    INT16 length;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    INPUT_2BYTES(prJdecInst, length);
    INPUT_BYTE(prJdecInst, pt_jpg_obj->ui1_data_precision);
    INPUT_2BYTES(prJdecInst, pt_info->ui2_image_height);
    INPUT_2BYTES(prJdecInst, pt_info->ui2_image_width);
    INPUT_BYTE(prJdecInst, pt_info->ui2_num_components);

    length -= 8;

    if (pt_info->b_saw_SOF)
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sof(): Invalid JPEG file structure: two SOF markers \n\r");
        /* - Invalid JPEG file structure: two SOF markers*/
        pt_jpg_obj->ui4_JPGErrorType = 13;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: immediately return to caller*/
        return;
    }

    if ((pt_info->ui2_num_components != 0x1) &&
        (pt_info->ui2_num_components != 0x3) &&
        (pt_info->ui2_num_components != 0x4)) // [Kate20080611] CQ BDP00112427 and CQ BDP00112423
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sof(): not support %d color component \n\r", pt_info->ui2_num_components);
        /* - Invalid JPEG file structure: two SOF markers*/
        pt_jpg_obj->ui4_JPGErrorType = 13;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: immediately return to caller*/
        return;
    }

    if (pt_info->ui2_num_components > 0x3)
    {
        prJdecInst->i4JdecErrorType = -E_JPG_ERR_INVALID_SOF;
    }

    /* We don't support files in which the image height is initially specified */
    /* as 0 and is later redefined by DNL.  As long as we have to check that,  */
    /* might as well have a general sanity check. */
    if (pt_info->ui2_image_height == 0 || pt_info->ui2_image_width == 0 || pt_info->ui2_num_components == 0)
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sof(): Empty JPEG image (DNL not supported) \n\r");
        /* - Empty JPEG image (DNL not supported)*/
        pt_jpg_obj->ui4_JPGErrorType = 14;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: immediately return to caller*/
        return;
    }

    if (length != ((INT16)pt_info->ui2_num_components * 3))
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sof(): Empty JPEG image (DNL not supported) \n\r");
        /* - Empty JPEG image (DNL not supported).*/
        pt_jpg_obj->ui4_JPGErrorType = 14;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: immediately return to caller*/
        return;
    }

    for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
    {
        compptr->i2_component_index = ci;
        INPUT_BYTE(prJdecInst, compptr->i2_component_id);
        INPUT_BYTE(prJdecInst, c);
        compptr->i2_h_samp_factor = (c >> 4) & 15;
        compptr->i2_v_samp_factor = (c     ) & 15;
        INPUT_BYTE(prJdecInst, compptr->i2_quant_tbl_no);
    }
    pt_info->b_saw_SOF = TRUE;
}

/*-----------------------------------------------------------------------------
 * Name:  vGet_sos
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_sos (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT32 length;
    register INT16 i, ci, n, c, cc;
    COMPONENT_INFO_T * compptr;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    if (pt_info->b_saw_SOF == FALSE)
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sos(): pt_info->b_saw_SOF == FALSE \n\r");
        pt_jpg_obj->ui4_JPGErrorType = 15;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        return;
    }

    INPUT_2BYTES(prJdecInst, length);
    INPUT_BYTE(prJdecInst, n); /* Number of components */


    if (length != (n * 2 + 6))
    {
        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vGet_sos(): pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 16;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            return;
        }
    }

    if (n < 1 || n > MAX_COMPS_IN_SCAN)
    {
        vJdecLog_M("[JPG_SW] ERR: vGet_sos(): n < 1 || n > MAX_COMPS_IN_SCAN \n\r");
        pt_jpg_obj->ui4_JPGErrorType = 8;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        return;
    }

    pt_info->ui1_comps_in_scan = (UINT8) n;

    /* Collect the component-spec parameters */
    for (i = 0; i < n; i++)
    {
        INPUT_BYTE(prJdecInst, cc);
        INPUT_BYTE(prJdecInst, c);

        for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
        {
            if (cc == compptr->i2_component_id)
                goto id_found;
        }
        vJdecLog_M("[JPG_SW] ERR: vGet_sos(): Collect the component-spec parameters \n\r");
        pt_jpg_obj->ui4_JPGErrorType = 17;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        return;

id_found:

        pt_info->apt_cur_comp_info[i] = compptr;
        compptr->i2_dc_tbl_no = (c >> 4) & 15;
        compptr->i2_ac_tbl_no = (c     ) & 15;
    }

    /* Collect the additional scan parameters pt_info->Ss, pt_info->ui1_Se, pt_info->ui1_Ah/pt_info->Al. */
    INPUT_BYTE(prJdecInst, c);
    pt_info->ui1_Ss = (UINT8) c;
    INPUT_BYTE(prJdecInst, c);
    pt_info->ui1_Se = (UINT8) c;
    INPUT_BYTE(prJdecInst, c);

    pt_info->ui1_Ah = (c >> 4) & 15;
    pt_info->ui1_Al = (c     ) & 15;

    /* Prepare to scan data & restart markers */
    pt_info->ui1_next_restart_num = 0;

    /* Count another SOS marker */
    (pt_info->ui1_input_scan_number)++;
}

/*-----------------------------------------------------------------------------
 * Name:  vGet_dht
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_dht (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT32 length;
    UINT8 bits[17];
    UINT8 huffval[256];
    UINT16 i, count;
    INT16 index;
    register HUFF_TBL_T *htblptr;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    x_memset((void *)huffval, 0, 256);
    INPUT_2BYTES(prJdecInst, length);

    length -= 2;

    while (length > 16)  /* safe while*/
    {
        INPUT_BYTE(prJdecInst, index);

        vJdecLog_L("[JPG_SW] Define Huffman Table 0x%02x \n", index);

        bits[0] = 0;
        count = 0;
        for (i = 1; i <= 16; i++)
        {
            INPUT_BYTE(prJdecInst, bits[i]);
            count += bits[i];
        }

        length -= 1 + 16;

        vJdecLog_L("[JPG_SW] \n        %3d %3d %3d %3d %3d %3d %3d %3d\n",
           bits[1], bits[2], bits[3], bits[4],
           bits[5], bits[6], bits[7], bits[8]);
        vJdecLog_L("[JPG_SW] \n        %3d %3d %3d %3d %3d %3d %3d %3d\n",
           bits[9], bits[10], bits[11], bits[12],
           bits[13], bits[14], bits[15], bits[16]);

        /* Here we just do minimal validation of the counts to avoid walking
        * off the end of our table space.  jdhuff.c will check more carefully.
        */
        if (count > 256 || ((INT32) count) > length)
        {

            vJdecLog_L("[JPG_SW] Bogus Huffman table definition\n");

            if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
            {
                vJdecLog_L("[JPG_SW] ERR: vGet_dht(): Bogus Huffman table definition \n\r");
                pt_jpg_obj->ui4_JPGErrorType = 3;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            }
            /* Error handler: set to nearest value*/
            if (count > 256)
                count = 256;
            else /* ((INT32) count) > length) */
                count = (UINT16)length;
        }

        for (i = 0; i < count; i++)
        {
            if (i > 255)
            {
                break;
                // ASSERT(0); ??
            }
            INPUT_BYTE(prJdecInst, huffval[i]);
        }

        length -= count;

        if (index & 0x10)
        {   /* AC table definition */
            index -= 0x10;
            // For klocwork issues
            if ((index < 0) || (index > (NUM_HUFF_TBLS - 1)))
            {
                //ASSERT(0);
			    vJdecLog_M("[JPG_SW] ERR: vGet_dht(): index error0 \n\r");
                pt_jpg_obj->ui4_JPGErrorType = 16;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
				return;
            }
            htblptr = &pt_info->at_ac_huff_tbl[index];
        }
        else
        {   /* DC table definition */
            // For klocwork issues
            if ((index < 0) || (index > (NUM_HUFF_TBLS - 1)))
            {
                vJdecLog_M("[JPG_SW] ERR: vGet_dht(): index error1 \n\r");
                pt_jpg_obj->ui4_JPGErrorType = 16;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
				return;;
            }
            htblptr = &pt_info->at_dc_huff_tbl[index];
        }

        if (index < 0 || index >= NUM_HUFF_TBLS)
        {

            vJdecLog_L("[JPG_SW] Bogus DHT index %d \n", index);

            vJdecLog_L("[JPG_SW] ERR: vGet_dht(): Bogus DHT index \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 18;
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            /* Error handler: immediately return to caller*/
            return;
        }

        for (i = 0; i < 17; i++)
            (*htblptr).aui1_bits[i] = bits[i];

        for (i = 0; i < 256; i++)
            (*htblptr).aui1_huffval[i] = huffval[i];
    }

    if (length != 0)
    {

        vJdecLog_L("[JPG_SW] Bogus marker length \n");

        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vGet_dht(): Bogus marker length \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 16;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
        }
    }
}

/*-----------------------------------------------------------------------------
 * Name:  vGet_dqt
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_dqt (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT32 length;
    INT16 n, i, prec;
    UINT16 tmp;
    register UINT16 *quant_ptr;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    INPUT_2BYTES(prJdecInst, length);
    length -= 2;

    while (length > 0) /* safe while*/
    {
        INPUT_BYTE(prJdecInst, n);

        prec = n >> 4;
        n &= 0x0F;

        vJdecLog_L("[JPG_SW] Define Quantization Table %d  precision %d \n", n, prec);

        if (n >= NUM_QUANT_TBLS)
        {
            vJdecLog_L("[JPG_SW] Bogus DQT index %d \n", n);

            vJdecLog_M("[JPG_SW] ERR: vGet_dqt(): Bogus DQT index \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 20;
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            /* Error handler:
               immediately return to caller*/
            return;
        }

        quant_ptr = (UINT16*)(pt_info->aaui2_quantval)+(n*DCTSIZE2);


        for (i = 0; i < DCTSIZE2; i++)
        {
            if (prec)
            {
                INPUT_2BYTES(prJdecInst, tmp);
            }
            else
            {
                INPUT_BYTE(prJdecInst, tmp);
            }
            /* We convert the zigzag-order table to natural array order. */
            (quant_ptr[jdec_natural_order[i]]) = (UINT16) tmp;
        }


        for (i = 0; i < DCTSIZE2; i += 8)
        {
            vJdecLog_L("[JPG_SW] \n        %4u %4u %4u %4u %4u %4u %4u %4u",
            quant_ptr[i],   quant_ptr[i+1],
            quant_ptr[i+2], quant_ptr[i+3],
            quant_ptr[i+4], quant_ptr[i+5],
            quant_ptr[i+6], quant_ptr[i+7]);
        }

        length -= DCTSIZE2+1;
        if (prec) length -= DCTSIZE2;
    }

    if (length != 0)
    {
        vJdecLog_L("[JPG_SW] Bogus marker length \n");

        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vGet_dqt(): Bogus marker length \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 16;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
        }
        /* Error handler:
           NONE*/
    }
}

/*-----------------------------------------------------------------------------
 * Name:  vGet_dri
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vGet_dri (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT32 length;
    UINT16 tmp;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    INPUT_2BYTES(prJdecInst, length);

    if (length != 4)
    {
        vJdecLog_L("[JPG_SW] Bogus marker length \n");

        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vGet_dri(): Bogus marker length \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 16;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
        }
        /* Error handler: NONE*/
    }

    INPUT_2BYTES(prJdecInst, tmp);

    vJdecLog_L("[JPG_SW] Define Restart Interval %u \n",tmp);

    pt_info->ui2_restart_interval = tmp;
}

//#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
//extern void* x_alloc_aligned_dma_mem(UINT32 u4Size, UINT32 u4Align);
//extern void* x_free_aligned_dma_mem(void * pUser);
//#else
//extern void* x_alloc_aligned_nc_mem(UINT32 u4Size, UINT32 u4Align);
//extern void* x_free_aligned_nc_mem(void * pUser);
//#endif


VOID vCopy_exif_string(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T *pt_jpg_obj, UINT32 ui4_count, UINT32 ui4_offset, UINT32 ui4_tiff_start_pos, CHAR **pps_out)
{
    IMG_BASIC_INFO_T    *pt_info;
    UINT32              ui4_i;
    UINT32              ui4_current_pos;

    pt_info = pt_jpg_obj->pt_current_sel;
    
    if (!pt_info->b_big_endian)
    {
        ui4_offset = REORDER4(ui4_offset);
    }
    
    /* push current position */
    ui4_current_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj);

    /* move to the string beginning position */
    if (ui4_count <= 4)
    {
        vSet_InputPos(prJdecInst, pt_jpg_obj, ui4_current_pos - 4);
    }
    else
    {
        vSet_InputPos(prJdecInst, pt_jpg_obj, ui4_tiff_start_pos + ui4_offset);
    }

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    //*pps_out = (INT8 *)x_alloc_aligned_dma_mem((ui4_count + 1) * sizeof(CHAR), 32);
#if CONFIG_SYS_MEM_PHASE2
    *pps_out = (INT8 *)x_alloc_vmem((ui4_count + 1) * sizeof(CHAR));
#elif CONFIG_SYS_MEM_PHASE3
    *pps_out = (INT8 *)x_mem_valloc((ui4_count + 1) * sizeof(CHAR));
#endif
    #else
    *pps_out = (CHAR *)x_alloc_aligned_nc_mem((ui4_count + 1) * sizeof(CHAR), 32);
    #endif
    

    if (*pps_out == NULL)
    {
        vJdecLog_M("{IMG JPG} memory allocation for storing EXIF information failed.\n");

        return;
    }

    /* copy */
    for (ui4_i = 0; ui4_i < ui4_count; ui4_i++)
    {
        INPUT_BYTE(prJdecInst, (*pps_out)[ui4_i]);
    }

    (*pps_out)[ui4_i] = '\0';

    /* pop position */
    vSet_InputPos(prJdecInst, pt_jpg_obj, ui4_current_pos);
}

VOID vSet_orientation(UINT32 ui4_orientation, IMG_ROTATE_T *pe_rotation)
{
#if 0	//[CJYang]
    switch (ui4_orientation)
    {
        case EXIF_ORIENTATION_CW_90:
            *pe_rotation = IMG_ROTATE_CLOCKWS_90_OP_TYPE;
            break;

        case EXIF_ORIENTATION_CW_90_FLIP:
            *pe_rotation = IMG_ROTATE_CLOCKWS_90_OP_TYPE_WITH_FLIP;
            break;

        case EXIF_ORIENTATION_CW_180:
            *pe_rotation = IMG_ROTATE_CLOCKWS_180_OP_TYPE;
            break;

        case EXIF_ORIENTATION_CW_180_FLIP:
            *pe_rotation = IMG_ROTATE_CLOCKWS_180_OP_TYPE_WITH_FLIP;
            break;

        case EXIF_ORIENTATION_CW_270:
            *pe_rotation = IMG_ROTATE_CLOCKWS_270_OP_TYPE;
            break;

        case EXIF_ORIENTATION_CW_270_FLIP:
            *pe_rotation = IMG_ROTATE_CLOCKWS_270_OP_TYPE_WITH_FLIP;
            break;

        case EXIF_ORIENTATION_NONE_FLIP:
            *pe_rotation = IMG_ROTATE_CLOCKWS_0_OP_TYPE_WITH_FLIP;
            break;

        default:
            *pe_rotation = IMG_ROTATE_CLOCKWS_0_OP_TYPE;
    }
#endif
}

/*-----------------------------------------------------------------------------
 * Name:  fgJump2Thumbnail
 *
 * Description: When Digest mode, to decode the thumbnail instead of primary picture.
 *              Try to search the SOI of thumbnail.
 *
 * Inputs:  none
 *
 * Outputs: -
 *
 * Returns: TRUE if find out the SOI of Thumbnail
 *          otherwise, FALSE
 ----------------------------------------------------------------------------*/
BOOL fgJump2Thumbnail(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    UINT32 i;
    UINT32 dwAPP1length;           /* - After parsing useful data, the remain length of APP1.*/
    UINT32 dw0thIFDOfst;
    UINT32 dwTIFFOfst;             /* - After parsing useful data, the remain length of TIFF.*/
    UINT16 wIDFFieldCnt;
    UINT32 ui4_tmp;
    /*UINT32 dw0thIDFLength;*/
    UINT32 dwNextIDFOfst;
    UINT16 wTag;
    UINT16 wType;
    volatile UINT32 dwCount;
    UINT32 dwValue;
    UINT32 dwThumbnailOfst;
    UINT32 unused_32;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32  ui4_tiff_start_pos;
	char *pcExifStr = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return JPEG_RETURN_ERR;
    }
    /* - Get APP1 length.*/
    INPUT_2BYTES(prJdecInst, dwAPP1length);
    dwAPP1length -= 2;

    /* store thumbnail compressed data end pos*/
    pt_info->ui4_compressed_data_end_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj) + dwAPP1length;
    /* - Check Exif Identifier code. (6Bytes)*/
    INPUT_4BYTES(prJdecInst, unused_32);
    dwAPP1length -= 4;
    if(unused_32 != EXIF_ID_CODE)
    {
        vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
        return(FALSE);
    }
    /* - 2-Byte zero padding.*/
    INPUT_2BYTES(prJdecInst, unused_32);
    dwAPP1length -= 2;
    if(unused_32 != 0x0)
    {
        vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
        return(FALSE);
    }
    /* - TIFF*/
    dwTIFFOfst = 0;

    ui4_tiff_start_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj);

    /* - Big Endian or Little Endian*/
    INPUT_2BYTES(prJdecInst, unused_32);

    pt_info->b_big_endian = (unused_32 == BIG_ENDIAN_ID) ? TRUE : FALSE;

    /* - Skip to Check {0x00, 0x2A}*/
    INPUT_2BYTES(prJdecInst, unused_32);

    /* - Get 0th IFD offset.*/
    INPUT_4BYTES(prJdecInst, dw0thIFDOfst);
    dwAPP1length -= 8;
    dwTIFFOfst += 8;

    dw0thIFDOfst = (pt_info->b_big_endian) ? dw0thIFDOfst : REORDER4(dw0thIFDOfst);

    /* - Jump to 0th IFD and Skip 0th IFD.*/
    ui4_tmp = dw0thIFDOfst - dwTIFFOfst;
    SKIP_BYTES(prJdecInst, ui4_tmp);
    dwAPP1length -= (ui4_tmp);
    dwTIFFOfst += (ui4_tmp);

    INPUT_2BYTES(prJdecInst, wIDFFieldCnt);
    dwAPP1length -= 2;
    dwTIFFOfst += 2;

    wIDFFieldCnt = pt_info->b_big_endian ? wIDFFieldCnt : REORDER2(wIDFFieldCnt);

    /* get EXIF IFD0 data */
    for (i = 0; i < wIDFFieldCnt; i++)
    {
        UINT16  ui2_tag;
        UINT16  ui2_type;
        UINT32  ui4_count;
        UINT32  ui4_offset;

        /* get tag */
        INPUT_2BYTES(prJdecInst, ui2_tag);

        if (!pt_info->b_big_endian)
        {
            ui2_tag = REORDER2(ui2_tag);
        }

        /* get type */
        INPUT_2BYTES(prJdecInst, ui2_type);

        if (!pt_info->b_big_endian);
        {
            ui2_type = REORDER2(ui2_type);
        }

        /* get count */
        INPUT_4BYTES(prJdecInst, ui4_count);

        if (!pt_info->b_big_endian)
        {
            ui4_count = REORDER4(ui4_count);
        }
        
        /* get offset */
        INPUT_4BYTES(prJdecInst, ui4_offset);

        dwAPP1length -= 12;
        dwTIFFOfst   += 12;

        switch (ui2_tag)
        {
            case EXIF_ARTIST_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    ui4_count,
                    ui4_offset,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.ps_artist);
                break;

            case EXIF_DATE_TIME_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    ui4_count,
                    ui4_offset,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.ps_date_time);
                break;

            case EXIF_DESCRIPTION_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    ui4_count,
                    ui4_offset,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.ps_description);
                break;

            case EXIF_MAKE_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    ui4_count,
                    ui4_offset,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.ps_make);
                break;

            case EXIF_MODEL_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    ui4_count,
                    ui4_offset,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.ps_model);
                break;

            case EXIF_ORIENTATION_TAG:
                pt_jpg_obj->t_primary.t_basic.b_orientation_tag_exist = TRUE;
                pt_jpg_obj->t_primary.t_basic.ui4_orientation_tag_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj) - 4;
                ui4_offset = u4BYTERearrange(prJdecInst, pt_jpg_obj, ui2_type, ui4_offset);
				//[CJYang]
                //vSet_orientation(ui4_offset, &pcExifStr);//&pt_jpg_obj->t_primary.t_basic.t_exif_info.e_orientation);
                break;

            /* ignore all other IFD0 tags */
            default:
                /* do nothing */
                ui2_tag = ui2_tag;
        }

#if CONFIG_DRV_LINUX_DATA_CONSISTENCY       
        if (pcExifStr != NULL)
            //x_free_aligned_dma_mem((void *)pcExifStr);
#if CONFIG_SYS_MEM_PHASE2
            x_free_vmem((void *)pcExifStr);
#elif CONFIG_SYS_MEM_PHASE3
            x_mem_vfree((void *)pcExifStr);
#endif
#else
        if (pcExifStr != NULL)
            x_free_aligned_nc_mem((void *)pcExifStr);
#endif
    }

    if (wIDFFieldCnt > 0)
    {
        pt_jpg_obj->t_primary.t_basic.b_exif_exist = TRUE;
    }

    /*
    dw0thIDFLength = wIDFFieldCnt * IFD_TAG_SIZE;
    SKIP_BYTES(prJdecInst, dw0thIDFLength);

    dwAPP1length -= dw0thIDFLength;
    dwTIFFOfst += dw0thIDFLength;
    */

    INPUT_4BYTES(prJdecInst, dwNextIDFOfst);
    dwAPP1length -= 4;
    dwTIFFOfst += 4;

    dwNextIDFOfst = (pt_info->b_big_endian) ? dwNextIDFOfst : REORDER4(dwNextIDFOfst);

    /* - To avoid the 0th IFD offset is larger than 1th IFD offset.*/
    if (dwTIFFOfst > dwNextIDFOfst)
    {
        vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
        return FALSE;
    }

    /* - To avoid the 1th IFD offset is over remain APP1 data size and thus make dwAPP1length -(dwNextIDFOfst - dwTIFFOfst) underflow.*/
    // [Kate20080701] BDP00112439
    if ((dwNextIDFOfst - dwTIFFOfst) > dwAPP1length)
    {
        vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
        return FALSE;
    }

    /* - Jump to 1st IFD and search "JPEGInterchangeFormatLength" Tag : 0x0201*/
    ui4_tmp = dwNextIDFOfst - dwTIFFOfst;
    SKIP_BYTES(prJdecInst, ui4_tmp);
    dwAPP1length -= (ui4_tmp);
    dwTIFFOfst += (ui4_tmp);

    INPUT_2BYTES(prJdecInst, wIDFFieldCnt);

    dwAPP1length -= 2;
    dwTIFFOfst += 2;
    if(!pt_info->b_big_endian)
    {
        wIDFFieldCnt = (wIDFFieldCnt>>8) | ((wIDFFieldCnt<<8)&0xFF00);
    }

    dwThumbnailOfst = 0x55AA55AA;
    for(i=0;i<wIDFFieldCnt;i++)
    {
        // BDP00214801; not to reach file end if dwAPP1length isn't enough
        if (dwAPP1length < 12)
        {
            vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
            return FALSE;
        }
        INPUT_2BYTES(prJdecInst, wTag);
        INPUT_2BYTES(prJdecInst, wType);
        INPUT_4BYTES(prJdecInst, dwCount);
        INPUT_4BYTES(prJdecInst, dwValue);

        dwAPP1length -= 12;
        dwTIFFOfst += 12;

        wTag    = pt_info->b_big_endian ? wTag   : REORDER2(wTag);
        wType   = pt_info->b_big_endian ? wType  : REORDER2(wType);
        dwCount = pt_info->b_big_endian ? dwCount: REORDER4(dwCount);
        
        if(wTag == 0x0201)
        {
            dwThumbnailOfst = u4BYTERearrange(prJdecInst, pt_jpg_obj, wType, dwValue);
        }

        switch (wTag)
        {
            case EXIF_ARTIST_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    dwCount,
                    dwValue,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.ps_artist);
                break;

            case EXIF_DATE_TIME_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    dwCount,
                    dwValue,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.ps_date_time);
                break;

            case EXIF_DESCRIPTION_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    dwCount,
                    dwValue,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.ps_description);
                break;

            case EXIF_MAKE_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    dwCount,
                    dwValue,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.ps_make);
                break;

            case EXIF_MODEL_TAG:
                vCopy_exif_string(
                    prJdecInst,
                    pt_jpg_obj,
                    dwCount,
                    dwValue,
                    ui4_tiff_start_pos,
                    &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.ps_model);
                break;

            case EXIF_ORIENTATION_TAG:
                pt_jpg_obj->t_thumbnail.t_basic.b_orientation_tag_exist = TRUE;
                pt_jpg_obj->t_thumbnail.t_basic.ui4_orientation_tag_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj) - 4;
                dwValue = u4BYTERearrange(prJdecInst, pt_jpg_obj, wType, dwValue);
				//[CJYang]
                //vSet_orientation(dwValue, &pcExifStr);//&pt_jpg_obj->t_thumbnail.t_basic.t_exif_info.e_orientation);
                break;

            /* ignore all other IFD1 tags */
            default:
                /* do nothing */
                wTag = wTag;
        }
    }

    if (wIDFFieldCnt > 0)
    {
        pt_jpg_obj->t_thumbnail.t_basic.b_exif_exist = TRUE;
    }

    ui4_tmp = dwThumbnailOfst - dwTIFFOfst;

    /* - If APP1 exist, but Thumbnail does not exist.*/
    if ((dwThumbnailOfst == 0x55AA55AA)
        || (dwAPP1length < (ui4_tmp + 2)))
    {
        vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
        return(FALSE);
    }
    SKIP_BYTES(prJdecInst, ui4_tmp);
    dwAPP1length -= (ui4_tmp);
    dwTIFFOfst += (ui4_tmp);

    INPUT_2BYTES(prJdecInst, unused_32);
    dwAPP1length -= 2;

    /* - Check if it is SOI of Thumbnail.*/
    if(unused_32 == 0xFFD8)
    {
        pt_jpg_obj->pt_current_sel->ui4_pm_data_begin_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj) - 2;
            
        return TRUE;
    }

    vSkip_input_data(prJdecInst, pt_jpg_obj, dwAPP1length);
    return(FALSE);
}

/*-----------------------------------------------------------------------------
 * Name:  vAPP1_Handle2
 *
 * Description: To handle APP1 data
 *
 * Inputs:  none
 *
 * Outputs: none
 *
 * Returns: none
 ----------------------------------------------------------------------------*/
void vAPP1_Handle2(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    if (pt_jpg_obj->b_APP1Exist == FALSE)
    {
        pt_jpg_obj->t_primary.t_basic.b_exif_exist   = FALSE;
        pt_jpg_obj->t_thumbnail.t_basic.b_exif_exist = FALSE;
        
        pt_jpg_obj->pt_current_sel = (IMG_BASIC_INFO_T*) &(pt_jpg_obj->t_thumbnail);
        if (fgJump2Thumbnail(prJdecInst, pt_jpg_obj))
        {
            pt_jpg_obj->b_APP1Exist = TRUE;
        }
        else
        {
            pt_jpg_obj->pt_current_sel = (IMG_BASIC_INFO_T*) &(pt_jpg_obj->t_primary);
            pt_jpg_obj->b_APP1Exist = FALSE;
        }
    }
    else
    {
        vSkip_marker(prJdecInst, pt_jpg_obj);
    }
    /*copy and update input object to thumbnail input*/

#ifdef USE_EXIF_ORIENTATION_TAG
        /*vExif_TAG();*/
#endif
}

/*-----------------------------------------------------------------------------
 * Name:  vSkip_marker
 *
 * Description: Routines for processing APPn and COM markers.
 *              These are either saved in memory or discarded, per application request.
 *              APP0 and APP14 are specially checked to see if they are
 *              JFIF and Adobe markers, respectively.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vSkip_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    UINT32 length;

#ifdef USE_EXIF_ORIENTATION_TAG
    if (_fgAPP1Skip)
    {
        length = _dwAPP1SkipLen;
        _fgAPP1Skip = FALSE;
    }
    else
#endif
    {
        INPUT_2BYTES(prJdecInst, length);
        length -= 2;
    }
    if (length > 0)
    {
        vSkip_input_data (prJdecInst, pt_jpg_obj, length);
    }

}

/*-----------------------------------------------------------------------------
 * Name:  vNext_marker
 *
 * Description: Find the next JPEG marker, save it in cinfo->pt_info->unread_marker.
 *              Returns FALSE if had to suspend before reaching a marker;
 *              in that case cinfo->pt_info->unread_marker is unchanged.
 *
 *              Note that the result might not be a valid marker code,
 *              but it will never be 0 or FF.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vNext_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    UINT8 c = 0;
    register UINT32 i, j, k;

    for (i = 0; i < BIG_LOOP; i++)
    {
        INPUT_BYTE(prJdecInst, c);
        /* Skip any non-FF bytes.
        * This may look a bit inefficient, but it will not occur in a valid file.
        * We sync after each discarded byte so that a suspending data source
        * can discard the byte from its buffer.
        */
        for (j = 0; j < BIG_LOOP; j++)
        {
            if (c != 0xFF)
            {
                pt_jpg_obj->ui2_discarded_bytes = pt_jpg_obj->ui2_discarded_bytes + 1;
                INPUT_BYTE(prJdecInst, c);
            }
            else
            {
                break;
            }
        }
        /* This loop swallows any duplicate FF bytes.  Extra FFs are legal as
        * pad bytes, so don't count them in pt_info->discarded_bytes.  We assume there
        * will not be so many consecutive FF bytes as to overflow a suspending
        * data source's input buffer.
        */
        k = 0;
        do
        {
            INPUT_BYTE(prJdecInst, c);
            k++;
        } while ( (c == 0xFF) && (k < BIG_LOOP) );
        if (c != 0)
            break;      /* found a valid marker, exit loop */
        /* Reach here if we found a stuffed-zero data sequence (FF/00).
        * Discard it and loop back to try again.
        */
        pt_jpg_obj->ui2_discarded_bytes = pt_jpg_obj->ui2_discarded_bytes + 2;

        if (pt_jpg_obj->b_ReachFileEnd == TRUE)
            break;
    }

    /* - After finding the next marker, to check the discard bytes.*/
    if (pt_jpg_obj->ui2_discarded_bytes != 0)
    {
        if (pt_jpg_obj->ui2_discarded_bytes > JPG_CORRUPT_DATA_LIMIT)
        {
            /* - When corrupt JPEG data exceed JPG_CORRUPT_DATA_LIMIT, it becomes a fatal error.
               - Corrupt JPEG data: Too many Discarded bytes.*/
            vJdecLog_M("[JPG_SW] ERR: vNext_marker(): When corrupt JPEG data exceed JPG_CORRUPT_DATA_LIMIT, it becomes a fatal error. Corrupt JPEG data: Too many Discarded bytes. \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 22;
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            /* - Error handler: immediately return to caller*/
            return;
        }
        else
        {
            if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
            {
                vJdecLog_M("[JPG_SW] ERR: vNext_marker(): When corrupt JPEG data exceed JPG_CORRUPT_DATA_LIMIT, it becomes a fatal error. Corrupt JPEG data: Too many Discarded bytes. \n\r");
                pt_jpg_obj->ui4_JPGErrorType = 22;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            }
            /* - Error handler: NONE*/
        }
        pt_jpg_obj->ui2_discarded_bytes = 0;
    }
    pt_jpg_obj->i2_unread_marker = c;
}
/*-----------------------------------------------------------------------------
 * Name:  vFirst_marker
 *
 * Description: Like vNext_marker, but used to obtain the initial SOI marker.
 * For this marker, we do not allow preceding garbage or fill; otherwise,
 * we might well scan an entire input file before realizing it ain't JPEG.
 * If an application wants to process non-JFIF files, it must seek to the
 * SOI before calling the JPEG library.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vFirst_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    register INT16 c, c2;

    c = 0;
    c2 = 0;
    INPUT_BYTE(prJdecInst, c);
    INPUT_BYTE(prJdecInst, c2);

    if (c != 0xFF || c2 != (INT16) M_SW_SOI)
    {
        vJdecLog_M("[JPG_SW] ERR: vFirst_marker(): Not a JPEG file : start value of the file does not equal to 0xFF, 0xD8. \n\r");
        /* - Not a JPEG file : start value of the file does not equal to 0xFF, 0xD8.*/
        pt_jpg_obj->ui4_JPGErrorType = 23;
        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        /* Error handler: immediately return to caller*/
        return;
    }
    pt_jpg_obj->i2_unread_marker = c2;
}

/*-----------------------------------------------------------------------------
 * Name:  i2Consume_markers
 *
 * Description: Read markers until SOS or EOI.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
INT16 i2Consume_markers (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    UINT32 i;
    BOOL REACHED_SOS;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return JPEG_RETURN_ERR;
    }
    REACHED_SOS = FALSE;

    /* Outer loop repeats once for each marker. */
    for (i = 0; i < BIG_LOOP; i++)  /* BIG_LOOP = 5000*/
    {
        /* Collect the marker proper, unless we already did. */
        /* vFirst_marker() enforces the requirement that SOI appear first. */
        if (pt_jpg_obj->i2_unread_marker == 0)
        {
            if (pt_jpg_obj->b_saw_SOI == FALSE)
            {
                vFirst_marker (prJdecInst, pt_jpg_obj);
            }
            else
            {
                vNext_marker(prJdecInst, pt_jpg_obj);
            }
        }

        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  vFirst_marker or vNext_marker error \n\r");
            return JPEG_RETURN_ERR;
        }
        /* At this point cinfo->pt_info->unread_marker contains the marker code and the
        * input point is just past the marker proper, but before any parameters.
        * A suspension will cause us to return with this state still true.
        */
        switch (pt_jpg_obj->i2_unread_marker) {
        case M_SW_SOI:
            vGet_soi(prJdecInst, pt_jpg_obj);
            break;
        case M_SW_SOF0:    /* Baseline */
        case M_SW_SOF1:    /* Extended sequential, Huffman */
            vGet_sof (prJdecInst, pt_jpg_obj);
            pt_info->b_progressive_mode = FALSE;
            pt_jpg_obj->b_arith_code = FALSE;
            break;

        case M_SW_SOF2:    /* Progressive, Huffman */
            vGet_sof (prJdecInst, pt_jpg_obj);
            pt_info->b_progressive_mode = TRUE;
            pt_jpg_obj->b_arith_code = FALSE;
        break;

        case M_SW_SOF9:    /* Extended sequential, arithmetic */
            vGet_sof (prJdecInst, pt_jpg_obj);
            pt_info->b_progressive_mode = FALSE;
            pt_jpg_obj->b_arith_code = TRUE;
            break;

        case M_SW_SOF10:   /* Progressive, arithmetic */
            vGet_sof (prJdecInst, pt_jpg_obj);
            pt_info->b_progressive_mode = TRUE;
            pt_jpg_obj->b_arith_code = TRUE;
            break;

        /* Currently unsupported SOFn types */
        case M_SW_SOF3:    /* Lossless, Huffman */
        case M_SW_SOF5:    /* Differential sequential, Huffman */
        case M_SW_SOF6:    /* Differential progressive, Huffman */
        case M_SW_SOF7:    /* Differential lossless, Huffman */
        case M_SW_JPG:     /* Reserved for JPEG extensions */
        case M_SW_SOF11:   /* Lossless, arithmetic */
        case M_SW_SOF13:   /* Differential sequential, arithmetic */
        case M_SW_SOF14:   /* Differential progressive, arithmetic */
        case M_SW_SOF15:   /* Differential lossless, arithmetic */
            vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  unsupported SOFn types \n\r");
            pt_jpg_obj->ui4_JPGErrorType = 24;
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            /* Error handler: immediately return to caller*/
            return JPEG_RETURN_ERR;

        case M_SW_SOS:
            vGet_sos (prJdecInst, pt_jpg_obj);
            pt_jpg_obj->i2_unread_marker = 0;  /* processed the marker */
            if (pt_info->b_first_SOS) /* 1st SOS */
            {
                vFirst_SOS_setup (prJdecInst, pt_jpg_obj);
                pt_info->b_first_SOS = FALSE;
            }
            else
            {      /* 2nd or later SOS marker */
                if (!pt_info->b_has_multiple_scans)
                {
                    vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  !pt_info->b_has_multiple_scans\n\r");
                    pt_jpg_obj->ui4_JPGErrorType = 19;
                    pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
                    return JPEG_RETURN_ERR;
                }

                vStart_input_pass (prJdecInst, pt_jpg_obj);
                if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
                {
                    vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  vStart_input_pass error\n\r");
                    return JPEG_RETURN_ERR;
                }
            }

            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  M_SW_SOS error\n\r");
                return JPEG_RETURN_ERR;
            }

            REACHED_SOS = TRUE;
            break;

        case M_SW_EOI:
            pt_jpg_obj->i2_unread_marker = 0;  /* processed the marker */
            return JPEG_REACHED_EOI;

        case M_SW_DAC:
            vSkip_marker(prJdecInst, pt_jpg_obj);
            break;

        case M_SW_DHT:
            vGet_dht (prJdecInst, pt_jpg_obj);
            break;

        case M_SW_DQT:
            vGet_dqt (prJdecInst, pt_jpg_obj);
            break;

        case M_SW_DRI:
            vGet_dri (prJdecInst, pt_jpg_obj);
            break;

        case M_SW_APP1:
#if 0
            pt_jpg_obj->i2_unread_marker = 0;
            vAPP1_Handle2(prJdecInst, pt_jpg_obj);
            pt_info = pt_jpg_obj->pt_current_sel;
#else
            vSkip_marker(prJdecInst, pt_jpg_obj);
#endif
            break;

        case M_SW_APP0:
        case M_SW_APP2:
        case M_SW_APP3:
        case M_SW_APP4:
        case M_SW_APP5:
        case M_SW_APP6:
        case M_SW_APP7:
        case M_SW_APP8:
        case M_SW_APP9:
        case M_SW_APP10:
        case M_SW_APP11:
        case M_SW_APP12:
        case M_SW_APP13:
        case M_SW_APP14:
        case M_SW_APP15:
            vSkip_marker(prJdecInst, pt_jpg_obj);
            break;

        case M_SW_COM:
            vSkip_marker(prJdecInst, pt_jpg_obj);
             break;

        case M_SW_RST0:    /* these are all parameterless */
        case M_SW_RST1:
        case M_SW_RST2:
        case M_SW_RST3:
        case M_SW_RST4:
        case M_SW_RST5:
        case M_SW_RST6:
        case M_SW_RST7:
        case M_SW_TEM:
            break;

        case M_SW_DNL:     /* Ignore DNL ... perhaps the wrong thing */
            vSkip_marker(prJdecInst, pt_jpg_obj);
            break;

        default:      /* must be DHP, EXP, JPGn, or RESn */
            /* For now, we treat the reserved markers as fatal errors since they are
            * likely to be used to signal incompatible JPEG Part 3 extensions.
            * Once the JPEG 3 version-number marker is well defined, this code
            * ought to change!
            */
            break;
        }

        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: i2Consume_markers():  Outer loop repeats once for each marker.\n\r");
            return JPEG_RETURN_ERR;
        }


        /* Successfully processed marker, so reset state variable */
        pt_jpg_obj->i2_unread_marker = 0;
        if (REACHED_SOS == TRUE)
        {
            if (pt_jpg_obj->pt_current_sel == (IMG_BASIC_INFO_T*) &(pt_jpg_obj->t_thumbnail))
            {
                /* store thumbnail compressed data begin pos*/
                pt_info->ui4_compressed_data_begin_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj);

                /* skip thumbnail comressed data */
                vSet_InputPos(prJdecInst, pt_jpg_obj, pt_info->ui4_compressed_data_end_pos);

                /* change current select to primary image */
                pt_jpg_obj->pt_current_sel = (IMG_BASIC_INFO_T*) &(pt_jpg_obj->t_primary);
                pt_info = pt_jpg_obj->pt_current_sel;

                /* restart for primary image */
                REACHED_SOS = FALSE;
            }
            else
            {
                /* store primary compressed data begin pos*/
                pt_info->ui4_compressed_data_begin_pos = u4Get_InputPos(prJdecInst, pt_jpg_obj);
                return JPEG_MARKER_OK;
            }

            /* set the start position of the picture mode decoding */
            pt_jpg_obj->pt_current_sel->ui4_pm_data_begin_pos = 0;
        }
    } /* end loop */
    return JPEG_RETURN_ERR;  /* exceed maximum marker number*/
}

/*-----------------------------------------------------------------------------
 * Name:  vRead_restart_marker
 *
 * Description: Read a restart marker, which is expected to appear next in the datastream;
 *              if the marker is not there, take appropriate recovery action.
 *              Returns FALSE if suspension is required.
 *
 *              This is called by the entropy decoder after it has read an appropriate
 *              number of MCUs.  cinfo->pt_info->unread_marker may be nonzero if the entropy decoder
 *              has already read a marker from the data source.  Under normal conditions
 *              cinfo->pt_info->unread_marker will be reset to 0 before returning; if not reset,
 *              it holds a marker which the decoder will be unable to read past.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vRead_restart_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    /* Obtain a marker unless we already did. */
    /* Note that vNext_marker will complain if it skips any data. */
    if (pt_jpg_obj->i2_unread_marker == 0)
    {
        vNext_marker (prJdecInst, pt_jpg_obj);
        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vRead_restart_marker(): vNext_marker error.\n\r");
            return;
        }
    }

    if (pt_jpg_obj->i2_unread_marker == ((INT16) M_SW_RST0 + pt_info->ui1_next_restart_num))
    {
        /* Normal case --- swallow the marker and let entropy decoder continue */
        vJdecLog_M("[JPG_SW] RST%d\n", pt_info->ui1_next_restart_num);

        pt_jpg_obj->i2_unread_marker = 0;
    }
    else
    {
        /* Uh-oh, the restart markers have been messed up. */
        /* Let the data source manager determine how to resync. */
        vJpeg_resync_to_restart (prJdecInst, pt_jpg_obj);
        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
            vJdecLog_M("[JPG_SW] ERR: vRead_restart_marker(): Uh-oh, the restart markers have been messed up.\n\r");
            return;
        }
    }

    /* Update next-restart state */
    pt_info->ui1_next_restart_num = (pt_info->ui1_next_restart_num + 1) & 7;
}
/*-----------------------------------------------------------------------------
 * Name:  u4BYTERearrange
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT32 u4BYTERearrange(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 wType, UINT32 dwValue)
{
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return JPEG_RETURN_ERR;
    }
    switch(wType)
    {
        case TYPE_BYTE:
            break;
        case TYPE_ASCII:
            break;
        case TYPE_SHORT:
            dwValue = (dwValue & 0xFFFF0000)>>16;
            if(!pt_info->b_big_endian)
            {
                dwValue = (dwValue&0xFF00)>>8 | (dwValue&0xFF)<<8;
            }
            break;
        case TYPE_LONG:
            if (!pt_info->b_big_endian)
            {
            dwValue =( ((dwValue&0xFF000000)>>24) |
            ((dwValue&0xFF0000)>>8) |
            ((dwValue&0xFF00)<<8) |
            (dwValue&0xFF)<<24 );
            }
            break;
        case TYPE_RATIONAL:
            break;
        case TYPE_UNDEFINED:
            break;
        case TYPE_SLONG:
            break;
        case TYPE_SRATIONAL:
            break;
    }
    return dwValue;
}
/*-----------------------------------------------------------------------------
 * Name:  vExif_TAG
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
#ifdef USE_EXIF_ORIENTATION_TAG
void vExif_TAG(void)
{
    UINT32 i;

    UINT16 endian, wTmp;
    UINT8 bTmp[6];
    UINT16 wTag;
    UINT16 wType;
    UINT32 dwCount;
    UINT32 dwValue;

    _fgAPP1Skip = TRUE;

    INPUT_2BYTES(prJdecInst, _dwAPP1SkipLen);
    _wExifOfst = _dwAPP1SkipLen;
    _dwAPP1SkipLen -= 2;

    /* - Check Exif*/
    for (i = 0; i < 6; i++)
    {
        INPUT_BYTE(prJdecInst, bTmp[i]);
    }
    _dwAPP1SkipLen -= 6;

    if (bTmp[0] == 0x45 && bTmp[1] == 0x78 && bTmp[2] == 0x69 && bTmp[3] == 0x66 && bTmp[4] == 0x00 && bTmp[5] == 0x00)
    {
        _fgExifExist = TRUE;
    }
    else
    {
        _fgExifExist = FALSE;
        return;
    }

    if (_fgExifExist)
    {
        INPUT_2BYTES(prJdecInst, endian);
        _dwAPP1SkipLen -= 2;

        if (endian == 0x4949)       /* - small endian*/
        {
            pt_info->b_big_endian = FALSE;
        }
        else if (endian == 0x4D4D)  /* - big endian*/
        {
            pt_info->b_big_endian = TRUE;
        }

        /* - Skip {0x2A 0x00}*/
        INPUT_2BYTES(prJdecInst, wTmp);
        _dwAPP1SkipLen -= 2;

        /* - Get OFFSET of IFD and skip the TIFF header.*/
        INPUT_4BYTES(prJdecInst, dwValue);
        _dwAPP1SkipLen -= 4;

        dwValue = u4BYTERearrange (prJdecInst, pt_jpg_obj, TYPE_LONG, dwValue);
        dwValue = dwValue - 8;

        for ( i=0 ; i<dwValue ; i++)
        {
            INPUT_BYTE(prJdecInst, bTmp[0]);
            _dwAPP1SkipLen -= 1;
        }

        /* - IFD Tag count*/
        INPUT_2BYTES(prJdecInst, _wTagCount);
        _dwAPP1SkipLen -= 2;
        if (!pt_info->b_big_endian)
            _wTagCount = ((_wTagCount&0xFF00) >> 8) | ((_wTagCount&0xFF) << 8);
        _wIDF0CntOfst = pt_info->_dwJPGReadByteNum;

        /* - Search orientation Tag / Type / Count / value.*/
        for( i=0; i<_wTagCount; i++)
        {
            INPUT_2BYTES(prJdecInst, wTag);
            if (!pt_info->b_big_endian)
                wTag = (wTag&0xFF00) >> 8 | ((wTag&0xFF)<<8);

            INPUT_2BYTES(prJdecInst, wType);
            if (!pt_info->b_big_endian)
                wType = (wType&0xFF00) >> 8 | ((wType&0xFF)<<8);

            INPUT_4BYTES(prJdecInst, dwCount);
            dwCount = u4BYTERearrange(prJdecInst, pt_jpg_obj, wType, dwCount);

            INPUT_4BYTES(prJdecInst, dwValue);
            dwValue = u4BYTERearrange(prJdecInst, pt_jpg_obj, wType, dwValue);
            _dwAPP1SkipLen -= 12;
            /* - Check Orientation tag.*/
            if (wTag == 0x0112)
            {
                _fgOrientationTagExist = TRUE;
                _dwOrientation = dwValue;
                if ((_dwOrientation == 0x0) || (_dwOrientation > 0x08))
                    _dwOrientation = 0x01;
                _dwCurOrientation = _dwOrientation;
                _dwOrgOrientation = _dwOrientation;
                break;
            }
        }
        _wExifOfst = pt_info->_dwJPGReadByteNum;
    }
}
#endif


