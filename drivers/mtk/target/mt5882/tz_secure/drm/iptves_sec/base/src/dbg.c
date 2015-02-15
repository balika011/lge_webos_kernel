/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2004,2005,2006,2007,2008,2009 Sony Corporation
 */
//#include <stdio.h>
#include <stdarg.h>

#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define DBG_L_CTX_MAX           1
#define DBG_L_INDENT_STEP       2
#define DBG_L_ENAME_MAX         32
#define DBG_L_FNAME_MAX         256
#define DBG_L_ERR_STK_MAX       32

#define DBG_L_FP        stdout

#if defined(_WIN32)
#define fprintf fprintf_s
#endif

#define L_BUFFERSIZE    1000
//#define L_PRINT(b)      do{fprintf(DBG_L_FP,"%s",b);fflush(DBG_L_FP);}while(0)
#define L_PRINT(b)      do{dprintf("%s",b);}while(0)

int iptv_tee_log_enable = 0;

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int32_t   err;
    char        ename[DBG_L_ENAME_MAX];
    char        fname[DBG_L_FNAME_MAX];
    char        func[DBG_L_FNAME_MAX];
    int         line;
} err_stk_t;

typedef struct {
    int         id;
    int         level;
    u_int32_t   indent;
    int         stk_cnt;
    err_stk_t   err_stk[DBG_L_ERR_STK_MAX];
} dbg_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/
static dbg_ctx_t        dbg_l_ctx_tbl[DBG_L_CTX_MAX];
static int              dbg_l_initialized = 0;

/*============================================================================
 * Local Functions
 *==========================================================================*/

static unsigned int _u4Debug = 1;

static int
l_Vsnprintf(char               *buffer,
            size_t              sizeOfBuffer,
            size_t              count,
            const char         *format,
            va_list             argptr)
{
#if defined(_WIN32)
    int ret;
    ret = _vsnprintf_s(buffer, sizeOfBuffer, count, format, argptr);
    if (ret == -1)
        L_PRINT(("l_Vsnprintf [failed]"));
    return ret;
#else
    //return (vsnprintf(buffer, count, format, argptr));
    return 0;
#endif
}




static int
l_Snprintf(char                *buffer,
           size_t               sizeOfBuffer,
           size_t               count,
           const char          *format,
           ...)
{
    int ret;
    va_list args;

    va_start(args, format);
    //ret = l_Vsnprintf(buffer, sizeOfBuffer, count, format, args); @@@@@@@@@@@@
	ret = sprintf(buffer, format, args);
    va_end(args);
    return ret;
	
}



static dbg_ctx_t *
dbg_l_GetContext(void)
{
#if 0
    int i;

    assert(dbg_l_initialized > 0);

    for (i = 0; i < DBG_L_CTX_MAX; i++)
        if (dbg_l_ctx_tbl[i].id == 1)
            break;

    assert(i < DBG_L_CTX_MAX);

    return i == DBG_L_CTX_MAX ? NULL : &dbg_l_ctx_tbl[i];
#else
    return &dbg_l_ctx_tbl[0];
#endif
}

static  u_int32_t
dbg_l_IncIndent(void)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();

    dbg_ctx->indent += DBG_L_INDENT_STEP;

    return dbg_ctx->indent;
}

static  u_int32_t
dbg_l_DecIndent(void)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();

    assert(dbg_ctx->indent != 0);
    dbg_ctx->indent -= DBG_L_INDENT_STEP;

    return dbg_ctx->indent;
}

