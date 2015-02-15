#include "vdec_verify_file_common.h"
#include "vdec_ide.h"
#include "vdec_verify_mpv_prov.h"
#ifdef SATA_HDD_FS_SUPPORT
#include "sata_fs_io.h"
#endif

#ifdef CONFIG_TV_DRV_VFY 
#include "vdec_vfy_debug.h"
#endif // CONFIG_TV_DRV_VFY

#if (!CONFIG_DRV_LINUX)
#include <stdio.h>
#include <string.h>
#include "x_typedef.h"
#include "x_printf.h"
#else
#ifndef VDEC_EMUVER_FILEIO

#include "x_os.h"                       //For <string.h>
#include "x_typedef.h"
#include "x_printf.h"
#include "x_debug.h"

#else //VDEC_EMUVER_FILEIO

#if 0
extern FILE *fopen(const UINT8 *filename, const UINT8 *mode);
extern INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fseek(FILE *stream, INT32 offset, INT32 whence);
extern INT32 fclose(FILE *stream);
extern INT64 ftell(FILE *stream);
#endif

FILE*   linux_fopen(const char *name, const char *mode)
{
    printk("fake linux_open\n");
    return 0;
}

size_t  linux_fread(void *buff, size_t length, size_t cnt, FILE *pfile)
{
    printk("fake linux_fread\n");
    return 0;
}

size_t  linux_fwrite(const void *buff, size_t length, size_t cnt, FILE *pfile)
{
    printk("fake linux_fwrite\n");
   return 0;
}

UINT32   linux_fclose(FILE *pfile)
{
    printk("fake linux_fclose\n");
   return 0;
}


UINT32 linux_ftell (FILE *pfile)
{
    printk("fake linux_ftell\n");
   return 0;
}

UINT32  linux_fseek(FILE *pfile, long offset, int cnt)
{
    printk("fake linux_fseek\n");
   return 0;
}
#endif //VDEC_EMUVER_FILEIO
#endif


void vVDecOutputDebugString(const CHAR * format, ...)
{
#ifdef IDE_WRITE_SUPPORT
  return;
#else

   #if (!CONFIG_DRV_LINUX)
       CHAR szTmpStr[300] = {0};
       va_list argptr;
       va_start(argptr, format);
       vsprintf(szTmpStr, format, argptr);
       szTmpStr[299] = '\0';
       va_end(argptr);
       UTIL_Printf("%s", szTmpStr);
    #endif
#endif
}

UINT32 u4fsize(FILE* pFile)
{
    UINT32 u4Result = 0;
    UINT32 u4Offset = 0;

    if(pFile == NULL)
        return u4Result;

    u4Offset = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    u4Result = ftell(pFile);
    fseek(pFile, u4Offset, SEEK_SET);
    return u4Result;
}

// *********************************************************************
// Function    : void vErrMessage(UINT32 u4InstID, UCHAR *pbDramAddr)
// Description : print error message in record file
// Parameter   : None
// Return      : None
// *********************************************************************
void vErrMessage(UINT32 u4InstID, CHAR *pbDramAddr)
{

}

// *********************************************************************
// Function    : void vDataFromFile(UINT32 u4InstID, UINT32 *pu4W, UINT32 *pu4H)
// Description : print error message in record file
// Parameter   : None
// Return      : None
// *********************************************************************
void vDataFromFile(UINT32 u4InstID, UINT32 *pu4W, UINT32 *pu4H)
{
    FILE* pFile = NULL;
    UINT32 u4Temp;
    UINT32 u4FileSize;

    u4Temp = sprintf(_bTempStr1[u4InstID], "%s", _bFileStr1[u4InstID][0]);
    u4Temp += sprintf(_bTempStr1[u4InstID] + u4Temp, "%s", "width.txt");
    pFile = fopen(_bTempStr1[u4InstID], "rb");
    u4FileSize = u4fsize(pFile);
    if(pFile)
    {
        //fgfileread(u4FILE, (UINT32)pu4W, u4FileSize);
        fread(pu4W, 1, u4FileSize, pFile);
        fclose(pFile);
    }
    else
    {
        sprintf(_bTempStr1[u4InstID], "%s", "No W\\n\\0");
        vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);
    }


    u4Temp = sprintf(_bTempStr1[u4InstID], "%s", _bFileStr1[u4InstID][0]);
    u4Temp += sprintf(_bTempStr1[u4InstID] + u4Temp, "%s", "height.txt");  
    pFile = fopen(_bTempStr1[u4InstID], "rb");
    u4FileSize = u4fsize(pFile);
    if(pFile)
    {
        //fgfileread(u4FILE, (UINT32)pu4H, u4FileSize);
        fread(pu4H, 1, u4FileSize, pFile);
        fclose(pFile);
    }
    else
    {
        sprintf(_bTempStr1[u4InstID], "%s", "No W\\n\\0");
        vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);
    }
}

