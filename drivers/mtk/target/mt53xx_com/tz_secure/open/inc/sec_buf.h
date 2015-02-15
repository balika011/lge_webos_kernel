
#include "tzst.h"
#include "x_typedef.h"

char sec_buf_verify(UINT32 u4Addr , UINT32 u4Size);

BOOL TZ_SEC_BUF_OPEN_SESSION(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_CLOSE_SESSION(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_ALLOC(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_FREE(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_FRAGMENT_ALLOC(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_FRAGMENT_FREE(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_FLUSH(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_VERIFY(void *arg, UINT32 u4Size);
BOOL TZ_SEC_BUF_CPB_MOVE_DATA(void *arg, UINT32 u4ArgSize);

