/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2004,2005,2006,2007,2008,2009 Sony Corporation
 */
//#include <stdio.h>
#include <errno.h>
#if defined(_WIN32)
#include <windows.h>
#else /* !_WIN32 */
#include <unistd.h>
#endif /* !_WIN32 */

#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "mem_api_iptves.h"
#include "fio_api_iptves.h"

COMPONENT(BASE);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/


//MTK FIX

typedef struct {
    int   openmode;
    //FILE *handle;
} fio_l_context_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/


#if 0
static int
l_Fopen(FILE **out_fp, const char *in_fname, const char *in_mode)
{
    FILE *fp = NULL;

    exh_InitOk();

#if defined(_WIN32)
    int ret = fopen_s(&fp, in_fname, in_mode);

    if (ret != 0) {
        MSG1(("fopen_s errno %i", ret));
        if (ret == ENOENT)
            exh_Throw(RET_FIO_NOTEXIST);
        else
            exh_Throw(RET_FIO_CANNOTOPEN);
    }
#else
    fp = fopen(in_fname, in_mode);

    if (fp == NULL) {
        MSG1(("fopen error %i", errno));
        if (errno == ENOENT)
            exh_Throw(RET_FIO_NOTEXIST);
        else
            exh_Throw(RET_FIO_CANNOTOPEN);
    }
#endif /* _WIN32 */

    *out_fp = fp;

exh_CLEANUP:
    exh_Return();
}
#endif


/*============================================================================
 * Global Functions
 *==========================================================================*/
int
fio_Open(const char* in_filename, int in_openmode, fio_handle_t *out_handle)
{
#if 0
    fio_l_context_t *context = NULL;
    FILE *fin;

    exh_InitOk();
    MSGENT();

    if (!out_handle) exh_Throw(RET_ILLEGALARGS);
    switch (in_openmode) {
    case FIO_OPENMODE_READ:
        exh_Check(l_Fopen(&fin, in_filename, "rb"));
        break;
    case FIO_OPENMODE_WRITE:
        exh_Check(l_Fopen(&fin, in_filename, "wb"));
        break;
    case FIO_OPENMODE_READWRITE:
        exh_Check(l_Fopen(&fin, in_filename, "r+b"));
        break;
    case FIO_OPENMODE_APPEND:
        exh_Check(l_Fopen(&fin, in_filename, "ab"));
        break;
    case FIO_OPENMODE_READ_TEXT:
        exh_Check(l_Fopen(&fin, in_filename, "rt"));
        break;
    case FIO_OPENMODE_WRITE_TEXT:
        exh_Check(l_Fopen(&fin, in_filename, "wt"));
        break;
    case FIO_OPENMODE_READWRITE_TEXT:
        exh_Check(l_Fopen(&fin, in_filename, "r+t"));
        break;
    case FIO_OPENMODE_APPEND_TEXT:
        exh_Check(l_Fopen(&fin, in_filename, "at"));
        break;
    default:
        exh_Throw(RET_FIO_ILLEGALMODE);
    }

    context = (fio_l_context_t *)mem_Calloc(1, sizeof(fio_l_context_t));
    if (!context) {
        fclose(fin);
        exh_Throw(RET_NOMEM);
    }
    context->openmode = in_openmode;
    context->handle = fin;
    *out_handle = (fio_handle_t)context;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif

	return 0;

}

int
fio_Close(fio_handle_t io_handle)
{
#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;

    exh_InitOk();
    MSGENT();

    if (!context) exh_Throw(RET_ILLEGALHANDLE);
    fclose(context->handle);
    mem_Free(context);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif
	
}

int
fio_Read(fio_handle_t io_handle, fio_size_t in_size, fio_size_t *out_size, void *out_buffer)
{
#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;
    fio_size_t size;
    FILE* fin;

    exh_InitOk();
    MSGENT();

    if (!out_size || !out_buffer || in_size <= 0) exh_Throw(RET_ILLEGALARGS);
    if (!context || !(fin = context->handle) ||
        (context->openmode & FIO_L_OPENMODEFLAG_READ) == 0)
        exh_Throw(RET_ILLEGALHANDLE);
    size = fread(out_buffer, 1, in_size, fin);
    if (size == 0 && !feof(fin)) {
        MSG1(("fread errno %i", errno));
        exh_Throw(RET_FIO_CANNOTREAD);
    }
    *out_size = size;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif

}