// *********************************************************************
// Function    : BOOL fgReadPCFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize)
// Description : read bitstream from pc to memory
// Parameter   : None
// Return      : TRUE/FALSE
// *********************************************************************
BOOL fgReadPCFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
    FILE* pFile = NULL;
    UINT32 u4FileSize = 0;
    UINT32 u4ReadSize = u4Size;
    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
    {
        printk("%s @ line %d some param error %s\n",__FUNCTION__,__LINE__,strFileName);
        return FALSE;
    }
    else
    {
        printk("%s @ line %d read file %s go\n",__FUNCTION__,__LINE__,strFileName);
    }

    *pu4RealReadSize = 0;

    if((pFile = fopen(strFileName, "rb")) == NULL)
        return FALSE;
    
    u4FileSize = u4fsize(pFile);
    *pu4FileLength = u4FileSize;

    if (u4OffSet >= u4FileSize)
    {
        fclose(pFile);
        vVDecOutputDebugString("\n read offset(%d) > filesize(%d)", u4OffSet, u4FileSize);
        return FALSE;
    }

    if(fseek (pFile, u4OffSet, SEEK_SET))
    {
        fclose(pFile);
        vVDecOutputDebugString("\n seek fail: %s", strFileName);
        return FALSE;
    }

    if (u4Size > (u4FileSize - u4OffSet))
        u4ReadSize = u4FileSize - u4OffSet;
    //UTIL_Printf("FILE size = %d,buffer size = %d",u4FileSize,u4Size);
    *pu4RealReadSize = fread((void *)pvAddr, 1, u4ReadSize, pFile);
    fclose(pFile);

    if (u4ReadSize != *pu4RealReadSize)
    {
        vVDecOutputDebugString("\n read fail: %s", strFileName);
        return FALSE;
    }

    return TRUE;
}



