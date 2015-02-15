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
 * $RCSfile: dlm_gentool.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision:  $
 * $SWAuthor:  $
 * $MD5HEX:  $
 *
 * Description:
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_dlm.h"
#include "dlm_digest.h"
#include "dlm_fm.h"
#include "dlm_util.h"
#include "dlm.h"
#include "bigd.h"
#include "assert.h"

#ifndef DLM_AES_ALGORITHM_ENCRYPT
#define DLM_AES_ALGORITHM_ENCRYPT
#endif

#ifdef DLM_AES_ALGORITHM_ENCRYPT
#include "dlm_aes.h"
#else
#include "efuse.h"
#endif

static UCHAR uac_internal_buf[X_INTERNAL_BUFFER_SIZE]; /* this code should run in linux or windows */
static UINT32 ui4_iv = 0;
extern char gToolDir[1024];
extern UCHAR aes_key[16];
extern UCHAR aes_iv[16];
extern BIGD n_vendor, e_vendor, d_vendor, encryptedSignature, signature;
extern BOOL use_customer_aes_key;
extern BOOL use_customer_rsa_key;
/*-----------------------------------------------------------------------------
 * Name: x_dlm_generate_hash
 *
 * Description: Compute codefile hash code
 *
 * Inputs:  filename, the file to compute hash code
 * Outputs: digest, the hash result
 * Returns: DLM_OK, compute hash done
 *          DLM_OPEN_FILE_FAILED, open file failed
 ----------------------------------------------------------------------------*/
INT32 x_dlm_generate_hash(CHAR *filename, UCHAR *digest)
{
    INT32 in_file;
    INT32 i4_size;
#ifdef DLM_GCPU_SHA_256	
    INT32 i4_tmp_size;
    INT32 i4_adjust_size;
#endif	

    if ((in_file = dlm_fopen(filename, kDlm_Read)) == -1)
    {
        dlm_log("Open file for hash module failed\n");
        return DLM_OPEN_FILE_FAILED;
    }
	dlm_digest_init();

    while (!dlm_feof(in_file))
    {
        i4_size = dlm_fread(in_file, (CHAR*)uac_internal_buf, X_INTERNAL_BUFFER_SIZE);

        if (i4_size != -1)
        {   
#ifdef DLM_GCPU_SHA_256
			if ((i4_size % 64) && !dlm_feof(in_file))
			{
				/* adjust i4_len len to 64 bytes alignment */
				i4_adjust_size = (64 - i4_size % 64);		
				i4_tmp_size = dlm_fread(in_file, (CHAR*)(uac_internal_buf + i4_size), i4_adjust_size);
				dlm_digest_update(uac_internal_buf, (UINT32)(i4_size + i4_tmp_size));
			}
			else
#endif				
			{
				dlm_digest_update(uac_internal_buf,i4_size);
			}
        }
    }
    dlm_digest_final(digest);

    dlm_fclose(in_file);
    return DLM_OK; 
}

#ifndef DLM_AES_ALGORITHM_ENCRYPT
/*-----------------------------------------------------------------------------
 * Name: x_dlm_update_signature
 *
 * Description: This API let build codefile utility to patch its signature
 *
 * Inputs:  file, the file to update signature
 *          p_sig, the signature to fill into file
 * Outputs: NA
 * Returns: DLM_OK, update ok
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_DEL_FILE_FAILED, remove file failed
 *          DLM_RENAME_FILE_FAILED, rename filename failed
 ----------------------------------------------------------------------------*/

