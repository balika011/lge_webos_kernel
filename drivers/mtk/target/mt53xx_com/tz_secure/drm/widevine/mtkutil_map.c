
#include "log.h"
#include "mtkutil_map.h"
#include "mtkcrypto_usage_table.h"


#define P_CREATE        ((PairFunc *)this->entry)->create
#define P_DESTROY       ((PairFunc *)this->entry)->destroy
#define P_DUPLICATE     ((PairFunc *)this->entry)->duplicate
#define P_MATCH         ((PairFunc *)this->entry)->match
#define P_GETDATA       ((PairFunc *)this->entry)->getdata
#define P_GETKEY        ((PairFunc *)this->entry)->getdata


//KEY PAIR
KeyPair *KeyPairCreate(const KeyId *id , const Key *data)
{
    KeyPair *ret = malloc(sizeof(KeyPair));
    if(ret == NULL)
        goto final;    
    VecInit(&ret->id);
    VecCpy(&ret->id ,id);
    ret->data = KeyDup(data);
final:
    return ret;
}

KeyPair *KeyPairDup(const KeyPair *this)
{
    KeyPair *ret = malloc(sizeof(KeyPair));
    if(ret == NULL)
        goto final;    
    VecInit(&ret->id);
    VecCpy(&ret->id , &this->id);
    ret->data = KeyDup(this->data);
final:    
    return ret;
}

void KeyPairDestroy(KeyPair *this)
{
    VecTerm(&this->id);
    KeyDestroy(this->data);
    free(this);
}

bool KeyPairMatch(const KeyPair *this, const KeyId *id)
{
    return VecCmp(&this->id,id)==0;
}

Key * KeyPairGetData(const KeyPair *this)
{
    return this->data;
}

//SESION ID PAIR

SCTXPair *SCTXPairCreate(const SessionId *id , struct SessionContext *sctx)
{
    SCTXPair *ret = malloc(sizeof(SCTXPair));
    if(ret == NULL)
        goto final;    
    ret->id = *id;
    ret->sctx = sctx;
final:    
    return ret;
}

SCTXPair *SCTXPairDup(const SCTXPair *this)
{
    SCTXPair *ret = malloc(sizeof(SCTXPair));
    if(ret ==NULL)
        goto final;    
    ret->id = this->id;
    ret->sctx = this->sctx;
final:    
    return ret;
}

void SCTXPairDestroy(SCTXPair *this)
{
    SCTXDestroy(this->sctx);
    free(this);
}

bool SCTXPairMatch(const SCTXPair *this, const SessionId *id)
{
    return this->id == *id;
}

struct SessionContext * SCTXPairGetData(const SCTXPair *this)
{
    return this->sctx;
}

// Usage Table Entry 

UTEPair *UTEPairCreate(const Vector *id , struct UsageTableEntry *UTE){
    UTEPair *ret = malloc(sizeof(UTEPair));
    if(ret ==NULL)
        goto final;    
    VecInit(&ret->pst_hash);
    VecCpy(&ret->pst_hash ,id);
    ret->UTE = UTE;
final:
    return ret;
}

UTEPair *UTEPairDup(const UTEPair *this){
    UTEPair *ret = malloc(sizeof(UTEPair));
    if(ret ==NULL)
        goto final;    
    VecInit(&ret->pst_hash);
    VecCpy(&ret->pst_hash , &this->pst_hash);
    ret->UTE = this->UTE;
final:
    return ret;
}

void UTEPairDestroy(UTEPair *this){
    VecTerm(&this->pst_hash);
    UTEDestroy(this->UTE);        
    free(this);
}

void UTEPairDestroyKeepData(UTEPair *this){
    VecTerm(&this->pst_hash);
    free(this);
}

bool UTEPairMatch(const UTEPair *this, const Vector *id){
    return VecCmp(&this->pst_hash, id)==0;
}

UsageTableEntry * UTEPairGetData(const UTEPair *this){
    return this->UTE;
};

Vector* UTEPairGetKey(const UTEPair *this){
    return &this->pst_hash;
};

