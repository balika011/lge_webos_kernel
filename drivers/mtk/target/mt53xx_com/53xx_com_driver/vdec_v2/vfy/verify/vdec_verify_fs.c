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

/** @file dmx_ide.c
 *  Demux driver - IDE test port
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_typedef.h"
#include "vdec_vfy_debug.h"
#include "drv_config.h"
#include "vdec_verify_mm_map.h"
#include "x_os.h"
#include "UDVT.h"
#include "UDVT_IF.h"
#include "drv_fs.h"
#include <linux/fs.h>
#include <asm/uaccess.h>

#include <linux/module.h>
#include <linux/slab.h>

#include "x_drv_map.h"


#define PSEEK_SET   0   /* offset from begining of file*/
#define PSEEK_CUR   1   /* offset from current file pointer*/
#define PSEEK_END   2   /* offset from end of file*/

typedef struct
{
  INT32 handle;
  INT32 offset;
  UINT32 filesize;
}
FILE;

/* End of file character.
   Some things throughout the library rely on this being -1.  */
#ifndef EOF
#define EOF (-1)
#endif

/* The possibilities for the third argument to `fseek'.
   These values should not be changed.  */
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define OPEN_R 0
#define OPEN_W 4
#define OPEN_A 8
#define OPEN_B 1
#define OPEN_PLUS 2

extern INT32 _sys_open(const UINT8 *name, UINT32 openmode);

extern INT32 _sys_close(UINT32 fh);

extern INT32 _sys_read(UINT32 fh, UINT8 *buf, UINT32 len, UINT32 mode);

extern INT32 _sys_write(UINT32 fh, UINT8 *buf, UINT32 len, UINT32 mode);

extern INT32 _sys_seek(UINT32 fh, INT64 pos);

extern INT32 _sys_flen(UINT32 fh);


