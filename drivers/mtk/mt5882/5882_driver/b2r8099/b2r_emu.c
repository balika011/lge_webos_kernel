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
#ifdef __KERNEL__

#define DRV_LINUX        
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_common.h"
#include "x_rand.h"
#include "x_stl_lib.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
#include "b2r_emu.h"
#ifdef DRV_LINUX
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/param.h>
#else
#include "x_fm.h"
#endif 
#include "x_printf.h"

//-----------------------------------------------------------------------------
// Configurations for test case
//-----------------------------------------------------------------------------

#define EMU_FILE_HANDLE_MAX  4

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct _EMU_FILE_ICE_PRIV_T
{
    UINT32 u4Data;
    UINT32 u4Offset;
    UINT32 u4Size;
}EMU_FILE_ICE_PRIV_T;

typedef struct _EMU_FILE_T
{
    UINT8  used;
    EMU_SRC_TYPE_T eType;
    UINT8  path[256];
    union {
        UINT32 u4Data;
        EMU_FILE_ICE_PRIV_T ice;
    }priv;
} EMU_FILE_T;

typedef struct _EMU_FILE_FUNC_T
{
    BOOL   (*open) (EMU_FILE_T *stream);
    BOOL   (*close)(EMU_FILE_T *stream);
    UINT32 (*read) (void * ptr, UINT32 size, EMU_FILE_T *stream);
    UINT32 (*write)(void * ptr, UINT32 size, EMU_FILE_T *stream);
    UINT32 (*seek) (UINT32 offset, EMU_FILE_T *stream);
    UINT32 (*size) (EMU_FILE_T *stream);
}EMU_FILE_FUNC_T;


//-----------------------------------------------------------------------------
// extern functions
//-----------------------------------------------------------------------------
extern INT32 _t32_HostBreakPT(INT32 i4TargetFlag, CHAR *szHostCommand);


//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// function declares
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// static  functions
//-----------------------------------------------------------------------------
#define EMU_LOG(fmt ...)   Printf(fmt)

//utility

//ICE
enum
{
    E_T32_NOP,
    E_T32_EXEC,
    E_T32_RESTART,
    E_T32_EXIT,
    E_T32_EXEC_RELOAD_SYM,
    E_T32_DETECT_HOST,
    E_T32_GET_FSIZE,
    E_T32_END
};

#if 0
INT32 _t32_HostBreakPT(INT32 i4TargetFlag, CHAR *szHostCommand)
{
    /*
    __asm
    {
        nop
    }
    */
    UNUSED(szHostCommand);
    return i4TargetFlag;
}
#endif

INT32 emu_HostExec(INT32 i4TargetFlag, const CHAR *szFormat, ...)
{
    static CHAR szBuffer[256];
    VA_LIST t_ap;

    VA_START(t_ap, szFormat);
    vsprintf(szBuffer, szFormat, t_ap);
    VA_END(t_ap);

    EMU_LOG("<EMU>ICE_CMD:%d, %s\n", i4TargetFlag, szBuffer);
    return _t32_HostBreakPT( i4TargetFlag, szBuffer );
}

static BOOL emu_ICE_open (EMU_FILE_T *stream)
{
    stream->priv.ice.u4Offset = 0;  //offset
    stream->priv.ice.u4Size   = 0;

    stream->priv.ice.u4Size = (UINT32)emu_HostExec(E_T32_GET_FSIZE, "%s", stream->path);

    return TRUE;
}

static BOOL emu_ICE_close(EMU_FILE_T *stream)
{
    return TRUE;
}

static UINT32 emu_ICE_read (void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    CHAR _data[2];
    UINT32 _size;
    UINT32 _offset;

    _offset = stream->priv.ice.u4Offset;

    if (_offset + size > stream->priv.ice.u4Size)
    {
        _size = (stream->priv.ice.u4Size > _offset) ? (stream->priv.ice.u4Size - _offset):0;
    }
    else
    {
        _size = size;
    }

    if (_size == 0)
    {

    }
    else if (_size == 1)
    {
        emu_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x++0x1 /SKIP 0x%x /ny", stream->path, (UINT32)_data, _offset);
        *(CHAR *)ptr = _data[0];
    }
    else
    {
        emu_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x++0x%x /SKIP 0x%x /ny", stream->path, (UINT32)ptr, (_size-1), _offset);
    }

    stream->priv.ice.u4Offset += _size;

    return _size;
}

