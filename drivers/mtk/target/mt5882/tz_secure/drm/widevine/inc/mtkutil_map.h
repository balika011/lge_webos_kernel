#ifndef MTKUTIL_MAP_H
#define MTKUTIL_MAP_H

#include "mtkcrypto_key.h"
#include "mtkutil_vector.h"

typedef enum{
    MAP_KEY, //Map for Key 
    MAP_SCTX,//Map for Session Contex
    MAP_UTE  //Map for Usage Table Entry
}MapType;

typedef void * (*PairCreate)(const void *id , void *data);
typedef void   (*PairDestroy)(void *this);
typedef void * (*PairDup)(const void *this);
typedef bool   (*PairMatch)(const void *this, const void *id);
typedef void * (*PairData)(const void *this);
typedef void * (*PairKey)(const void *this);

typedef struct PairFunc{

    PairCreate create;
    PairDestroy   destroy;
    PairDup    duplicate;
    PairMatch  match;
    PairData   getdata;
    PairKey    getkey;    
}PairFunc;

typedef struct Map{
    void * entry;
    struct Map  *next;
}Map;


void MapInit(Map *this,MapType type);
void MapTerm(Map *this);
void MapClear(Map *this);

void MapInsert(Map *this, const void *pair);
bool MapErase(Map *this , const void *id);

void *MapFind(const Map *this, const void *id);
size_t MapSize(Map *this);

Map *MapIterBgn(Map *this);
Map *MapIterNext(Map *iter);
void *MapIterData(Map *this, Map *iter);
void *MapIterKeya(Map *this, Map *iter);


// Key Map
typedef Vector KeyId;
typedef struct KeyPair{
    KeyId id;
    Key *data;
}KeyPair;


KeyPair *KeyPairCreate(const KeyId *id ,const Key *data);
KeyPair *KeyPairDup(const KeyPair *this);
void KeyPairDestroy(KeyPair *this);

bool KeyPairMatch(const KeyPair *this, const KeyId *id);
Key * KeyPairGetData(const KeyPair *this);

//Session ID Map

typedef struct SCTXPair{
    SessionId id;
    struct SessionContext *sctx;
}SCTXPair;

SCTXPair *SCTXPairCreate(const SessionId *id , struct SessionContext *sctx);
SCTXPair *SCTXPairDup(const SCTXPair *this);

void SCTXPairDestroy(SCTXPair *this);

bool SCTXPairMatch(const SCTXPair *this, const SessionId *id);
struct SessionContext * SCTXPairGetData(const SCTXPair *this);


//UTE Map
typedef struct UTEPair{
    Vector pst_hash;
    struct UsageTableEntry *UTE;
}UTEPair;

struct UTEPair *UTEPairCreate(const Vector *id , struct UsageTableEntry *UTE);
struct UTEPair *UTEPairDup(const struct UTEPair *this);
void UTEPairDestroy(struct UTEPair *this);
void UTEPairDestroyKeepData(UTEPair *this);
bool UTEPairMatch(const struct UTEPair *this, const Vector *id);
struct UsageTableEntry * UTEPairGetData(const struct UTEPair *this);

#endif