//MAP
void MapInit(Map *this,MapType type)
{
    this->next=this;
    this->entry = malloc(sizeof(PairFunc));
    if(this->entry ==NULL)
        return;    
    
   ((PairFunc *)this->entry)->getkey=NULL;

    if(MAP_KEY == type)
    {
        ((PairFunc *)this->entry)->create=(PairCreate)KeyPairCreate;
        ((PairFunc *)this->entry)->destroy=(PairDestroy)KeyPairDestroy;
        ((PairFunc *)this->entry)->duplicate=(PairDup)KeyPairDup;
        ((PairFunc *)this->entry)->match=(PairMatch)KeyPairMatch;
        ((PairFunc *)this->entry)->getdata=(PairData)KeyPairGetData;
    }
    else if(MAP_SCTX == type)
    {
        ((PairFunc *)this->entry)->create=(PairCreate)SCTXPairCreate;
        ((PairFunc *)this->entry)->destroy=(PairDestroy)SCTXPairDestroy;
        ((PairFunc *)this->entry)->duplicate=(PairDup)SCTXPairDup;
        ((PairFunc *)this->entry)->match=(PairMatch)SCTXPairMatch;
        ((PairFunc *)this->entry)->getdata=(PairData)SCTXPairGetData;
    }
    else if(MAP_UTE == type)
    {
        ((PairFunc *)this->entry)->create=(PairCreate)UTEPairCreate;
        ((PairFunc *)this->entry)->destroy=(PairDestroy)UTEPairDestroy;
        ((PairFunc *)this->entry)->duplicate=(PairDup)UTEPairDup;
        ((PairFunc *)this->entry)->match=(PairMatch)UTEPairMatch;
        ((PairFunc *)this->entry)->getdata=(PairData)UTEPairGetData;
        ((PairFunc *)this->entry)->getkey=(PairData)UTEPairGetKey;        
    }    
    else
    {
        ((PairFunc *)this->entry)->create=NULL;
        ((PairFunc *)this->entry)->destroy=NULL;
        ((PairFunc *)this->entry)->duplicate=NULL;
        ((PairFunc *)this->entry)->match=NULL;
        ((PairFunc *)this->entry)->getdata=NULL;
    }
}

void MapTerm(Map *this)
{
    Map *iter = this;
    Map *next;
    
    iter = iter->next;

    while(iter != this)
    {
        next = iter->next;
        P_DESTROY(iter->entry);
        free(iter);
        iter = next;
    }
    
    free(this->entry);
    this->next = this;
}

void MapClear(Map *this)
{
    Map *iter = this;
    Map *next;
    
    iter = iter->next;

    while(iter != this)
    {
        next = iter->next;
        P_DESTROY(iter->entry);
        free(iter);
        iter = next;
    }
    
    this->next = this;
}

void *MapFind(const Map *this,const void *id)
{
    Map *iter = this->next;
    while( iter != this)
    {
        if(P_MATCH(iter->entry, id))
        {
            return  P_GETDATA(iter->entry);
        }
        iter = iter->next;
    }
    return NULL;
}

void MapInsert(Map *this, const void *pair)
{
    Map *next = this->next;

    this->next = malloc(sizeof(Map));
    if(this->next ==NULL){
        this->next = next;
        return;
    }    
    this->next->entry = P_DUPLICATE(pair);
    this->next->next = next;
}

bool MapErase(Map *this, const void *id)
{
    Map *iter = this->next;
    Map *pre = this;

    while(iter != this)
    {
        if(P_MATCH(iter->entry, id))
        {
            pre->next = iter->next;
            P_DESTROY(iter->entry);
            free(iter);
            return true;
        }

        pre = iter;
        iter = iter->next;
    }
    return false;
}

size_t MapSize(Map *this)
{
    Map *iter = this;
    size_t cnt=0;   
    
    iter = iter->next;
    while(iter != this)
    {
        cnt ++; 
        iter = iter->next;
    }
    
    return cnt;
}

Map *MapIterBgn(Map *this)
{
    return this;
}

Map *MapIterNext(Map *iter)
{
    return iter->next;
}

void *MapIterData(Map *this, Map *iter)
{
    return  P_GETDATA(iter->entry);
}

void *MapIterKeya(Map *this, Map *iter)
{
    return  P_GETKEY(iter->entry);
}