static UINT32 emu_ICE_write(void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    return 0;
}

static UINT32 emu_ICE_seek (UINT32 offset, EMU_FILE_T *stream)
{
    if (offset < stream->priv.ice.u4Size)   // should use "<",liuqu,20110808
    {
        stream->priv.ice.u4Offset = offset;
    }
    else
    {
        stream->priv.ice.u4Offset = stream->priv.ice.u4Size;
    }

    return stream->priv.ice.u4Offset;
}

static UINT32 emu_ICE_size (EMU_FILE_T *stream)
{
    return stream->priv.ice.u4Size;
}

//IDE
//todo



//USB
#ifdef DRV_LINUX

static BOOL emu_USB_open (EMU_FILE_T *stream)
{
    struct file *fp = NULL;

    if (!stream)
    {
        return FALSE;
    }
   Printf("emu_USB_open dir=%s\n",stream->path);
    fp = filp_open((char *)stream->path, O_RDONLY, 0);

    if (IS_ERR(fp))
    {
        return FALSE;
    }
  Printf("emu_USB_open dir=%s,IS_ERR(fp)=%d\n",stream->path,IS_ERR(fp));
    stream->priv.u4Data = (UINT32)fp;

    return TRUE;
}

static BOOL emu_USB_close(EMU_FILE_T *stream)
{
    struct file *fp = NULL;

    if (!stream)
    {
        return FALSE;
    }

    fp = (struct file *)(stream->priv.u4Data);

    if (IS_ERR(fp))
    {
        return FALSE;
    }

    filp_close (fp, NULL);

    return TRUE;
}

static UINT32 emu_USB_read (void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    struct file *fp = NULL;

    if (!stream)
    {
        return 0;
    }
    fp = (struct file *)(stream->priv.u4Data);

    if (IS_ERR(fp))
    {
        return 0;
    }

    return fp->f_op->read(fp, (char *)ptr, (size_t)size, &fp->f_pos);
}

static UINT32 emu_USB_write(void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    return 0;
}

static UINT32 emu_USB_seek (UINT32 offset, EMU_FILE_T *stream)
{
    struct file *fp = NULL;

    if (!stream)
    {
        return 0;
    }

    fp = (struct file *)(stream->priv.u4Data);

    if (IS_ERR(fp))
    {
        return 0;
    }

    return (UINT32)fp->f_op->llseek(fp, (loff_t)offset, SEEK_SET);
}

static UINT32 emu_USB_size (EMU_FILE_T *stream)
{
    loff_t cur_pos, length;
    struct file *fp = NULL;

    if (!stream)
    {
        return 0;
    }

    fp = (struct file *)(stream->priv.u4Data);

    if (IS_ERR(fp))
    {
        return 0;
    }

    cur_pos = fp->f_pos;

    fp->f_op->llseek(fp, 0, SEEK_END);

    length = fp->f_pos;

    fp->f_op->llseek(fp, cur_pos, SEEK_SET);
    
    return (UINT32)length;
    
}

#else /*DRV_LINUX*/

static BOOL emu_USB_open (EMU_FILE_T *stream)
{
    HANDLE_T h_file = 0;

    if (!stream)
    {
        return FALSE;
    }

    if (FMR_OK != x_fm_open(FM_ROOT_HANDLE, (char *)stream->path, FM_READ_ONLY, 0777, FALSE, &h_file))
    {
        return FALSE;
    }

    stream->priv.u4Data = (UINT32)h_file;

    return TRUE;
}

static BOOL emu_USB_close(EMU_FILE_T *stream)
{
    HANDLE_T h_file = 0;

    if (!stream)
    {
        return FALSE;
    }

    h_file = (HANDLE_T)(stream->priv.u4Data);

    x_fm_close(h_file);

    return TRUE;
}

