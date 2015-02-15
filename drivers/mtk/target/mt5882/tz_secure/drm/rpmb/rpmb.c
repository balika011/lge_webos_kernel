#include "rpmb.h"
#include "tzst.h"
#include "x_typedef.h"
#include "tomcrypt.h"
#include "tee_securestorage.h"

static inline void TZ_CACHE_STR(const void* addr , size_t size) 
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferStart((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

static inline void TZ_CACHE_END(const void* addr , size_t size)
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferEnd((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

#define KEY_SIZE (256/8)
#define SIGNED_DATA_SIZE 284

#define true  (1)
#define false (0)

typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;
typedef unsigned char bool;

static unsigned char    savedNonce[RPMB_NONCE_SIZE];
static unsigned short   savedWriteCnt;

static unsigned char    macKey[KEY_SIZE]={
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

typedef struct RpmbLoadKeyArg{
    uint32_t *result;
    uint8_t *key;
    uint32_t keySize;    
}RpmbLoadKeyArg;

typedef struct RpmGetWriteCntReqArg{
    uint32_t *result;
    RPMB_DATAFRAM *dFrm;
}RpmGetWriteCntReqArg;

typedef struct RpmSetWriteCntRespArg{
    uint32_t *result;
    RPMB_DATAFRAM *dFrm;
}RpmSetWriteCntRespArg;

static inline void generateNonce(){
    rng_get_bytes(savedNonce, sizeof(savedNonce), NULL);
}

static inline unsigned int verifyNonoce(const unsigned char *nonce){
    return memcmp(&savedNonce[0] , nonce , sizeof(savedNonce))==0;
}

void ReverseEndian(void *data, size_t size){
    unsigned int i;
    unsigned char tmp;
    unsigned char *swp=(unsigned char *)data;
    for(i = 0 ; i< (size/2); ++i){
        tmp = swp[i];
        swp[i]=swp[size -1 -i];
        swp[size -1 -i] = tmp;
    }
}

static inline int sign(const unsigned char *data , unsigned char *signature)
{
    unsigned char tmp[SIGNED_DATA_SIZE];
    int sha256idx = register_hash(&sha256_desc);

    memcpy_s2s_chk(tmp, data, SIGNED_DATA_SIZE,NULL);
    ReverseEndian(tmp,SIGNED_DATA_SIZE);

    if(sha256idx==-1){
        return false;
    }

    if( hmac_memory(sha256idx ,macKey, KEY_SIZE, tmp, SIGNED_DATA_SIZE, signature ,NULL)==CRYPT_OK){
        ReverseEndian(signature,KEY_SIZE);
        return true;
    }else{
        return false;
    }
}

static inline int verify(void *data, void *check)
{
    unsigned char signature[KEY_SIZE];

    sign((unsigned char*)data , (unsigned char*)signature);
    return memcmp(check , signature , sizeof(signature))==0;
}

void rpmbGetCntReq(RPMB_DATAFRAM *dFrm)
{
    generateNonce();

    dFrm->type     = REQ_GET_CNT;
    dFrm->result   = RESULT_OK;
    dFrm->blkCnt   = 0x0;
    dFrm->addr     = 0x0;
    dFrm->writeCnt = 0x0;

    memcpy_s2n_chk(dFrm->nonce , savedNonce , sizeof(dFrm->nonce),NULL);
    memset(dFrm->data  , 0x0        , sizeof(dFrm->data));
    memset(dFrm->mac   , 0x0        , sizeof(dFrm->mac));
    memset(dFrm->stuff , 0x0        , sizeof(dFrm->stuff));
}

int rpmbSetCntResp(RPMB_DATAFRAM *dFrm)
{
    if(dFrm->result != RESULT_OK)
    {
        return false;
    }

    if(dFrm->type != RESP_GET_CNT)
    {
        return false;
    }

    if(verifyNonoce(dFrm->nonce) == false)
    {
        return false;
    }

    if(verify(dFrm , dFrm->mac) ==false)
    {
        return false;
    }

    savedWriteCnt = dFrm->writeCnt;
    return true;
}

RPMB_DATAFRAM rpmbGetWriteReq(const unsigned char *data , RPMB_ADDR addr, unsigned short blkCnt)
{

    RPMB_DATAFRAM dFrame;

    unsigned char signature[KEY_SIZE];
    symmetric_CBC cbc;
    uint8_t iv_buffer[ 16];
    uint8_t tmp[RPMB_DATA_SIZE];
    int idx;

    dFrame.type     = REQ_WR_DATA;
    dFrame.result   = RESULT_OK;
    dFrame.blkCnt   = blkCnt;
    dFrame.addr     = addr;
    dFrame.writeCnt = savedWriteCnt;

    memset(dFrame.nonce , 0x0 , sizeof(dFrame.nonce)); // WRITE CMD Don't Generate Nonce According to the RPMB SPEC

    //Last 16 byte of data is used for increamental counter
    memcpy_s2s_chk(tmp, data , sizeof(tmp),NULL);
    memcpy_s2s_chk(tmp + sizeof(tmp) - sizeof(savedWriteCnt), &savedWriteCnt , sizeof(savedWriteCnt),NULL);
#ifndef PSUEDO_RPMB_DEV    
    memset(iv_buffer    , 0x0 , sizeof(iv_buffer));        
    idx=register_cipher(&aes_desc);    
    cbc_start( idx, iv_buffer, macKey, 16, 0 , &cbc);
    cbc_encrypt(tmp, dFrame.data, sizeof(dFrame.data), &cbc);
    cbc_done(&cbc);
#else
    memcpy_s2s_chk(dFrame.data, tmp , sizeof(tmp),NULL);
#endif
    
    sign((const unsigned char *)&dFrame , signature);
    memcpy_s2s_chk(dFrame.mac   , signature        , sizeof(dFrame.mac),NULL);

    memset(dFrame.stuff , 0x0        , sizeof(dFrame.stuff));

    return dFrame;

}

int rpmbSetWriteResp(RPMB_DATAFRAM dFrame)
{
#ifndef PSUEDO_RPMB_DEV
    if(dFrame.result != RESULT_OK)
    {
        return false;
    }

    if(dFrame.type != RESP_WR_DATA)
    {
        return false;
    }

    if(verify(&dFrame , dFrame.mac) ==false)
    {
        return false;
    }
#endif
    savedWriteCnt = dFrame.writeCnt;
    return true;
}

RPMB_DATAFRAM rpmbGetReadeReq(RPMB_ADDR addr)
{
    RPMB_DATAFRAM dFrame;

    generateNonce();

    dFrame.type     = REQ_RD_DATA;
    dFrame.result   = RESULT_OK;
    dFrame.blkCnt   = 0x0;
    dFrame.addr     = addr;
    dFrame.writeCnt = 0x0;

    memcpy_s2s_chk(dFrame.nonce , savedNonce , sizeof(dFrame.nonce),NULL);

    memset(dFrame.data  , 0x0        , sizeof(dFrame.data));
    memset(dFrame.mac   , 0x0        , sizeof(dFrame.mac));
    memset(dFrame.stuff , 0x0        , sizeof(dFrame.stuff));

    return dFrame;
}

int rpmbSetReadResp(RPMB_DATAFRAM dFrame , RPMB_ADDR *addr , unsigned char *data)
{
    symmetric_CBC cbc;
    uint8_t iv_buffer[ 16];
    int idx;
#ifndef PSUEDO_RPMB_DEV
    if(dFrame.result != RESULT_OK)
    {
        return false;
    }

    if(dFrame.type != RESP_RD_DATA)
    {
        return false;
    }

    if(verify((const unsigned char *)&dFrame , dFrame.mac) ==false)
    {
        return false;
    }

    if(verifyNonoce(dFrame.nonce) == false)
    {
        return false;
    }
#endif

    *addr = dFrame.addr;
    
    memset(iv_buffer    , 0x0 , sizeof(iv_buffer));
    idx=register_cipher(&aes_desc);    
    cbc_start( idx, iv_buffer, macKey, 16, 0 , &cbc);
    cbc_decrypt(dFrame.data, data, sizeof(dFrame.data), &cbc);
    cbc_done(&cbc);
    
    return true;
}

void rpmbLoadMacKey(const char *key)
{
    static char init = false;
    if(init == false){
        memcpy_s2s_chk(macKey , key, KEY_SIZE,NULL);
        init = true;           
    }
}

BOOL TZ_RPMB_LOAD_KEY(void *arg, UINT32 u4Size)
{
    RpmbLoadKeyArg *rArg =arg;
    uint8_t decKey[KEY_SIZE];
    size_t keySize=KEY_SIZE;
    
    
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->key!=NULL)){
        
        TZ_CACHE_STR(rArg->result, sizeof(uint32_t));
        TZ_CACHE_STR(rArg->key   , rArg->keySize);

        _teeSecureStorageSecretDec(rArg->key, rArg->keySize, decKey, &keySize);
        rpmbLoadMacKey(decKey);
        *(rArg->result) = 0;

        TZ_CACHE_END(rArg->result, sizeof(uint32_t));
        TZ_CACHE_END(rArg->key   , rArg->keySize);
       
        return true;        
    }
    return false;
}
BOOL TZ_RPMB_GET_WCNT_REQ(void *arg, UINT32 u4Size)
{
    RpmGetWriteCntReqArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->dFrm!=NULL)){
        
        TZ_CACHE_STR(rArg->result, sizeof(uint32_t));
        TZ_CACHE_STR(rArg->dFrm   , sizeof(RPMB_DATAFRAM));

        rpmbGetCntReq(rArg->dFrm);
        *(rArg->result) = 0;

        TZ_CACHE_END(rArg->result, sizeof(uint32_t));
        TZ_CACHE_END(rArg->dFrm   , sizeof(RPMB_DATAFRAM));
       
        return true;        
    }
    return false;
}

BOOL TZ_RPMB_SET_WCNT_RESP(void *arg, UINT32 u4Size)
{
    RpmSetWriteCntRespArg *rArg =arg;
    RPMB_DATAFRAM dFrm;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->dFrm!=NULL)){
        
        TZ_CACHE_STR(rArg->result, sizeof(uint32_t));
        TZ_CACHE_STR(rArg->dFrm   , sizeof(RPMB_DATAFRAM));

        memcpy_n2s_chk(&dFrm , rArg->dFrm , sizeof(dFrm),NULL);
        rpmbSetCntResp(&dFrm);
        *(rArg->result) = 0;

        TZ_CACHE_END(rArg->result, sizeof(uint32_t));
        TZ_CACHE_END(rArg->dFrm   , sizeof(RPMB_DATAFRAM));
       
        return true;        
    }
    return false;
}
