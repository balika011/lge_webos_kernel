#ifndef MTKCRYPTO_NONCETBL_MOCK_H_
#define MTKCRYPTO_NONCETBL_MOCK_H_

#define KEY_TABLE_SIZE 16

typedef enum {

    kNTStateInvalid,
    kNTStateValid,
    kNTStateFlushPending

}NonceTableState ;

typedef struct NonceTable {

  NonceTableState state[KEY_TABLE_SIZE];
  uint32_t age[KEY_TABLE_SIZE];
  uint32_t nonces[KEY_TABLE_SIZE];
}NonceTable;

void NTInit(NonceTable *this);
void NTTerm(NonceTable *this);
void NTFlush(NonceTable *this);
bool NTCheckNonce(NonceTable *this,uint32_t nonce);
void NTAddNonce(NonceTable *this, uint32_t nonce);

#endif