INT32 x_dlm_update_signature(CHAR *file, CHAR *p_sig)
{
    INT32 in_file, out_file;
    INT32 i4_cnt = sizeof (DEFAULT_CODE_FILE_HDR_T) - 32;

    if ((in_file = dlm_fopen (file, kDlm_Read)) == -1)
    {
        dlm_log("Open %s failed\n", file);
        return DLM_OPEN_FILE_FAILED;
    }

    if ((out_file = dlm_fopen ("/mnt/ms/_up_sig.tmp", kDlm_Write)) == -1)
    {
        dlm_log("open _up_sig.tmp failed\n");
        return DLM_OPEN_FILE_FAILED;
    }

    /* copy common header */
    dlm_fread(in_file, (CHAR*)uac_internal_buf, i4_cnt);
    dlm_fwrite(out_file, (CHAR*)uac_internal_buf, i4_cnt);

    /* reinitialize vector */
    ui4_iv = DLM_APPEND_DATA_IV;

    /* encrypted digest */
    /* dlm_dump_buffer("x_dlm_update_signature key", uac_internal_buf, 4); */
    dlm_encrypt_efuse((UCHAR *)p_sig, 32, *((UINT32 *)uac_internal_buf), &ui4_iv);
    dlm_dump_buffer("Encrypted digest:", (UCHAR*)p_sig, 32);

    /* update digest */
    dlm_fwrite(out_file, (CHAR*)p_sig, 32);
    /* seek space */
    dlm_fread(in_file, (CHAR*) uac_internal_buf, 32);

    /* copy remainder data */
    while (!dlm_feof(in_file))
    {
        i4_cnt = dlm_fread(in_file, (CHAR*)uac_internal_buf, X_INTERNAL_BUFFER_SIZE);
        if (i4_cnt != -1)
        {
            dlm_fwrite(out_file, (CHAR*)uac_internal_buf, i4_cnt);
            dlm_log(".");
        }          
    }          
    dlm_log("\n");

    dlm_fclose(in_file);
    dlm_fclose(out_file);

    /* update filename */
    if (dlm_delete_file(file)==-1)
    {
        dlm_log("Remove file %s failed\n", file);
        return DLM_DEL_FILE_FAILED;
    }

    if (dlm_rename_file("/mnt/ms/_up_sig.tmp", file) == -1)
    {
        dlm_log("Rename file %s failed\n", file);
        return DLM_RENAME_FILE_FAILED;
    }

    return DLM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlm_add_append_file
 *
 * Description: Add user specify data to codefile
 *
 * Inputs:  pt_append, user defined data
 *          file, the file describe
 *          ui4_key, the key to encrypt data
 * Outputs: NA
 * Returns: DLM_OK, add data to append file ok
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_WRITE_DATA_FAILED, write data failed
 ----------------------------------------------------------------------------*/
INT32 dlm_add_append_file(Append_T *pt_append, INT32 file, UINT32 ui4_key)
{
    INT32 in_file;
    CHAR buf[1024];
    CHAR s_compress_name[DLM_MAX_PATH_LENGTH];
    INT32 compress_file;
    UINT32 ui4_val_len;
    INT32 compress_len = 0;
    INT32 index = 0; //for debug
	INT32 i4_status;

    while (pt_append)
    {
        ui4_val_len = dlm_file_size(pt_append->ps_filename);

        /* reinitialize vector */
        ui4_iv = DLM_APPEND_DATA_IV;

        in_file = dlm_fopen (pt_append->ps_filename, kDlm_Read);
        if (in_file == -1)
        {
            dlm_log("Open append data file %s failed\n", pt_append->ps_filename);
            return DLM_OPEN_FILE_FAILED;
        }

        pt_append->ui4_length = ui4_val_len;

        if (dlm_fwrite(file, (CHAR*)pt_append->uc_type, 4) == -1)
        {
            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
            return DLM_WRITE_DATA_FAILED;
        }
#if 0
        if (dlm_fwrite(file, (CHAR*)&pt_append->uac_privacy, 4) == -1)
        {
            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
            return DLM_WRITE_DATA_FAILED;
        }

        if (dlm_fwrite(file, (CHAR*)&(pt_append->ui4_length), 4) == -1)
        {
            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
            return DLM_WRITE_DATA_FAILED;
        }
#endif
        memset(s_compress_name, 0, DLM_MAX_PATH_LENGTH);

        /*for compress*/
        if(pt_append->uac_privacy[1] && pt_append->ui4_length >= COMPRESS_UNIT_LENGTH)
        {
            UCHAR*   tmp_buf = NULL;
            tmp_buf = (UCHAR*)x_mem_alloc(COMPRESS_UNIT_LENGTH);
            if(tmp_buf == NULL)
            {
                dlm_log("Allocate memory fail for compress, so do not do compressing\n");
                return DLM_ALLOCATE_MEMORY_FAILED;
            }
            compress_len = 0;
            index = 0;
           
            strcpy(s_compress_name, pt_append->ps_filename);
            strcat(s_compress_name, ".compress");
            //dlm_log("original file: %s, compress file name: %s\n", pt_append->ps_filename, s_compress_name);
            compress_file = dlm_fopen(s_compress_name, kDlm_Write);
            if (compress_file == -1)
            {
                dlm_log("Open compress file %s failed\n", s_compress_name);
                return DLM_OPEN_FILE_FAILED;
            }
            else
            {   
                /*do not compress first X bytes*/
                ui4_val_len = dlm_fread(in_file, tmp_buf, UNCOMPRESS_LENGTH);
                if (ui4_val_len > 0)
                {
                    if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, ui4_val_len) == -1)
                    {
                        dlm_log("write first 512bytes data %s failed\n", pt_append->ps_filename);
                        return DLM_WRITE_DATA_FAILED;
                    }
                }
                else
                {
                    dlm_log("read first 512bytes data %s failed\n", pt_append->ps_filename);
                    return DLM_WRITE_DATA_FAILED;
                }
                //dlm_log("uncompressed file length: %d\n", i4_val_len);

                /*Compress remain data  Y bytes as Unit*/
                while(!dlm_feof(in_file))
                {
                    INT32   i4_compress_sub_length = 0;
                    
                    index++;
                    /*read Y bytes data from file and write to temporary file which is input file when doing compress operation*/
                    i4_compress_sub_length = dlm_fread(in_file, tmp_buf, COMPRESS_UNIT_LENGTH);
                    if (i4_compress_sub_length > 0)
                    {
                        INT32 compress_in_file = -1;
                        INT32 compress_out_file = -1;
                        //dlm_log("Compress(index:%d, original length: %d, ", index, i4_compress_sub_length);
                        compress_in_file = dlm_fopen("compress.in", kDlm_Write);
                        if (compress_file == -1)
                        {
                            dlm_log("Open compress.tmp failed\n");
                            return DLM_WRITE_DATA_FAILED;
                        } 

                        if (dlm_fwrite(compress_in_file, (CHAR*)tmp_buf, i4_compress_sub_length) == -1)
                        {
                            dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
                        dlm_fclose(compress_in_file);
                        
                        /*compress data using compress tool*/
                        char cmd[2*DLM_MAX_PATH_LENGTH];
                        char outfile[DLM_MAX_PATH_LENGTH];
                        memset(cmd, 0, 2*DLM_MAX_PATH_LENGTH);
                        memset(outfile, 0, DLM_MAX_PATH_LENGTH);
                        sprintf(outfile, "%s.compress%d", pt_append->ps_filename, index);
                        sprintf(cmd, "%s/lzhsenc compress.in %s", gToolDir, outfile);
						DLM_TRACE(("system(lzhsenc) cmd = %s\n", cmd));
                        i4_status = system(cmd);
						if(-1 == i4_status)
						{
							dlm_log("system(lzhsenc) error!\n");
						}
						else
						{ 
							DLM_TRACE(("exit status value = [0x%x]\n", i4_status));
							if(WIFEXITED(i4_status))
							{ 
								if(0 == WEXITSTATUS(i4_status))
								{ 
									dlm_log("run lzhsenc successfully.\n");
								}
								else
								{ 
									dlm_log("run lzhsenc fail, exit code: %d\n", WEXITSTATUS(i4_status));
								}
							}
							else
							{
								dlm_log("exit status = [%d]\n", WEXITSTATUS(i4_status));
							}  
						}
                        remove("compress.in");
                        
                        /*read compressed data and write to temporary file and later packing to target packed file*/
                        compress_out_file = dlm_fopen(outfile, kDlm_Read);
                        if (compress_file == -1)
                        {
                            dlm_log("Open compress.tmp failed\n");
                            return DLM_WRITE_DATA_FAILED;
                        } 

                    #if 0
                        //read compressed file header and get the the compressed data length
                        i4_val_len = dlm_fread(compress_out_file, tmp_buf, 0x10);            
                        if(i4_val_len <= 0 )
                        {
                            dlm_log("read compress header %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }

                        dlm_dump_buffer("Compressed File Header:", tmp_buf, 0x10);
                        int tmp_length = (int)((tmp_buf[7] << 24) + (tmp_buf[6] << 16) + (tmp_buf[5] << 8) + tmp_buf[4]);
                        dlm_dump_buffer("length:", (UCHAR*)&tmp_length, 4);

                        tmp_length += 0x10;
                        //write compressed data length to output temporary file
                        if (dlm_fwrite(compress_file, (UCHAR*)&tmp_length, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }

                        //write compressed data header to output temporary file
                        if (dlm_fwrite(compress_file, tmp_buf, 0x10) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }

                        //write compress data to output temporary file
                        int remain_length = tmp_length - 0x10;
                        while(remain_length > 0)
                        {
                            if(remain_length >= COMPRESS_UNIT_LENGTH)
                            {
                                i4_val_len = dlm_fread(compress_out_file, tmp_buf, COMPRESS_UNIT_LENGTH); 
                            }
                            else
                            {
                                i4_val_len = dlm_fread(compress_out_file, tmp_buf, remain_length); 
                            }
                            if(i4_val_len > 0)
                            {
                                remain_length -= i4_val_len;
                                if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, i4_val_len) == -1)
                                {
                                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                                    return DLM_WRITE_DATA_FAILED;
                                }
                            }
                        }
                    #else
                        //write sub compress data length
                        if (dlm_fwrite(compress_file, (UCHAR*)&i4_compress_sub_length, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
                        
                        //read compressed file header and get the the compressed data length                     
                        int tmp_length = dlm_file_size(outfile);
                        //dlm_log("compress length: %d)\n", tmp_length);
                        
                        //write compressed data length to output temporary file
                        if (dlm_fwrite(compress_file, (UCHAR*)&tmp_length, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }

                        //write compress data to output temporary file
                        while(!dlm_feof(compress_out_file))
                        {
                            ui4_val_len = dlm_fread(compress_out_file, tmp_buf, COMPRESS_UNIT_LENGTH); 
                            
                            if(ui4_val_len > 0)
                            {
                                if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, ui4_val_len) == -1)
                                {
                                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                                    return DLM_WRITE_DATA_FAILED;
                                }
                            }
                        }
                    #endif

                        fclose(compress_out_file);                       
                        remove(outfile);
                        //calculate total compressed data length
                        compress_len += (tmp_length + 4 + 4);
                        
                    }
                }
            }
            
            dlm_fclose (in_file);
            dlm_fclose (compress_file);
            //dlm_log("original total length: %d, compress total length: %d\n", pt_append->ui4_length, compress_len);

            if (dlm_fwrite(file, (CHAR*)&pt_append->uac_privacy, 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }

            compress_len += UNCOMPRESS_LENGTH;
            if (dlm_fwrite(file, (CHAR*)&compress_len, 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }
            
            in_file = dlm_fopen (s_compress_name, kDlm_Read);
            if (in_file == -1)
            {
                dlm_log("[%d]Open append data file %s failed\n", __LINE__, s_compress_name);
                return DLM_OPEN_FILE_FAILED;
            }
        }
        else
        {
            /*if file size is less than UNIT_LENGTH, do not compress file and set compress flag as false*/
            pt_append->uac_privacy[1] = 0;
            if (dlm_fwrite(file, (CHAR*)&pt_append->uac_privacy, 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }
            
            if (dlm_fwrite(file, (CHAR*)&(pt_append->ui4_length), 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }
        }

        /* copy append data */
        while (!dlm_feof(in_file))
        {
            ui4_val_len = dlm_fread(in_file, (CHAR*)buf, 1024);
            if (ui4_val_len != -1)
            {
                /* if append data need scramble */
                if (pt_append->uac_privacy[0])
                {
                    dlm_encrypt_efuse((unsigned char *) buf, ui4_val_len, ui4_key, &ui4_iv);
                }

                if (dlm_fwrite(file, (CHAR*)buf, ui4_val_len) == -1)
                {
                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                    return DLM_WRITE_DATA_FAILED;
                }
            }
        }

        dlm_fclose (in_file);
        if(pt_append->uac_privacy[1] && strlen(s_compress_name) != 0)
        {
            remove(s_compress_name);
        }
        pt_append = pt_append->pt_next;
    }

    return DLM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: x_dlm_generate_code_file
 *
 * Description: This API provide user to upload or generate a new code file
 *
 * Inputs:  pt_hdr, the default header
 *          pt_append, specify which data need append to codefile
 *          filename, the filnal filename of codefile
 * Outputs: no
 * Returns: DLM_OK, codefile is generated
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_WRITE_DATA_FAILED, write append data to code file failed
 ----------------------------------------------------------------------------*/
/*
 *   4       64       2      4          32                  32
 * +-----+--------+-------+--------+------------+----------------------------+
 * | ID  |Version | Lenght | Model Name | Encrypted code file digest |
 * +-----+--------+-------+--------+------------+----------------------------+
 * | Append data                                                           |
 * +-----------------------------------------------------------------------+ 
 * | Append data                                                           |
 * +-----------------------------------------------------------------------+ 
 * | ....                                                                  |
 * +-----------------------------------------------------------------------+
 * 
 * Append data header
 *    1    1       4       n
 * +----+-----+--------+---------------------------------------------------+
 * | T  |  P  |    L   |  V                                                |
 * +----+-----+--------+---------------------------------------------------+
 * T: type
 *    255 for end of append data
 * P: privacy
 *    0: value is plain text
 *    1: value is encrypted
 * L: value length
 * V: Append data 
 */
INT32 x_dlm_generate_code_file(DEFAULT_CODE_FILE_HDR_T *pt_user_hdr, Append_T *pt_append, CHAR *filename)
{
    INT32 out_file;
    UINT32 ui4_key = DLM_HDR_KEY;
    Append_T *pt_append_backup = pt_append;
    DEFAULT_CODE_FILE_HDR_T *pt_hdr = (DEFAULT_CODE_FILE_HDR_T*) uac_internal_buf;

    if ((out_file = dlm_fopen (filename, kDlm_Write)) == -1)
    {
        dlm_log("Open output codefile %s failed\n", filename);
        return DLM_OPEN_FILE_FAILED;
    }

    x_memcpy(pt_hdr, pt_user_hdr, sizeof(DEFAULT_CODE_FILE_HDR_T));

    /* accumulate total size */
    pt_hdr->ui4_total_size = sizeof( DEFAULT_CODE_FILE_HDR_T );
    while (pt_append_backup)
    {
        pt_hdr->ui4_total_size += 12;
        pt_hdr->ui4_total_size += dlm_file_size(pt_append_backup->ps_filename);
        pt_append_backup = pt_append_backup->pt_next;
    }
    dlm_log("Code size = %u\n", (unsigned int)pt_hdr->ui4_total_size);

    /* story my magic */
    x_memcpy(pt_hdr->s_magic, DLM_HDR_MAGIC, 8);

#if HDR_ENCRYPTED
    ui4_iv = DLM_HDR_IV; /* use different IV */
    /* dlm_dump_buffer("Key", (UCHAR*) pt_hdr->uac_id, 4); */
    dlm_encrypt_efuse((UCHAR *) pt_hdr, sizeof( DEFAULT_CODE_FILE_HDR_T ) - 32, ui4_key, &ui4_iv);
    dlm_dump_buffer("Encrypted Header:", (UCHAR*) pt_hdr, sizeof( DEFAULT_CODE_FILE_HDR_T ) - 32);
#endif

    if (dlm_fwrite(out_file, (CHAR*)pt_hdr, sizeof( DEFAULT_CODE_FILE_HDR_T )) == -1)
    {
        dlm_fclose (out_file);
        return DLM_WRITE_DATA_FAILED;
    }

    /* use uac_id as a key */
    if (dlm_add_append_file(pt_append, out_file, *((UINT32*) pt_user_hdr->uac_id)) != 0)
    {
        dlm_log("Add append data failed\n");
        exit(-1);
    }

    dlm_fclose (out_file);
    return DLM_OK;
} 

#else


void pr_msg(char *msg, BIGD b)
{
#if DEBUG_DLM_TRACE
    printf("%s", msg);
    bdPrint(b, 1);
#endif
}

void pr_bytesmsg(char *msg, unsigned char *bytes, size_t len)
{
    size_t i;

    printf("%s", msg);
    for (i = 0; i < len; i++)
    {
        printf("%02x", bytes[i]);
    }
    printf("\n");
}

#define KEYSIZE 2048

// p_m=input, pSignature=output, p_n=public_key, p_e=3, p_d=private_key
int RSAEncryption(BIGD p_m, BIGD pSignature, BIGD p_n, BIGD p_e, BIGD p_d)
{
#if DEBUG_DLM_TRACE
    size_t nbits = KEYSIZE; /* NB a multiple of 8 here */
#endif
    BIGD c, s, m1;
    int res;

    pr_msg("p_m=\n", p_m);
    pr_msg("p_n=\n", p_n);
    pr_msg("p_e=\n", p_e);
    pr_msg("p_d=\n", p_d);

#if DEBUG_DLM_TRACE
    printf("Test BIGDIGITS with a new %d-bit RSA key and given message data.\n", (int)nbits);
#endif

    /* Initialise */
    c = bdNew();
    s = bdNew();
    m1 = bdNew();

    /* Sign s = m^d mod n (NB m is not a valid PKCS-v1_5 signature block) */
    bdModExp(pSignature, p_m, p_d, p_n);
    pr_msg("s=\n", pSignature);

    /* Check verify m1 = s^e mod n */
    bdModExp(m1, pSignature, p_e, p_n);
    pr_msg("m'=\n", m1);
    res = bdCompare(m1, p_m);

    printf("Verification %s\n", (res == 0 ? "OK" : "FAILED!"));
    assert(res == 0);

    /* Clean up */
    bdFree(&c);
    bdFree(&s);
    bdFree(&m1);

    /* Show the current version number */
#if DEBUG_DLM_TRACE
    printf("Version=%d\n", bdVersion());
    printf("OK, successfully completed tests.\n");
#endif

    return 0;
}

/*-----------------------------------------------------------------------------
 * Name: x_dlm_update_signature
 *
 * Description: This API let build codefile utility to patch its signature
 *
 * Inputs:  file, the file to update signature
 *          p_sig, the signature to fill into file
 * Outputs: NA
 * Returns: DLM_OK, update ok
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_DEL_FILE_FAILED, remove file failed
 *          DLM_RENAME_FILE_FAILED, rename filename failed
 ----------------------------------------------------------------------------*/

INT32 x_dlm_update_signature(CHAR *file, CHAR *p_sig)
{
    INT32 in_file, out_file;
    INT32 i4_cnt = sizeof (DEFAULT_CODE_FILE_HDR_T) - DLM_HDR_SIGNATURE_SIZE;
    UINT32 ui4_key = DLM_HDR_KEY;
    UCHAR uc_key[16];
    UCHAR uc_vector[16];
    UCHAR * degist = (UCHAR*)x_mem_alloc(DLM_HDR_SIGNATURE_SIZE);

	x_memset(degist, 0, DLM_HDR_SIGNATURE_SIZE);
	x_memset(uc_vector, 0, 16);
	
	if (!use_customer_aes_key) 
	{
	    x_memcpy(uc_key, &ui4_key, sizeof(UINT32));
	    x_memcpy(uc_key + sizeof(UINT32), &ui4_key, sizeof(UINT32));
	    x_memcpy(uc_key + 2 * sizeof(UINT32), &ui4_key, sizeof(UINT32));
	    x_memcpy(uc_key + 3 * sizeof(UINT32), &ui4_key, sizeof(UINT32));
	}
	else
	{
		x_memcpy(uc_key, aes_key, 16);
		x_memcpy(uc_vector, aes_iv, 16);
	}
    if ((in_file = dlm_fopen (file, kDlm_Read)) == -1)
    {
        dlm_log("Open %s failed\n", file);
        return DLM_OPEN_FILE_FAILED;
    }

    if ((out_file = dlm_fopen ("/mnt/ms/_up_sig.tmp", kDlm_Write)) == -1)
    {
        dlm_log("open _up_sig.tmp failed\n");
        return DLM_OPEN_FILE_FAILED;
    }

    /* copy common header */
    dlm_fread(in_file, (CHAR*)uac_internal_buf, i4_cnt);
    dlm_fwrite(out_file, (CHAR*)uac_internal_buf, i4_cnt);

	x_memcpy(degist, p_sig, 32);
	dlm_dump_buffer("Source digest:", (UCHAR*)degist, DLM_HDR_SIGNATURE_SIZE);
	
//// RSA encryption enhance here   
    if (!use_customer_rsa_key)
	{	
	    dlm_aes((UCHAR *)degist,
	            NULL,
	            DLM_HDR_SIGNATURE_SIZE,
	            uc_key,
	            uc_vector,
	            AES_ENCRYPT);
	}
	else 
	{
	    bdConvFromHex(signature, "b39885b0b0b6a5e6b32b2a04eeb08f0453a0a4352ff7860e07bcde8b091223c9a8b7fdaceb354deced03cd0d9e9c95bc68f36c1bdc37c25cf3e778762d4c4f0d0e50aa9a67aab8603ea8150dbcc85b23e71a0b745599194e66543394b023288d64dd9f0078302a922dd1ccab2d730c96bac94915c03f7822ecbebe62ae76598732bb835ee7d98f4ba118295ecf622ea02b63f09caf50f46a9a72091c1c085b5e5ad3390e12e10cca976ec1ab65bfca3125444872d4085689d1931fd3d6ce05db950af2428f5229fb4f83738170974e3095a0623adb15b0857ad6129fbe4c974dd4e671aacf77d853f73a8d25a67aec287953f4df9f7bec80f4315318ada87017");
        bdConvFromHex(encryptedSignature, "77bb03cb2079c3ef221cc6adf475b4ad8d15c2ce1ffa595eafd33f075b616d311b2553c89cce33f348ad335e69bdb92845f79d67e8252c3df7efa5a41e32df5e098b1c66efc725957f1ab8b3d3303cc29a115cf83910bb89998d77b8756cc5b39893bf55a57571b6c93688721e4cb30f2730db63d57fa56c9dd47eec744ee65902c3edbd961349f7085829a9a639693651b0f2f14a7ecf6e6bcaebd5ae23e374e77b025e1a8703ceb065309fbd69aa8740ea7d1a0124547fbf6c0243d4e37be560321cdf99dcaf09ad07c3224120a9d4b55cf6ca7ddf918ef8c1da5970115e74b6d2fe30c54fba3f22a3ce824f7bec7ac91219b7f4416ecec44b3fa082ebb28b");
    		
		InFromBuffer(degist, DLM_HDR_SIGNATURE_SIZE, signature);
		RSAEncryption(signature,encryptedSignature,n_vendor,e_vendor,d_vendor);
		x_memset(degist, 0, DLM_HDR_SIGNATURE_SIZE);
		OutToBuffer(degist, DLM_HDR_SIGNATURE_SIZE, encryptedSignature);
	}

    dlm_dump_buffer("Encrypted digest:", (UCHAR*)degist, DLM_HDR_SIGNATURE_SIZE);

    /* update digest */
    dlm_fwrite(out_file, (CHAR*)degist, DLM_HDR_SIGNATURE_SIZE);
    /* seek space */
    dlm_fread(in_file, (CHAR*) uac_internal_buf, DLM_HDR_SIGNATURE_SIZE);
    
    /* copy remainder data */
    while (!dlm_feof(in_file))
    {
        i4_cnt = dlm_fread(in_file, (CHAR*)uac_internal_buf, X_INTERNAL_BUFFER_SIZE);
        if (i4_cnt != -1)
        {
            dlm_fwrite(out_file, (CHAR*)uac_internal_buf, i4_cnt);
            dlm_log(".");
        }          
    }          
    dlm_log("\n");

    dlm_fclose(in_file);
    dlm_fclose(out_file);

    /* update filename */
    if (dlm_delete_file(file)==-1)
    {
        dlm_log("Remove file %s failed\n", file);
        return DLM_DEL_FILE_FAILED;
    }

    if (dlm_rename_file("/mnt/ms/_up_sig.tmp", file) == -1)
    {
        dlm_log("Rename file %s failed\n", file);
        return DLM_RENAME_FILE_FAILED;
    }

    return DLM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlm_add_append_file
 *
 * Description: Add user specify data to codefile
 *
 * Inputs:  pt_append, user defined data
 *          file, the file describe
 *          ui4_key, the key to encrypt data
 * Outputs: NA
 * Returns: DLM_OK, add data to append file ok
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_WRITE_DATA_FAILED, write data failed
 ----------------------------------------------------------------------------*/
INT32 dlm_add_append_file(Append_T *pt_append, INT32 file, UINT32 ui4_key)
{
    INT32 in_file;
    CHAR buf[1024];
    INT32 i4_val_len;
    INT32 i4_ret;
    UCHAR uc_digest[32];
    UCHAR uc_magic[16];
    UCHAR uc_key[16];
    UCHAR uc_vector[16];
    CHAR s_compress_name[DLM_MAX_PATH_LENGTH];
    INT32 compress_file;
    UINT32 ui4_val_len;
    INT32 compress_len = 0;
    INT32 index = 0; //for debug
    UCHAR alignment_buffer[16];
	INT32 i4_status;

	x_memset(alignment_buffer, 0, 16);

    while (pt_append)
    {
        x_memset(uc_vector, 0, 16);
	    if (!use_customer_aes_key) 
		{
	        x_memcpy(uc_key, &ui4_key, sizeof(UINT32));
	        x_memcpy(uc_key + sizeof(UINT32), &ui4_key, sizeof(UINT32));
	        x_memcpy(uc_key + 2*sizeof(UINT32), &ui4_key, sizeof(UINT32));
	        x_memcpy(uc_key + 3*sizeof(UINT32), &ui4_key, sizeof(UINT32));
	    }
		else
		{
			x_memcpy(uc_key, aes_key, 16);
			x_memcpy(uc_vector, aes_iv, 16);
		}

        x_memset(uc_digest, 0, 32);
        x_memset(uc_magic, 0, 16);
        x_memcpy(uc_magic, DLM_APPEND_MAGIC, 16);
        dlm_dump_buffer("Append Header Key",uc_key,16);
        
        dlm_aes(uc_magic, 
             NULL,
		     16,  
		     uc_key,
		     uc_vector,
		     AES_ENCRYPT);
        
        i4_val_len = dlm_file_size(pt_append->ps_filename);
        
        /* reinitialize vector */
        ui4_iv = DLM_APPEND_DATA_IV;

        in_file = dlm_fopen (pt_append->ps_filename, kDlm_Read);
        if (in_file == -1)
        {
            dlm_log("Open append data file %s failed\n", pt_append->ps_filename);
            return DLM_OPEN_FILE_FAILED;
        }
       	pt_append->ui4_length = i4_val_len;

		memset(s_compress_name, 0, DLM_MAX_PATH_LENGTH);

        /*for compress*/
        if(pt_append->uac_privacy[1] && pt_append->ui4_length >= COMPRESS_UNIT_LENGTH)
        {
            UCHAR*   tmp_buf = NULL;
            tmp_buf = (UCHAR*)x_mem_alloc(COMPRESS_UNIT_LENGTH * 2);
            if(tmp_buf == NULL)
            {
                dlm_log("Allocate memory fail for compress, so do not do compressing\n");
                return DLM_ALLOCATE_MEMORY_FAILED;
            }
            compress_len = 0;
            index = 0;
           
            strcpy(s_compress_name, pt_append->ps_filename);
            strcat(s_compress_name, ".compress");
            //dlm_log("original file: %s, compress file name: %s\n", pt_append->ps_filename, s_compress_name);
            compress_file = dlm_fopen(s_compress_name, kDlm_Write);
            if (compress_file == -1)
            {
                dlm_log("Open compress file %s failed\n", s_compress_name);
                return DLM_OPEN_FILE_FAILED;
            }
            else
            {   
                /*do not compress first X bytes*/
                ui4_val_len = dlm_fread(in_file, tmp_buf, UNCOMPRESS_LENGTH);
                if (ui4_val_len > 0)
                {
                    if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, ui4_val_len) == -1)
                    {
                        dlm_log("write first 1MB bytes data %s failed\n", pt_append->ps_filename);
                        return DLM_WRITE_DATA_FAILED;
                    }
					
					if ( x_memcmp(tmp_buf, DLM_APPEND_HDR_MAGIC, 4) == 0 )
			        {
			        	dlm_log("Note!!!!!! extra append data length +8 must align with 16 bytes.");
			            /* the folllowing expresion is base on little endian */
			            INT32 i4_extra_offset = tmp_buf[4] + 
			                              tmp_buf[5] * 256 + 
			                              tmp_buf[6] * 256 * 256 + 
			                              tmp_buf[7] * 256 * 256 * 256 ;

			            dlm_log("    Real extra_app_hdr_len = %d\n", i4_extra_offset);
			            /* adjust i4_extra_app_hdr_len len to 4 bytes alignment */
			            if ( (i4_extra_offset + 8) % 16 )
			                i4_extra_offset += (16 - ((i4_extra_offset + 8) % 16));
						else
							i4_extra_offset += 8;
						
			            dlm_log("    Adjust extra_app_hdr_len = %d\n", i4_extra_offset);
			        	ui4_val_len = dlm_fread(in_file, tmp_buf, i4_extra_offset);

						if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, i4_extra_offset) == -1)
	                    {
	                        dlm_log("write first %d bytes data %s failed\n", i4_extra_offset, pt_append->ps_filename);
	                        return DLM_WRITE_DATA_FAILED;
	                    }
						compress_len += i4_extra_offset;
			        }
					
	   	    		compress_len += UNCOMPRESS_LENGTH;
                }
                else
                {
                    dlm_log("read first 1MB bytes data %s failed\n", pt_append->ps_filename);
                    return DLM_WRITE_DATA_FAILED;
                }
                //dlm_log("uncompressed file length: %d\n", i4_val_len);

                /*Compress remain data  Y bytes as Unit*/
                while(!dlm_feof(in_file))
                {
                    INT32   i4_compress_sub_length = 0;
                    
                    index++;
                    /*read Y bytes data from file and write to temporary file which is input file when doing compress operation*/
                    i4_compress_sub_length = dlm_fread(in_file, tmp_buf, COMPRESS_UNIT_LENGTH);
                    if (i4_compress_sub_length > 0)
                    {
                        INT32 compress_in_file = -1;
                        INT32 compress_out_file = -1;
                        //dlm_log("Compress(index:%d, original length: %d, ", index, i4_compress_sub_length);
                        compress_in_file = dlm_fopen("compress.in", kDlm_Write);
                        if (compress_in_file == -1)
                        {
                            dlm_log("Open compress.tmp failed\n");
                            return DLM_WRITE_DATA_FAILED;
                        } 

                        if (dlm_fwrite(compress_in_file, (CHAR*)tmp_buf, i4_compress_sub_length) == -1)
                        {
                            dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
                        dlm_fclose(compress_in_file);
                        
                        /*compress data using compress tool*/
                        char cmd[2*DLM_MAX_PATH_LENGTH];
                        char outfile[DLM_MAX_PATH_LENGTH];
                        memset(cmd, 0, 2*DLM_MAX_PATH_LENGTH);
                        memset(outfile, 0, DLM_MAX_PATH_LENGTH);
                        sprintf(outfile, "%s.compress%d", pt_append->ps_filename, index);
                        sprintf(cmd, "%s/lzhsenc compress.in %s", gToolDir, outfile);
						DLM_TRACE(("system(lzhsenc) cmd = %s\n", cmd));

                        i4_status = system(cmd);
						if(-1 == i4_status)
						{
							dlm_log("lzhsenc system error!");
						}
						else
						{ 
							DLM_TRACE(("exit status value = [0x%x]\n", i4_status));
							if(WIFEXITED(i4_status))
							{ 
								if(0 == WEXITSTATUS(i4_status))
								{ 
									dlm_log(("run lzhsenc successfully.\n"));
								}
								else
								{ 
									dlm_log("run lzhsenc fail, exit code: %d\n", WEXITSTATUS(i4_status));
								}
							}
							else
							{
								dlm_log("exit status = [%d]\n", WEXITSTATUS(i4_status));
							}  
						}

                        remove("compress.in");
                        
                        /*read compressed data and write to temporary file and later packing to target packed file*/
                        compress_out_file = dlm_fopen(outfile, kDlm_Read);
                        if (compress_out_file == -1)
                        {
                            dlm_log("Open compress.tmp failed\n");
                            return DLM_WRITE_DATA_FAILED;
                        } 
                       
                        //write sub compress data length
                        if (dlm_fwrite(compress_file, (UCHAR*)&i4_compress_sub_length, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
						
                        //read compressed file header and get the the compressed data length                     
                        int tmp_length = dlm_file_size(outfile);
//						dlm_log("decompress data length: %d\n", i4_compress_sub_length);
   //                     dlm_log("compress length: %d\n", tmp_length);
                        
                        //write compressed data length to output temporary file
                        if (dlm_fwrite(compress_file, (UCHAR*)&tmp_length, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
						
						//for AES 16 bytes alginment.
						if (dlm_fwrite(compress_file, (UCHAR*)&index, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
						if (dlm_fwrite(compress_file, (CHAR*)alignment_buffer, 4) == -1)
                        {
                            dlm_log("write compress data length %s failed\n", pt_append->ps_filename);
                            return DLM_WRITE_DATA_FAILED;
                        }
						
						compress_len += 16;
                        //write compress data to output temporary file
                        while(!dlm_feof(compress_out_file))
                        {
                            ui4_val_len = dlm_fread(compress_out_file, tmp_buf, COMPRESS_UNIT_LENGTH); 
                            
                            if(ui4_val_len > 0)
                            {
                                if (dlm_fwrite(compress_file, (CHAR*)tmp_buf, ui4_val_len) == -1)
                                {
                                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                                    return DLM_WRITE_DATA_FAILED;
                                }
                            }
							compress_len += ui4_val_len;
							//for AES 16 bytes alginment.
							if (ui4_val_len % 16 != 0){
								if (dlm_fwrite(compress_file, (CHAR*)alignment_buffer, 16 - (ui4_val_len % 16)) == -1)
								{
                                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                                    return DLM_WRITE_DATA_FAILED;
                                }
								compress_len += 16 - (ui4_val_len % 16);
							}
                        }

                        fclose(compress_out_file);                       
                        remove(outfile);
                        
                    }
                }
            }
            
            dlm_fclose (in_file);
            dlm_fclose (compress_file);
        if (dlm_fwrite(file, (CHAR*)pt_append->uc_type, 4) == -1)
        {
            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
            return DLM_WRITE_DATA_FAILED;
        }

        if (dlm_fwrite(file, (CHAR*)&pt_append->uac_privacy, 4) == -1)
        {
            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
            return DLM_WRITE_DATA_FAILED;
	        }

            if (dlm_fwrite(file, (CHAR*)&compress_len, 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }

			if (dlm_fwrite(file, (CHAR *)uc_magic, 16) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }
			
            i4_ret = x_dlm_generate_hash(s_compress_name,uc_digest);
	        if (i4_ret != DLM_OK)
	        {
	            return i4_ret;
	        }
			
			dlm_aes(uc_digest, 
		             NULL,
				     32,  
				     uc_key,
				     uc_vector,
				     AES_ENCRYPT);	
			
			if (dlm_fwrite(file, (CHAR *)uc_digest, 32) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }
			
            in_file = dlm_fopen (s_compress_name, kDlm_Read);
            if (in_file == -1)
            {
                dlm_log("[%d]Open append data file %s failed\n", __LINE__, s_compress_name);
                return DLM_OPEN_FILE_FAILED;
            }
        }
        else
        {
            /*if file size is less than UNIT_LENGTH, do not compress file and set compress flag as false*/
            if (dlm_fwrite(file, (CHAR*)pt_append->uc_type, 4) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }
			pt_append->uac_privacy[1] = 0;
	        if (dlm_fwrite(file, (CHAR*)&pt_append->uac_privacy, 4) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }

            if (dlm_fwrite(file, (CHAR*)&pt_append->ui4_length, 4) == -1)
            {
                dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
                return DLM_WRITE_DATA_FAILED;
            }

			if (dlm_fwrite(file, (CHAR *)uc_magic, 16) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }

			i4_ret = x_dlm_generate_hash(pt_append->ps_filename,uc_digest);
	        if (i4_ret != DLM_OK)
	        {
	            return i4_ret;
	        }
			
			dlm_aes(uc_digest, 
		             NULL,
				     32,  
				     uc_key,
				     uc_vector,
				     AES_ENCRYPT);	
			
	  		if (dlm_fwrite(file, (CHAR *)uc_digest, 32) == -1)
	        {
	            dlm_log("Write append data %s type failed\n", pt_append->ps_filename);
	            return DLM_WRITE_DATA_FAILED;
	        }
        }

		/* copy append data */
        while (!dlm_feof(in_file))
        {
            i4_val_len = dlm_fread(in_file, (CHAR*)buf, 1024);
            if (i4_val_len != -1)
            {
                /* if append data need scramble */
                if (pt_append->uac_privacy[0])
                {
                    dlm_aes((unsigned char *)buf, 
                             NULL,
                		     i4_val_len,  
                		     uc_key,
                		     uc_vector,
                		     AES_ENCRYPT);
                    
                }
                
                if (dlm_fwrite(file, (CHAR*)buf, i4_val_len) == -1)
                {
                    dlm_log("Copy append data %s failed\n", pt_append->ps_filename);
                    return DLM_WRITE_DATA_FAILED;
                }
            }
        }
        dlm_fclose (in_file);
        pt_append = pt_append->pt_next;
    }

    return DLM_OK;
}



/*-----------------------------------------------------------------------------
 * Name: x_dlm_generate_code_file
 *
 * Description: This API provide user to upload or generate a new code file
 *
 * Inputs:  pt_hdr, the default header
 *          pt_append, specify which data need append to codefile
 *          filename, the filnal filename of codefile
 * Outputs: no
 * Returns: DLM_OK, codefile is generated
 *          DLM_OPEN_FILE_FAILED, open file failed
 *          DLM_WRITE_DATA_FAILED, write append data to code file failed
 ----------------------------------------------------------------------------*/
/*
 *   4       64       2      4          32                  32
 * +-----+--------+-------+--------+------------+----------------------------+
 * | ID  |Version | Lenght | Model Name | Encrypted code file digest |
 * +-----+--------+-------+--------+------------+----------------------------+
 * | Append data                                                           |
 * +-----------------------------------------------------------------------+ 
 * | Append data                                                           |
 * +-----------------------------------------------------------------------+ 
 * | ....                                                                  |
 * +-----------------------------------------------------------------------+
 * 
 * Append data header
 *    1    1       4       n
 * +----+-----+--------+---------------------------------------------------+
 * | T  |  P  |    L   |  V                                                |
 * +----+-----+--------+---------------------------------------------------+
 * T: type
 *    255 for end of append data
 * P: privacy
 *    0: value is plain text
 *    1: value is encrypted
 * L: value length
 * V: Append data 
 */
INT32 x_dlm_generate_code_file(DEFAULT_CODE_FILE_HDR_T *pt_user_hdr, Append_T *pt_append, CHAR *filename)
{
    INT32 out_file;
    UINT32 ui4_key = DLM_HDR_KEY;
    Append_T *pt_append_backup = pt_append;
    DEFAULT_CODE_FILE_HDR_T *pt_hdr = (DEFAULT_CODE_FILE_HDR_T*) uac_internal_buf;
    UCHAR uc_key[16];
    UCHAR uc_vector[16];
    INT32 i =0;
    
    x_memset(uc_vector, 0, 16);

	if (!use_customer_aes_key)
	{
	    x_memcpy(uc_key,&ui4_key,sizeof(UINT32));
	    x_memcpy(uc_key + sizeof(UINT32),&ui4_key,sizeof(UINT32));
	    x_memcpy(uc_key + 2*sizeof(UINT32),&ui4_key,sizeof(UINT32));
	    x_memcpy(uc_key + 3*sizeof(UINT32),&ui4_key,sizeof(UINT32));
	}
	else 
	{
	    x_memcpy(uc_key, aes_key, 16);
		x_memcpy(uc_vector, aes_iv, 16);
	}

    if ((out_file = dlm_fopen (filename, kDlm_Write)) == -1)
    {
        dlm_log("Open output codefile %s failed\n", filename);
        return DLM_OPEN_FILE_FAILED;
    }

    x_memcpy(pt_hdr, pt_user_hdr, sizeof(DEFAULT_CODE_FILE_HDR_T));

    /* accumulate total size */
    pt_hdr->ui4_total_size = sizeof( DEFAULT_CODE_FILE_HDR_T );
    while (pt_append_backup)
    {
        pt_hdr->ui4_total_size += 60;
    
        pt_hdr->ui4_total_size += dlm_file_size(pt_append_backup->ps_filename);
        pt_append_backup = pt_append_backup->pt_next;
    }
    dlm_log("Code size = %u\n", (unsigned int)pt_hdr->ui4_total_size);

    /* story my magic */
    x_memcpy(pt_hdr->s_magic, DLM_HDR_MAGIC, 8);

#if HDR_ENCRYPTED
    dlm_aes((UCHAR *) pt_hdr,
            NULL,
            sizeof( DEFAULT_CODE_FILE_HDR_T ) - DLM_HDR_SIGNATURE_SIZE,
            uc_key,
            uc_vector,
            AES_ENCRYPT);
    dlm_dump_buffer("Encrypted Header:", (UCHAR*) pt_hdr, sizeof( DEFAULT_CODE_FILE_HDR_T ) - DLM_HDR_SIGNATURE_SIZE);
#endif

    if (dlm_fwrite(out_file, (CHAR*)pt_hdr, sizeof( DEFAULT_CODE_FILE_HDR_T )) == -1)
    {
        dlm_fclose (out_file);
        return DLM_WRITE_DATA_FAILED;
    }

    /* use uac_id as a key */
    if (dlm_add_append_file(pt_append, out_file, *((UINT32*) pt_user_hdr->uac_id)) != 0)
    {
        dlm_log("Add append data failed\n");
        exit(-1);
    }

    dlm_fclose (out_file);
    return DLM_OK;
} 

#endif

#if CLI_LVL_ALL

#endif

