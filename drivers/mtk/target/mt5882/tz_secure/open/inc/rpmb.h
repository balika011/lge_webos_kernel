#ifndef __RPMB_H__
#define __RPMB_H__

#include "x_typedef.h"

#define RPMB_NONCE_SIZE 16
#define RPMB_DATA_SIZE 256

typedef struct RPMB_DATAFRAM{
    unsigned short type;
    unsigned short result;
    unsigned short blkCnt;
    unsigned short addr;
    unsigned int  writeCnt;
    unsigned char nonce[RPMB_NONCE_SIZE];
    unsigned char data[RPMB_DATA_SIZE];
    unsigned char mac[32];
    unsigned char stuff[196];
}RPMB_DATAFRAM;

typedef  enum{
    REQ_SET_KEY = 0x1,
    REQ_GET_CNT = 0x2,
    REQ_WR_DATA = 0x3,
    REQ_RD_DATA = 0x4,
}RPMB_REQ;

typedef  enum{
    RESP_SET_KEY = 0x100,
    RESP_GET_CNT = 0x200,
    RESP_WR_DATA = 0x300,
    RESP_RD_DATA = 0x400,
}RPMB_RESP;


typedef  enum{
    RESULT_OK     = 0x0,
    RESULT_NG     = 0x1,
    RESULT_ATH_NG = 0x2,
    RESULT_CNT_NG = 0x3,
    RESULT_ADR_NG = 0x4,
    RESULT_WR_NG  = 0x5,
    RESULT_RD_NG  = 0x6,
    RESULT_NO_KEY = 0x7
}RPMB_RESULT;


typedef enum{
    RPMB_WV_GENERATION   = 0x0,
}RPMB_ADDR;


RPMB_DATAFRAM rpmbGetWriteReq(const unsigned char *data , RPMB_ADDR addr, unsigned short blkCnt);
int rpmbSetWriteResp(RPMB_DATAFRAM dFrame);
RPMB_DATAFRAM rpmbGetReadeReq(RPMB_ADDR addr);
int rpmbSetReadResp(RPMB_DATAFRAM dFrame , RPMB_ADDR *addr , unsigned char *data);

BOOL TZ_RPMB_GET_WCNT_REQ(void *arg, UINT32 u4Size);
BOOL TZ_RPMB_GET_WCNT_REQ(void *arg, UINT32 u4Size);
BOOL TZ_RPMB_LOAD_KEY(void *arg, UINT32 u4Size);

#endif
