
#include "x_typedef.h"
#include "vdec_verify_mpv_prov.h"

#ifdef SATA_HDD_FS_SUPPORT

#include "drv_fs.h"

#ifndef CONFIG_TV_DRV_VFY
#include <stdio.h>
#endif // CONFIG_TV_DRV_VFY

#include "x_printf.h"
#include "x_debug.h"
#include "x_stl_lib.h"


BOOL fgHDDFsMount(UINT32 u4InstID)
{
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
    
    return TRUE;
}

BOOL fgHDDFsUnMount (UINT32 u4InstID)
{
    DrvFSUnMount();    
    return TRUE;
}

BOOL fgHDDFsOpenFile(  UINT32 u4InstID,
    CHAR *strFileName,
    INT32 *pi4FileId)
{

    INT32 i4_ret;
    INT32 i4Fd;

     i4_ret = DrvFSOpenFile(strFileName, DRV_FS_RDONLY, &i4Fd);
     if (i4_ret < 0)
     {
         UTIL_Printf("Fs open file fail %d\n", i4_ret);
         return FALSE;
     }

     *pi4FileId = i4Fd;
     
     return TRUE;
}

BOOL fgHDDFsCloseFile(  UINT32 i4FileId)
{
   DrvFSCloseFile(i4FileId);
   return TRUE;
}

BOOL fgHDDFsReadFile(  UINT32 u4InstID,
    CHAR *strFileName,
    void* pvAddr,
    UINT32 u4Offset,
    UINT32 u4Length,
    UINT32 *pu4RealReadSize,
    UINT32 *pu4TotalFileLength,
    INT32 *pi4FileId)
{

    INT32 i4_ret;
    INT32 i4Fd;
    //UINT32 u4FILE = 0;
    UINT32 u4FileSize = 0;
    UINT32 u4ReadSize = 0;

     i4_ret = DrvFSOpenFile(strFileName, DRV_FS_RDONLY, &i4Fd);
     if (i4_ret < 0)
     {
         UTIL_Printf("Fs open file fail %d\n", i4_ret);
         //ASSERT(0);
         return 0;
     }

     *pi4FileId = i4Fd;
    
//          UTIL_Printf(" i4Fd(%x)\n", i4Fd);
      i4_ret = DrvFSGetFileSize(i4Fd, &u4FileSize);
      if (i4_ret < 0)
      {
         UTIL_Printf("Fs get file size fail %d\n", i4_ret);
         //ASSERT(0);
         return 0;
      }
      
      *pu4TotalFileLength = u4FileSize;             

      if (u4Offset >= u4FileSize)
      {
          DrvFSCloseFile(i4Fd);
          UTIL_Printf(" read offset(%d) > filesize(%d)\n", u4Offset, u4FileSize);
          return FALSE;
      }

      if(DrvFSSeekFile(i4Fd, (INT64) u4Offset, 0))
      {
         DrvFSCloseFile(i4Fd);
         UTIL_Printf(" seek fail: %s\n", strFileName);
         return FALSE;
      }

 //         UTIL_Printf("u4Length(%d), read offset(%d), filesize(%d)\n ", u4Length, u4Offset, u4FileSize);
    if (u4Length >= (u4FileSize - u4Offset))
        u4ReadSize = u4FileSize - u4Offset;
    
    
 //         UTIL_Printf(" u4ReadSize(%d), filesize(%d)\n", u4ReadSize, u4FileSize);
    
    i4_ret = DrvFSReadFile(i4Fd, pvAddr, u4ReadSize);
    if(i4_ret < 0)
    {
          UTIL_Printf("read file Fail!\n");
          return FALSE;
    }
    DrvFSCloseFile(i4Fd);
    
   *pu4RealReadSize = i4_ret;
 //        UTIL_Printf("i4_ret : %d\n", i4_ret);
//       UTIL_Printf(" read fail: %s\n", strFileName);
    if (u4ReadSize != *pu4RealReadSize)
    {
        UTIL_Printf("\n read fail: %s", strFileName);
        return FALSE;
    }

    return TRUE;
    
}


BOOL fgHDDFsWriteFile(  UINT32 u4InstID,
    CHAR *strFileName,
    void* pvAddr,
    UINT32 u4Length,
    INT32 *pi4FileId)
{

    INT32 i4_ret;
    INT32 i4Fd;

     i4_ret = DrvFSOpenFile(strFileName, DRV_FS_W_C, &i4Fd);
     if (i4_ret < 0)
     {
         UTIL_Printf("Fs open file fail %d\n", i4_ret);
         ASSERT(0);
         return 0;
     }

     *pi4FileId = i4Fd;
    
    i4_ret = DrvFSWriteFile(i4Fd, pvAddr, u4Length);
    if(i4_ret < 0)
    {
          UTIL_Printf("write file Fail!\n");
          return FALSE;
    }

    DrvFSCloseFile(i4Fd);
    return TRUE;
    
}


#endif