static  void
dbg_l_Clear(dbg_ctx_t *dbg_ctx)
{
    int cnt;

    dbg_ctx->id      = 0;
    dbg_ctx->indent  = 0;
    dbg_ctx->stk_cnt = 0;
    for (cnt = 0; cnt < DBG_L_ERR_STK_MAX; cnt++) {
        err_stk_t *err_stk = &dbg_ctx->err_stk[cnt];

        err_stk->err = RET_OK;
        memset(err_stk->ename, 0, DBG_L_ENAME_MAX);
        memset(err_stk->fname, 0, DBG_L_FNAME_MAX);
        memset(err_stk->func,  0, DBG_L_FNAME_MAX);
        err_stk->line = 0;
    }
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
void
dbg_l_Msg(const char           *in_fmt, ...)
{



#if 0
    va_list args;
    char buf[L_BUFFERSIZE];

    va_start(args, in_fmt);
    l_Vsnprintf(buf, sizeof(buf), sizeof(buf), in_fmt, args);
    L_PRINT(buf);
    dprintf(in_fmt, args);
    va_end(args);
#endif

}
void dbg_append()
{
		dbg_ctx_t *dbg_ctx = dbg_l_GetContext();

		u_int32_t i;
		char buf[L_BUFFERSIZE] = {0};
		memset(buf, 0, L_BUFFERSIZE);

		//printf("dbg_append:");
    for (i = 0; i < dbg_ctx->indent && i<L_BUFFERSIZE-1; i++)
			buf[i] = ' ';
		dprintf("%s", buf);
		


}

/* called by enter and lexit*/
void
dbg_l_MsgWithIndent(const char *in_fmt, ...)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    va_list args;
    u_int32_t i;
    char buf[L_BUFFERSIZE];
	
	

    va_start(args, in_fmt);
    for (i = 0; i < dbg_ctx->indent && i<L_BUFFERSIZE-1; i++)
        buf[i] = ' ';
	dprintf("%s", buf);
	dprintf(in_fmt, args);
    //l_Vsnprintf(buf + i, sizeof(buf) - i, sizeof(buf) - i, in_fmt, args);
    L_PRINT(buf);
    va_end(args);
}


void
dbg_l_MsgEnter(const char  *in_fname)
{
    u_int32_t i;
	char buf[L_BUFFERSIZE] = {0};
	
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    dbg_ctx->indent += DBG_L_INDENT_STEP;

	
    for (i = 0; i < dbg_ctx->indent && i<L_BUFFERSIZE-1; i++)
        buf[i] = ' ';


    if (in_fname) {
		dbg_append();
        dprintf("===== ENTER: %s =====\n", in_fname);

    }

	
}


void
dbg_l_MsgExit(const char *in_fname, u_int32_t in_ret)
{


    u_int32_t i;
	char buf[L_BUFFERSIZE];

    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
	
    

    for (i = 0; i < dbg_ctx->indent && i<L_BUFFERSIZE; i++)
        buf[i] = ' ';


    if (in_fname) {
		
		dbg_append();
        dprintf("===== EXIT : %s ===== [ret=0x%x]\n", in_fname, in_ret);

    }


	dbg_ctx->indent -= DBG_L_INDENT_STEP;

}

#if 0
void
dbg_l_Dump(po_size_t            in_len,
           const u_int8_t      *in_buf)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    u_int32_t i, j, x;
    u_int8_t c;
    char buf[L_BUFFERSIZE];

    for (i = 0; i < in_len; i += 16) {
        for (x = 0; x < dbg_ctx->indent; x ++) buf[x] = ' ';
        for (j = 0; j < 16 && i + j < in_len; j++) {
            l_Snprintf(buf + x, sizeof(buf) - x, sizeof(buf) - x, ":%02X", in_buf[i+j]);
            x += 3;
        }
        while (j++ < 16) {
            buf[x ++] = ':';
            buf[x ++] = '-';
            buf[x ++] = '-';
        }
        buf[x ++] = ' ';
        for (j = 0; j < 16 && i + j < in_len; j++) {
            c = in_buf[i+j];
            buf[x ++] = (c >= 0x20 && c < 0x7f) ? c : '.';
        }
        buf[x ++] = '\n';
        buf[x] = 0;
        L_PRINT(buf);
    }
}
#endif

