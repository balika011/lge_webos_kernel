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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: b2r_emu.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file b2r_emu.c
 *  this file contains function test.
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_common.h"
#include "x_stl_lib.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
#include "x_hal_arm.h"
#include "b2r_emu.h"
#include "x_printf.h"
#include "fbm_drvif.h"
//-----------------------------------------------------------------------------
// Configurations for test case
//-----------------------------------------------------------------------------
static unsigned char *cmodel_buff;
static unsigned char *y_dram;
static unsigned char *c_dram;
static unsigned int  dram_size;
static unsigned char **y = NULL;
static unsigned char **c = NULL;
static int hdew;
static int vdew;
static int line_ofst;
static int addr_mode;
static int addr_swap;
static int m_420i;
static int out_i;
static int field;
static int m_422;
static int mirror;
static int flip;
static int h_offset;
static int v_offset;
static int r_map_y;
static int r_map_c;
static int tag_removal;
static int enh_layer;
static int is_new = 0;

static unsigned int y_color;
static unsigned int c_color;
static int is_fixed_y_color = 0;
static int is_fixed_c_color = 0;

//for 10bit mode
static unsigned short **y_w = NULL;
static unsigned short **c_w = NULL;
static int bit_depth = 8;


static int CRC32_D32( int crc, int data )
{
    int i;
    int d[32];
    int c[32];
    int newcrc[32];
    int result;

    for ( i = 0; i < 32; i++ )
    {
        d[i] = ( data >> i ) & 0x1;
        c[i] = ( crc >> i ) & 0x1;
    }

    newcrc[0] = d[31] ^ d[30] ^ d[29] ^ d[28] ^ d[26] ^ d[25] ^ d[24] ^ d[16] ^ d[12] ^ d[10] ^ d[9] ^ d[6] ^ d[0] ^ c[0] ^ c[6] ^ c[9] ^ c[10] ^ c[12] ^ c[16] ^ c[24] ^ c[25] ^ c[26] ^ c[28] ^ c[29] ^ c[30] ^ c[31];
    newcrc[1] = d[28] ^ d[27] ^ d[24] ^ d[17] ^ d[16] ^ d[13] ^ d[12] ^ d[11] ^ d[9] ^ d[7] ^ d[6] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[6] ^ c[7] ^ c[9] ^ c[11] ^ c[12] ^ c[13] ^ c[16] ^ c[17] ^ c[24] ^ c[27] ^ c[28];
    newcrc[2] = d[31] ^ d[30] ^ d[26] ^ d[24] ^ d[18] ^ d[17] ^ d[16] ^ d[14] ^ d[13] ^ d[9] ^ d[8] ^ d[7] ^ d[6] ^ d[2] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[2] ^ c[6] ^ c[7] ^ c[8] ^ c[9] ^ c[13] ^ c[14] ^ c[16] ^ c[17] ^ c[18] ^ c[24] ^ c[26] ^ c[30] ^ c[31];
    newcrc[3] = d[31] ^ d[27] ^ d[25] ^ d[19] ^ d[18] ^ d[17] ^ d[15] ^ d[14] ^ d[10] ^ d[9] ^ d[8] ^ d[7] ^ d[3] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[3] ^ c[7] ^ c[8] ^ c[9] ^ c[10] ^ c[14] ^ c[15] ^ c[17] ^ c[18] ^ c[19] ^ c[25] ^ c[27] ^ c[31];
    newcrc[4] = d[31] ^ d[30] ^ d[29] ^ d[25] ^ d[24] ^ d[20] ^ d[19] ^ d[18] ^ d[15] ^ d[12] ^ d[11] ^ d[8] ^ d[6] ^ d[4] ^ d[3] ^ d[2] ^ d[0] ^ c[0] ^ c[2] ^ c[3] ^ c[4] ^ c[6] ^ c[8] ^ c[11] ^ c[12] ^ c[15] ^ c[18] ^ c[19] ^ c[20] ^ c[24] ^ c[25] ^ c[29] ^ c[30] ^ c[31];
    newcrc[5] = d[29] ^ d[28] ^ d[24] ^ d[21] ^ d[20] ^ d[19] ^ d[13] ^ d[10] ^ d[7] ^ d[6] ^ d[5] ^ d[4] ^ d[3] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[3] ^ c[4] ^ c[5] ^ c[6] ^ c[7] ^ c[10] ^ c[13] ^ c[19] ^ c[20] ^ c[21] ^ c[24] ^ c[28] ^ c[29];
    newcrc[6] = d[30] ^ d[29] ^ d[25] ^ d[22] ^ d[21] ^ d[20] ^ d[14] ^ d[11] ^ d[8] ^ d[7] ^ d[6] ^ d[5] ^ d[4] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[4] ^ c[5] ^ c[6] ^ c[7] ^ c[8] ^ c[11] ^ c[14] ^ c[20] ^ c[21] ^ c[22] ^ c[25] ^ c[29] ^ c[30];
    newcrc[7] = d[29] ^ d[28] ^ d[25] ^ d[24] ^ d[23] ^ d[22] ^ d[21] ^ d[16] ^ d[15] ^ d[10] ^ d[8] ^ d[7] ^ d[5] ^ d[3] ^ d[2] ^ d[0] ^ c[0] ^ c[2] ^ c[3] ^ c[5] ^ c[7] ^ c[8] ^ c[10] ^ c[15] ^ c[16] ^ c[21] ^ c[22] ^ c[23] ^ c[24] ^ c[25] ^ c[28] ^ c[29];
    newcrc[8] = d[31] ^ d[28] ^ d[23] ^ d[22] ^ d[17] ^ d[12] ^ d[11] ^ d[10] ^ d[8] ^ d[4] ^ d[3] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[3] ^ c[4] ^ c[8] ^ c[10] ^ c[11] ^ c[12] ^ c[17] ^ c[22] ^ c[23] ^ c[28] ^ c[31];
    newcrc[9] = d[29] ^ d[24] ^ d[23] ^ d[18] ^ d[13] ^ d[12] ^ d[11] ^ d[9] ^ d[5] ^ d[4] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[4] ^ c[5] ^ c[9] ^ c[11] ^ c[12] ^ c[13] ^ c[18] ^ c[23] ^ c[24] ^ c[29];
    newcrc[10] = d[31] ^ d[29] ^ d[28] ^ d[26] ^ d[19] ^ d[16] ^ d[14] ^ d[13] ^ d[9] ^ d[5] ^ d[3] ^ d[2] ^ d[0] ^ c[0] ^ c[2] ^ c[3] ^ c[5] ^ c[9] ^ c[13] ^ c[14] ^ c[16] ^ c[19] ^ c[26] ^ c[28] ^ c[29] ^ c[31];
    newcrc[11] = d[31] ^ d[28] ^ d[27] ^ d[26] ^ d[25] ^ d[24] ^ d[20] ^ d[17] ^ d[16] ^ d[15] ^ d[14] ^ d[12] ^ d[9] ^ d[4] ^ d[3] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[3] ^ c[4] ^ c[9] ^ c[12] ^ c[14] ^ c[15] ^ c[16] ^ c[17] ^ c[20] ^ c[24] ^ c[25] ^ c[26] ^ c[27] ^ c[28] ^ c[31];
    newcrc[12] = d[31] ^ d[30] ^ d[27] ^ d[24] ^ d[21] ^ d[18] ^ d[17] ^ d[15] ^ d[13] ^ d[12] ^ d[9] ^ d[6] ^ d[5] ^ d[4] ^ d[2] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[2] ^ c[4] ^ c[5] ^ c[6] ^ c[9] ^ c[12] ^ c[13] ^ c[15] ^ c[17] ^ c[18] ^ c[21] ^ c[24] ^ c[27] ^ c[30] ^ c[31];
    newcrc[13] = d[31] ^ d[28] ^ d[25] ^ d[22] ^ d[19] ^ d[18] ^ d[16] ^ d[14] ^ d[13] ^ d[10] ^ d[7] ^ d[6] ^ d[5] ^ d[3] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[3] ^ c[5] ^ c[6] ^ c[7] ^ c[10] ^ c[13] ^ c[14] ^ c[16] ^ c[18] ^ c[19] ^ c[22] ^ c[25] ^ c[28] ^ c[31];
    newcrc[14] = d[29] ^ d[26] ^ d[23] ^ d[20] ^ d[19] ^ d[17] ^ d[15] ^ d[14] ^ d[11] ^ d[8] ^ d[7] ^ d[6] ^ d[4] ^ d[3] ^ d[2] ^ c[2] ^ c[3] ^ c[4] ^ c[6] ^ c[7] ^ c[8] ^ c[11] ^ c[14] ^ c[15] ^ c[17] ^ c[19] ^ c[20] ^ c[23] ^ c[26] ^ c[29];
    newcrc[15] = d[30] ^ d[27] ^ d[24] ^ d[21] ^ d[20] ^ d[18] ^ d[16] ^ d[15] ^ d[12] ^ d[9] ^ d[8] ^ d[7] ^ d[5] ^ d[4] ^ d[3] ^ c[3] ^ c[4] ^ c[5] ^ c[7] ^ c[8] ^ c[9] ^ c[12] ^ c[15] ^ c[16] ^ c[18] ^ c[20] ^ c[21] ^ c[24] ^ c[27] ^ c[30];
    newcrc[16] = d[30] ^ d[29] ^ d[26] ^ d[24] ^ d[22] ^ d[21] ^ d[19] ^ d[17] ^ d[13] ^ d[12] ^ d[8] ^ d[5] ^ d[4] ^ d[0] ^ c[0] ^ c[4] ^ c[5] ^ c[8] ^ c[12] ^ c[13] ^ c[17] ^ c[19] ^ c[21] ^ c[22] ^ c[24] ^ c[26] ^ c[29] ^ c[30];
    newcrc[17] = d[31] ^ d[30] ^ d[27] ^ d[25] ^ d[23] ^ d[22] ^ d[20] ^ d[18] ^ d[14] ^ d[13] ^ d[9] ^ d[6] ^ d[5] ^ d[1] ^ c[1] ^ c[5] ^ c[6] ^ c[9] ^ c[13] ^ c[14] ^ c[18] ^ c[20] ^ c[22] ^ c[23] ^ c[25] ^ c[27] ^ c[30] ^ c[31];
    newcrc[18] = d[31] ^ d[28] ^ d[26] ^ d[24] ^ d[23] ^ d[21] ^ d[19] ^ d[15] ^ d[14] ^ d[10] ^ d[7] ^ d[6] ^ d[2] ^ c[2] ^ c[6] ^ c[7] ^ c[10] ^ c[14] ^ c[15] ^ c[19] ^ c[21] ^ c[23] ^ c[24] ^ c[26] ^ c[28] ^ c[31];
    newcrc[19] = d[29] ^ d[27] ^ d[25] ^ d[24] ^ d[22] ^ d[20] ^ d[16] ^ d[15] ^ d[11] ^ d[8] ^ d[7] ^ d[3] ^ c[3] ^ c[7] ^ c[8] ^ c[11] ^ c[15] ^ c[16] ^ c[20] ^ c[22] ^ c[24] ^ c[25] ^ c[27] ^ c[29];
    newcrc[20] = d[30] ^ d[28] ^ d[26] ^ d[25] ^ d[23] ^ d[21] ^ d[17] ^ d[16] ^ d[12] ^ d[9] ^ d[8] ^ d[4] ^ c[4] ^ c[8] ^ c[9] ^ c[12] ^ c[16] ^ c[17] ^ c[21] ^ c[23] ^ c[25] ^ c[26] ^ c[28] ^ c[30];
    newcrc[21] = d[31] ^ d[29] ^ d[27] ^ d[26] ^ d[24] ^ d[22] ^ d[18] ^ d[17] ^ d[13] ^ d[10] ^ d[9] ^ d[5] ^ c[5] ^ c[9] ^ c[10] ^ c[13] ^ c[17] ^ c[18] ^ c[22] ^ c[24] ^ c[26] ^ c[27] ^ c[29] ^ c[31];
    newcrc[22] = d[31] ^ d[29] ^ d[27] ^ d[26] ^ d[24] ^ d[23] ^ d[19] ^ d[18] ^ d[16] ^ d[14] ^ d[12] ^ d[11] ^ d[9] ^ d[0] ^ c[0] ^ c[9] ^ c[11] ^ c[12] ^ c[14] ^ c[16] ^ c[18] ^ c[19] ^ c[23] ^ c[24] ^ c[26] ^ c[27] ^ c[29] ^ c[31];
    newcrc[23] = d[31] ^ d[29] ^ d[27] ^ d[26] ^ d[20] ^ d[19] ^ d[17] ^ d[16] ^ d[15] ^ d[13] ^ d[9] ^ d[6] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[6] ^ c[9] ^ c[13] ^ c[15] ^ c[16] ^ c[17] ^ c[19] ^ c[20] ^ c[26] ^ c[27] ^ c[29] ^ c[31];
    newcrc[24] = d[30] ^ d[28] ^ d[27] ^ d[21] ^ d[20] ^ d[18] ^ d[17] ^ d[16] ^ d[14] ^ d[10] ^ d[7] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[7] ^ c[10] ^ c[14] ^ c[16] ^ c[17] ^ c[18] ^ c[20] ^ c[21] ^ c[27] ^ c[28] ^ c[30];
    newcrc[25] = d[31] ^ d[29] ^ d[28] ^ d[22] ^ d[21] ^ d[19] ^ d[18] ^ d[17] ^ d[15] ^ d[11] ^ d[8] ^ d[3] ^ d[2] ^ c[2] ^ c[3] ^ c[8] ^ c[11] ^ c[15] ^ c[17] ^ c[18] ^ c[19] ^ c[21] ^ c[22] ^ c[28] ^ c[29] ^ c[31];
    newcrc[26] = d[31] ^ d[28] ^ d[26] ^ d[25] ^ d[24] ^ d[23] ^ d[22] ^ d[20] ^ d[19] ^ d[18] ^ d[10] ^ d[6] ^ d[4] ^ d[3] ^ d[0] ^ c[0] ^ c[3] ^ c[4] ^ c[6] ^ c[10] ^ c[18] ^ c[19] ^ c[20] ^ c[22] ^ c[23] ^ c[24] ^ c[25] ^ c[26] ^ c[28] ^ c[31];
    newcrc[27] = d[29] ^ d[27] ^ d[26] ^ d[25] ^ d[24] ^ d[23] ^ d[21] ^ d[20] ^ d[19] ^ d[11] ^ d[7] ^ d[5] ^ d[4] ^ d[1] ^ c[1] ^ c[4] ^ c[5] ^ c[7] ^ c[11] ^ c[19] ^ c[20] ^ c[21] ^ c[23] ^ c[24] ^ c[25] ^ c[26] ^ c[27] ^ c[29];
    newcrc[28] = d[30] ^ d[28] ^ d[27] ^ d[26] ^ d[25] ^ d[24] ^ d[22] ^ d[21] ^ d[20] ^ d[12] ^ d[8] ^ d[6] ^ d[5] ^ d[2] ^ c[2] ^ c[5] ^ c[6] ^ c[8] ^ c[12] ^ c[20] ^ c[21] ^ c[22] ^ c[24] ^ c[25] ^ c[26] ^ c[27] ^ c[28] ^ c[30];
    newcrc[29] = d[31] ^ d[29] ^ d[28] ^ d[27] ^ d[26] ^ d[25] ^ d[23] ^ d[22] ^ d[21] ^ d[13] ^ d[9] ^ d[7] ^ d[6] ^ d[3] ^ c[3] ^ c[6] ^ c[7] ^ c[9] ^ c[13] ^ c[21] ^ c[22] ^ c[23] ^ c[25] ^ c[26] ^ c[27] ^ c[28] ^ c[29] ^ c[31];
    newcrc[30] = d[30] ^ d[29] ^ d[28] ^ d[27] ^ d[26] ^ d[24] ^ d[23] ^ d[22] ^ d[14] ^ d[10] ^ d[8] ^ d[7] ^ d[4] ^ c[4] ^ c[7] ^ c[8] ^ c[10] ^ c[14] ^ c[22] ^ c[23] ^ c[24] ^ c[26] ^ c[27] ^ c[28] ^ c[29] ^ c[30];
    newcrc[31] = d[31] ^ d[30] ^ d[29] ^ d[28] ^ d[27] ^ d[25] ^ d[24] ^ d[23] ^ d[15] ^ d[11] ^ d[9] ^ d[8] ^ d[5] ^ c[5] ^ c[8] ^ c[9] ^ c[11] ^ c[15] ^ c[23] ^ c[24] ^ c[25] ^ c[27] ^ c[28] ^ c[29] ^ c[30] ^ c[31];

    result = 0;

    for ( i = 0; i < 32; i++ )
    {
        result += ( newcrc[i] << i );
    }

    return result;
}