BOOL fgReadHDDFile(UINT32 u4InstID, BOOL fgReadInfo,  void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength, INT32* pi4FileId)
{
#ifdef  SATA_HDD_READ_SUPPORT
#ifndef  SATA_HDD_FS_SUPPORT

    FILE* pFile = NULL;
    UINT32 u4FILE = 0;
    UINT32 u4FileSize = 0;

    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
    {
        UTIL_Printf("[MPV] fgReadHDDFile, input param error!\n");
        return FALSE;
    }

    *pu4RealReadSize = 0;

    if(0)// '\0' != _bFileStr1[_u4VDecID][9][0])
    {
        //u4FILE = u4fOpenFile(_bFileStr1[_u4VDecID][9], 1/*"rb"*/, &u4FileSize);
    }
    else
    {
        //u4FILE = u4fOpenFile(strFileName, 1/*"rb"*/, &u4FileSize);
    }

     
   if (fgReadInfo)
   {
    u4FILE = i4ReadFileFormHDD(
        u4InstID,
        strFileName,
        pvAddr,
        u4OffSet,
        u4Size,
        pu4RealReadSize,
        pu4FileLength
    );
    }
    else
    {
        if((pFile = fopen(strFileName, "rb")) == NULL)
        {
            return FALSE;
        }
        else
        {
            fclose(pFile);
            return TRUE;
        }
    }

    if (0 != u4FILE)
    {
        UTIL_Printf("[MPV] fgReadHDDFile, open file error!\n");
        return FALSE;
    }

    if (!((u4Size == *pu4RealReadSize) || ((*pu4FileLength - u4OffSet) == *pu4RealReadSize)))
    {
        vVDecOutputDebugString("\n read fail: %s", strFileName);
        UTIL_Printf("[MPV] fgReadHDDFile, read error!\n");
        return FALSE;
    }
#else  
    BOOL fgRet;

    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
    {
      return FALSE;
    }

    *pu4RealReadSize = 0;

    if (fgReadInfo)
    {

      fgRet = fgHDDFsReadFile(
           u4InstID,
           strFileName,
           pvAddr,
           u4OffSet,
           u4Size,
           pu4RealReadSize,
           pu4FileLength,
           pi4FileId
       );

       if (!fgRet)
      {
        //UTIL_Printf("[MPV] fgReadHDDFile, read file result error!");
        return FALSE;
      }

        if (!((u4Size == *pu4RealReadSize) || ((*pu4FileLength - u4OffSet) == *pu4RealReadSize)))
        {
           vVDecOutputDebugString("\n read fail: %s", strFileName);
           UTIL_Printf("[MPV] fgReadHDDFile, read file error!\n");
           return FALSE;
        }

    }
    else
    {
       fgRet = fgHDDFsOpenFile(
           u4InstID,
           strFileName,
           pi4FileId
       );

       if (fgRet)
       {
          fgHDDFsCloseFile(*pi4FileId);
       }
       else
       {
         UTIL_Printf("[MPV] fgReadHDDFile, open file false!\n");
         return FALSE;
       }
        
    }
    
    return TRUE;
#endif

#else
   UTIL_Printf("[MPV] fgReadHDDFile, return false directly!\n");

   return FALSE;
#endif   
}

BOOL fgOpenHDDFile(UINT32 u4InstID, char *strFileName , char *strMode, VDEC_INFO_VERIFY_FILE_INFO_T *ptFileInfo)
{ 
    UINT32 u4Offset;
    BOOL fgRet = FALSE;
    #ifdef RING_VFIFO_SUPPORT
    u4Offset = ptFileInfo->u4FileOffset;
    #else
    u4Offset = 0;
    #endif
    
    strcpy(ptFileInfo->bFileName,strFileName); 

    vHDDLockOpenFileSem(hVdecOpenFileSem);
    if(fgReadHDDFile(u4InstID, ptFileInfo->fgGetFileInfo, ptFileInfo->pucTargetAddr, ptFileInfo->u4TargetSz, strFileName, u4Offset, &(ptFileInfo->u4RealGetBytes), &(ptFileInfo->u4FileLength), &(ptFileInfo->i4FileId)))
    {
        //vVDecOutputDebugString("The file's size is 0x%.8x bytes\n", ptFileInfo->u4FileLength);
        fgRet = TRUE;
    }
    else
    {
        vVDecOutputDebugString("\n NULL \n");
        fgRet = FALSE;
    }    
    vHDDUnlockOpenFileSem(hVdecOpenFileSem);

    return fgRet;
}

// *********************************************************************
// Function    : BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO)
// Description : Write the decoded data to PC for compare
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO)
{
#ifdef IDE_WRITE_SUPPORT
  #if (!CONFIG_DRV_LINUX)
    UTIL_Printf("%s",pvAddr);
  #endif
  return TRUE;
#else

    BOOL fgResult = FALSE;

    #ifdef PCFILE_WRITE
    FILE* pFile = NULL;

    if ((NULL == pvAddr) || (0 == u4Size))
        return FALSE;

    if(pFILE_INFO->pFile == NULL)
    {
        if(pFILE_INFO->bFileName == NULL)
            return FALSE;
        else
            pFILE_INFO->pFile = fopen(pFILE_INFO->bFileName, "wb");
    }
    pFile = pFILE_INFO->pFile;
    
    if(pFile == NULL)
        return FALSE;

    fgResult = (fwrite ((char* )pvAddr, 1, u4Size, pFile) == u4Size);
    if (FALSE == fgResult)
    {
        vVDecOutputDebugString("\n write fail: %s", pFILE_INFO->bFileName);
        fclose(pFILE_INFO->pFile);
    }
    //fclose(pFile);

    #else
      #if (!CONFIG_DRV_LINUX)
         UTIL_Printf("%s", (char* )pvAddr);
      #endif
         UTIL_Printf("%s", (char* )pvAddr);
    fgResult = TRUE;
    
    #endif

    return fgResult;
#endif
}

