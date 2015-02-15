/****************************************************************************
*
* File:        drv_version.c
*
* Description: Driver Software Version
*  
* Author:      kl-lees
*
* History:     13 July 2009 - Creation
*
*****************************************************************************/

/****************************************************************************/
/*     			      HEADER FILES				    						*/
/****************************************************************************/
#include "../../sony_drv_inc/utility_if.h"
//#include "x_fm.h"
#include "x_os.h"

/*------------------------------------------------------------------*/
/*! @struct FM_FILE_INFO_T
 *  @brief  File information.
 *  @code
 *  typedef struct _FM_FILE_INFO_T
 *  {
 *      UINT32      ui4_inode;
 *      UINT32      ui4_mode;
 *      UINT32      ui4_uid;
 *      UINT32      ui4_gid;
 *      UINT64      ui8_size;
 *      UINT64      ui8_offset; 
 *      UINT32      ui4_blk_size;
 *      UINT64      ui8_blk_cnt;
 *      TIME_T      ui8_create_time;
 *      TIME_T      ui8_access_time;
 *      TIME_T      ui8_modify_time;
 *      UINT32      ui4_start_lba;
 *      BOOL        b_copy_protected;
 *  } FM_FILE_INFO_T;
 *  @endcode
 *  @li@c  ui4_inode                                        - Inode
 *  @li@c  ui4_mode                                        - Mode
 *  @li@c  ui4_uid                                           - UID
 *  @li@c  ui4_gid                                           - GID
 *  @li@c  ui8_size                                          - Size
 *  @li@c  ui8_offset                                        - Offset used for chop
 *  @li@c  ui4_blk_size                                    - Block size
 *  @li@c  ui8_blk_cnt                                     - Block count
 *  @li@c  ui8_create_time                               - Created time
 *  @li@c  ui8_access_time                              - Last accessed time
 *  @li@c  ui8_modify_time                             - Last modified time
 *  @li@c  ui4_start_lba                                   - Start LBA
 *  @li@c  b_copy_protected                            - Copy protected or not
 */
/*------------------------------------------------------------------*/
typedef struct _FM_FILE_INFO_T
{
    UINT32      ui4_inode;
    UINT32      ui4_mode;
    UINT32      ui4_uid;
    UINT32      ui4_gid;
    UINT64      ui8_size;
    UINT64      ui8_offset;
    UINT32      ui4_blk_size;
    UINT64      ui8_blk_cnt;
    TIME_T      ui8_create_time;
    TIME_T      ui8_access_time;
    TIME_T      ui8_modify_time;

    UINT32      ui4_start_lba;
    BOOL        b_copy_protected;
} FM_FILE_INFO_T;

extern INT32 x_fm_open(
    HANDLE_T        h_dir,
    const CHAR      *ps_path,
    UINT32          ui4_flags,
    UINT32          ui4_mode,
    BOOL            b_ext_buf,
    HANDLE_T        *ph_file);
extern INT32 x_fm_read(
    HANDLE_T        h_file,
    VOID            *pv_data,
    UINT32          ui4_count,
    UINT32          *pui4_read);
extern INT32 x_fm_close(
    HANDLE_T        h_file);
extern INT32 x_fm_get_info_by_handle( // allen add for usb1 size
	HANDLE_T		h_file,
	FM_FILE_INFO_T	*pt_info);



/****************************************************************************/
/*     			      TYPE DEFINITIONS			    						*/
/****************************************************************************/
/* PRIVATE DRIVER RELEASE VERSION */
#define DRV_VER         0x01    /* Drv Version      0xXA -> v(Software_version).AXX  */
#define DRV_REV1        0x00    /* Drv Revision 1   0xXB -> v(Software_version).XBX  */
#define DRV_REV2        0x04    /* Drv Revision 2   0xXC -> v(Software_version).XXC  */


/****************************************************************************
*				FUNCTION DEFINITIONS			    						*
*****************************************************************************/

/*****************************************************************************
*	FUNCTION: UINT16 vAPIGetDriverVersion(void)
*	DESCRIPTION: API to return Private Driver Version.
*	PRECOND.: None.
*	POSTCOND.: None.
*****************************************************************************/
UINT16 vAPIGetDriverVersion (void)
{
	UINT16 ui2_drv_version;
	
	ui2_drv_version = ( (DRV_VER << 12) | (DRV_REV1 << 8) | (DRV_REV2 << 4) );

    return ui2_drv_version;                 
}

//return 0 if OK
UINT16 vUsbVerifyCheck(void)
{
//function call ap
#if 0 
    char pu1Buf[35];
    HANDLE_T rImgFile = NULL;
    char szFileNameWithPath[] = "/mnt/usb/Mass-000/usb_rd_vf_test";
    char pu1CheckStr[] = "123456789abcdefghijklmnopqrstuvwxyz";
    INT32 i4_check_length = 35;
    UINT16 i2_result = 0;
    UINT32 i4_read = 0, i4_to_read = 1024;
    UINT32 i4_temp;
    FM_FILE_INFO_T t_File_info;

    do
    {
        //open file, break if fail
        i4_temp = x_fm_open(NULL, szFileNameWithPath, 0, 0777, FALSE, &rImgFile); //¶}ÀÉ
        if ( i4_temp != 0)
        {
            i2_result = 0x0070; 
            break;
        }
        // read & check size ,break if fail 
        x_fm_get_info_by_handle(rImgFile ,&t_File_info); 
        if (t_File_info.ui8_size != 1024)
        {
            i2_result = 0x0071; 
            break;
        }

        //read content and check, break if fail
        while(i4_to_read > 0)
        {
            if (x_fm_read(rImgFile, pu1Buf, i4_check_length, &i4_read) != 0)
            {
                i2_result = 0x0070; 
                break;
            }
            if(i4_read == 0 || i4_read > i4_check_length)
            {
                i2_result = 0x0072;
                break;
            }
            if (x_memcmp(pu1Buf, pu1CheckStr, i4_read) != 0)
            {
                i2_result = 0x0072; 
                break;
            }
            i4_to_read -= i4_read;
        }
        break;
    } while(0);
    //free resource
    if (rImgFile)
    {
        x_fm_close(rImgFile);
    }
	return i2_result;
#else
    return 0;
#endif
}
/*****************************************************************************
*   End of file
*****************************************************************************/

