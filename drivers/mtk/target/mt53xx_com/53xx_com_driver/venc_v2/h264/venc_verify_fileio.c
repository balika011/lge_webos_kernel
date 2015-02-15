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

#include "drv_config.h"
#include "x_os.h"                       //For <string.h>
#include "x_printf.h"
#include "x_assert.h"
#include "x_stl_lib.h"
#include "x_debug.h"
#include "stdio.h"                     //Should support on Verify/Emulation Buil
#include "x_hal_1176.h"

#include "drv_fs.h"
#include "file_io.h"
#include "drv_common.h"

#include "venc_264_verify.h"
#include "venc_verify_fileio.h"

HANDLE_T _ahFileIOSema;

//-----------------------------------------------------------------------------
BOOL fgVEnc_FsMount(void)
{
    UINT32 dwDriveNo   = 0;
    UINT32 u4DrvFSTag  = 0;
    INT32 i4_ret;    
    
    i4_ret = DrvFSMount(dwDriveNo, &u4DrvFSTag);

    if (i4_ret < 0)
    {
      Printf("Fs mount fail %d\n", i4_ret);
      ASSERT(0);
      return 0;
    }
    
    return TRUE;
}

BOOL fgVEnc_FsUnMount (void)
{
    DrvFSUnMount();    
    return TRUE;
}

BOOL fgVEnc_FsOpenFile(CHAR *strFileName, INT32 *pi4FileId)
{

    INT32 i4_ret;
    INT32 i4Fd;

     i4_ret = DrvFSOpenFile(strFileName, DRV_FS_RDONLY, &i4Fd);
     if (i4_ret < 0)
     {
         Printf("Fs open file fail %d\n", i4_ret);
         return FALSE;
     }

     *pi4FileId = i4Fd;
     
     return TRUE;
}

BOOL fgVEnc_FsGetFileSize(UINT32 i4FileId, UINT32* pu4FileSize)
{
  INT32 i4_ret;
  
  i4_ret = DrvFSGetFileSize(i4FileId, pu4FileSize);

  if (i4_ret < 0)
  {
    Printf("Fs get fiel size fail %d\n", i4_ret);
    return FALSE;
  }

  return TRUE;
}

BOOL fgVEnc_FsCloseFile(  UINT32 i4FileId)
{
   DrvFSCloseFile(i4FileId);
   return TRUE;
}

BOOL fgVEnc_FsReadFile(void* pvReadBuffer, UINT32 u4ReadSize, INT32 i4FileId)
{
  INT32 i4_ret;

  i4_ret = DrvFSReadFile(i4FileId, pvReadBuffer, u4ReadSize);
  if(i4_ret < 0)
  {
    Printf("read file Fail!\n");
    ASSERT(0);
    return FALSE;
  }

#if VENC_MAP_UNMAP_SUPPORT
  BSP_dma_map_vaddr((UINT32) pvReadBuffer, u4ReadSize, TO_DEVICE);
  //temp unmap
  BSP_dma_unmap_vaddr((UINT32) pvReadBuffer, u4ReadSize, TO_DEVICE);
#else
  BSP_FlushDCacheRange((UINT32) pvReadBuffer, u4ReadSize);
#endif

  return TRUE;  
}


BOOL fgVEnc_FsWriteFile(void* pvWriteSrcBuffer, UINT32 u4Length, INT32 i4FileId)
{
  INT32 i4_ret;
  
  i4_ret = DrvFSWriteFile(i4FileId, pvWriteSrcBuffer, u4Length);
  if(i4_ret < 0)
  {
    Printf("write file Fail!\n");
    return FALSE;
  }

  return TRUE;    
}


BOOL fgVEnc_FSMount(void)
{
    #ifdef VENC_VERIFY_FS_SUPPORT
    UINT32 dwDriveNo   = 0;
    UINT32 u4DrvFSTag  = 0;
    INT32 i4_ret;    

    i4_ret = DrvFSMount(dwDriveNo, &u4DrvFSTag);

    if (i4_ret < 0)
    {
      UTIL_Printf("Fs mount fail %d\n", i4_ret);
      ASSERT(0);
      return 0;
    }
    #else //VENC_VERIFY_FS_SUPPORT
    //Null Function
    #endif //VENC_VERIFY_FS_SUPPORT
    
    return TRUE;
}