int
fio_Write(fio_handle_t io_handle, u_int32_t in_size, const void *in_buffer)
{
#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;
    u_int32_t size;
    FILE *fout;

    exh_InitOk();
    MSGENT();

    if (!in_buffer || in_size <= 0) exh_Throw(RET_ILLEGALARGS);
    if (!context || !(fout = context->handle)) exh_Throw(RET_ILLEGALHANDLE);
    if ((context->openmode & (FIO_L_OPENMODEFLAG_WRITE | FIO_L_OPENMODEFLAG_APPEND)) == 0)
        exh_Throw(RET_ILLEGALHANDLE);
    size = fwrite(in_buffer, 1, in_size, fout);
    if (size < in_size) {
        MSG1(("fwrite errno %i", errno));
        exh_Throw(RET_FIO_CANNOTWRITE);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();

#endif

}

int
fio_Seek(fio_handle_t io_handle, fio_offset_t in_offset, int in_whence)
{

#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;
    int whence;
    FILE *fp;

    exh_InitOk();
    MSGENT();

    if (!context || !(fp = context->handle)) exh_Throw(RET_ILLEGALHANDLE);
    switch (in_whence) {
    case FIO_SEEK_SET:
        whence = SEEK_SET;
        break;
    case FIO_SEEK_CUR:
        whence = SEEK_CUR;
        break;
    case FIO_SEEK_END:
        whence = SEEK_END;
        break;
    default:
        exh_Throw(RET_ILLEGALARGS);
    }
    if (fseek(fp, in_offset, whence)) {
        MSG1(("fseek errno %i", errno));
        exh_Throw(RET_FIO_CANNOTSEEK);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif	
}

int
fio_Tell(fio_handle_t io_handle, fio_offset_t *out_size)
{

#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;
    fio_offset_t size;
    FILE *fp;

    exh_InitOk();
    MSGENT();

    if (!out_size) exh_Throw(RET_ILLEGALARGS);
    if (!context || !(fp = context->handle)) exh_Throw(RET_ILLEGALHANDLE);
    if ((size = ftell(fp)) < 0) {
        MSG1(("ftell errno %i", errno));
        exh_Throw(RET_FIO_CANNOTTELL);
    }
    *out_size = size;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif

}

int
fio_Length(fio_handle_t io_handle, fio_size_t *out_size)
{
#if 0
    fio_l_context_t *context = (fio_l_context_t *)io_handle;
    fio_offset_t cur, size;
    FILE *fp;

    exh_InitOk();
    MSGENT();

    if (!out_size) exh_Throw(RET_ILLEGALARGS);
    if (!context || !(fp = context->handle)) exh_Throw(RET_ILLEGALHANDLE);
    if ((cur = ftell(fp)) < 0) exh_Throw(RET_FIO_CANNOTTELL);
    if (fseek(fp, 0, SEEK_END)) exh_Throw(RET_FIO_CANNOTSEEK);
    if ((size = ftell(fp)) < 0) exh_Throw(RET_FIO_CANNOTTELL);
    if (fseek(fp, cur, SEEK_SET)) exh_Throw(RET_FIO_CANNOTSEEK);
    *out_size = (fio_size_t)size;

exh_CLEANUP:
    if (!exh_IsOk()) {
        MSG1(("fio_Length error %x : errno %i", exh_Get(), errno));
    }
    MSGEXT(exh_Get());
    exh_Return();
#endif	
}

int
fio_Remove(const char *in_filename)
{

#if 0
#if defined(_WIN32)
    DWORD dwAttr;
#endif /* _WIN32 */

    exh_InitOk();
    MSGENT();

#if defined(_WIN32)
    dwAttr = GetFileAttributes(in_filename);
    if (dwAttr == 0xFFFFFFFF)
        exh_Throw(RET_FIO_NOTEXIST);

    if (!DeleteFile(in_filename)) exh_Throw(RET_FIO_CANNOTOPEN);
#else /* !_WIN32 */
    if (unlink(in_filename)) {
        if (errno == ENOENT)
            exh_Throw(RET_FIO_NOTEXIST);
        exh_Throw(RET_FIO_CANNOTOPEN);
    }
#endif /* !_WIN32 */

    if (errno) {
        MSG1(("fio_Remove errno %i", errno));
    }
exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
#endif	
}

