#include "x_timer.h"
#include "mtkcrypto_usage_table.h"
#include "mtkcrypto_glue.h"
#include "log.h"

#define CONTRAINT_TIMER_TIMEOUT (60)
#define ANTI_ROLLBACK_TIMER_UPPERBOUND (60)

#define MAX_INT64 (0x7fffffffffffffff)

#define MAX_USAGE_ENTRY (60)
#define USAGE_TABLE_FILE_SIZE ((128)*MAX_USAGE_ENTRY + 128)

void UTStartTimer(void);
int64_t UTCurrentTimer(void);


typedef struct FakeFile{
    //Generation Number Saved in Secure Storage
    int64_t generation;
    //sec since epoch
    int64_t epoch;
    //Usage Table Load from None Secure Storage
    size_t size;
    //Write Timeout Timer
    HAL_TIME_T lastUpdate;
    char waitEcho;
    //Initialized
    char init;
    uint32_t data[USAGE_TABLE_FILE_SIZE/4];        
}FakeFile;

static FakeFile fakeFile;
//static int64_t timer_start;
static HAL_TIME_T system_timer_init;
static bool UpdateFakeFile(UsageTable *this){    
    size_t file_size = sizeof(StoredUsageTable) + MapSize(&this->table_) * sizeof(AlignedStoredUsageEntry);

    uint32_t buffer[USAGE_TABLE_FILE_SIZE/4];
    StoredUsageTable *stored_table = (StoredUsageTable *)buffer;
    StoredUsageTable *encrypted_table =(StoredUsageTable *)fakeFile.data;

    UsageTableEntry *entry;
    Map *iter ,*iterNext;
    
    uint8_t iv_buffer[KEY_IV_SIZE];

    unsigned int sig_length = sizeof(stored_table->signature);

    file_size = (file_size + AES_BLOCK_SIZE -1) &~ (AES_BLOCK_SIZE -1);

    if(file_size >USAGE_TABLE_FILE_SIZE){
        return false;
    }
    
    this->generation_++;
    memset(buffer, 0, file_size);
    stored_table->generation = this->generation_;
    stored_table->count = 0;
    stored_table->timer = UTCurrentTimer();    

    iter = MapIterBgn(&this->table_);
    iterNext = MapIterNext(iter);

    while(iter!=iterNext){
        entry = MapIterData(iter, iterNext);
        UTESaveToBuffer(entry ,&stored_table->entries[stored_table->count].entry);
        stored_table->count++;        
        iterNext = MapIterNext(iterNext);
    }

    RAND_GEN(encrypted_table->iv,  KEY_IV_SIZE);    
    memcpy(iv_buffer, encrypted_table->iv, KEY_IV_SIZE);

    AES_CBC_ENCRYPT(this->ce_->keybox.device_key.value.buf, this->ce_->keybox.device_key.value.size,
            iv_buffer,            
            (uint8_t*)buffer + SHA256_DIGEST_LENGTH + KEY_IV_SIZE,
            (uint8_t*)fakeFile.data + SHA256_DIGEST_LENGTH + KEY_IV_SIZE,        
            file_size - SHA256_DIGEST_LENGTH - KEY_IV_SIZE);
    
    //Sign Signature
    HMAC_SHA256(this->ce_->keybox.device_key.value.buf, this->ce_->keybox.device_key.value.size,
                (uint8_t*)fakeFile.data + sizeof(encrypted_table->signature),
                file_size - sizeof(encrypted_table->signature),
                (uint8_t*)fakeFile.data, &sig_length);
    
    fakeFile.size = file_size;
    fakeFile.generation = this->generation_;

    if(fakeFile.waitEcho == false){
        HAL_GetTime(&fakeFile.lastUpdate);        
        fakeFile.waitEcho = true;
    }

    return true;

}

static bool ComputeHash(const Vector *pst, Vector *pst_hash){

    uint8_t hash[SHA256_DIGEST_LENGTH];

    HASH_SHA256(pst->buf , pst->size , hash);
    VecAssign(pst_hash, hash, SHA256_DIGEST_LENGTH);
    return true;
}