BOOL fgVEnc_OpenFile(CHAR *strFileName, INT32 *pi4FileId, UINT32 u4Opentype)
{
  INT32 i4_ret = TRUE;
  #ifndef VENC_VERIFY_FS_SUPPORT
  FILE *ptFPath;
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  i4_ret = fgVEnc_FsOpenFile(strFileName, pi4FileId);  
  #else //VENC_VERIFY_FS_SUPPORT

  x_thread_delay(5);
  
  switch (u4Opentype)
  {
    case VENC_FILEIO_TYPE_RT:
      if ((ptFPath=fopen((UCHAR *)strFileName,"rt")) == NULL)  
      {
        ASSERT(0);
        return FALSE;
      }
      break;
      
    case VENC_FILEIO_TYPE_WB:
      if ((ptFPath=fopen((UCHAR *)strFileName,"wb")) == NULL)  
      {
        ASSERT(0);
        return FALSE;
      }
      break;
      
    case VENC_FILEIO_TYPE_RB:
      if ((ptFPath=fopen((UCHAR *)strFileName,"rb")) == NULL)  
      {
        ASSERT(0);
        return FALSE;
      }
      break;
  }

  //*pi4FileId = ptFPath;
  *pi4FileId = (INT32) ptFPath;
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);

  return i4_ret;
}


BOOL fgVEnc_ReadFile(void* pvReadBuffer, UINT32 u4ReadSize, INT32 i4FileId)
{
  INT32 i4_ret = TRUE;
  #ifndef VENC_VERIFY_FS_SUPPORT
  UINT32 i;
  UINT32 u4MaxReadSize = 0x3E800;

  UINT32 u4PartialReadCnt = 0;
  UINT32 u4RemReadSize = 0;
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  i4_ret = fgVEnc_FsReadFile(pvReadBuffer, u4ReadSize, i4FileId);  
  #else //VENC_VERIFY_FS_SUPPORT
  
  if (u4ReadSize <= u4MaxReadSize)
  {
    x_thread_delay(10);
    fread(pvReadBuffer, 1, u4ReadSize, (FILE*) i4FileId);
    x_thread_delay(5);
  }
  else
  {
    u4PartialReadCnt = u4ReadSize/u4MaxReadSize;
    u4RemReadSize = u4ReadSize - u4PartialReadCnt*u4MaxReadSize;

    for (i=0; i<u4PartialReadCnt; i++)
    {
      x_thread_delay(10);
      fread((void*)(pvReadBuffer + i*u4MaxReadSize), 1, u4MaxReadSize, (FILE*) i4FileId);
      x_thread_delay(5);
    }
    x_thread_delay(10);
    fread((void*)(pvReadBuffer + u4PartialReadCnt*u4MaxReadSize), 1, u4RemReadSize, (FILE*) i4FileId);
    x_thread_delay(5);
  }
  
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);

  return i4_ret;
}
#if 0
BOOL fgVEnc_ReadFile(void* pvReadBuffer, UINT32 u4ReadSize, INT32 i4FileId)
{
  INT32 i4_ret = TRUE;
  //FILE *ptFPath;

  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  i4_ret = fgVEnc_FsReadFile(pvReadBuffer, u4ReadSize, i4FileId);  
  #else //VENC_VERIFY_FS_SUPPORT

  x_thread_delay(10);
  
  fread(pvReadBuffer, 1, u4ReadSize, (FILE*) i4FileId);

  x_thread_delay(5);
  
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);

  return i4_ret;
}
#endif //0


BOOL fgVEnc_GetFileSize(UINT32 i4FileId, UINT32* pu4FileSize)
{
  INT32 i4_ret = TRUE;

  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  i4_ret = fgVEnc_FsGetFileSize(i4FileId, pu4FileSize);
  #else //VENC_VERIFY_FS_SUPPORT
  x_thread_delay(5);
  
  fseek((FILE*) i4FileId,0,SEEK_END);

  x_thread_delay(5);
  
  *pu4FileSize = ftell((FILE*) i4FileId);

  x_thread_delay(5);
  
  fseek((FILE*) i4FileId,0,SEEK_SET);
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);
  
  return i4_ret;
}


BOOL fgVEnc_CloseFile(UINT32 i4FileId)
{
  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  DrvFSCloseFile(i4FileId);
  #else //VENC_VERIFY_FS_SUPPORT

  x_thread_delay(5);
  
  fclose((FILE*) i4FileId);
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);
  
  return TRUE;
}


BOOL fgVEnc_SeekFile(UINT32 i4FileId, UINT32 u4FileOffset)
{
  VERIFY (x_sema_lock(_ahFileIOSema, X_SEMA_OPTION_WAIT) == OSR_OK);
  
  #ifdef VENC_VERIFY_FS_SUPPORT
  
  //Null
  
  #else //VENC_VERIFY_FS_SUPPORT

  x_thread_delay(10);
              
  fseek((FILE*) i4FileId, u4FileOffset, SEEK_SET);

  x_thread_delay(10);
  
  #endif //VENC_VERIFY_FS_SUPPORT

  VERIFY (x_sema_unlock(_ahFileIOSema) == OSR_OK);
  
  return TRUE;
}