BOOL fgOverWrData2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, char *strFileName)
{
#ifdef IDE_WRITE_SUPPORT
  #if (!CONFIG_DRV_LINUX)
     UTIL_Printf("%s",pvAddr);
  #endif
  return TRUE;
#else
#ifdef  SATA_HDD_READ_SUPPORT
    INT32 i4Field = 0;
    BOOL fgResult;

    fgResult = fgHDDFsWriteFile(0, strFileName, pvAddr, u4Size, &i4Field);

    return fgResult;
#else
    FILE* pFile = NULL;
    BOOL fgResult = FALSE;

    if ((NULL == pvAddr) || (NULL == strFileName) || (0 == u4Size))
        return FALSE;
    if((pFile = fopen(strFileName, "wb")) == NULL)
        return FALSE;

    fgResult = (fwrite ((char* )pvAddr, 1, u4Size, pFile) == u4Size);
    fclose(pFile);
    
    if (FALSE == fgResult)
    {
        vVDecOutputDebugString("\n write fail: %s", strFileName);
    }
    return fgResult;
#endif
#endif
}

BOOL fgWrData2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, char *strFileName)
{
    #ifdef IDE_WRITE_SUPPORT
    return fgIdeWrData2PC(pvAddr,u4Size, u4Mode,strFileName);
    #else
    return fgOverWrData2PC(pvAddr,u4Size, u4Mode,strFileName);
    #endif
}

BOOL fgOpenPCFile(UINT32 u4InstID, char *strFileName , char *strMode, VDEC_INFO_VERIFY_FILE_INFO_T *ptFileInfo)
{
    FILE *pFile = NULL;
    UINT32 u4Offset;

    #ifdef RING_VFIFO_SUPPORT
    u4Offset = ptFileInfo->u4FileOffset;
    #else
     u4Offset = 0;
    #endif
    printk("%s@%d read %s fgGetFileInfo %d\n",__FUNCTION__,__LINE__,strFileName,ptFileInfo->fgGetFileInfo);
    if(ptFileInfo->fgGetFileInfo)
    {
        strcpy(ptFileInfo->bFileName,strFileName);   
        if(fgReadPCFile(ptFileInfo->pucTargetAddr, ptFileInfo->u4TargetSz, strFileName, u4Offset, &(ptFileInfo->u4RealGetBytes), &(ptFileInfo->u4FileLength)))
        {
            //vVDecOutputDebugString("The file's size is 0x%.8x bytes\n", ptFileInfo->u4FileLength);
            if(ptFileInfo->u4FileLength > ptFileInfo->u4RealGetBytes)
            {
                vVDecOutputDebugString("\n Over scan\n");
            }
            return TRUE;
        }
        else
        {
            vVDecOutputDebugString("\n NULL \n");
            return FALSE;
        }
    }
    else
    {
        if((pFile = fopen(strFileName, "rb")) == NULL)
        {
            return FALSE;
        }
        else
        {
            fclose(pFile);
            return TRUE;
        }
    }

}


