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

//============================================================================
// Include files
//============================================================================
#include <linux/module.h>
#include <linux/slab.h>
#include "x_os.h"

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


FILE *fopen(const UINT8 *filename, const UINT8 *mode)
{
  INT32 ret;
  FILE *fp;

  UINT32 sys_mode;

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