//from fill_yc.c
static unsigned char range_map( unsigned char x, int r_map )
{
    int temp;

    if ( x >= 128 )
    {
        temp = ( ( x - 128 ) * r_map + 1028 ) >> 3;
        
        if ( temp >= 256 )
        {
            temp = 255;
        }
    }
    else
    {
        temp = ( 1028 - ( 128 - x ) * r_map ) >> 3;

        if ( temp < 0 )
        {
            temp = 0;
        }
    }

    return (unsigned char) temp;
}

static int dram_checksum( int checksum, unsigned char *dram, int addr )
{
    int data;
    int data_inv;

    data = ( ( dram[addr + 15] << 24 ) + ( dram[addr + 14] << 16 ) + ( dram[addr + 13] << 8 ) + dram[addr + 12] ) ^
           ( ( dram[addr + 11] << 24 ) + ( dram[addr + 10] << 16 ) + ( dram[addr +  9] << 8 ) + dram[addr +  8] ) ^
           ( ( dram[addr +  7] << 24 ) + ( dram[addr +  6] << 16 ) + ( dram[addr +  5] << 8 ) + dram[addr +  4] ) ^
           ( ( dram[addr +  3] << 24 ) + ( dram[addr +  2] << 16 ) + ( dram[addr +  1] << 8 ) + dram[addr +  0] );

    data_inv = data ^ 0xcb314756;

    return checksum + data_inv;
}