#if 0
#if FATS_SUPPORT
INT32 DrvFSOpenFile(char* pcDirFileName, UINT32 dwFlags, INT32* piFd)
{
    struct file *filp;

    if ((NULL == pcDirFileName) || (NULL == piFd))
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }
    
    //UTIL_Printf("[Drv_FS] %s: #%d: open file: %s\n", __FUNCTION__, __LINE__, pcDirFileName);

    if(dwFlags == DRV_FS_W_C)
    {
    //    filp = filp_open(pcDirFileName, O_CREAT|O_WRONLY, dwFlags);
    			filp = (struct file *)UDVT_IF_OpenFile(pcDirFileName,"rb");
    }
    else if(dwFlags == DRV_FS_RW_C)
    {
    //    filp = filp_open(pcDirFileName, O_CREAT|O_RDWR, dwFlags);
    			filp = (struct file *)UDVT_IF_OpenFile(pcDirFileName,"rwb");
    }
    else
    {
    //    filp = filp_open(pcDirFileName, O_RDONLY, dwFlags);
    			filp = (struct file *)UDVT_IF_OpenFile(pcDirFileName,"rb");
    }

    if(filp == 0)
    //if (IS_ERR(filp))
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_FAIL);
        return (DRV_FSR_FAIL);
    }

    *piFd = (INT32)filp;

    //UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSGetFileSize(INT32 iFd, UINT32 *pu4FileSize)
{
    struct file *filp = (struct file *)iFd;
 //   INT32 iRet;
 //   INT32 iCur;
       
    if (NULL == pu4FileSize)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

		#if 0
    if (NULL == filp->f_op)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }
    
    /* get current offset */
    iCur = filp->f_op->llseek(filp, 0, SEEK_CUR);
    
    /* move to end to get end offset, that is the file size */
    *pu4FileSize = filp->f_op->llseek(filp, 0, SEEK_END);

    /* move back to current offset */
    iRet = filp->f_op->llseek(filp, iCur, SEEK_SET);
		#endif

		*pu4FileSize = UDVT_IF_GetFileLength((UINT32)iFd);
    //UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSSeekFile(INT32 iFd, INT64 iOffset, INT32 iOrigin)
{
    struct file *filp = (struct file *)iFd;
    INT32 iPos;
    
    if ((iOrigin < 0) || (iOrigin > 2)) /*PSEEK_SET: 0, PSEEK_CUR: 1, PSEEK_END 2*/
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    //UTIL_Printf("[Drv_FS] %s: #%d: old Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);

    //UTIL_Printf("[Drv_FS] %s: #%d: old Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);

    //iPos = filp->f_pos;

    if (iOrigin == PSEEK_SET)
    {
    		iPos = (INT32)UDVT_IF_SeekFile((UINT32)filp,(INT32)iOffset,0);
        //iPos = iOffset;
        //filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_CUR)
    {
    		iPos = (INT32)UDVT_IF_SeekFile((UINT32)filp,(INT32)iOffset,1);
        //iPos += iOffset;
        //filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_END)
    {
        //if (NULL == filp->f_op)
        //{
        //    UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        //    return DRV_FSR_NULL_POINT;
        //}
        //iPos = filp->f_op->llseek(filp, iOffset, SEEK_END);
        iPos = (INT32)UDVT_IF_SeekFile((UINT32)filp,(INT32)iOffset,2);
    }    

    //UTIL_Printf("[Drv_FS] %s: #%d: new Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);
   
    
    
    return (iPos);
}


INT32 DrvFSReadFile(INT32 iFd, void* pbBuf, UINT32 u4Count)
{
    struct file *filp = (struct file *)iFd;
  //  mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    //if (NULL == filp->f_op)
    //{
    //    UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
    //    return DRV_FSR_NULL_POINT;
    //}

    //oldfs = get_fs();
    //set_fs(KERNEL_DS);

    //iRet = filp->f_op->read(filp, pbBuf, u4Count, &filp->f_pos);
		iRet = (INT32)UDVT_IF_ReadFile(pbBuf,1,u4Count,(UINT32)filp);
    //set_fs(oldfs);

    //UTIL_Printf("[Drv_FS] %s: #%d: read %d bytes Offset=%d\n", __FUNCTION__, __LINE__, iRet, (unsigned)filp->f_pos);
    return (iRet);
}


INT32 DrvFSWriteFile(INT32 iFd, const void *pbBuf, DWRD dwSize)
{
    struct file *filp = (struct file *)iFd;
    //mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (0 == dwSize)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    //oldfs = get_fs();
    //set_fs(KERNEL_DS);
    iRet = (INT32)UDVT_IF_WriteFile((void *)pbBuf,1,dwSize,(UINT32)filp);
    //iRet = filp->f_op->write(filp, pbBuf, dwSize, &filp->f_pos);

    //set_fs(oldfs);

    //UTIL_Printf("[Drv_FS] %s: #%d: write (%d, %d) bytes Offset=%d from 0x%8x\n", __FUNCTION__, __LINE__, iRet, dwSize, (unsigned)filp->f_pos, (unsigned)pbBuf);
    return (iRet);
}


INT32 DrvFSCloseFile(INT32 iFd)
{
    struct file *filp = (struct file *)iFd;

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    //filp_close(filp, NULL);
		UDVT_IF_CloseFile((UINT32)filp);
    //UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}
#else
INT32 DrvFSOpenFile(char* pcDirFileName, UINT32 dwFlags, INT32* piFd)
{
    struct file *filp;

    if ((NULL == pcDirFileName) || (NULL == piFd))
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }
    
//   UTIL_Printf("[Drv_FS] %s: #%d: open file: %s\n", __FUNCTION__, __LINE__, pcDirFileName);

    if(dwFlags == DRV_FS_W_C)
    {
        filp = filp_open(pcDirFileName, O_CREAT|O_WRONLY, dwFlags);
    }
    else if(dwFlags == DRV_FS_RW_C)
    {
        filp = filp_open(pcDirFileName, O_CREAT|O_RDWR, dwFlags);
    }
    else
    {
        filp = filp_open(pcDirFileName, O_RDONLY, dwFlags);
    }

    if (IS_ERR(filp))
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_FAIL);
        return (DRV_FSR_FAIL);
    }

    *piFd = (INT32)filp;

    //UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSGetFileSize(INT32 iFd, UINT32 *pu4FileSize)
{
    struct file *filp = (struct file *)iFd;
    INT32 iRet;
    INT32 iCur;
       
    if (NULL == pu4FileSize)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }
    
    /* get current offset */
    iCur = filp->f_op->llseek(filp, 0, SEEK_CUR);
    
    /* move to end to get end offset, that is the file size */
    *pu4FileSize = filp->f_op->llseek(filp, 0, SEEK_END);

    /* move back to current offset */
    iRet = filp->f_op->llseek(filp, iCur, SEEK_SET);

    //UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSSeekFile(INT32 iFd, INT64 iOffset, INT32 iOrigin)
{
    struct file *filp = (struct file *)iFd;
    INT32 iPos;
    
    if ((iOrigin < 0) || (iOrigin > 2)) /*PSEEK_SET: 0, PSEEK_CUR: 1, PSEEK_END 2*/
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

   // UTIL_Printf("[Drv_FS] %s: #%d: old Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);

    iPos = filp->f_pos;

    if (iOrigin == PSEEK_SET)
    {
        iPos = iOffset;
        filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_CUR)
    {
        iPos += iOffset;
        filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_END)
    {
        if (NULL == filp->f_op)
        {
            UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
            return DRV_FSR_NULL_POINT;
        }
        iPos = filp->f_op->llseek(filp, iOffset, SEEK_END);
    }    

    UTIL_Printf("[Drv_FS] %s: #%d: new Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);
    return (iPos);
}


INT32 DrvFSReadFile(INT32 iFd, void* pbBuf, UINT32 u4Count)
{
    struct file *filp = (struct file *)iFd;
    mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    iRet = filp->f_op->read(filp, pbBuf, u4Count, &filp->f_pos);

    set_fs(oldfs);

    //UTIL_Printf("[Drv_FS] %s: #%d: read %d bytes Offset=%d\n", __FUNCTION__, __LINE__, iRet, (unsigned)filp->f_pos);
    return (iRet);
}


INT32 DrvFSWriteFile(INT32 iFd, const void *pbBuf, DWRD dwSize)
{
    struct file *filp = (struct file *)iFd;
    mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (0 == dwSize)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    
    iRet = filp->f_op->write(filp, pbBuf, dwSize, &filp->f_pos);

    set_fs(oldfs);

    UTIL_Printf("[Drv_FS] %s: #%d: write (%d, %d) bytes Offset=%d from 0x%8x\n", __FUNCTION__, __LINE__, iRet, dwSize, (unsigned)filp->f_pos, (unsigned)pbBuf);
    return (iRet);
}


INT32 DrvFSCloseFile(INT32 iFd)
{
    struct file *filp = (struct file *)iFd;

    if (NULL == filp)
    {
        UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    filp_close(filp, NULL);

    UTIL_Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}

#endif

INT32 DrvFSMount(UINT32 dwDriveNo, UINT32 *pu4DrvFSTag)
{
#if (CONFIG_DRV_LINUX)
    return 0;
#else
    INT32   iRet;
    UINT32  dwDeviceNo  = 1;       
    DIR         rDir;
    DIR_ENTRY   rEntry;
    static char pbDrvName[MAX_FILENAME_SZ*2+4]    = "/A/"; 

    _DrvFs_Lock();
    if (_gu4DrvFsDevNo == 0xFFFFFFFF)
    {
      //Init 
      if (_ahDrvFsSema == NULL)
      {
        VERIFY (x_sema_create(&_ahDrvFsSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
      }
    }
    _DrvFs_Unlock();

	if (DEVICETYPE != DEVICEHDD)
	{
		DEVICETYPE = DEVICEHDD;
		_gu4DrvFsDevNo = 0xFFFFFFFF; //Reset
	}

    if (0xFFFFFFFF != _gu4DrvFsDevNo)
    {
      _gu4DrvFsMountRefCount ++;
      return 0;
    }
    
    if (NULL == pu4DrvFSTag)
    {
        return (DRV_FSR_NULL_POINT);
    }
    
    UTIL_Printf("[Drv_FS] Mount Drive No %d.\n",dwDriveNo);
    
    /***************************************************
     * 1. Open HDD Storage Device
     **************************************************/
    /* open dev */
    *pu4DrvFSTag = (UINT32)STORGOpenDev(DRVT_HW_IDE, 1, DRVT_HARD_DISK, 2);
    if (*pu4DrvFSTag == NULL)
    {
        *pu4DrvFSTag = (UINT32)STORGOpenDev(DRVT_HW_IDE, 0, DRVT_HARD_DISK, 0);
    }
    UTIL_Printf("[Drv_FS] STORGOpenDev : 0x%8x.\n",*pu4DrvFSTag);
    _u4DrvFSTag = *pu4DrvFSTag;


    /***************************************************
     * 2. Driver FS Init
     **************************************************/
    iFsFatInit();

    
    /***************************************************
     * 3. Mount the device
     **************************************************/     
    dwDeviceNo = GET_BLKDEV_ID(dwDeviceNo, (((unsigned int)dwDriveNo) & 0xffff));
      
    iRet = iFsFatMount(dwDeviceNo, &dwDriveNo, FAT_MOUNT_DEFAULT, NULL, NULL);
    UTIL_Printf("[Drv_FS] mount disc 0x%x, result : %d.\n",dwDeviceNo,iRet);
    if(iRet < 0)
    {
        return FALSE;
    }

    /***************************************************
     * 4. Read The Root Dir
     **************************************************/   
    pbDrvName[1] = 'A' + dwDriveNo;
    
    iRet = iFsOpenDir(pbDrvName, &rDir);
    UTIL_Printf("[Drv_FS] open root dir result : %d.\n",iRet);
    if(iRet < 0)
    {
        return FALSE;
    }
     
    do
    {
        iRet = iFsReadDir(&rDir, &rEntry);
        if(iRet < 0)
        {
            UTIL_Printf("[Drv_FS] read root dir finish : %d.\n",iRet);
            break;
        }
               
        UTIL_Printf("[Drv_FS] find file : %s.\n",rEntry.prFileName);
    }
    while(TRUE);
    
    iFsCloseDir(&rDir);     

    _gu4DrvFsDevNo = dwDriveNo;
    _gu4DrvFsMountRefCount ++;

    return (DRV_FSR_SUCCESS);
#endif    
}

INT32 DrvFSUnMount()
{
#if (CONFIG_DRV_LINUX)
    return 0;
#else
    _gu4DrvFsMountRefCount--;
    if (0 != _gu4DrvFsMountRefCount)
      return 0;

    _gu4DrvFsDevNo = 0xFFFFFFFF;
    return (STORGCloseDev((STORG_DEV_T*)_u4DrvFSTag));
#endif    
}
#endif
#if 0
FILE *fopen(const UINT8 *filename, const UINT8 *mode)
{
  INT32 ret;
  FILE *fp;

  UINT32 sys_mode;
  printk("fopen: %s ===\n", filename);
  if (strchr(mode, 'b'))
    sys_mode = OPEN_B;
  else
    sys_mode = 0;

  if (strchr(mode, 'w'))
    sys_mode |= OPEN_W;
  if (strchr(mode, 'a'))
    sys_mode |= OPEN_A;

  ret = _sys_open(filename, sys_mode);

  if (ret < 0)
  {
    printk("fopen: %s open error\n", filename);
    return NULL;
  }

  fp = kmalloc(sizeof(FILE), GFP_KERNEL);
  if (fp == NULL)
  {
    _sys_close(ret);
    printk("fopen: not enough memory.\n");
    return NULL;
  }

  fp->handle = ret;
  fp->offset = 0;
  fp->filesize = _sys_flen(ret);

  printk("fopen: %s handle %d size %d\n", filename, fp->handle, fp->filesize);

  return fp;
}
EXPORT_SYMBOL(fopen);

INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream)
{
  INT32 ret;
  UINT32 offset;

  if (size == 0 || n == 0)
  {
    printk("fread: zero size %d*%d read\n", size, n);
    return 0;
  }

  offset = size * n;
  if (stream->offset + offset > stream->filesize)
    offset = stream->filesize - stream->offset;

  ret = _sys_read(stream->handle, ptr, offset, OPEN_B);

  if (ret == 0)
  {
    printk("fread: read %d bytes\n", offset);
    stream->offset += offset;
    return n;
  }

  if (ret == -1)
  {
    printk("fread");
    return ret;
  }

  if (ret & 0x80000000)
    ret &= ~0x80000000;

  stream->offset += offset - ret;
  printk("fread: read %d/%d bytes\n", offset - ret, offset);
  return (offset - ret) / size;
}
EXPORT_SYMBOL(fread);

INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream)
{
  INT32 ret;

  if (size == 0 || n == 0)
  {
    printk("fwrite: zero size %d*%d write\n", size, n);
    return 0;
  }

  ret = _sys_write(stream->handle, ptr, size * n, OPEN_B);

  if (ret == 0)
  {
    printk("fwrite: write %d bytes\n", size * n);
    stream->offset += size * n;
    return n;
  }

  if (ret > 0)
  {
    printk("fwrite: write %d/%d bytes\n", size * n - ret, size * n);
    stream->offset += size * n - ret;
    return (size * n - ret) / size;
  }

  return -1;
}
EXPORT_SYMBOL(fwrite);

INT32 fseek(FILE *stream, INT32 offset, INT32 whence)
{
  INT32 ret, new_offset;

  if (whence == SEEK_END)
  {
    new_offset = stream->filesize + offset;
  }
  else if (whence == SEEK_CUR)
  {
    new_offset = stream->offset + offset;
  }
  else if (whence == SEEK_SET)
  {
    new_offset = offset;
  }
  else
  {
    printk("fseek: invalid option %d\n", whence);
    return -1;
  }

  ret = _sys_seek(stream->handle, (INT64)new_offset);

  if (ret >= 0)
  {
    printk("fseek: offset set to %d\n", new_offset);
    stream->offset = new_offset;
    return 0;
  }

  printk("fseek: set %d error\n", offset);
  return -1;
}
EXPORT_SYMBOL(fseek);

INT32 fclose(FILE *stream)
{
  INT32 ret = _sys_close(stream->handle);

  if (ret < 0)
  {
    printk("fclose: file %d close error\n", stream->handle);
    return -1;
  }

  printk("fclose: file %d was closed \n", stream->handle);
  kfree(stream);

  return 0;
}
EXPORT_SYMBOL(fclose);

INT64 ftell(FILE *stream)
{
  printk("ftell: current offset is %d\n", (INT32)stream->offset);
  return stream->offset;
}
EXPORT_SYMBOL(ftell);
#if 0
static INT32 _Cmd_SemihostingRead(INT32 i4Argc, const CHAR** aszArgv)
{
  FILE *fp;
  UINT8 buf[256];
  INT32 size, i;

  if (i4Argc < 2)
  {
    printk("Usage: %s [file name on PC]", aszArgv[0]);
    return 0;
  }

  fp = fopen(aszArgv[1], "rb");
  if (fp == NULL)
  {
    printk("Open %s error\n", aszArgv[1]);
    return  0;
  }

  i = fseek(fp, 0, SEEK_END);

  if (i < 0)
  {
    printk("fseek error 1\n");
    fclose(fp);
    return -1;
  }
  size = ftell(fp);
  i = fseek(fp, 0, SEEK_SET);
  if (i < 0)
  {
    printk("fseek error 2\n");
    fclose(fp);
    return -1;
  }

  printk("file size is %d\n", size);

  while (size > 0)
  {
    if (size > sizeof(buf))
      i = sizeof(buf);
    else
      i = size;
    fread(buf, i, 1, fp);
    size -= i;
    printk("%s", buf);
  }
  printk("\n");

  fclose(fp);

  return 0;
}

static INT32 _Cmd_SemihostingWrite(INT32 i4Argc, const CHAR** aszArgv)
{
  FILE *fp;
  UINT8 buf[256];
  INT32 size, i;

  if (i4Argc < 2)
  {
    printk("Usage: %s [file name on PC]", aszArgv[0]);
    return 0;
  }

  fp = fopen(aszArgv[1], "wb");
  if (fp == NULL)
  {
    printk("Open %s error\n", aszArgv[1]);
    return  0;
  }

  i = fseek(fp, 0, SEEK_END);

  if (i < 0)
  {
    printk("fseek error 1\n");
    fclose(fp);
    return -1;
  }
  size = ftell(fp);
  i = fseek(fp, 0, SEEK_SET);
  if (i < 0)
  {
    printk("fseek error 2\n");
    fclose(fp);
    return -1;
  }

  printk("file size is %d\n", size);

  strcpy(buf, "This is a test\n");

  for (i=0;i<10;i++)
  {
    if (fwrite(buf, strlen(buf), 1, fp) <= 0)
      printk("write error\n");
  }

  printk("write finished\n");

  fclose(fp);

  return 0;
}


static CLI_EXEC_T _arSemiCmdTbl[] =
{
  //Semihosting test command
  {"read",	"r", _Cmd_SemihostingRead,	NULL,	"Read a file",	CLI_GUEST},
  {"write",	"w", _Cmd_SemihostingWrite,	NULL,	"Write a file",	CLI_GUEST},

  //Last CLI command record, NULL
  {NULL, NULL, NULL, NULL, NULL, CLI_GUEST}
};

static CLI_EXEC_T _rSemiCmdTbl =
{
  "semihosting",
  "semi",
  NULL,
  _arSemiCmdTbl,
  "Semihosting command",
  CLI_GUEST
};

CLI_EXEC_T *GetSemihostCmdTbl(void)
{
  return &_rSemiCmdTbl;
}

EXPORT_SYMBOL(GetSemihostCmdTbl);
#endif
#endif