static UINT32 emu_USB_read (void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    HANDLE_T h_file = 0;
    UINT32 u4Read = 0;
    if (!stream)
    {
        return 0;
    }
    
    h_file = (HANDLE_T)(stream->priv.u4Data);

    if (!h_file)
    {
        return 0;
    }

    x_fm_read(h_file, (VOID *)ptr, size, &u4Read);

    ASSERT(size == u4Read);

    return u4Read;
}

static UINT32 emu_USB_write(void * ptr, UINT32 size, EMU_FILE_T *stream)
{
    return 0;
}

static UINT32 emu_USB_seek (UINT32 offset, EMU_FILE_T *stream)
{
    HANDLE_T h_file = 0;
    UINT64 u8Tmp;
    if (!stream)
    {
        return 0;
    }
    h_file = (HANDLE_T)(stream->priv.u4Data);

    VERIFY(FMR_OK == x_fm_lseek(h_file, (UINT64)offset, FM_SEEK_BGN, &u8Tmp));

    return (UINT32)u8Tmp;
}

static UINT32 emu_USB_size (EMU_FILE_T *stream)
{
    HANDLE_T h_file = 0;
    UINT64 curpos, length, pos;
    if (!stream)
    {
        return 0;
    }
    h_file = (HANDLE_T)(stream->priv.u4Data);

    VERIFY(FMR_OK == x_fm_lseek(h_file, 0, FM_SEEK_CUR, &curpos));
    VERIFY(FMR_OK == x_fm_lseek(h_file, 0, FM_SEEK_END, &length));
    VERIFY(FMR_OK == x_fm_lseek(h_file, curpos, FM_SEEK_BGN, &pos));

    return (UINT32)length;
}
#endif /*DRV_LINUX*/

static EMU_FILE_T _emu_files [EMU_FILE_HANDLE_MAX] = {{0}, {0}, {0}, {0}};

static EMU_FILE_FUNC_T _emu_file_func []=
{
    //ICE
    {emu_ICE_open,  emu_ICE_close,  emu_ICE_read,   emu_ICE_write,  emu_ICE_seek,   emu_ICE_size},
    //IDE
    {NULL, NULL, NULL, NULL, NULL, NULL},
    //USB
    {emu_USB_open,  emu_USB_close,  emu_USB_read,   emu_USB_write,  emu_USB_seek,   emu_USB_size}
};


//-----------------------------------------------------------------------------
// Public function : 
//-----------------------------------------------------------------------------
EMU_FILE_HANDLE emu_fopen(CHAR *pu1Path, EMU_SRC_TYPE_T eType)
{
    UINT32 i;
    EMU_FILE_T *pEmuFile;
    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;

    for (i = 0; i < EMU_FILE_HANDLE_MAX; i++)
    {
        if (!_emu_files[i].used)
        {
            break;
        }
    }

    if (i == EMU_FILE_HANDLE_MAX)
    {
        return EMU_FILE_HANDLE_NULL;
    }

    pEmuFile = &_emu_files[i];
    strcpy((char *)pEmuFile->path, (char *)pu1Path);

    //sub part
    pEmuFileFunc = &_emu_file_func[eType];
    if (!(pEmuFileFunc->open && pEmuFileFunc->open(pEmuFile)))
    {
        return EMU_FILE_HANDLE_NULL;
    }

    pEmuFile->eType = eType;
    pEmuFile->used  = 1;

    return i;
}

VOID emu_fclose(EMU_FILE_HANDLE hHandle)
{
    EMU_FILE_T *pEmuFile = NULL;
    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;

    if (hHandle >= 0 && hHandle < EMU_FILE_HANDLE_MAX)
    {
        pEmuFile = &_emu_files[hHandle];
    }

    if (!(pEmuFile && pEmuFile->used))
    {
        return;
    }

    //sub part
    pEmuFileFunc = &_emu_file_func[pEmuFile->eType];
    if (pEmuFileFunc->close)
    {
        pEmuFileFunc->close(pEmuFile);
    }

    memset((void *)pEmuFile, 0, sizeof(EMU_FILE_T));

    return;
}

