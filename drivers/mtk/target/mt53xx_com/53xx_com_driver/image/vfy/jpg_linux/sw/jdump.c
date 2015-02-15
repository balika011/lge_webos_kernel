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
 * $RCSfile: jdump.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/1 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 405fde34e0b122aa17126fb45619f98f $ Asa
 *
 * Description:
 *         This file contains all engine APIs of SW JPEG decoder engine.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_common.h"
#include "x_handle.h"
//#include "x_dbg.h"
#include "x_fm.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "djpeg_sw_api.h"
#include "x_debug.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#ifdef _WIN32_
#ifdef _DUMP_BMP_
#define PUT_2B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
    array[offset+1] = (char) (((value) >> 8) & 0xFF))
#define PUT_4B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
    array[offset+1] = (char) (((value) >> 8) & 0xFF), \
    array[offset+2] = (char) (((value) >> 16) & 0xFF), \
    array[offset+3] = (char) (((value) >> 24) & 0xFF))
#endif /* _DUMP_BMP_ */
#endif /* _WIN32_ */
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
#ifdef _WIN32_
#ifdef _DUMP_BMP_
/*-----------------------------------------------------------------------------
 * Name:  vDump_dst_surface
 *
 * Description: dump destination surface to a bitmap file
 *
 * Inputs:  h_dst_surf      destination surface which will be dump
 *
 * Outputs: -
 *
 * Return:  -
 ----------------------------------------------------------------------------*/
#if 0
VOID vDump_dst_surface(CHAR* ps_outfile_name, GL_LOCK_INFO_T* pt_lock_info)
{
    INT32 i,j;
    UINT8 ui1_G, ui1_R, ui1_B;
    INT8 ai1_bmpfileheader[14];
    INT8 ai1_bmpinfoheader[40];
    INT8 i1_padding;
    FILE *p_bmpfile;
    SIZE_T z_width = pt_lock_info->at_subplane[0].ui4_width, z_height =  pt_lock_info->at_subplane[0].ui4_height;
    UINT8* pui1_surf_data = pt_lock_info->at_subplane[0].pv_bits;
    UINT8* pui1_pos;
    SIZE_T z_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    INT32 i4_headersize, i4_bfSize;
    INT16 i2_bits_per_pixel, i2_cmap_entries;


    if ((p_bmpfile = fopen (ps_outfile_name, "wb")) == NULL)
    {
        printf ("can't open display dump BMP file");
        return;
    }

    /* Unquantized, full color RGB */
    i2_bits_per_pixel = 24;
    i2_cmap_entries = 0;

    /* File size */
    i4_headersize = 14 + 40 + i2_cmap_entries * 4;    /* Header and colormap */
    i4_bfSize = i4_headersize + (INT32) (z_width * 3) * (INT32) z_height;

    /* Set unused fields of header to 0 */
    for (i = 0; i < sizeof (ai1_bmpfileheader); i++)
        ai1_bmpfileheader[i] = 0;

    for (i = 0; i < sizeof (ai1_bmpinfoheader); i++)
        ai1_bmpinfoheader[i] = 0;

    /* Fill the file header */
    ai1_bmpfileheader[0] = 0x42;    /* first 2 bytes are ASCII 'B', 'M' */
    ai1_bmpfileheader[1] = 0x4D;
    PUT_4B (ai1_bmpfileheader, 2, i4_bfSize);    /* bfSize */
    /* we leave bfReserved1 & bfReserved2 = 0 */
    PUT_4B (ai1_bmpfileheader, 10, i4_headersize);    /* bfOffBits */

    /* Fill the info header (Microsoft calls this a BITMAPINFOHEADER) */
    PUT_2B (ai1_bmpinfoheader, 0, 40);    /* biSize */
    PUT_4B (ai1_bmpinfoheader, 4, z_width);    /* biWidth */
    PUT_4B (ai1_bmpinfoheader, 8, z_height);    /* biHeight */
    PUT_2B (ai1_bmpinfoheader, 12, 1);    /* biPlanes - must be 1 */
    PUT_2B (ai1_bmpinfoheader, 14, i2_bits_per_pixel);    /* biBitCount */
    /* we leave biCompression = 0, for none */
    /* we leave biSizeImage = 0; this is correct for uncompressed data */
    PUT_2B (ai1_bmpinfoheader, 32, i2_cmap_entries);    /* biClrUsed */
    /* we leave biClrImportant = 0 */

    fwrite (ai1_bmpfileheader, 1, 14, p_bmpfile);
    fwrite (ai1_bmpinfoheader, 1, 40, p_bmpfile);
    i1_padding = (UINT8)((4 - ((z_width*3) % 4)) % 4);
    if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888)
    {
        for (i=z_height-1 ; i>=0 ; i--)
        {
            pui1_pos = pui1_surf_data + i*z_pitch;
            for (j=0 ; j<(INT32)z_width ; j++)
            {
                ui1_R = pui1_pos[j*4+2];
                ui1_G = pui1_pos[j*4+1];
                ui1_B = pui1_pos[j*4];
                fputc (ui1_B, p_bmpfile);
                fputc (ui1_G, p_bmpfile);
                fputc (ui1_R, p_bmpfile);
            }

            if (i1_padding)
            {
                for (j=0 ; j<i1_padding ; j++)
                {
                    fputc (0, p_bmpfile);
                }
            }
        }
    }
    else if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444)
    {
        for (i=z_height-1 ; i>=0 ; i--)
        {
            pui1_pos = pui1_surf_data + i*z_pitch;
            for (j=0 ; j<(INT32)z_width ; j++)
            {
                ui1_R = (pui1_pos[j*2+1] & 0xf)<<4;
                ui1_G = (pui1_pos[j*2] & 0xf0) ;
                ui1_B = (pui1_pos[j*2] & 0xf)<<4;
                fputc (ui1_B, p_bmpfile);
                fputc (ui1_G, p_bmpfile);
                fputc (ui1_R, p_bmpfile);
            }

            if (i1_padding)
            {
                for (j=0 ; j<i1_padding ; j++)
                {
                    fputc (0, p_bmpfile);
                }
            }
        }
    }
    else
    {
        UTIL_Printf("[IMG] not support");
    }
    fclose (p_bmpfile);
}
#endif
#endif/* _DUMP_BMP_ */
#endif /* _WIN32_ */

