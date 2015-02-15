
#include "log.h"
#include "type_c.h"
#include "mtkcrypto_sessionkeytbl.h"
#include "mtkcrypto_key.h"


void SKTInit(SessionKeyTable *this)
{
    MapInit(&this->keys,MAP_KEY);
}

void SKTTerm(SessionKeyTable *this)
{
    MapTerm(&this->keys);
}

bool SKTInsert(
        SessionKeyTable *this,
        const KeyId *id,
        const Key *data)
{
    KeyPair *kp;

    if (MapFind(&this->keys,id)!=NULL) 
        return false;

    kp = KeyPairCreate(id ,data);

    if(kp==NULL)
        return false;
    
    MapInsert(&this->keys ,kp);

    KeyPairDestroy(kp);

    return true;
}

Key* SKTFind(SessionKeyTable *this,
        const KeyId *id)
{
    Key* ret=MapFind(&this->keys,id);
    if(ret == NULL)
    {
        LOGD("SKTFind: Key ID NULL");
    }else {
        LOGD("SKTFind: Key ID %d:%d:%d",*id->buf, ret->control.control_bits,ret->control.duration);
    }
    return ret;
}

void SKTRemove(
        SessionKeyTable *this,
        const KeyId *key_id)
{
    MapErase(&this->keys,key_id);
}

bool SKTUpdateDuration(
        SessionKeyTable *this,
        const KeyControlBlock *control)
{
    Map *iter = this->keys.next;

    while(iter != &this->keys)
    {
        if(!KeyUpdateDuration(((KeyPair *)iter->entry)->data,control))
        {
            return false;
        }
        iter = iter->next;
    }

    return true;
}