UINT32 emu_fread(EMU_FILE_HANDLE hHandle, CHAR *pu1Addr, UINT32 u4Size)
{
    UINT32 u4Ret = 0;

    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;
    EMU_FILE_T *pEmuFile = NULL;
    pEmuFile     = &_emu_files[hHandle];
    pEmuFileFunc = &_emu_file_func[pEmuFile->eType];

    if (pEmuFileFunc->read)
    {
        u4Ret = pEmuFileFunc->read((void *)pu1Addr, u4Size, pEmuFile);
    }
    return u4Ret;
}

UINT32 emu_fwrite(EMU_FILE_HANDLE hHandle, UINT8 *pu1Addr, UINT32 u4Size)
{
    UINT32 u4Ret = 0;
    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;
    EMU_FILE_T *pEmuFile = NULL;
    pEmuFile     = &_emu_files[hHandle];
    pEmuFileFunc = &_emu_file_func[pEmuFile->eType];

    if (pEmuFileFunc->write)
    {
        u4Ret = pEmuFileFunc->write((void *)pu1Addr, u4Size, pEmuFile);
    }

    return u4Ret;
}

UINT32 emu_fsize(EMU_FILE_HANDLE hHandle)
{
    UINT32 u4Ret = 0;
    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;
    EMU_FILE_T *pEmuFile = NULL;
    pEmuFile     = &_emu_files[hHandle];
    pEmuFileFunc = &_emu_file_func[pEmuFile->eType];

    if (pEmuFileFunc->size)
    {
        u4Ret = pEmuFileFunc->size(pEmuFile);
    }
    EMU_LOG("file size:%d\n", u4Ret);
    return u4Ret;
}

UINT32 emu_fseek(EMU_FILE_HANDLE hHandle, UINT32 u4Offset)
{
    UINT32 u4Ret = 0;
    EMU_FILE_FUNC_T *pEmuFileFunc = NULL;
    EMU_FILE_T *pEmuFile = NULL;
    pEmuFile     = &_emu_files[hHandle];
    pEmuFileFunc = &_emu_file_func[pEmuFile->eType];

    if (pEmuFileFunc->seek)
    {
        u4Ret = pEmuFileFunc->seek(u4Offset, pEmuFile);
    }

    return u4Ret;
}

//utilly
inline static char *__strpbrk(const char *cs, const char *ct)
{
    const char *sc1, *sc2;

    for (sc1 = cs; *sc1 != '\0'; ++sc1)
    {
        for (sc2 = ct; *sc2 != '\0'; ++sc2) 
        {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;
}

char *emu_strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == NULL)
        return NULL;

    end = __strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
        
    *s = end;
    return sbegin;
}

char *emu_str_find(char* s, char* a)
{
    UINT32 i= 0;
    UINT32 n;
    n = x_strlen(a);

    while (s[i])
    {
        if (x_strncmp((const char*)(s+i), (const char*)a, n) == 0)
        {
            return s+i;
        }
        i++;
    }
    return 0;
}

void emu_item_add(EMU_ITEM_T *ptItems, UINT32 pos, CHAR *data)
{
    x_memset((void *)(&ptItems[pos]), 0, sizeof(EMU_ITEM_T));
    if (pos)
    {
        ptItems[pos-1].next = &ptItems[pos];
    }
    if (data)
    {
        x_strcpy((char *)(ptItems[pos].data), data);
    }
}

UINT32 emu_str2int(char *str)
{
    UINT32 u4Len;

    if (str == NULL)
    {
        return 0;
    }

    u4Len = strlen(str);

    if ((str[0] == '0') && (str[1] == 'x'))
    {
        return StrToHex(&str[2], u4Len - 2);
    }

    #if 0
    //remove invalid bytes
    while (u4Len > 0 && !(str[u4Len-1]>='0' && str[u4Len-1]<='9'))
    {
        u4Len--;
    }
    #endif
    
    return StrToInt(str);
}


