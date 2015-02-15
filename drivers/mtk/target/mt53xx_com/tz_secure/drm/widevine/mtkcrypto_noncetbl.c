

#include "type_c.h"
#include "log.h"
#include "mtkcrypto_noncetbl.h"
#include "wv_cdm_constants.h"

void NTInit(NonceTable *this)
{
    int i;
    for (i = 0; i < KEY_TABLE_SIZE; ++i) {
        this->state[i] = kNTStateInvalid;
    }
}

void NTTerm(NonceTable *this)
{
}


void NTAddNonce(NonceTable *this, uint32_t nonce) {
    int new_slot = -1;
    int oldest_slot = -1;
    int i;

    // Flush any nonces that have been checked but not flushed.
    // After flush, nonces will be either valid or invalid.
    NTFlush(this);

    for (i = 0; i < KEY_TABLE_SIZE; ++i) {
        // Increase age of all valid nonces.
        if (kNTStateValid == this->state[i]) {
            ++this->age[i];
            if (-1 == oldest_slot) {
                oldest_slot = i;
            } else {
                if (this->age[i] > this->age[oldest_slot]) {
                    oldest_slot = i;
                }
            }
        } else {
            if (-1 == new_slot) {
                this->age[i] = 0;
                this->nonces[i] = nonce;
                this->state[i] = kNTStateValid;
                new_slot = i;
            }
        }
    }
    if (-1 == new_slot) {
        // reuse oldest
        // assert (oldest_slot != -1)
        i = oldest_slot;
        this->age[i] = 0;
        this->nonces[i] = nonce;
        this->state[i] = kNTStateValid;
    }
}

bool NTCheckNonce(NonceTable *this,uint32_t nonce) {
    int i;
    for (i = 0; i < KEY_TABLE_SIZE; ++i) {
        if (kNTStateInvalid != this->state[i]) {
            if (nonce == this->nonces[i]) {
                this->state[i] = kNTStateFlushPending;
                return true;
            }
        }
    }
    return false;
}

void NTFlush(NonceTable *this) {
    int i;
    for (i = 0; i < KEY_TABLE_SIZE; ++i) {
        if (kNTStateFlushPending == this->state[i]) {
            this->state[i] = kNTStateInvalid;
        }
    }
}

