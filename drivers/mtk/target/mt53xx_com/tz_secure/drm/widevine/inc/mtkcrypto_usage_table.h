#ifndef MTKCRYPTO_USAGE_TABLE_H_
#define MTKCRYPTO_USAGE_TABLE_H_


#include "mtkutil_map.h"
#include "mtkutil_vector.h"

#include "type_c.h"

#include "OEMCryptoCENC.h"
#include "mtkcrypto_sessionctx.h"
#include "mtkcrypto_engine.h"

typedef struct StoredUsageEntry {
  // To save disk space, we only store a hash of the pst.
  uint8_t pst_hash[SHA256_DIGEST_LENGTH];
  int64_t time_of_license_received;
  int64_t time_of_first_decrypt;
  int64_t time_of_last_decrypt;
  enum OEMCrypto_Usage_Entry_Status status;
  uint8_t mac_key_server[MAC_KEY_SIZE];
  uint8_t mac_key_client[MAC_KEY_SIZE];
}StoredUsageEntry;

typedef union {
  struct StoredUsageEntry entry;
  uint8_t padding[128];  // multiple of block size and bigger than entry size.
} AlignedStoredUsageEntry;

typedef struct StoredUsageTable {
  uint8_t signature[SHA256_DIGEST_LENGTH];
  uint8_t iv[KEY_IV_SIZE];
  int64_t generation;  
  int64_t timer;  
  size_t count;
  AlignedStoredUsageEntry entries[];
}StoredUsageTable;

//Usage Table Enrty


typedef struct UsageTableEntry {
    Vector pst_hash_;
    int64_t time_of_license_received_;
    int64_t time_of_first_decrypt_;
    int64_t time_of_last_decrypt_;

    enum OEMCrypto_Usage_Entry_Status status_;

    Vector mac_key_server_;
    Vector mac_key_client_;

    SessionContext *session_;
}UsageTableEntry;

UsageTableEntry* UTECreate(const Vector *pst_hash, SessionContext *ctx);
UsageTableEntry* UTECreate_FromSUE(const StoredUsageEntry *buffer);
void UTEDestroy(UsageTableEntry *this);

void UTEInit(UsageTableEntry *this, const Vector *pst_hash, SessionContext *ctx);
void UTEInit_FromSUE(UsageTableEntry *this, const StoredUsageEntry *buffer);
void UTETerm(UsageTableEntry *this);

void UTESaveToBuffer(UsageTableEntry *this,StoredUsageEntry *buffer);
OEMCrypto_Usage_Entry_Status UTEstatus(UsageTableEntry *this);

void UTEDeactivate(UsageTableEntry *this);
bool UTEUpdateTime(UsageTableEntry *this);

OEMCryptoResult UTEReportUsage(UsageTableEntry *this,
    SessionContext *session,
    const Vector *pst,
    OEMCrypto_PST_Report *buffer,
    size_t *buffer_length);

bool UTEVerifyOrSetMacKeys(UsageTableEntry *this, const Vector *server, const Vector *client);

const Vector UTEpst_hash(UsageTableEntry *this);
void UTEset_session(UsageTableEntry *this,SessionContext *session);

//Usage Table

typedef struct UsageTable{

  Map table_;
  int64_t generation_;
  CryptoEngine *ce_;

  //wvcdm::Lock lock_;
  
}UsageTable;

UsageTable* UTCreate(CryptoEngine *ce);
void UTDestroy(UsageTable* this);

void UTInit(UsageTable* this, CryptoEngine *ce);
void UTTerm(UsageTable* this);

UsageTableEntry *UTFindEntry(UsageTable* this, const Vector *pst);
UsageTableEntry *UTCreateEntry(UsageTable* this , const Vector *pst, SessionContext *ctx);

OEMCryptoResult UTUpdateTable(UsageTable* this);
OEMCryptoResult UTDeactivateEntry(UsageTable* this, Vector *pst);

bool UTDeleteEntry(UsageTable* this, Vector *pst , bool saved);
bool UTClear(UsageTable* this);

bool UTRPMBUpdateChecker(void);
void UTFileIsWritten(void);

bool UTWaitingEcho(void);
bool UTSetFileEpoch(int64_t epoch);
size_t UTGetFakeFile(uint8_t *dst);
size_t UTSetFakeFile(const uint8_t *src, size_t size);
int64_t UTLoadGenerationFromFakeFile(void);
void UTSetGenerationToFakeFile(int64_t generation);
size_t UTGetFakeFileData(uint8_t *dst);

#endif
