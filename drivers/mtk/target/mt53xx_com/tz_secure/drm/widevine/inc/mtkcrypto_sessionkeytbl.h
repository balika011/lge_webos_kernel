#ifndef MTKCRYPTO_SESSION_KEY_TABLE_H_
#define MTKCRYPTO_SESSION_KEY_TABLE_H_

#include "mtkutil_vector.h"
#include "mtkutil_map.h"

// SessionKeyTable holds the keys for the current session
typedef struct SessionKeyTable {

  Map keys;

}SessionKeyTable;

void SKTInit(SessionKeyTable *this);

void SKTTerm(SessionKeyTable *this);

bool SKTInsert(
        SessionKeyTable *this,
        const KeyId *id,
        const Key *data);

Key* SKTFind(SessionKeyTable *this,
        const KeyId *id);

void SKTRemove(
        SessionKeyTable *this,
        const KeyId *key_id);

bool SKTUpdateDuration(
        SessionKeyTable *this,
        const KeyControlBlock *control);

#endif