bool UTRPMBUpdateChecker(void){
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT           
	HAL_TIME_T time;
    HAL_TIME_T elapseTime;
    
    if(fakeFile.waitEcho == true){        
        HAL_GetTime(&time);    
        HAL_GetDeltaTime(&elapseTime , &fakeFile.lastUpdate , &time);

        if(elapseTime.u4Seconds > CONTRAINT_TIMER_TIMEOUT){
            //Write Request Didn't Echod 
            return false;
        }
    }
#endif
    return true;
}

void UTStartTimer(){
    static char isInited = 0;   
    if(!isInited){        
        HAL_GetTime(&system_timer_init);        
        isInited=1;
    }
}

int64_t UTCurrentTimer() {
	HAL_TIME_T time;
	HAL_TIME_T elapseTime;
   
	HAL_GetTime(&time);    
    HAL_GetDeltaTime(&elapseTime , &system_timer_init , &time);    
    return elapseTime.u4Seconds + fakeFile.epoch;
}

//UsageReportPermCheck


//Usage Table Entry
UsageTableEntry* UTECreate(const Vector *pst_hash, SessionContext *ctx){

    UsageTableEntry *ret = malloc(sizeof(UsageTableEntry));
    if(ret != NULL)
    {
        UTEInit(ret , pst_hash , ctx);
    }
    return ret;    
}

UsageTableEntry* UTECreate_FromSUE(const StoredUsageEntry *buffer){
    UsageTableEntry *ret = malloc(sizeof(UsageTableEntry));
    if(ret != NULL)
    {
        UTEInit_FromSUE(ret , buffer);
    }
    return ret;    
}

void UTEDestroy(UsageTableEntry *this){
    if(this!=NULL)
    {        
        UTETerm(this);
        free(this);
    }
}

void UTEInit(UsageTableEntry *this, const Vector *pst_hash, SessionContext *ctx){

    VecInit(&(this->pst_hash_));    
    VecInit(&(this->mac_key_server_));
    VecInit(&(this->mac_key_client_));

    VecCpy(&(this->pst_hash_) , pst_hash);
    this->time_of_license_received_ = UTCurrentTimer();
    this->time_of_first_decrypt_= 0;
    this->time_of_last_decrypt_= 0;
    this->status_= kUnused;
    this->session_= ctx;

}

void UTEInit_FromSUE(UsageTableEntry *this, const StoredUsageEntry *buffer){

    VecInit(&(this->pst_hash_));    
    VecInit(&(this->mac_key_server_));
    VecInit(&(this->mac_key_client_));

    VecAssign(&(this->pst_hash_),buffer->pst_hash, SHA256_DIGEST_LENGTH);

    this->time_of_license_received_ = buffer->time_of_license_received;
    this->time_of_first_decrypt_ = buffer->time_of_first_decrypt;
    this->time_of_last_decrypt_ = buffer->time_of_last_decrypt;
    this->status_ = buffer->status;
    this->session_= NULL;
    
    VecAssign(&(this->mac_key_server_),buffer->mac_key_server, MAC_KEY_SIZE);
    VecAssign(&(this->mac_key_client_),buffer->mac_key_client, MAC_KEY_SIZE);
}

void UTETerm(UsageTableEntry *this){
    VecTerm(&(this->pst_hash_));
    VecTerm(&(this->mac_key_server_));
    VecTerm(&(this->mac_key_client_));

    if(this->session_){
        SCTXReleaseUsageEntry(this->session_);
    }
}

void UTESaveToBuffer(UsageTableEntry *this,StoredUsageEntry *buffer){

    if (this->pst_hash_.size != sizeof(buffer->pst_hash)) {
        return;
    }

    if (this->mac_key_server_.size != MAC_KEY_SIZE) {
        return;
    }

    if (this->mac_key_server_.size != MAC_KEY_SIZE) {
        return;
    }    

    memcpy(buffer->pst_hash, this->pst_hash_.buf, sizeof(buffer->pst_hash));

    buffer->time_of_license_received = this->time_of_license_received_;
    buffer->time_of_first_decrypt = this->time_of_first_decrypt_;
    buffer->time_of_last_decrypt = this->time_of_last_decrypt_;
    buffer->status = this->status_;
    memcpy(buffer->mac_key_server, this->mac_key_server_.buf, MAC_KEY_SIZE);
    memcpy(buffer->mac_key_client, this->mac_key_client_.buf, MAC_KEY_SIZE);
}