static void fill_yc(void)
{
    //cfg
    int checksum;

    //allocate memory
    int v, h;


    //fill y, c from y_dram, c_dram
    int i;
    int line_cnt_ini;
    int line_cnt_step;
    int line_cnt;
    int raster;

    int y_addr_line;
    int y_adr_mul_line;
    int y_mul_h;
    int y_mul_l;
    int y_adr_blk_ofst;
    int y_addr_step;
    int blkry_addr_ini;
    int blkry_addr_org;
    int blkry_addr;

    int c_addr_line;
    int c_adr_mul_line;
    int c_mul_h;
    int c_mul_l;
    int c_adr_blk_ofst;
    int c_addr_step;
    int blkrc_addr_ini;
    int blkrc_addr_org;
    int blkrc_addr;

    Printf("%s\n", __FUNCTION__);

    hdew += h_offset;

    raster = addr_mode >> 1;

    line_cnt_ini = out_i & field;
    line_cnt_step = ( out_i << 1 ) + !out_i;

    line_cnt = v_offset + line_cnt_ini;

    checksum = 0;

    for ( v = 0; v < vdew; v++ )
    {
        //find line initial address
        y_addr_line     = line_cnt;
        y_adr_mul_line  = ( raster ) ? y_addr_line : ( y_addr_line >> 5 ) << 5;
        y_mul_h         = y_adr_mul_line * ( line_ofst >> 2 );
        y_mul_l         = y_adr_mul_line * ( line_ofst & 0x3 );
        y_adr_blk_ofst  = ( addr_mode == 0 ) ? ( ( y_addr_line & 0x1f ) << 2 ) + ( ( y_mul_l >> 7 ) << 7 ) + ( ( y_mul_l >> 5 ) & 0x3 ) :
                          ( addr_mode == 1 ) ? ( y_addr_line & 0x1f ) + y_mul_l :
                                               y_mul_l;
        blkry_addr_ini  = ( y_mul_h << 2 ) + y_adr_blk_ofst;
        blkry_addr_org  = blkry_addr_ini;

        c_addr_line     = ( m_422 )  ? line_cnt :
                          ( m_420i ) ? ( ( line_cnt >> 2 ) << 1 ) + ( line_cnt & 0x1 ) :
                                       line_cnt >> 1;
        c_adr_mul_line  = ( raster ) ? c_addr_line : ( c_addr_line >> 4 ) << 4;
        c_mul_h         = c_adr_mul_line * ( line_ofst >> 2 );
        c_mul_l         = c_adr_mul_line * ( line_ofst & 0x3 );
        c_adr_blk_ofst  = ( addr_mode == 0 ) ? ( ( c_addr_line & 0xf ) << 2 ) + ( ( c_mul_l >> 6 ) << 6 ) + ( ( c_mul_l >> 4 ) & 0x3 ) :
                          ( addr_mode == 1 ) ? ( c_addr_line & 0xf ) + c_mul_l :
                                               c_mul_l;
        blkrc_addr_ini  = ( c_mul_h << 2 ) + c_adr_blk_ofst;
        blkrc_addr_org  = blkrc_addr_ini;

        for ( h = 0; h < hdew; h += 16 )
        {
            //swap
            blkry_addr      = ( addr_swap == 0 ) ? (      ( blkry_addr_org >> 7 )            << 7 ) +
                                                   (    ( ( blkry_addr_org >> 2 ) & 0x1  )   << 6 ) +
                                                   (    ( ( blkry_addr_org >> 3 ) & 0xf  )   << 2 ) +
                                                        (   blkry_addr_org        & 0x3  )          :

                              ( addr_swap == 1 ) ? (      ( blkry_addr_org >> 8 )            << 8 ) +
                                                   ( ( !( ( blkry_addr_org >> 7 ) & 0x1  ) ) << 7 ) +
                                                        (   blkry_addr_org        & 0x7f )          :

                              ( addr_swap == 5 ) ? (      ( blkry_addr_org >> 6 )            << 6 ) +
                                                   (    (   blkry_addr_org        & 0x1  )   << 5 ) +
                                                   (    ( ( blkry_addr_org >> 2 ) & 0x1  )   << 4 ) +
                                                   (    ( ( blkry_addr_org >> 1 ) & 0x1  )   << 3 ) +
                                                        ( ( blkry_addr_org >> 3 ) & 0x7  )          :

                              ( addr_swap == 6 ) ? (      ( blkry_addr_org >> 5 )            << 5 ) +
                                                   (    (   blkry_addr_org        & 0x1  )   << 4 ) +
                                                        ( ( blkry_addr_org >> 1 ) & 0xf  )          :

                                                            blkry_addr_org;

            blkrc_addr      = ( addr_swap == 0 ) ? (      ( blkrc_addr_org >> 6 )            << 6 ) +
                                                   (    ( ( blkrc_addr_org >> 2 ) & 0x1  )   << 5 ) +
                                                   (    ( ( blkrc_addr_org >> 3 ) & 0x7  )   << 2 ) +
                                                        (   blkrc_addr_org        & 0x3  )          :

                              ( addr_swap == 1 ) ? (      ( blkrc_addr_org >> 7 )            << 7 ) +
                                                   ( ( !( ( blkrc_addr_org >> 6 ) & 0x1  ) ) << 6 ) +
                                                        (   blkrc_addr_org        & 0x3f )          :

                              ( addr_swap == 5 ) ? (      ( blkrc_addr_org >> 6 )            << 6 ) +
                                                   (    (   blkrc_addr_org        & 0x1  )   << 5 ) +
                                                   (    ( ( blkrc_addr_org >> 2 ) & 0x1  )   << 4 ) +
                                                   (    ( ( blkrc_addr_org >> 1 ) & 0x1  )   << 3 ) +
                                                        ( ( blkrc_addr_org >> 3 ) & 0x7  )          :

                              ( addr_swap == 6 ) ? (      ( blkrc_addr_org >> 5 )            << 5 ) +
                                                   (    (   blkrc_addr_org        & 0x1  )   << 4 ) +
                                                        ( ( blkrc_addr_org >> 1 ) & 0xf  )          :

                                                            blkrc_addr_org;

            //read 16 pixels
            for ( i = 0; i < 16; i++ )
            {
                if ( h + i < hdew )
                {
                    y[v][h + i] = y_dram[( blkry_addr << 4 ) + i];
                    c[v][h + i] = c_dram[( blkrc_addr << 4 ) + i];
                }
            }

            //dram checksum
            if ( h >= ( ( h_offset >> 4 ) << 4 ) )
            {
                checksum = dram_checksum( checksum, y_dram, blkry_addr << 4 );

                if ( m_422 | ( !m_422 & !( v & 0x1 ) ) )
                {
                    checksum = dram_checksum( checksum, c_dram, blkrc_addr << 4 );
                }
            }

            //update address
            y_addr_step     = ( addr_mode == 0 ) ? ( ( blkry_addr_org & 0x3 ) == 0x3 ) ? 0x7d : 0x1 :
                              ( addr_mode == 1 ) ? 0x20 :
                                                   0x1;
            blkry_addr_org  = blkry_addr_org + y_addr_step;

            c_addr_step     = ( addr_mode == 0 ) ? ( ( blkrc_addr_org & 0x3 ) == 0x3 ) ? 0x3d : 0x1 :
                              ( addr_mode == 1 ) ? 0x10 :
                                                   0x1;
            blkrc_addr_org  = blkrc_addr_org + c_addr_step;
        }

        //update line count
        line_cnt += line_cnt_step;
    }

    //remove h_offset part
    hdew -= h_offset;

    if ( h_offset > 0 )
    {
        for ( v = 0; v < vdew; v++ )
        {
            for ( h = 0; h < hdew; h++ )
            {
                y[v][h] = y[v][h_offset + h];
                c[v][h] = c[v][h_offset + h];
            }
        }
    }
    
    #if 0
    //mirror/flip
    if ( mirror || flip )
    {
        int h_tmp;
        int v_tmp;

        unsigned char **y_tmp;
        unsigned char **c_tmp;

        y_tmp = (unsigned char **) calloc( vdew, sizeof(unsigned char *) );
        c_tmp = (unsigned char **) calloc( vdew, sizeof(unsigned char *) );
        for ( v = 0; v < vdew; v++ )
        {
            y_tmp[v] = (unsigned char *) calloc( hdew, sizeof(unsigned char) );
            c_tmp[v] = (unsigned char *) calloc( hdew, sizeof(unsigned char) );
        }

        for ( v = 0; v < vdew; v++ )
        {
            v_tmp = ( flip ) ? vdew - 1 - v : v;

            for ( h = 0; h < hdew; h += 2 )
            {
                h_tmp = ( mirror ) ? hdew - 1 - h : h;

                if ( mirror )
                {
                    y_tmp[v_tmp][h_tmp]     = y[v][h];
                    y_tmp[v_tmp][h_tmp - 1] = y[v][h + 1];

                    c_tmp[v_tmp][h_tmp - 1] = c[v][h];
                    c_tmp[v_tmp][h_tmp]     = c[v][h + 1];
                }
                else
                {
                    y_tmp[v_tmp][h_tmp]     = y[v][h];
                    y_tmp[v_tmp][h_tmp + 1] = y[v][h + 1];

                    c_tmp[v_tmp][h_tmp]     = c[v][h];
                    c_tmp[v_tmp][h_tmp + 1] = c[v][h + 1];
                }
            }
        }

        for ( v = 0; v < vdew; v++ )
        {
            for ( h = 0; h < hdew; h++ )
            {
                y[v][h] = y_tmp[v][h];
                c[v][h] = c_tmp[v][h];
            }
        }
    }
    #endif
    
    //range map
    for ( v = 0; v < vdew; v++ )
    {
        for ( h = 0; h < hdew; h++ )
        {
            y[v][h] = range_map( y[v][h], r_map_y );
            c[v][h] = range_map( c[v][h], r_map_c );
        }
    }
     
    //RealD tag removal
    if ( tag_removal )
    {
        int replace_y;
        int replace_c;

        replace_y = ( enh_layer ) ? 0x7e : 0x10;
        replace_c = 0x80;

        if ( out_i )
        {
            if ( flip )
            {
                for ( h = 0; h < hdew; h++ )
                {
                    y[0][h] = replace_y;
                    c[0][h] = replace_c;
                }
            }
            else
            {
                for ( h = 0; h < hdew; h++ )
                {
                    y[vdew - 1][h] = replace_y;
                    c[vdew - 1][h] = replace_c;
                }
            }
        }
        else
        {
            if ( flip )
            {
                for ( h = 0; h < hdew; h++ )
                {
                    y[0][h] = replace_y;
                    c[0][h] = replace_c;

                    y[1][h] = replace_y;
                    c[1][h] = replace_c;
                }
            }
            else
            {
                for ( h = 0; h < hdew; h++ )
                {
                    y[vdew - 1][h] = replace_y;
                    c[vdew - 1][h] = replace_c;

                    y[vdew - 2][h] = replace_y;
                    c[vdew - 2][h] = replace_c;
                }
            }
        }
    }
}