void dbg_l_Dump( po_size_t len, const unsigned char *data)
{
	int pos = 0;
	unsigned char buf[16] = {0};
	int i = 0, j = 0;
	if (!data || !len)
	{
		return;
	}
	//printf("Dumping %s:\n", name);
	

	//if (hdcp2x_log_cli)
	{
		//printf("------------------------------");
		while(pos < len /*len*/) // only print message ID
		{
			
			if (pos%16 == 0)
			{
				i = 0;
				dprintf(" ");
				for(j=0; j<16; j++)
				{
					dprintf("%c", buf[j]);
				}
				dprintf("\n");
				dbg_append();
				
			}
			dprintf(":%02X", data[pos]); // secure data, cannot show in mtktool
			buf[i] = (data[pos] >= 0x20 && data[pos] < 0x7f) ? data[pos] : '.';
			i++;
			pos ++;
		}
		dprintf("\n");
		//printf("\n------------------------------\n");
	}
	
}



void
dbg_l_ByteDump(po_size_t        in_len,
               const u_int8_t  *in_buf)
{
	int i = 0;
	
	dbg_append();
	for(i=0; i<in_len; i++)
	{
		dprintf(" %02X ", in_buf[i]);
	}
	dprintf("\n");

	#if 0
    u_int32_t i, x;
    char buf[L_BUFFERSIZE];

    x = 0;
    for (i = 0; i < in_len; i ++) {
        l_Snprintf(buf + x, sizeof(buf) - x, sizeof(buf) - x, "%02X", in_buf[i]);
        x += 2;

        if (i % 4 == 3) buf[x ++] = ' ';
    }
    buf[x] = 0;
    L_PRINT(buf);

	#endif
	//dprintf("to-do:dbg_l_ByteDump\n");
}

void
dbg_l_Print(po_size_t           in_len,
            const u_int8_t     *in_buf)
{
    static char l_hextab[20] = "0123456789abcdef";
    u_int32_t i, x;
    u_int8_t c;
    char buf[L_BUFFERSIZE];

	
	

    x = 0;
    for (i = 0; i < in_len; i++) {
        c = in_buf[i];
        if (c == '\'' || c == '\\') {
            buf[x ++] = '\\';
            buf[x ++] = c;
        } else if (c == '\t') {
            buf[x ++] = '\\';
            buf[x ++] = 't';
        } else if (c == '\n') {
            buf[x ++] = '\\';
            buf[x ++] = 'n';
        } else if (c == '\r') {
            buf[x ++] = '\\';
            buf[x ++] = 'r';
        } else if (c < ' ' || c >= 0x7f) {
            buf[x ++] = '\\';
            buf[x ++] = 'x';
            buf[x ++] = l_hextab[c>>4];
            buf[x ++] = l_hextab[c&15];
        } else {
            buf[x ++] = c;
        }
        if (x > L_BUFFERSIZE - 16) {
            buf[x] = 0;
            L_PRINT(buf);
            x = 0;
        }
    }
    buf[x ++] = '\n';
    buf[x] = 0;
    L_PRINT(buf);
}

u_int32_t
dbg_l_WriteLog(u_int32_t        in_err,
               const char      *in_ename,
               const char      *in_fname,
               const char      *in_func,
               int              in_line)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    err_stk_t *err_stk;

	

	dprintf("\t !!! err:%x 	ename:%s fname:%s func:%s line:%d\n", in_err, in_ename, in_fname, in_func, in_line);


    exh_InitOk();

    if (dbg_ctx->stk_cnt < DBG_L_ERR_STK_MAX) {
        err_stk = &dbg_ctx->err_stk[dbg_ctx->stk_cnt++];
        err_stk->err = in_err;
        exh_Check(base_Strncpy(err_stk->ename, DBG_L_ENAME_MAX , in_ename, DBG_L_ENAME_MAX - 1));
        exh_Check(base_Strncpy(err_stk->fname, DBG_L_FNAME_MAX , in_fname, DBG_L_FNAME_MAX - 1));
        exh_Check(base_Strncpy(err_stk->func, DBG_L_FNAME_MAX ,  in_func, DBG_L_FNAME_MAX - 1));
        err_stk->line = in_line;
    }

    exh_Set(in_err);