OEMCrypto_Usage_Entry_Status UTEstatus(UsageTableEntry *this){ 
    return this->status_; 
}

void UTEDeactivate(UsageTableEntry *this){
    this->status_ = kInactive;
    if (this->session_){
        SCTXReleaseUsageEntry(this->session_);
        this->session_ = NULL;
    }
}

bool UTEUpdateTime(UsageTableEntry *this){
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT       
    int64_t now = UTCurrentTimer();
    switch (this->status_) {
        case kUnused:
            this->status_ = kActive;
            this->time_of_first_decrypt_ = now;
            this->time_of_last_decrypt_ = now;
            return true;
        case kActive:
            this->time_of_last_decrypt_ = now;
            return true;
        case kInactive:
            return false;
    }
#endif    
    return false;    
}

OEMCryptoResult UTEReportUsage(UsageTableEntry *this,
        SessionContext *session,
        const Vector *pst,
        OEMCrypto_PST_Report *buffer,
        size_t *buffer_length){

    int64_t now = UTCurrentTimer();
    unsigned int md_len = sizeof(buffer->signature);  
    size_t length_needed = sizeof(OEMCrypto_PST_Report) + pst->size;

    if (*buffer_length < length_needed) {        
        *buffer_length = length_needed;
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }
    
    *buffer_length = length_needed;
    if (!buffer) {
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    memset(buffer ,0, length_needed);
    
    buffer->seconds_since_license_received =htonll64(now - this->time_of_license_received_);
    buffer->seconds_since_first_decrypt = htonll64(now - this->time_of_first_decrypt_);
    buffer->seconds_since_last_decrypt = htonll64(now - this->time_of_last_decrypt_);
    buffer->status = this->status_;
    buffer->clock_security_level = kInsecureClock;
    buffer->pst_length = pst->size;

    memcpy(buffer->pst, pst->buf, pst->size);

    if(!HMAC_SHA1(this->mac_key_client_.buf, this->mac_key_client_.size,
                (uint8_t *)buffer + SHA_DIGEST_LENGTH,  length_needed - SHA_DIGEST_LENGTH,
                buffer->signature, &md_len)){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    SCTXset_mac_key_server(session, &this->mac_key_server_);
    SCTXset_mac_key_client(session, &this->mac_key_client_);

    return OEMCrypto_SUCCESS;
}

bool UTEVerifyOrSetMacKeys(UsageTableEntry *this, const Vector *server, const Vector *client){
    if (this->mac_key_server_.size== 0) {  // Not set yet, so set it now.
        VecCpy(&this->mac_key_server_ ,server);
        VecCpy(&this->mac_key_client_ ,client);
        return true;
    } else {
        return ((VecCmp(&this->mac_key_server_, server) ==0)&& (VecCmp(&this->mac_key_client_,client)==0));
    }
}

const Vector UTEpst_hash(UsageTableEntry *this){ 
    return this->pst_hash_; 
}

void UTEset_session(UsageTableEntry *this,SessionContext *session) { 
    this->session_ = session; 
}

//==================================================
//Usage Table
//==================================================

bool UTWaitingEcho(){
    return fakeFile.waitEcho;
}

int64_t UTLoadGenerationFromFakeFile(void){
    return fakeFile.generation;
}

void UTSetGenerationToFakeFile(int64_t generation){
    fakeFile.generation = generation;
}

bool UTSetFileEpoch(int64_t epoch){
    if(!fakeFile.init){        
        UTStartTimer();
        fakeFile.epoch = epoch;
    }
    fakeFile.init=true;
    return true;
}

size_t UTSetFakeFile(const uint8_t *src, size_t size){
    if(src!=NULL){
        if(sizeof(FakeFile)>size)
            memcpy(&fakeFile, src , sizeof(FakeFile));
        fakeFile.waitEcho = 0;
    }
    return size;
}

size_t UTGetFakeFile(uint8_t *dst){
    if(dst!=NULL){
        memcpy(dst , &fakeFile, sizeof(FakeFile));
    }
    return sizeof(FakeFile);
}

size_t UTSetFakeFileData(const uint8_t *src, size_t size){    
    if((src!=NULL)
        &&(USAGE_TABLE_FILE_SIZE>size)){
        memcpy(fakeFile.data, src , size);
        fakeFile.size = size;
    }
    return size;
}

size_t UTGetFakeFileData(uint8_t *dst){
    if(dst!=NULL){
        memcpy(dst , fakeFile.data, fakeFile.size);
    }
    return fakeFile.size;
}

void UTFileIsWritten(){
    fakeFile.waitEcho = false;
}

UsageTable* UTCreate(CryptoEngine *ce){
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    UsageTable *ret = malloc(sizeof(UsageTable));
    if(ret != NULL)
    {
        UTInit(ret , ce);
    }
    return ret;
#else
    return NULL;
#endif
}

void UTDestroy(UsageTable* this){
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT        
    UTTerm(this);
    free(this);
#endif    
}

void UTInit(UsageTable* this, CryptoEngine *ce){

    uint32_t encrypted_buffer[USAGE_TABLE_FILE_SIZE/4];
    uint32_t buffer[USAGE_TABLE_FILE_SIZE/4];

    StoredUsageTable *stored_table;
    StoredUsageTable *encrypted_table;

    unsigned int sig_length = sizeof(stored_table->signature);
    uint8_t computed_signature[SHA256_DIGEST_LENGTH];
    uint8_t iv_buffer[KEY_IV_SIZE];

    size_t file_size;

    UsageTableEntry *entry;
    UTEPair *tblEntry;
    
    this->ce_ = ce;
    this->generation_ = 0;
    
    MapInit(&this->table_ , MAP_UTE);

    stored_table = (StoredUsageTable *)buffer;
    encrypted_table = (StoredUsageTable *)encrypted_buffer;

    file_size = UTGetFakeFileData((uint8_t*)encrypted_buffer);
    
    //File Not Exist , Reinit 
    if(file_size ==0){
        UpdateFakeFile(this);
        file_size = UTGetFakeFileData((uint8_t*)encrypted_buffer);
    }
    
    //Check Signature
    if(!HMAC_SHA256(this->ce_->keybox.device_key.value.buf, this->ce_->keybox.device_key.value.size,
                (uint8_t*)encrypted_buffer + SHA256_DIGEST_LENGTH,  file_size - SHA256_DIGEST_LENGTH,
                computed_signature, &sig_length)){
        return;
    }

    if (memcmp(encrypted_table->signature, computed_signature, sig_length)) {
        return;
    }

    //Decrypt the table.
    memset(buffer, 0, USAGE_TABLE_FILE_SIZE);
    memcpy(iv_buffer, encrypted_table->iv, KEY_IV_SIZE);

    AES_CBC_DECRYPT(this->ce_->keybox.device_key.value.buf, this->ce_->keybox.device_key.value.size,
            iv_buffer,
            (uint8_t*)encrypted_buffer + SHA256_DIGEST_LENGTH + KEY_IV_SIZE,        
            (uint8_t*)buffer + SHA256_DIGEST_LENGTH + KEY_IV_SIZE,
            file_size - SHA256_DIGEST_LENGTH - KEY_IV_SIZE);

    //Check Generation
    this->generation_ =  UTLoadGenerationFromFakeFile();
    
    if (stored_table->generation == this->generation_ + 1) {
        LOGW("UsageTable: File is one generation old.  Acceptable rollback.");
    } else if (stored_table->generation == this->generation_ - 1) {
        LOGW("UsageTable: File is one generation new.  Acceptable rollback.");
        // This might happen if the generation number was rolled back?
    } else if (stored_table->generation != this->generation_) {
        LOGE("UsageTable: Rollback detected. Clearing Usage Table. %lx -> %lx", this->generation_, stored_table->generation);
        this->generation_ = 0;
        UpdateFakeFile(this);        
        return;
    }

    if(stored_table->timer >  UTCurrentTimer() + ANTI_ROLLBACK_TIMER_UPPERBOUND){
        //Timer Being Rollbacked , Ignore all the previous saved usage Table
        this->generation_ = 0;
        UpdateFakeFile(this);                
        return;
    }

    // At this point, the stored table looks valid. We can load in all the
    // entries.
    for (size_t i = 0; i < stored_table->count; i++) {
        entry = UTECreate_FromSUE(&stored_table->entries[i].entry);
        tblEntry = UTEPairCreate(&entry->pst_hash_ , entry);
        MapInsert(&this->table_ , tblEntry);
        UTEPairDestroyKeepData(tblEntry);        
    }

    LOGI("UsageTable: loaded %d entryies.", stored_table->count);
}

void UTTerm(UsageTable* this){
    MapTerm(&this->table_);
}

static bool UTisFulled(UsageTable *this){
    return  MapSize(&this->table_) >= MAX_USAGE_ENTRY; 
}

static bool UTFlushLeastRecentEntry(UsageTable *this){
    Map *iter ,*iterNext;
    UsageTableEntry *tblEntry;
    int64_t time=MAX_INT64;
    Vector pst_hash;
    bool ret = false;

    VecInit(&pst_hash);
    
    iter = MapIterBgn(&this->table_);
    iterNext = MapIterNext(iter);
    
    while(iter!=iterNext){
        tblEntry = MapIterData(iter, iterNext);
        if((time  > tblEntry->time_of_license_received_)&&
           (tblEntry->session_ == NULL)){
           
            time = tblEntry->time_of_license_received_;
            VecCpy(&pst_hash, MapIterKeya(iter, iterNext));
            ret = true;
        }
        iterNext = MapIterNext(iterNext);
    }

    if(ret){
        UTDeleteEntry(this , &pst_hash , false);
    }
    VecTerm(&pst_hash);
    return ret;
}


UsageTableEntry *UTFindEntry(UsageTable* this, const Vector *pst){

    Vector pst_hash;
    UsageTableEntry *ret;

    VecInit(&pst_hash);
    ComputeHash(pst, &pst_hash);
    ret = MapFind(&this->table_, &pst_hash);
    VecTerm(&pst_hash);
    return ret;
}

UsageTableEntry *UTCreateEntry(UsageTable* this , const Vector *pst, SessionContext *ctx){

    Vector pst_hash;
    UsageTableEntry *entry=NULL;
    UTEPair *tblEntry =NULL;

    VecInit(&pst_hash); 
    ComputeHash(pst, &pst_hash);

    if(UTisFulled(this)){
        if(!UTFlushLeastRecentEntry(this)){
            goto final;
        }
    }
    
    entry = UTECreate(&pst_hash, ctx);
    tblEntry = UTEPairCreate(&pst_hash , entry);
    MapInsert(&this->table_, tblEntry);
    UTEPairDestroyKeepData(tblEntry);

final:
    
    VecTerm(&pst_hash);
    return entry;
}

OEMCryptoResult UTUpdateTable(UsageTable* this){
    if (UpdateFakeFile(this)) return OEMCrypto_SUCCESS;
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
}

OEMCryptoResult UTDeactivateEntry(UsageTable* this, Vector *pst){
    UsageTableEntry *entry = UTFindEntry(this, pst);
    if (!entry) return OEMCrypto_ERROR_INVALID_CONTEXT;
    UTEDeactivate(entry);
    if (UpdateFakeFile(this)) return OEMCrypto_SUCCESS;
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
}

bool UTDeleteEntry(UsageTable* this, Vector *pst , bool saved){
    Vector pst_hash;

    VecInit(&pst_hash); 

    ComputeHash(pst, &pst_hash);
    MapErase(&this->table_, &pst_hash);

    VecTerm(&pst_hash);
    if(saved){
        return UpdateFakeFile(this);
    }else{
        return true;
    }
}

bool UTClear(UsageTable* this){
    MapClear(&this->table_);
    return UpdateFakeFile(this);
}

