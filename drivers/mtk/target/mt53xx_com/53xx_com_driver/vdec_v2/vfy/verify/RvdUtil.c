/*****************************************************************************
* LEGAL DISCLAIMER
*
* (Header of MediaTek Software/Firmware Release or Documentation)
*
* BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN
* "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER
* DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF
* ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE
* MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR
* ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO CONFORM TO
* A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR
* REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK
* FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH
* THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS
* PRINCIPLES.
*****************************************************************************/
#include "RvdUtil.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "x_typedef.h"


typedef enum {
  RVD_CMD_NONE,
  RVD_CMD_READFILE,
  RVD_CMD_WRITEFILE,
  RVD_CMD_PRINTF,
  RVD_CMD_FOPEN,
  RVD_CMD_FCLOSE,
  RVD_CMD_FPRINTF,
  RVD_CMD_FPRINTF2,
  RVD_CMD_FREAD,
  RVD_CMD_FWRITE,
  VSV_CMD_MAX
} ERvdUtilCmd;


UINT32 _u4RvdUtilCmd = RVD_CMD_NONE;
volatile INT32 _i4RvdReturnValue;

// File parameter
char *_pcRvdFileName;
UINT32 _u4RvdFileBufSa;
UINT32 _u4RvdFileBufSize;

UINT32 _u4RvdFileHandle;
char *_pcRvdFileMode;

// Printf parameter
BYTE _abRvdPrintfMsg[256];


void vRvdUtil(void)
{
}


INT32 i4RvdReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    // Parameter setting
    _pcRvdFileName = pcFileName;
    _u4RvdFileBufSa = u4FileBufSa;
    _u4RvdFileBufSize = u4BufSize;

    _u4RvdUtilCmd = RVD_CMD_READFILE;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to Read File

    return _i4RvdReturnValue;
}


INT32 i4RvdWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    // Parameter setting
    _pcRvdFileName = pcFileName;
    _u4RvdFileBufSa = u4FileBufSa;
    _u4RvdFileBufSize = u4BufSize;

    _u4RvdUtilCmd = RVD_CMD_WRITEFILE;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to Write File

    return _i4RvdReturnValue;
}


INT32 i4RvdPrintf(const char *format, ...)
{
    va_list ap;

    // Parameter setting
    va_start(ap, format);
    vsprintf((char *)_abRvdPrintfMsg, format, ap);
    va_end(ap);

    _u4RvdUtilCmd = RVD_CMD_PRINTF;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to printf

    return _i4RvdReturnValue;
}


INT32 i4RvdFprintf2(char *pcFileName,const char *format, ...)
{
    va_list ap;

    // Parameter setting
    va_start(ap, format);
    vsprintf((char *)_abRvdPrintfMsg, format, ap);
    va_end(ap);
    _pcRvdFileName = pcFileName;


    _u4RvdUtilCmd = RVD_CMD_FPRINTF2;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to Fprintf

    return _i4RvdReturnValue;
}


#define RVD_MIN_FILE_HANDLE 100
#define RVD_MAX_FILE_HANDLE 1024
UINT32 _au4RvdFileHandleUsed[32] = { 0 };
#define RvdGetFileHandleUsed(FileHandle) ((_au4RvdFileHandleUsed[(FileHandle>>5)] >> (FileHandle&0x1F)) & 1)
#define RvdSetFileHandleUsed(FileHandle) (_au4RvdFileHandleUsed[(FileHandle>>5)] |= (1 << (FileHandle&0x1F)))
#define RvdClrFileHandleUsed(FileHandle) (_au4RvdFileHandleUsed[(FileHandle>>5)] &= ~(1 << (FileHandle&0x1F)))
UINT32 _u4RvdFileHandleIdx = RVD_MIN_FILE_HANDLE;


INT32 i4RvdFopen(char *pcFileName,char *pcMode)
{
    INT32 i4FileHandle;

    // RVD does not generate handle automatically,
    // So generate handle here.
    i4FileHandle = _u4RvdFileHandleIdx+1;
    while(RvdGetFileHandleUsed(i4FileHandle))
    {
        i4FileHandle++;
        if(i4FileHandle>RVD_MAX_FILE_HANDLE)
           i4FileHandle = RVD_MIN_FILE_HANDLE;

        if(i4FileHandle == _u4RvdFileHandle) // All file handle are used.
            return -1;
    }
    _u4RvdFileHandleIdx = i4FileHandle;

    // Parameter setting
    _pcRvdFileName = pcFileName;
    _pcRvdFileMode = pcMode;
    _u4RvdFileHandle = _u4RvdFileHandleIdx;

    _u4RvdUtilCmd = RVD_CMD_FOPEN;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to fopen

    if(_i4RvdReturnValue >= 0)
        RvdSetFileHandleUsed(_u4RvdFileHandleIdx);

    return _i4RvdReturnValue;
}


INT32 i4RvdFclose(INT32 i4FileHandle)
{
    if(!RvdGetFileHandleUsed(i4FileHandle))
        return -1;

    // Parameter setting
    _u4RvdFileHandle = i4FileHandle;

    _i4RvdReturnValue = -1; // This value should be modified by RVD
    _u4RvdUtilCmd = RVD_CMD_FCLOSE;
    vRvdUtil(); // Trigger RVD to fclose

    
    RvdClrFileHandleUsed(i4FileHandle);
    return _i4RvdReturnValue;
}


INT32 i4RvdFprintf(INT32 i4FileHandle,const char *format, ...)
{
    va_list ap;

    // Parameter setting
    va_start(ap, format);
    vsprintf((char *)_abRvdPrintfMsg, format, ap);
    va_end(ap);
    _u4RvdFileHandle = i4FileHandle;

    _u4RvdUtilCmd = RVD_CMD_FPRINTF;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to fprintf

    return _i4RvdReturnValue;
}


INT32 i4RvdFread(INT32 i4FileHandle,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    _u4RvdFileHandle = i4FileHandle;
    _u4RvdFileBufSa = u4FileBufSa;
    _u4RvdFileBufSize = u4BufSize;

    _u4RvdUtilCmd = RVD_CMD_FREAD;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to Read File

    return _i4RvdReturnValue;
}


INT32 i4RvdFwrite(INT32 i4FileHandle,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    _u4RvdFileHandle = i4FileHandle;
    _u4RvdFileBufSa = u4FileBufSa;
    _u4RvdFileBufSize = u4BufSize;

    _u4RvdUtilCmd = RVD_CMD_FWRITE;
    _i4RvdReturnValue = -1; // This value should be modified by RVD
    vRvdUtil(); // Trigger RVD to Read File

    return _i4RvdReturnValue;
}

#endif