exh_CLEANUP:
    exh_Return();
}

u_int32_t
dbg_l_GetDebugLevel(const char *in_cname)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();


#if defined(_WIN32)
    char level_str[32];
    u_int32_t size;
#else
    char      *level_str = NULL;
#endif
    int        level;

    assert(in_cname);

    if (dbg_l_initialized) {
			dbg_ctx->level = BUILD_LEVEL;
            level = dbg_ctx->level;
        
    } else
        level = -1;

    return level;
}

int
dbg_l_ClearErrStack(void)
{
    int      ret     = RET_OK;
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    dbg_ctx->stk_cnt = 0;
    return ret;
}

int
dbg_l_SaveErrStack(void)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    return dbg_ctx->stk_cnt;
}

int
dbg_l_RestoreErrStack(int       in_depth)
{
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    dbg_ctx->stk_cnt = in_depth;
    return RET_OK;
}

int
dbg_Init(void)
{
    int ret = RET_OK;

    char      *level_str = NULL;
    int   i;

    if (dbg_l_initialized++ == 0)
        for (i = 0; i < DBG_L_CTX_MAX; i++)
            dbg_l_Clear(&dbg_l_ctx_tbl[i]);


    dbg_l_ctx_tbl[0].id = 1;

    dbg_l_ctx_tbl[0].level = BUILD_LEVEL;
   


    return ret;
}

int
dbg_PrintErrStack(void)
{
    int      ret     = RET_OK;
    dbg_ctx_t *dbg_ctx = dbg_l_GetContext();
    err_stk_t *err_stk;
    int count;

	

    count = dbg_ctx->stk_cnt;
    if (count > 0) {
        ret = dbg_ctx->err_stk[count-1].err;

        dprintf("***** Error Stack Information ********************\n");
        dprintf("\n");

        while (--count >= 0) {
            err_stk = &dbg_ctx->err_stk[count];
            dprintf("  [%2d]: %-*s (0x%08x)\n",
                      count, DBG_L_ENAME_MAX, err_stk->ename, err_stk->err);
            dprintf("          %-25s [%-10s:%4d]\n",
                      err_stk->func, err_stk->fname, err_stk->line);
        }

        dprintf("\n");
        dprintf("**************************************************\n");
        dbg_ctx->stk_cnt = 0;
    }
    return ret;
}

int
dbg_Fin(void)
{
    int ret = RET_OK;
    int   i;

    if (--dbg_l_initialized == 0)
        for (i = 0; i < DBG_L_CTX_MAX; i++)
            dbg_l_Clear(&dbg_l_ctx_tbl[i]);
#if 1
    dbg_l_ctx_tbl[0].id = 1;
#endif

    return ret;
}




void DumpHex(char *name, const unsigned char *data, int len)
{


	int pos = 0;
	
	if (!data || !len)
	{
		return;
	}
	dprintf("tt_Dumping %s:\n", name);

	if (1 == iptv_tee_log_enable)
	{
		dprintf("------------------------------");
		while(pos < len /*len*/) // only print message ID
		{
			if (pos%16 == 0)
				dprintf("\n[%04x]|", pos);
			dprintf("0x%02x, ", data[pos]); // secure data, cannot show in mtktool
			pos ++;
		}
		dprintf("\n------------------------------\n");
	}

}


void DumpChar(char *name, const unsigned char *data,  int len)
{
	int pos = 0;
	
	if (!data || !len)
	{
		return;
	}
	dprintf("tt_Dumping %s:\n", name);

	//if (hdcp2x_log_cli)
	{
		dprintf("------------------------------\n");
		while(pos < len /*len*/) // only print message ID
		{
			//if (pos%16 == 0)
			//	printf("\n[%c]|", pos);
			dprintf("%c", data[pos]); // secure data, cannot show in mtktool
			pos ++;
		}
		dprintf("\n------------------------------\n");
	}
}