static void fill_yc_ext_10bit(char *y_422, char * uv_422, int pitch)
{
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;
    unsigned char *buff;
    int v,h;
    buff = y_dram;


    if (!y_422 || !uv_422 )
    {
        Printf("fill yc fail \n");
        return;
    }

    //422_y

    hFile = emu_fopen(y_422, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", y_422);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)buff, u4Size);
    emu_fclose(hFile);

    // change to y
    for ( v = 0; v < vdew; v++ )
    {
        for (h = 0; h < pitch; h++)
        {
            y_w[v][h] = buff[v*pitch*2 + h*2 + 1];
            y_w[v][h] = (y_w[v][h] << 2);
            y_w[v][h] = y_w[v][h] + ((buff[v*pitch*2 + h*2 ] & 0xc0) >> 6);
        }
    }

    
    // 422_uv
    buff = y_dram;
    hFile = emu_fopen(uv_422, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", uv_422);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)buff, u4Size);
    emu_fclose(hFile);

    // change to uv
    for ( v = 0; v < vdew; v++ )
    {
        for (h = 0; h < pitch; h++)
        {
            c_w[v][h] = buff[v*pitch*2 + h*2 + 1];
            c_w[v][h] = (c_w[v][h] << 2);
            c_w[v][h] = c_w[v][h] + ((buff[v*pitch*2 + h*2] & 0xc0) >> 6);
        }
    }


    if (is_fixed_y_color)
    {
        for (v = 0; v < vdew; v++)
            for (h = 0; h < pitch; h++)
                y_w[v][h] = y_color;
    }

    if (is_fixed_c_color)
    {
        for (v = 0; v < vdew; v++)
            for (h = 0; h < pitch; h++)
                c_w[v][h] = c_color;
    }

}

static void fill_yc_ext(char *y_422, char * uv_422, int pitch)
{
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;
    unsigned char *buff;
    int v,h;
    buff = y_dram;


    if (!y_422 || !uv_422 )
    {
        Printf("fill yc fail \n");
        return;
    }


    if (bit_depth == 10)
    {
        fill_yc_ext_10bit(y_422, uv_422, pitch);  
        return; 
    }

    //422_y

    hFile = emu_fopen(y_422, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", y_422);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)buff, u4Size);
    emu_fclose(hFile);

    // change to y
    for ( v = 0; v < vdew; v++ )
    {
        x_memcpy(y[v], buff, pitch);
        buff = buff + pitch;
    }

    
    // 422_uv
    buff = y_dram;
    hFile = emu_fopen(uv_422, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", uv_422);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)buff, u4Size);
    emu_fclose(hFile);

    // change to uv
    for ( v = 0; v < vdew; v++ )
    {
        x_memcpy(c[v], buff, pitch);
        buff = buff + pitch;
    }


    if (is_fixed_y_color)
    {
        for (v = 0; v < vdew; v++)
            for (h = 0; h < pitch; h++)
                y[v][h] = y_color;
    }

    if (is_fixed_c_color)
    {
        for (v = 0; v < vdew; v++)
            for (h = 0; h < pitch; h++)
                c[v][h] = c_color;
    }

}


//create dram
static void create_dram(void)
{
    //check file existence
    int i,j;

    //decide read size
    int full_h;
    int y_read_w;
    int y_read_h;
    int c_read_w;
    int c_read_h;
    int y_read_size;
    int c_read_size;

    full_h = ( ( out_i ) ? vdew << 1 : vdew ) + v_offset;

    y_read_w = line_ofst << 4;
    y_read_h = ( full_h & 0x1f ) ? ( ( full_h >> 5 ) + 1 ) << 5 : full_h;
    y_read_size = y_read_w * y_read_h;

    c_read_w = y_read_w;
    c_read_h = ( m_422 ) ? y_read_h : y_read_h >> 1;
    c_read_size = c_read_w * c_read_h;

    Printf("%s\n", __FUNCTION__);

    dram_size = y_read_size;

    //fill y_dram
    for (i = 0; i < y_read_h; i++)
        for(j = 0; j < y_read_w; j++)
            y_dram[i*y_read_w + j] = (is_fixed_y_color? y_color : (i+j));

    HalFlushDCacheMultipleLine((UINT32)y_dram, y_read_size);


    //fill c_dram
    for (i = 0; i < c_read_h; i++)
        for(j = 0; j < c_read_w; j++)
            c_dram[i*c_read_w + j] = (is_fixed_c_color? c_color : (i+j));

    HalFlushDCacheMultipleLine((UINT32)c_dram, c_read_size);


}


static void create_dram_ext(char *raw_y, char * raw_c)
{
    EMU_FILE_HANDLE hFile;
    UINT32 u4Size;

    if (!raw_y || !raw_c )
    {
        Printf("create dram fail \n");
        return;
    }

    hFile = emu_fopen(raw_y, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", raw_y);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)y_dram, u4Size);
    emu_fclose(hFile);
    HalFlushDCacheMultipleLine((UINT32)y_dram, u4Size);

    hFile = emu_fopen(raw_c, EMU_SRC_ICE);
    if (EMU_FILE_HANDLE_NULL == hFile)
    {
        Printf("Open file fail: %s\n", raw_c);
        return;
    }
    u4Size = emu_fsize(hFile);
    emu_fread(hFile, (CHAR*)c_dram, u4Size);
    emu_fclose(hFile);
    HalFlushDCacheMultipleLine((UINT32)c_dram, u4Size);

}




//get config
static void default_config(void)
{
    unsigned char  * pbbuff;
    unsigned short * psbuff;
    unsigned int   uiAddr;
    int i;

    hdew        = 720;
    vdew        = 480;
    line_ofst   = 45;   //need *16
    addr_mode   = 1;
    addr_swap   = 2;
    m_420i      = 0;
    out_i       = 0;
    field       = 0;
    m_422       = 0;
    mirror      = 0;
    flip        = 0;
    h_offset    = 0;
    v_offset    = 0;
    r_map_y     = 8;
    r_map_c     = 8;
    tag_removal = 0;
    enh_layer   = 0;

#ifdef B2R_FBM_CFG
    {
    FBM_POOL_T *prFbmPool;
    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_MPEG2);
    cmodel_buff = (unsigned char *)VIRTUAL(prFbmPool->u4Addr);   
    }
#else
    //static unsigned char **y;
    //static unsigned char **c;
    cmodel_buff = (unsigned char *)VIRTUAL(0x6000000);   
#endif

    pbbuff = cmodel_buff;
    //read dram
    y_dram = pbbuff;
    c_dram = y_dram + 3840*2176;
    pbbuff = c_dram + 3840*2176/2;
    psbuff = (unsigned short *)pbbuff;

    //other
    is_fixed_y_color = 0;
    is_fixed_c_color = 0;
    bit_depth = 8;

    //y & c buffer
    if (!y)
    {
        uiAddr = BSP_AllocAlignedDmaMemory(2176* sizeof(unsigned char *), 4);
        y   = (unsigned char **)VIRTUAL(uiAddr);
        y_w = (unsigned short **)y;
    }

    if (!c)
    {
        uiAddr = BSP_AllocAlignedDmaMemory(2176* sizeof(unsigned char *), 4);
        c = (unsigned char **)VIRTUAL(uiAddr);
        c_w = (unsigned short **)c;
    }

    for (i = 0; i < 2176; i++)
    {
        y[i]   = pbbuff;
        c[i]   = y[i] + 3840;
        pbbuff = c[i] + 3840;

        y_w[i] = (unsigned short *)y[i];
        c_w[i] = (unsigned short *)c[i];
    }

    if (((UINT32)pbbuff > 0x8000000))
    {
        Printf("memeory over flow! \n");
    }

}

//get crc
static int b2r_cmodel_get_crc_10bit(void)
{
    int v, h;
    int crc;
    int data;

    crc = 0;

    for ( v = 0; v < vdew; v++ )
    {
        //Printf("\nrow:%3d\n", v);    
        for ( h = 0; h < hdew; h++ )
        {
            //IEEE 802.3 CRC-32
            data = ( y_w[v][h] << 10 ) + c_w[v][h];
            crc = CRC32_D32( crc, ( y_w[v][h] << 10 ) + c_w[v][h] );
            //Printf("%3x<%5x> ", y[v][h], data);    
        }
    }

    return crc;
}
/*
int b2r_cmodel_get_crc(void)
{
    int v, h;
    int crc;
    int data;
    if (bit_depth == 10)
    {
        return b2r_cmodel_get_crc_10bit();
    }

    crc = 0;

    for ( v = 0; v < vdew; v++ )
    {
        //Printf("\nrow:%3d\n", v);    
        for ( h = 0; h < hdew; h++ )
        {
            //IEEE 802.3 CRC-32
            if (is_new)
            {
                data = ( y[v][h] << 12 ) + (c[v][h]<<2);
                crc = CRC32_D32( crc, ( y[v][h] << 12 ) + (c[v][h]<<2) );
            }
            else
            {
                data = ( y[v][h] << 8 ) + c[v][h];
                crc = CRC32_D32( crc, ( y[v][h] << 8 ) + c[v][h] );
            }
            //Printf("%3x<%5x> ", y[v][h], data);    
        }
    }

    return crc;
}
*/
  int b2r_cmodel_get_crc(void)
    {
        int v, h;
        int crc;
        int data;
        int h_tmp;
        int v_tmp;
        if (bit_depth == 10)
        {
            return b2r_cmodel_get_crc_10bit();
        }
    
        crc = 0;
    
        for ( v = 0; v < vdew; v++ )
        {
            //Printf("\nrow:%3d\n", v);    
              v_tmp = ( flip ) ? vdew - 1 - v : v;
            for ( h = 0; h < hdew; h++ )
            {
                //IEEE 802.3 CRC-32
                h_tmp = ( mirror ) ? hdew - 1 - h : h;
                if (is_new)
                {
                    data = ( y[v][h] << 12 ) + (c[v][h]<<2);
                    crc = CRC32_D32( crc, ( y[v][h] << 12 ) + (c[v][h]<<2) );
                }
                else
                {
                     data=  ( y[v_tmp][h_tmp] << 8 ) + c[v_tmp][h_tmp];
                
                     crc = CRC32_D32( crc, ( y[v_tmp][h_tmp] << 8 ) + c[v_tmp][h_tmp] );
                   //  Printf(" y[%d][%d]=%x,c[v_tmp][h_tmp]=%x\n",v_tmp,h_tmp, y[v_tmp][h_tmp],c[v_tmp][h_tmp]);
                }
            }
        }
    
        return crc;
    }

static int b2r_cmodel_get_field_crc_10bit(int top)
{
    int v, h;
    int crc;
    int idx;

    crc = 0;
    idx = top ? 0 : 1;

    for ( v = 0; v < vdew; v ++ )
    {
        //Printf("\nrow:%3d\n", v);    
        for ( h = 0; h < hdew; h++ )
        {
            //IEEE 802.3 CRC-32
            crc = CRC32_D32( crc, ( y_w[(v<<1) + idx][h] << 10 ) + c_w[(v<<1) - (v%2) + (idx<<1)][h] );
            //Printf("%2x ", y[v][h]);    
        }
    }

    return crc;
}

int b2r_cmodel_get_field_crc(int top)
{
    int v, h;
    int crc;
    int idx;

    if (bit_depth == 10)
    {
        return b2r_cmodel_get_field_crc_10bit(top);
    }

    crc = 0;
    idx = top ? 0 : 1;

    for ( v = 0; v < vdew; v ++ )
    {
        //Printf("\nrow:%3d\n", v);    
        for ( h = 0; h < hdew; h++ )
        {
            //IEEE 802.3 CRC-32
            if (is_new)
            {
                crc = CRC32_D32( crc, ( y[(v<<1) + idx][h] << 12 ) + (c[(v<<1) - (v%2) + (idx<<1)][h]<<2) );
            }
            else
            {
                crc = CRC32_D32( crc, ( y[(v<<1) + idx][h] << 8  ) + (c[(v<<1) - (v%2) + (idx<<1)][h]<<0) );
            }
            //Printf("%4x<%4d> ", y[v][h], data);    
        }
    }

    return crc;
}



//set test parmeter
void b2r_cmodel_set_parm(int _hdew, int _vdew)
{
    hdew = _hdew;
    vdew = _vdew;
}

void b2r_cmodel_set_max_hv(unsigned int max_hdew, unsigned int max_vdew, unsigned int pitch)
{
    hdew      = max_hdew;
    vdew      = max_vdew;
    line_ofst = pitch/16;
}

void b2r_cmodel_set_color(unsigned int _y_color, unsigned int _c_color)
{
    if (_y_color < 1024)
    {
        is_fixed_y_color = 1;
        y_color = _y_color;
    }

    if (_c_color < 1024)
    {
        is_fixed_c_color = 1;
        c_color = _c_color;
    }
}

void b2r_cmodel_set_crc_mode(int isOld)
{
    is_new = !isOld;
}

void b2r_cmodel_set_bit_depth(int depth)
{
    bit_depth = depth;
}

void b2r_cmodel_get_addr(unsigned int *_y_dram, unsigned int *_c_dram)
{
    *_y_dram    = (unsigned int)y_dram;
    *_c_dram    = (unsigned int)c_dram;
}

void b2r_cmodel_get_len_addr(unsigned int *_y_len, unsigned int *_c_len)
{
    *_y_len    = (unsigned int)y_dram;
    *_c_len    = (unsigned int)c_dram;
}


void b2r_cmodel_init(void)
{
    default_config();
}

void b2r_cmodel_do(void)
{
    create_dram();
    fill_yc();
}

void b2r_cmodel_do_ext(char *raw_y, char * raw_c, char *y_422, char * uv_422, int pitch)
{
    if (y_422 && uv_422)
    {
        fill_yc_ext(y_422, uv_422, pitch);
    }
    
    create_dram_ext(raw_y, raw_c);

#if 1//just for test
    if (!(y_422 && uv_422))
    {
        fill_yc();
    }
#endif    
}