// *********************************************************************
// Function    : void fgOpenFile(void)
// Description : Set related files pointer
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgOpenFile(UINT32 u4InstID, char *strFileName , char *strMode, VDEC_INFO_VERIFY_FILE_INFO_T *ptFileInfo)
{
    BOOL fgOpen = TRUE;
    
    #if 0
    UTIL_Printf("[VDEC%d]fgOpenFile: %s\n",
      u4InstID,
      strFileName);
    UTIL_Printf("[VDEC%d]fgOpenFile: fgGet:%d, addr:0x%x, sz:%d, begin\n",
      u4InstID,
      ptFileInfo->fgGetFileInfo,
      ptFileInfo->pucTargetAddr,
      ptFileInfo->u4TargetSz);
    #endif

    #ifdef  SATA_HDD_READ_SUPPORT
          fgOpen = fgOpenHDDFile(u4InstID, strFileName, strMode, ptFileInfo);
      #ifndef  SATA_HDD_FS_SUPPORT
          if(fgOpen == FALSE)
          {
                fgOpen = fgOpenPCFile(u4InstID, strFileName, strMode, ptFileInfo);
          }
      #endif
    #elif defined(IDE_READ_SUPPORT)
          fgOpen = fgOpenIdeFile(strFileName, strMode, ptFileInfo);
          if(fgOpen == FALSE)
          {
              fgOpen = fgOpenPCFile(u4InstID, strFileName, strMode, ptFileInfo);
          }
          
     #else
          fgOpen = fgOpenPCFile(u4InstID, strFileName, strMode, ptFileInfo);
     #endif

     #if 0
     UTIL_Printf("\n fgOpenFile: %s, fgGet:%d, addr:0x%x, sz:%d, real:%d, end\n",
      strFileName,
      ptFileInfo->fgGetFileInfo,
      ptFileInfo->pucTargetAddr,
      ptFileInfo->u4TargetSz,
      ptFileInfo->u4RealGetBytes);
     #endif

     return fgOpen;
}


#ifdef IDE_WRITE_SUPPORT
BOOL fgIdeWrData2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, char *strFileName)
{
    _DmxIdeSendName(strFileName);
    _DmxIdeSendData((UINT32)pvAddr, u4Mode, u4Size);
    return TRUE;
}
#endif


#ifdef IDE_READ_SUPPORT
BOOL fgReadIdeFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
        return FALSE;

    *pu4RealReadSize = 0;

    _DmxIdeSendName(strFileName);
	#ifdef SEMI_RING_FIFO
    _DmxIdeSeek(u4OffSet, SEEK_SET);
	_DmxIdeSendReadLen(u4Size);
	#endif
#if !CONFIG_DRV_LINUX
    if(!_DmxIdeGetFile((UINT32)pvAddr, 0, pu4RealReadSize))
    {
        vVDecOutputDebugString("\n read fail: %s", strFileName);
        return FALSE;
    }
#else
    if(!_DmxIdeGetFile( PHYSICAL((UINT32)pvAddr), 0, pu4RealReadSize, u4Size))
    {
        vVDecOutputDebugString("\n read fail: %s", strFileName);
        return FALSE;
    }
#endif

    *pu4FileLength = *pu4RealReadSize;

    return TRUE;
}

BOOL fgPureOpenIdeFile(char *strFileName , char *strMode, VDEC_INFO_VERIFY_FILE_INFO_T *ptFileInfo)
{
    _DmxIdeSendName(strFileName);
    return(_DmxIdeOpenFile());
}

BOOL fgOpenIdeFile(char *strFileName , char *strMode, VDEC_INFO_VERIFY_FILE_INFO_T *ptFileInfo)
{
    FILE* pFile = NULL;
    UINT32 u4Offset;

    #ifdef RING_VFIFO_SUPPORT
    u4Offset = ptFileInfo->u4FileOffset;
    #else
    u4Offset = 0;
    #endif
	#ifdef SEMI_RING_FIFO
	u4Offset = ptFileInfo->u4BSLenOffset;
	#endif
    if(ptFileInfo->fgGetFileInfo)
    {
        strcpy(ptFileInfo->bFileName,strFileName); 
        if(fgReadIdeFile(ptFileInfo->pucTargetAddr, ptFileInfo->u4TargetSz, strFileName, u4Offset, &(ptFileInfo->u4RealGetBytes), &(ptFileInfo->u4FileLength)))
        { 
			#ifdef SEMI_RING_FIFO
			if(ptFileInfo->u4FileLength > ptFileInfo->u4TargetSz)
				ptFileInfo->ucVFIFOEnough = 0;
			#endif
            return TRUE;
        }
        else
        {
            vVDecOutputDebugString("\n NULL \n");
            return FALSE;
        }
    }
    else
    {
        if((pFile = fopen(strFileName, "rb")) == NULL)
        {
            return FALSE;
        }
        else
        {
            fclose(pFile);
            return TRUE;
        }
    }
}
#endif

