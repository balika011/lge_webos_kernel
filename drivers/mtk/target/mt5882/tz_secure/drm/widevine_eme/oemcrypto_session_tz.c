/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include <time.h> 
#include "dmx_drm_drvif.h"
#include "oemcrypto_session_tz.h"
#include "oemcrypto_key_tz.h"
#include "rsapkcs.h"
#include "pkcs8.h"
 
static int session_num = 0;
//extern ltc_math_descriptor ltm_desc;

static UINT8 g_OEMKey[16] = { 0xC5, 0xC5, 0x93, 0xEB, 0x32, 0x89, 0x0A, 0xDA,
                               0x15, 0x3C, 0x9F, 0xA2, 0x1F, 0x0E, 0xDE, 0x01};

#if 1

// Increment counter for AES-CTR
void ctr128_inc(uint8_t* counter)
{
    uint32_t n = 16;
    do
    {
        if (++counter[--n] != 0) return;
    }
    while (n);
}

static unsigned char * HexEncode(const uint8_t* in_buffer, unsigned int size)
{
	static const char kHexChars[] = "0123456789ABCDEF";

	// Each input byte creates two output hex characters.
	static unsigned char out_buffer[1000] = {};
	memset(out_buffer,0,sizeof(out_buffer));
	if (out_buffer == NULL)
	{
		return NULL;
	}
	unsigned int i = 0;
	for (; i < size; ++i)
	{
		char byte = in_buffer[i];
		out_buffer[(i << 1)] = kHexChars[(byte >> 4) & 0xf];
		out_buffer[(i << 1) + 1] = kHexChars[byte & 0xf];
	}
	return out_buffer;
}


bool GetRandomNumTz(uint8_t * randomData, size_t dataLength)
{
 	//used in TZ, will use the km to substitute.
	#define SYSENTROPY_LEN 10
	prng_state prng;
	unsigned char sysentropy[SYSENTROPY_LEN]={};
	
	sprng_read(sysentropy, SYSENTROPY_LEN, NULL);
	if (fortuna_start(&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNumTz(): fortuna_start error \n");
	}
	if(fortuna_add_entropy(sysentropy, SYSENTROPY_LEN,&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNumTz(): for_add_entropy error \n");
	}
	if(fortuna_ready(&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNumTz(): fortuna_ready error \n");
	}
	fortuna_read(randomData,dataLength,&prng);
	fortuna_done(&prng);
	
	return true;
}


bool Insert(Keypair * keypair, int keypairlen,const KeyId key_id, const WvKey* key_data)
{
	int i=0;
    for (;i<keypairlen;i++)
    {
        if (keypair[i].is_valid == true)
        {
            memcpy(&(keypair[i].keyid),key_id,KEY_ID_LENGTH);
            memcpy(&(keypair[i].keycontent),key_data,KEY_DATA_LENGTH);
            return true;
        }

    }

    return false;
}

WvKey* Find(Keypair * keypair, int keypairlen,const KeyId key_id, int key_id_len)
{
	int i=0;
    for (;i<keypairlen;i++)
    {
        if (keypair[i].is_valid == true)
        {
            if (memcmp(&(keypair[i].keyid),key_id,key_id_len) == 0)
            {
                return &(keypair[i].keycontent);
            }
        }

    }
    return NULL;
}

void Remove(Keypair * keypair, int keypairlen,const KeyId key_id)
{	
	int i=0;
    for (;i<keypairlen;i++)
    {
        if (keypair[i].is_valid == true)
        {
            if (memcmp(&(keypair[i].keyid),key_id,sizeof(KeyId)) == 0)
            {
                keypair[i].is_valid = false;

            }
        }
    }

}

bool UpdateDuration(Keypair * keypair, int keypairlen,const KeyControlBlock* control)
{
	int i=0;
    for (;i<keypairlen;i++)
    {
        if (keypair[i].is_valid == true)
        {
            if (UpdateDurationKey(&(keypair[i].keycontent),control) != true)
            {
            	DMSG("Error: UpdateDurationKey.\n");
                return false;
            }
        }
    }

    return true;
}


// Internal utility function to derive key using CMAC-128

bool DeriveKey(SessionContext * sesscnt, const uint8_t* inkey, size_t inkeylen, const uint8_t* context,
               size_t cont_len, int counter, uint8_t* out, size_t * outlen)
{
	const uint8_t * key = NULL;
	size_t keylen = 0;
	if (inkey == NULL || inkeylen == 0)
	{
		if (g_keybox.device_key_.valid_ != true)
		{
			DMSG("[DeriveKey(): g_keybox.device_key_.valid_ != true] \n");
			return false;
		}
		key = (const uint8_t *)g_keybox.device_key_.keyvalue_;
		keylen = g_keybox.device_key_.keydatalen_;
	}
	else 
	{
		key = inkey;
		keylen = inkeylen;
	}
	if (counter > 4 || cont_len == 0 || out == NULL)
    {
        DMSG("[DeriveKey(): OEMCrypto_ERROR_INVALID_CONTEXT] \n");
        return false;
    }

	int idx = 0;
	omac_state omac;

	idx = find_cipher("aes");
	if (omac_init(&omac,idx,&key[0], keylen) != CRYPT_OK)
	{
		DMSG("[DeriveKey(): omac_init fail] \n");
		return false;	
	}
	uint8_t* message = (uint8_t *)malloc(cont_len+1+1);
    if (message == NULL)
    {
        DMSG("[DeriveKey(): OEMCrypto_ERROR_MALLOC_FAILURE] \n");
        return false;
    }
    message[0] = counter;
    memcpy_n2s_chk(message+1,context,cont_len,NULL);
	if (omac_process(&omac,message,cont_len+1) != CRYPT_OK)
	{
		free(message);
		DMSG("[DeriveKey(): omac_process fail] \n");
		return false;
	}
	free(message);
	unsigned long reslen = 128;
    uint8_t res[128];
    if (omac_done(&omac, res, &reslen) != CRYPT_OK)
    {
        DMSG("[DeriveKey(): omac_done] \n");
        return false;
    }
    memcpy_s2s_chk(out,res,reslen,NULL);
	* outlen = reslen;
	
    return true;
}


size_t RSASignatureSize(SessionContext *sesscnt)
{
	return 2048;
}


bool ParseKeyControl(SessionContext * sesscnt,const uint8_t* key_control_string,size_t key_cnt_strlen,
                     KeyControlBlock* key_control_block)
{

    key_control_block->valid_ = false;

    if (key_cnt_strlen < KEY_CONTROL_SIZE)
    {
        DMSG("ParseKeyControl: wrong size.\n");
        return false;
    }
    if (!SetFromString(key_control_block,key_control_string,key_cnt_strlen))
    {
        DMSG("KCB: BAD Size or Structure \n");
        return false;
    }

    DMSG("KCB: \n");
    DMSG("  valid:    %d \n", key_control_block->valid_);
    DMSG("  duration: %d \n", key_control_block->duration_);
    DMSG("  nonce:    %0X \n", key_control_block->nonce_);
    DMSG("  bits:     %0X \n", key_control_block->control_bits_);
    /*DMSG("  bit kControlAllowEncrypt %s. \n",
         (key_control_block->control_bits_ & kControlAllowEncrypt) ? "set" : "unset");
    DMSG("  bit kControlAllowDecrypt %s. \n",
         (key_control_block->control_bits_ & kControlAllowDecrypt) ? "set" : "unset");
    DMSG("  bit kControlAllowSign %s. \n",
         (key_control_block->control_bits_ & kControlAllowSign) ? "set" : "unset");
    DMSG("  bit kControlAllowVerify %s. \n",
         (key_control_block->control_bits_ & kControlAllowVerify) ? "set" : "unset");
    DMSG("  bit kControlObserveDataPath %s. \n",
         (key_control_block->control_bits_ & kControlObserveDataPath) ? "set" : "unset");
    DMSG("  bit kControlObserveHDCP %s. \n",
         (key_control_block->control_bits_ & kControlObserveHDCP) ? "set" : "unset");
    DMSG("  bit kControlObserveCGMS %s. \n",
         (key_control_block->control_bits_ & kControlObserveCGMS) ? "set" : "unset");
    DMSG("  bit kControlDataPathSecure %s. \n",
         (key_control_block->control_bits_ & kControlDataPathSecure) ? "set" : "unset");*/
    DMSG("  bit kControlNonceEnabled %s. \n",
         (key_control_block->control_bits_ & kControlNonceEnabled) ? "set" : "unset");
    //DMSG("  bit kControlHDCPRequired %s. \n",
    //     (key_control_block->control_bits_ & kControlHDCPRequired) ? "set" : "unset");
 
	
    uint32_t cgms_bits = key_control_block->control_bits_ & 0x3;
    const char* cgms_values[4] = {"free", "BAD", "once", "never"};
    DMSG("    CGMS = %s \n", cgms_values[cgms_bits]);

    if (!Validate(key_control_block))
    {
        DMSG("KCB: BAD Signature\n");
        return false;
    }
    if ((key_control_block->control_bits_ & kControlNonceEnabled)
        && (!CheckNonceNonTable(&(sesscnt->nonce_table_),key_control_block->nonce_)))
    {
        DMSG("KCB: BAD Nonce \n");
        return false;
    }

    return true;
}


bool SessAddConKeypair(SessionContext *sesscnt,const uint8_t * keyid, size_t keyid_len, WvKey * cntkey)
{
    int i = 0;
    while (i<MAX_SESSION_KEYPAIR && sesscnt->session_keys_[i].is_valid == true)
    {
        i++;
    }
    if (i == MAX_SESSION_KEYPAIR)
    {
        DMSG("Error sess key full \n");
        return false;
    }
	memset(sesscnt->session_keys_[i].keyid,0,sizeof(sesscnt->session_keys_[i].keyid));
    memcpy_n2s_chk(sesscnt->session_keys_[i].keyid,keyid,keyid_len,NULL);
    memcpy_s2s_chk(&(sesscnt->session_keys_[i].keycontent),cntkey,sizeof(WvKey),NULL);
    sesscnt->session_keys_[i].is_valid = true;
    return true;
}


bool  DecryptRSAKey(SessionContext *sesscnt,const uint8_t* enc_rsa_key,
                    size_t enc_rsa_key_length,
                    const uint8_t* enc_rsa_key_iv,
                    uint8_t* pkcs8_rsa_key)
{
    // Decrypt rsa key with keybox.
    uint8_t iv_buffer[KEY_IV_SIZE];
    memcpy_n2s_chk(iv_buffer, enc_rsa_key_iv, KEY_IV_SIZE,NULL);

	symmetric_CBC tmpcbc;
	int idx = find_cipher("aes");
	if (cbc_start(idx,
					iv_buffer,sesscnt->encryption_key_,16,
					0,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_start error \n");
		return false;
	}
	cbc_setiv(iv_buffer,KEY_IV_SIZE,&tmpcbc);
	if (cbc_decrypt(enc_rsa_key,pkcs8_rsa_key,enc_rsa_key_length,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_decrypt error \n");
		return false;
	}
	if (cbc_done(&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_done error \n");
		return false;
	}

    return true;
}

bool EncryptRSAKey(SessionContext *sesscnt, const uint8_t* pkcs8_rsa_key,
                   size_t enc_rsa_key_length,
                   const uint8_t* enc_rsa_key_iv,
                   uint8_t* enc_rsa_key)
{
    // Encrypt rsa key with keybox.
    uint8_t iv_buffer[KEY_IV_SIZE];
    memcpy_n2s_chk(iv_buffer, enc_rsa_key_iv,KEY_IV_SIZE,NULL);

	symmetric_CBC tmpcbc;
	int idx = find_cipher("aes");
	if (cbc_start(idx,
					iv_buffer,sesscnt->encryption_key_,16,
					0,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_start error \n");
		return false;
	}
	//cbc_setiv(iv_buffer,KEY_IV_SIZE,&tmpcbc);
	if (cbc_encrypt(pkcs8_rsa_key,enc_rsa_key,enc_rsa_key_length,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_decrypt error \n");
		return false;
	}
	if (cbc_done(&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_done error \n");
		return false;
	}

    return true;
}


// Internal utility function to decrypt the message
bool DecryptMessage(SessionContext* session,
                    const uint8_t* key, size_t key_len,
                    const uint8_t* iv, size_t iv_len,
                    const uint8_t* message, size_t mss_len,
                    uint8_t* decrypted)
{
    if (key_len==0 || iv_len==0 || mss_len==0 || !decrypted)
    {
        DMSG("[DecryptMessage(): OEMCrypto_ERROR_INVALID_CONTEXT] \n");
        return false;
    }
    //decrypted->resize(message.size());
    uint8_t iv_buffer[16];
    memcpy_n2s_chk(iv_buffer, &iv[0], 16,NULL);

	symmetric_CBC tmpcbc;
	int idx = find_cipher("aes");
	if (cbc_start(idx,
					iv_buffer,key,16,0,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptMessage(): cbc_start error \n");
		return false;
	}
	//cbc_setiv(iv_buffer,KEY_IV_SIZE,&tmpcbc);
	if (cbc_decrypt(message,decrypted,mss_len,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptMessage(): cbc_decrypt error \n");
		return false;
	}
	if (cbc_done(&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptMessage(): cbc_done error\n");
		return false;
	}

    return true;
}


bool LoadRSAKey(SessionContext *sesscnt, uint8_t* pkcs8_rsa_key,
                size_t rsa_key_length,
                const uint8_t* message,
                size_t message_length,
                const uint8_t* signature,
                size_t signature_length)
{
	DMSG(" enter \n");

    // Validate message signature
    if (!ValidateMessage(sesscnt,message, message_length, signature, signature_length))
    {
        DMSG("[LoadRSAKey(): Could not verify signature] \n");
        return false;
    }
	
	if (sesscnt->rsa_key_ != NULL)
	{
		//rsa_free(sesscnt->rsa_key_);
		crypto_rsa_free((crypto_rsa_key *)(sesscnt->rsa_key_));
       	sesscnt->rsa_key_ = NULL;
	}

	sesscnt->rsa_key_ = (rsa_key * )pkcs8_key_import(pkcs8_rsa_key,rsa_key_length);
	if (sesscnt->rsa_key_ == NULL)
	{
		DMSG("[LoadRSAKey(): load rsa key error] \n");
		return false;
	}
	sesscnt->rsa_key_->type = PK_PRIVATE;
	DMSG(" leave \n");
	return true;
}

bool GenerateNonceNonTable(NonceTable *tmpnoce_tbl,uint32_t *nonce_value)
{
    int new_slot = -1;
    int oldest_slot = -1;
    //NonceTable *tmpnoce_tbl = &(sesscnt->nonce_table_);
    // Flush any nonces that have been checked but not flushed.
    // After flush, nonces will be either valid or invalid.
    FlushNonTable(tmpnoce_tbl);

    uint32_t nonce;
    uint8_t* nonce_string = (uint8_t*)(&nonce);

    // Generate 4 bytes of random data
	if (!GetRandomNumTz(nonce_string, 4))
    {
        return false;
    }
	
    *nonce_value = nonce;
	//DMSG("GenerateNonceNonTable nonce=0x%x \n",*nonce_value);

    for (int i = 0; i < kTableSize; ++i)
    {
        // Increase age of all valid nonces.
        if (e_kNTStateValid == tmpnoce_tbl->state_[i])
        {
            ++tmpnoce_tbl->age_[i];
            if (-1 == oldest_slot)
            {
                oldest_slot = i;
            }
            else
            {
                if (tmpnoce_tbl->age_[i] > tmpnoce_tbl->age_[oldest_slot])
                {
                    oldest_slot = i;
                }
            }
        }
        else
        {
            if (-1 == new_slot)
            {
                tmpnoce_tbl->age_[i] = 0;
                tmpnoce_tbl->nonces_[i] = nonce;
                tmpnoce_tbl->state_[i] = e_kNTStateValid;
                new_slot = i;
				//DMSG("GenerateNonceNonTable new_slot tmpnoce_tbl->nonces_[%d]= 0x%x \n",i,nonce);
            }
        }
    }
    if (-1 == new_slot)
    {
        // reuse oldest
        // assert (oldest_slot != -1)
        int i = oldest_slot;
        tmpnoce_tbl->age_[i] = 0;
        tmpnoce_tbl->nonces_[i] = nonce;
        tmpnoce_tbl->state_[i] = e_kNTStateValid;
		//DMSG("GenerateNonceNonTable oldest_slot tmpnoce_tbl->nonces_[%d]= 0x%x \n",i,nonce);
    }
	
	return true;
}

bool CheckNonceNonTable(NonceTable *tmpnoce_tbl, uint32_t nonce)
{
    //NonceTable *tmpnoce_tbl = &(sesscnt->nonce_table_);
    for (int i = 0; i < kTableSize; ++i)
    {
        if (e_kNTStateInvalid != tmpnoce_tbl->state_[i])
        {
            if (nonce == tmpnoce_tbl->nonces_[i])
            {
                tmpnoce_tbl->state_[i] = e_kNTStateFlushPending;
                return true;
            }
        }
    }
	DMSG("[CheckNonceNonTable():  fail] \n");
    return false;
}

void FlushNonTable(NonceTable *tmpnoce_tbl)
{
    //NonceTable *tmpnoce_tbl = &(sesscnt->nonce_table_);
    for (int i = 0; i < kTableSize; ++i)
    {
        if (e_kNTStateFlushPending == tmpnoce_tbl->state_[i])
        {
            tmpnoce_tbl->state_[i] = e_kNTStateInvalid;
        }
    }
}
#endif

#if 1
//TZ interface for usr
SessionContext * CreatSessionTz()
{
    SessionContext * sess = (SessionContext *)malloc(sizeof(SessionContext));
    if (sess == NULL)
    {
        DMSG(" creat session error\n");
		return NULL;
    }
    else
    {
     	//DMSG(" creat session success\n");
    	memset(sess,0,sizeof(SessionContext));
        sess->valid_ = true;
		sess->has_mac_key = false;
		if (session_num == 0)
		{
			//ltc_mp = (ltc_math_descriptor)ltm_desc;
			if (register_cipher(&aes_desc) == -1)
			{
				DMSG("[DeriveKey(): register_cipher error \n");
				if (sess != NULL)
        			free(sess);
				return NULL;
			}
			if (register_hash(&sha256_desc) == -1)
			{
				DMSG("[DeriveKey(): register_hash error \n");
				if (sess != NULL)
        			free(sess);
				return NULL;
			}
			if (register_prng(&sprng_desc) == -1)
			{
				DMSG("[DeriveKey(): register_prng error \n");
				if (sess != NULL)
        			free(sess);
				return NULL;
			}
			if (register_prng(&fortuna_desc) == -1)
			{
				DMSG("[DeriveKey(): register_prng error \n");
				if (sess != NULL)
        			free(sess);
				return NULL;
			}
			if (register_hash(&sha1_desc) == -1)
			{
				DMSG("[DeriveKey(): register_prng error \n");
				if (sess != NULL)
        			free(sess);
				return NULL;
			}
		}
		session_num += 1;

		return sess;
    }
}

bool DestroySessionTz( SessionContext *sesscnt)
{
    if (sesscnt != NULL)
    {
        free(sesscnt);
		session_num -= 1;
		if (session_num <= 0)
		{
			session_num = 0;
			
		}
		//DMSG(" Destroy session success\n");
    }
    return true;
}

#define DERIVE_ENC_SER_CLINT_KEY
#ifdef DERIVE_ENC_SER_CLINT_KEY

bool DeriveKeys(SessionContext * sesscnt,const uint8_t * master_key, size_t mst_cnt_len,
                const uint8_t* mac_context, size_t mac_cnt_len,
                const uint8_t* enc_context,size_t enc_cnt_len)
{
    // Generate derived key for mac key
    uint8_t mac_key_server[MACKEY_LEN] = {0};
    size_t  mac_skey_len = 0;
    uint8_t mac_key_client[MACKEY_LEN] = {0};
    size_t  mac_ckey_len = 0;
    uint8_t mac_key_part2[MACKEY_LEN] = {0};
    size_t  mac_keyp2_len = 0;
    if (!DeriveKey(sesscnt, master_key,mst_cnt_len, mac_context, mac_cnt_len, 1, mac_key_server, &mac_skey_len))
    {
    	DMSG("[DeriveKeys(): DeriveKey fail] \n");
        return false;
    }
    if (!DeriveKey(sesscnt, master_key,mst_cnt_len, mac_context, mac_cnt_len, 2, mac_key_part2, &mac_keyp2_len))
    {
    	DMSG("[DeriveKeys(): DeriveKey fail] \n");
        return false;
    }
    memcpy_s2s_chk(mac_key_server+mac_skey_len, mac_key_part2, mac_keyp2_len,NULL);

    if (!DeriveKey(sesscnt, master_key,mst_cnt_len, mac_context, mac_cnt_len, 3, mac_key_client, &mac_ckey_len))
    {
    	DMSG("[DeriveKeys(): DeriveKey fail] \n");
        return false;
    }
    if (!DeriveKey(sesscnt, master_key,mst_cnt_len, mac_context, mac_cnt_len, 4, mac_key_part2, &mac_keyp2_len))
    {
    	DMSG("[DeriveKeys(): DeriveKey fail] \n");
        return false;
    }
    memcpy_s2s_chk(mac_key_client+mac_ckey_len, mac_key_part2, mac_keyp2_len,NULL);

    // Generate derived key for encryption key
    uint8_t enc_key[ENCKEY_LEN] = {0};
    size_t  enc_keylen = 0;
    if (!DeriveKey(sesscnt, master_key,mst_cnt_len, enc_context,enc_cnt_len, 1, enc_key, &enc_keylen))
    {
    	DMSG("[DeriveKeys(): DeriveKey fail] \n");
        return false;
    }

    set_mac_key_server(sesscnt, mac_key_server, MACKEY_LEN);
    set_mac_key_client(sesscnt, mac_key_client, MACKEY_LEN);
    set_encryption_key(sesscnt, enc_key,ENCKEY_LEN);
	sesscnt->has_mac_key = true;
	
	DMSG("DeriveKeys(): success \n");
    return true;
}

bool RSADeriveKeys(SessionContext * sesscnt,const uint8_t* enc_session_key, size_t enc_sesskey_len,
                   const uint8_t* mac_context,size_t mac_cnt_len,
                   const uint8_t* enc_context, size_t enc_cnt_len)
{
    if (!sesscnt->rsa_key_)
    {
        DMSG("[RSADeriveKeys(): no RSA key set] \n");
        return false;
    }
    if (enc_sesskey_len != RSA_KEY_LEN)
    {
        DMSG("[RSADeriveKeys(): encrypted session key is wrong size:%zu, should be %d] \n",
             enc_sesskey_len, RSA_KEY_LEN);
        
        return false;
    }
    //sesscnt->session_key_.resize(RSA_size(rsa_key_));
	int decrypted_size = RSA_KEY_LEN;
	unsigned char tmpsessionkey[RSA_KEY_LEN] = {0};

	int idx = find_hash("sha1");
	int statret = 0;
	rsa_decrypt_key_ex(&enc_session_key[0], enc_sesskey_len,tmpsessionkey,(unsigned long *)&decrypted_size,NULL,0
						,idx,LTC_PKCS_1_OAEP,&statret,sesscnt->rsa_key_);

    if (-1 == decrypted_size)
    {
        DMSG("[RSADeriveKeys(): error decrypting session key.] \n");       
        return false;
    }
	
	memcpy_s2s_chk(sesscnt->session_key_,tmpsessionkey,decrypted_size,NULL);
    //session_key_.resize(decrypted_size);
    
    if (decrypted_size != (int)(KEY_SIZE))
    {
        DMSG("[RSADeriveKeys(): error.  session key is wrong size: %d.] \n",
             decrypted_size);
        
        memset(sesscnt->session_key_,0, sizeof(sesscnt->session_key_));
        return false;
    }
	
    return DeriveKeys(sesscnt,sesscnt->session_key_,decrypted_size,mac_context,mac_cnt_len, enc_context,enc_cnt_len);
}
#endif

#define SHA256_SIGN_VERIFY
#ifdef SHA256_SIGN_VERIFY
bool GenerateSignature(SessionContext *sesscnt,
                       const uint8_t* message,
                       size_t message_length,
                       uint8_t* signature,
                       size_t* signature_length)
{
	DMSG(" enter, signature_length=%d \n ",*signature_length);

    if (message == NULL || message_length == 0 ||
        signature == NULL || *signature_length == 0)
    {
        DMSG("[OEMCrypto_GenerateSignature(): OEMCrypto_ERROR_INVALID_CONTEXT] \n");
        return false;
    }

    if (sesscnt->has_mac_key == false )
    {
        DMSG("[GenerateSignature(): No MAC WvKey] \n");
        return false;
    }

    if (*signature_length < SHA256_DIGEST_LENGTH)
    {
    	DMSG("[GenerateSignature(): signature_length < 32] \n");
        *signature_length = SHA256_DIGEST_LENGTH;
        return false;
    }

    unsigned int md_len = *signature_length;
	int ret = hmac_memory(find_hash("sha256"), sesscnt->mac_key_client_, SHA256_DIGEST_LENGTH, message, message_length, signature, (unsigned long *)signature_length);
     if(0 == ret)
     {
     	/*dprintf("GenerateSignature success length = %d, signature = \n",*signature_length);
		for(int i = 0; i < *signature_length; i++)
		{
	   	 	dprintf("%x ", signature[i]);
		}
		dprintf("\n");*/
	 	return true;
     }
	 else
	 {
	 	dprintf("GenerateSignature fail \n");
	 	return false;
	 }
	//return (0==hmac_memory(find_hash("sha256"), sesscnt->mac_key_client_, SHA256_DIGEST_LENGTH, message, message_length, signature, (unsigned long *)signature_length))?true:false; 

}

// Validate message signature
bool ValidateMessage(SessionContext *sesscnt,
                     const uint8_t* given_message,
                     size_t message_length,
                     const uint8_t* given_signature,
                     size_t signature_length)
{
	DMSG(" enter\n");

    if (signature_length != SHA256_DIGEST_LENGTH)
    {
    	DMSG("ValidateMessage: signature_length != 32.\n");
        return false;
    }
	
    uint8_t computed_signature[SHA256_DIGEST_LENGTH];
    unsigned int md_len = SHA256_DIGEST_LENGTH;
	

	if(0 != hmac_memory(find_hash("sha256"), &sesscnt->mac_key_server_[0], SHA256_DIGEST_LENGTH, given_message, message_length, computed_signature, (unsigned long *)&md_len))
	{
		DMSG("ValidateMessage: Could not compute signature.\n");
        return false;
	}

    if (memcmp(given_signature, computed_signature, signature_length))
    {
      //  DMSG("Invalid signature    given: %s",
      //       HexEncode(given_signature, signature_length).c_str());
      //  DMSG("Invalid signature computed: %s",
      //       HexEncode(computed_signature, signature_length).c_str());
      	DMSG(" given_signature != computed_signature \n");
        return false;
    }
	DMSG(" leave \n");
    return true;
}

bool Generic_Sign(SessionContext *sesscnt, const uint8_t* in_buffer,
                  size_t buffer_length,
                  OEMCrypto_Algorithm algorithm,
                  uint8_t* signature,
                  size_t* signature_length, uint32_t curtime)
{
    // Check there is a content key
    if (sesscnt->current_content_key_ == NULL)
    {
        DMSG("[Generic_Sign(): OEMCrypto_ERROR_NO_CONTENT_KEY] \n");
        return false;
    }
    if (*signature_length < SHA256_DIGEST_LENGTH)
    {
        *signature_length = SHA256_DIGEST_LENGTH;
        DMSG("[Generic_Sign(): bad signature length.\n");
        return false;
    }
	
    const WvKey * key = sesscnt->current_content_key_;
    const KeyControlBlock control = sesscnt->current_content_key_->control_;

    if ((int)(key->keydatalen_) != SHA256_DIGEST_LENGTH)
    {
        DMSG("[Generic_Sign(): CONTENT_KEY has wrong size; %d \n", key->keydatalen_);
        return false;
    }
    if (!(control.control_bits_ & kControlAllowSign))
    {
        DMSG("[Generic_Sign(): control bit says not allowed.\n");
        return false;
    }
    if (control.duration_ > 0)
    {
        if (control.duration_ < curtime)
        {
            DMSG("[Generic_Sign(): key expired.\n");
			DMSG("warning ::key time out,control.duration_  %d, curtime %d \n",control.duration_ , curtime);
            return false;
        }
    }
    if ( algorithm != OEMCrypto_HMAC_SHA256 )
    {
        DMSG("[Generic_Sign(): bad algorithm.\n");
        return false;
    }
    unsigned int md_len = *signature_length;

	if(0 == hmac_memory(find_hash("sha256"), key->keyvalue_,
		SHA256_DIGEST_LENGTH, in_buffer, buffer_length, signature, 
		(unsigned long *)&md_len))
    {
        *signature_length = md_len;
        return true;
    }
    DMSG("[Generic_Sign(): hmac failed.\n");

    return false;
}

bool Generic_Verify(SessionContext *sesscnt, const uint8_t* in_buffer,
                    size_t buffer_length,
                    OEMCrypto_Algorithm algorithm,
                    const uint8_t* signature,
                    size_t signature_length, uint32_t curtime)
{
    // Check there is a content key
    if (sesscnt->current_content_key_ == NULL)
    {
        DMSG("[Decrypt_Verify(): OEMCrypto_ERROR_NO_CONTENT_KEY]\n");
        return false;
    }
    if (signature_length < SHA256_DIGEST_LENGTH)
    {
    	DMSG("[Generic_Verify(): signature_length < 32] \n");
        return false;
    }
	
    const WvKey * key = sesscnt->current_content_key_;
    const KeyControlBlock control = sesscnt->current_content_key_->control_;
	
    if ((int)(key->keydatalen_) != SHA256_DIGEST_LENGTH)
    {
        DMSG("[Generic_Verify(): CONTENT_KEY has wrong size: %d \n", key->keydatalen_);
        return false;
    }
    if (!(control.control_bits_ & kControlAllowVerify))
    {
        DMSG("[Generic_Verify(): control bit says not allowed.\n");
        return false;
    }
    if (control.duration_ > 0)
    {
        if (control.duration_ < curtime)
        {
            DMSG("[Generic_Verify(): key expired.\n");
			DMSG("warning ::key time out,control.duration_  %d, curtime %d \n",control.duration_ , curtime);
            return false;
        }
    }
    if ( algorithm != OEMCrypto_HMAC_SHA256 )
    {
        DMSG("[Generic_Verify(): bad algorithm.\n");
        return false;
    }
	
    unsigned int md_len = signature_length;
    uint8_t computed_signature[SHA256_DIGEST_LENGTH];

	if(0 == hmac_memory(find_hash("sha256"), key->keyvalue_,
	SHA256_DIGEST_LENGTH, in_buffer, buffer_length, computed_signature, 
	(unsigned long *)&md_len))
    {
        return (0 == memcmp( signature, computed_signature, SHA256_DIGEST_LENGTH));
    }
    DMSG("[Generic_Verify(): HMAC failed.\n");
	
    return false;
}

#endif

#define KEY_RELATED
#ifdef  KEY_RELATED
bool InstallKey(SessionContext * sesscnt,const uint8_t* key_id, size_t key_id_len,
                const uint8_t* key_data,size_t key_data_len_len,
                const uint8_t* key_data_iv,size_t key_data_iv_len,
                const uint8_t* key_control,size_t key_control_len,
                const uint8_t* key_control_iv,size_t key_control_iv_len)
{
	DMSG(" enter\n");

    // Decrypt encrypted key_data using derived encryption key and offered iv
    uint8_t content_key[32] = {0};
    uint8_t  *key_control_str = (uint8_t *)malloc(key_control_len*2);
    if (key_control_str == NULL)
    {
        DMSG("[Installkey(): Could not malloc]\n");
        return false;
    }
	if (sesscnt->has_mac_key == false)
	{
		DMSG("[Installkey(): have not derived enc-mac keys]\n");
		free (key_control_str);
        return false;
	}
	
    KeyControlBlock key_control_block;

    if (!DecryptMessage(sesscnt, sesscnt->encryption_key_, ENCKEY_LEN, key_data_iv,key_data_iv_len,
                        key_data,key_data_len_len, content_key))
    {
        DMSG("[Installkey(): Could not decrypt key data]\n");
        free (key_control_str);
        return false;
    }

    // WvKey control must be supplied by license server
    if (key_control_len == 0)
    {
        DMSG("[Installkey(): WARNING: No WvKey Control]\n");
        key_control_block.valid_ = false;
        free (key_control_str);
        return false;
    }
    else
    {
        if (key_control_iv_len == 0)
        {
            DMSG("[Installkey(): ERROR: No WvKey Control IV]\n");
            free (key_control_str);
            return false;
        }
        if (!DecryptMessage(sesscnt, content_key,key_data_len_len, key_control_iv,key_control_iv_len,
                            key_control,key_control_len, key_control_str))
        {
            DMSG("[Installkey(): ERROR: Could not decrypt content key]\n");
            free (key_control_str);
            return false;
        }

        if (!ParseKeyControl(sesscnt, key_control_str,key_control_len, &key_control_block))
        {
            DMSG("Error InstallKey->parsing key control.\n");
            free (key_control_str);
            return false;
        }
    }

    WvKey  key ;
    key.type_ = (KeyType)KEYTYPE_CONTENT;
    memcpy_s2s_chk(key.keyvalue_,content_key,key_data_len_len,NULL);
    key.keydatalen_ = key_data_len_len;
    memcpy_s2s_chk(&(key.control_),&key_control_block,sizeof(key_control_block),NULL);
    key.has_control_ = true;
    key.valid_ = true;
    SessAddConKeypair(sesscnt,key_id,key_id_len,&key);
    
    free (key_control_str);
    return true;
}


bool RefreshKey(SessionContext * sesscnt,const uint8_t* key_id, size_t keyid_len,
                uint8_t* key_control,size_t key_cnt_len,
                const uint8_t* key_control_iv, size_t key_cnt_iv_len)
{
	DMSG("enter \n");

    if (keyid_len == 0)
    {
        // WvKey control is not encrypted if key id is NULL
        KeyControlBlock key_control_block;
        key_control_block.valid_ = true;
        if (!ParseKeyControl(sesscnt,key_control,key_cnt_len, &key_control_block))
        {
            DMSG("RefreshKey -> Parse key control error.\n");
            return false;
        }
        // Apply duration to all keys in this session
        return UpdateDuration(sesscnt->session_keys_,MAX_SESSION_KEYPAIR,&key_control_block);
              
     }

    WvKey* content_key = Find(sesscnt->session_keys_,MAX_SESSION_KEYPAIR,key_id,keyid_len);

    if (NULL == content_key)
    {
        DMSG("Error: no matching content key.\n");
        return false;
    }

    if (key_cnt_len == 0)
    {
        DMSG("Error: no key_control.\n");
        return false;
    }

    const uint8_t* content_key_value = content_key->keyvalue_;

    // Decrypt encrypted key control block
    uint8_t *control = NULL;
    control = (uint8_t *)malloc(key_cnt_len);
    if (control == NULL)
    {
        DMSG("RefreshKey malloc error\n");
        return false;
    }
    if (key_cnt_iv_len == 0)
    {
        // TODO(fredg): get confirmation from server team this is valid.
        DMSG("WvKey control block is NOT encrypted.\n");
		free(control);
        control = key_control;

		 KeyControlBlock key_control_block;
	    key_control_block.valid_ = true;
	    if (!ParseKeyControl(sesscnt,control,key_cnt_len, &key_control_block))
	    {
	        DMSG("Error RefreshKey ->parsing key control.\n");
	        return false;
	    }

	    if (!UpdateDurationKey(content_key,&key_control_block))
	    {
	        DMSG("Error RefreshKey ->updating duration.\n");
	        return false;
	    }

    }
    else
    {
        // TODO(fredg): get confirmation from server team this is valid, too.
        DMSG("WvKey control block is encrypted.\n");
        if (!DecryptMessage(sesscnt, content_key_value,content_key->keydatalen_,key_control_iv,
                            key_cnt_iv_len, key_control,key_cnt_len, control))
        {
            DMSG("Error RefreshKey ->decrypting key control block.\n");
            free(control);
            return false;
        }
		KeyControlBlock key_control_block;
	    key_control_block.valid_ = true;
	    if (!ParseKeyControl(sesscnt,control,key_cnt_len, &key_control_block))
	    {
	        DMSG("Error RefreshKey ->parsing key control.\n");
	        free(control);
	        return false;
	    }

	    if (!UpdateDurationKey(content_key,&key_control_block))
	    {
	        DMSG("Error RefreshKey ->updating duration.\n");
	        free(control);
	        return false;
	    }
		free(control);
    }
	
	return true;
}

bool UpdateMacKeys(SessionContext * sesscnt,const uint8_t* mac_keys, size_t mac_key_len,
                   const uint8_t* iv, size_t iv_len)
{
	DMSG("enter \n");

    // Decrypt mac key from enc_mac_key using device_keya
    uint8_t tmpmac_keys[MAC_KEY_SIZE*2] = {};
    if (!DecryptMessage(sesscnt, sesscnt->encryption_key_,ENCKEY_LEN, iv,iv_len,
                        mac_keys,mac_key_len, tmpmac_keys))
    {
    	DMSG(" DecryptMessage fail\n");
        return false;
    }
	
    memcpy_s2s_chk(sesscnt->mac_key_server_, tmpmac_keys, MAC_KEY_SIZE,NULL);
    memcpy_s2s_chk(sesscnt->mac_key_client_, tmpmac_keys+MAC_KEY_SIZE, MAC_KEY_SIZE,NULL);

    return true;
}

bool SelectContentKey(SessionContext * sesscnt,const KeyId key_id, int key_id_len)
{
	DMSG(" enter\n");

    const WvKey* content_key = Find(sesscnt->session_keys_,MAX_SESSION_KEYPAIR,key_id,key_id_len);

    if (NULL == content_key)
    {
        DMSG("[SelectContentKey(): No key matches key id]\n");
        return false;
    }
    sesscnt->current_content_key_ = content_key;
	
    return true;
}
#endif

#define NONCE_RELATED
#ifdef  NONCE_RELATED

void GenerateNonceSess(SessionContext * sesscnt, uint32_t * nonce)
{
    GenerateNonceNonTable(&(sesscnt->nonce_table_),nonce);
}

bool CheckNonceSess(SessionContext * sesscnt,uint32_t nonce)
{
    return CheckNonceNonTable(&(sesscnt->nonce_table_),nonce);
}

void FlushNoncesSess(SessionContext * sesscnt)
{
    FlushNonTable(&(sesscnt->nonce_table_));
}
#endif 


#define RSA_RELATED
#ifdef RSA_RELATED
int RewrapDeviceRSAKeySess(SessionContext* session_ctx,
                           const uint8_t* message,
                           size_t message_length,
                           const uint8_t* signature,
                           size_t signature_length,
                           const uint32_t nonce,
                           const uint8_t* enc_rsa_key,
                           size_t enc_rsa_key_length,
                           const uint8_t* enc_rsa_key_iv,
                           uint8_t* wrapped_rsa_key,
                           size_t*  wrapped_rsa_key_length)
{
	//DMSG(" enter, nonce= 0x%x\n",nonce);

    OEMCryptoResult result = OEMCrypto_SUCCESS;
	size_t buffer_size = enc_rsa_key_length + sizeof(WrappedRSAKey);

    // Validate nonce
    if (!CheckNonceSess(session_ctx,nonce))
    {
    	DMSG("RewrapDeviceRSAKeySess  CheckNonceSess error\n");
        return OEMCrypto_ERROR_INVALID_NONCE;
    }
    FlushNoncesSess(session_ctx);


    // Decrypt RSA key.
    uint8_t* pkcs8_rsa_key = (uint8_t*)malloc(enc_rsa_key_length);
    if (pkcs8_rsa_key == NULL)
    {
        DMSG("RewrapDeviceRSAKeySess  malloc error\n");
		return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

	DMSG(" call DecryptRSAKey\n");

    if (!DecryptRSAKey(session_ctx,enc_rsa_key, enc_rsa_key_length,
                       enc_rsa_key_iv, pkcs8_rsa_key))
    {
    	DMSG("DecryptRSAKey  error\n");
        result = OEMCrypto_ERROR_INVALID_RSA_KEY;
    }
    size_t padding = pkcs8_rsa_key[enc_rsa_key_length - 1];
    if (result == OEMCrypto_SUCCESS)
    {
        if (padding > 16)
        {
            DMSG("[RewrapRSAKey(): Encrypted RSA has bad padding: %d]\n", padding);
            result = OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }
    size_t rsa_key_length = enc_rsa_key_length - padding;
    // verify signature, verify RSA key, and load it.
    if (result == OEMCrypto_SUCCESS)
    {
    	DMSG(" call LoadRSAKey\n");
		
        if (!LoadRSAKey(session_ctx,pkcs8_rsa_key, rsa_key_length,
                        message, message_length,
                        signature, signature_length))
        {
        	DMSG("LoadRSAKey  error\n");
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            // return OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }

    // Now we generate a wrapped keybox.
    WrappedRSAKey* wrapped = (WrappedRSAKey*)(wrapped_rsa_key);
    // Pick a random context and IV for generating keys.
    if (result == OEMCrypto_SUCCESS)
    {   
   	 	DMSG(" call GetRandomNumTz\n");
		
		if (!GetRandomNumTz(wrapped->context, sizeof(wrapped->context)))
        {
        	DMSG("GetRandomNumTz  error\n");
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
        
		if (!GetRandomNumTz(wrapped->iv, sizeof(wrapped->iv)))
        {
        	DMSG("GetRandomNumTz  error\n");
            result =  OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }

    // Generate mac and encryption keys for encrypting the signature.
    if (result == OEMCrypto_SUCCESS)
    {
    	DMSG(" call DeriveKeys\n");
		
        if (!DeriveKeys(session_ctx,NULL,0,
                        wrapped->context,sizeof(wrapped->context),
                        wrapped->context,sizeof(wrapped->context)))
        {
        	DMSG("DeriveKeys  error\n");
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }

    // Encrypt rsa key with keybox.
    if (result == OEMCrypto_SUCCESS)
    {
    	DMSG(" call EncryptRSAKey\n");
		
        if (!EncryptRSAKey(session_ctx,pkcs8_rsa_key, enc_rsa_key_length,
                           wrapped->iv, wrapped->enc_rsa_key))
        {
        	DMSG("EncryptRSAKey  error\n");
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
    free(pkcs8_rsa_key);
	
    // The wrapped keybox must be signed with the same key we verify with. I'll
    // pick the server key, so I don't have to modify LoadRSAKey.
    if (result == OEMCrypto_SUCCESS)
    {
    	DMSG(" call sign \n");
	
        unsigned int sig_length = sizeof(wrapped->signature);

		if(0 != hmac_memory(find_hash("sha256"), &session_ctx->mac_key_server_[0],
			SHA256_DIGEST_LENGTH, wrapped->context, buffer_size - sizeof(wrapped->signature),
			wrapped->signature, 
			(unsigned long *)&sig_length))               
        {
        	DMSG("sign  error\n");
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
	//DMSG(" leave \n");
    return result;
}


int LoadDeviceRSAKeySessCtx(SessionContext* session_ctx,
                            const uint8_t* wrapped_rsa_key,
                            size_t wrapped_rsa_key_length)
{
	//DMSG(" enter\n");
    const WrappedRSAKey* wrapped
    = (const WrappedRSAKey*)(wrapped_rsa_key);
   
    // Generate mac and encryption keys for encrypting the signature.
    DMSG(" call DeriveKeys\n");
    if (!DeriveKeys(session_ctx,NULL,0,
                    wrapped->context,sizeof(wrapped->context),
                    wrapped->context,sizeof(wrapped->context)))
    {
    	DMSG("DeriveKeys  error\n");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    // Decrypt RSA key.
    uint8_t* pkcs8_rsa_key = (uint8_t*)malloc(wrapped_rsa_key_length
                                    - sizeof(wrapped->signature));
	if (pkcs8_rsa_key == NULL)
    {
        DMSG(" malloc pkcs8_rsa_key error\n");
		return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
	
    size_t enc_rsa_key_length = wrapped_rsa_key_length - sizeof(WrappedRSAKey);
    OEMCryptoResult result = OEMCrypto_SUCCESS;

	 DMSG(" call DecryptRSAKey\n");
    if (!DecryptRSAKey(session_ctx,wrapped->enc_rsa_key, enc_rsa_key_length,
                       wrapped->iv, pkcs8_rsa_key))
    {
    	DMSG("DecryptRSAKey  error\n");
        result = OEMCrypto_ERROR_INVALID_RSA_KEY;
    }
    size_t padding = pkcs8_rsa_key[enc_rsa_key_length - 1];
    if (result == OEMCrypto_SUCCESS)
    {
        if (padding > 16)
        {
            DMSG("[LoadDeviceRSAKey(): Encrypted RSA has bad padding: %d]\n", padding);
            result = OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }
    size_t rsa_key_length = enc_rsa_key_length - padding;
    // verify signature.
    if (result == OEMCrypto_SUCCESS)
    {
    	DMSG(" call LoadRSAKey\n");
        if (!LoadRSAKey(session_ctx,pkcs8_rsa_key, rsa_key_length,
                        wrapped->context,
                        wrapped_rsa_key_length - sizeof(wrapped->signature),
                        wrapped->signature,
                        sizeof(wrapped->signature)))
        {
        	DMSG("LoadRSAKey  error\n");
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            // return OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }
    free(pkcs8_rsa_key);
    return result;
}

bool GenerateRSASignature(SessionContext *sesscnt,
                          const uint8_t* message,
                          size_t message_length,
                          uint8_t* signature,
                          size_t* signature_length)
{
	//DMSG(" enter, signature_length=%d \n ",*signature_length);
    if (message == NULL || message_length == 0 ||
        signature == NULL || *signature_length == 0)
    {
        DMSG("[GenerateRSASignature(): OEMCrypto_ERROR_INVALID_CONTEXT]\n");
        return false;
    }
    if (!sesscnt->rsa_key_)
    {
        DMSG("[GenerateRSASignature(): no RSA key set]\n");
        return false;
    }
    if (*signature_length < RSA_KEY_LEN)
    {
    	DMSG("[GenerateRSASignature(): signature_length < 256] \n");
        *signature_length = RSA_KEY_LEN;
        return false;
    }

    // Hash the message using SHA1.
    uint8_t hash[SHA_DIGEST_LENGTH];
	hash_state hmac;
	int idx = 0;
	uint8_t key[SHA_DIGEST_LENGTH];
	unsigned long dislen = SHA_DIGEST_LENGTH;
	
	idx = find_hash("sha1");
	if (sha1_init(&hmac) != CRYPT_OK)
	{
		DMSG("[GeneratRSASignature(): error HMAC_Init.]\n");
	}
	if (sha1_process(&hmac,message, message_length) != CRYPT_OK)
	{
		DMSG("[GeneratRSASignature(): error hmac_process.]\n");	
	}
	if (sha1_done(&hmac,hash) != CRYPT_OK)
	{
		DMSG("[GeneratRSASignature(): error hmac_done.]\n");	
	}
	
    // Add PSS padding.
    //std::vector<uint8_t> padded_digest(*signature_length);
    uint8_t * padded_digest = (uint8_t *)malloc(*signature_length);
    if (padded_digest == NULL)
    {
        DMSG("[GeneratRSASignature(): error malloc.]\n");
        return  false;
    }

    int status =  0;
	unsigned long outlen = *signature_length;
	prng_state sprngstat;
	
	int idx_prng = find_prng("sprng");
	rsa_sign_hash_ex(hash,SHA_DIGEST_LENGTH,signature,(unsigned long *)signature_length,LTC_PKCS_1_PSS,&sprngstat
						, idx_prng,idx,20,sesscnt->rsa_key_);

	free (padded_digest);
	
    return true;
}
#endif
bool CheckDecCTRContentKey(SessionContext *sesscnt,uint32_t curtime,BufferType buffer_type,unsigned short * tmp_slot)
{
	if (sesscnt->current_content_key_ == NULL)
    {
        DMSG("[CheckDecCTRContentKey(): OEMCrypto_ERROR_NO_CONTENT_KEY]\n");
        return false;
    }
    const KeyControlBlock control = sesscnt->current_content_key_->control_;
    if (control.control_bits_ & kControlDataPathSecure)
    {
        if (buffer_type == kBufferTypeClear)
        {
            DMSG("[CheckDecCTRContentKey(): Secure key with insecure buffer]\n");
            return false;
        }
    }
    if (control.control_bits_ & kControlHDCPRequired)
    {
        // For reference implementation, we do not implement any HDCP.
        DMSG("[CheckDecCTRContentKey(): we do not implement any HDCP]\n");
        return false;
    }
    if (control.duration_ > 0)
    {
        if (control.duration_ < curtime)
        {
        	DMSG("warning ::key time out,control.duration_  %d, curtime %d \n",control.duration_ , curtime);
            return false;
        }
    }

	const uint8_t * content_key_value = sesscnt->current_content_key_->keyvalue_;
    const size_t cntkey_len = sesscnt->current_content_key_->keydatalen_;

    // Set the AES key.
    if (cntkey_len != AES_BLOCK_SIZE)
    {
        DMSG("[CheckDecCTRContentKey(): CONTENT_KEY has wrong size: %d \n", cntkey_len);
        return false;
    }
	//Set the content_key_value  into GCPU 
	GCPU_SetSecureSlot(AES_WV_SECURE_KEY_PTR, content_key_value, cntkey_len);

	*tmp_slot = AES_WV_SECURE_KEY_PTR;

	return true;
}

bool CheckEncDecCBCContentKey(SessionContext *sesscnt,uint32_t curtime,int enc_dec,unsigned short * tmp_slot)
{
	// Check there is a content key
    if (sesscnt->current_content_key_ == NULL)
    {
        DMSG("[CheckEncDecCBCContentKey(): OEMCrypto_ERROR_NO_CONTENT_KEY]\n");
        return false;
    }
    const WvKey * key = sesscnt->current_content_key_;
    const KeyControlBlock control = sesscnt->current_content_key_->control_;
    // Set the AES key.
    if ((int)(key->keydatalen_) != AES_BLOCK_SIZE)
    {
        DMSG("[CheckEncDecCBCContentKey(): CONTENT_KEY has wrong size: %d \n",(key->keydatalen_));
        return false;
    }
	if (enc_dec == 0)
	{
	    if (!(control.control_bits_ & kControlAllowEncrypt))
	    {
	        DMSG("[Generic_Encrypt()->CheckEncDecCBCContentKey: control bit says not allowed.\n");
	        return false;
	    }
	}
	else  //use for decrypt
	{
		if (!(control.control_bits_ & kControlAllowDecrypt))
		{
			DMSG("[Generic_Decrypt()->CheckEncDecCBCContentKey: control bit says not allowed.\n");
			return false;
		}
		if (control.control_bits_ & kControlDataPathSecure)
		{
			DMSG("[Generic_Decrypt()->CheckEncDecCBCContentKey: control bit says secure path only.\n");
			return false;
		}

	}
    if (control.duration_ > 0)
    {
        if (control.duration_ < curtime)
        {
            DMSG("[CheckEncDecCBCContentKey(): key expired.\n");
			DMSG("warning ::key time out,control.duration_  %d, curtime %d \n",control.duration_ , curtime);
            return false;
        }
    }

	const uint8_t * content_key_value = sesscnt->current_content_key_->keyvalue_;
    const size_t cntkey_len = sesscnt->current_content_key_->keydatalen_;

    // Set the AES key.
    if (cntkey_len != AES_BLOCK_SIZE)
    {
        DMSG("[CheckDecCTRContentKey(): CONTENT_KEY has wrong size: %d \n", cntkey_len);
        return false;
    }
	//Set the content_key_value  into GCPU 
	GCPU_SetSecureSlot(AES_WV_SECURE_KEY_PTR, content_key_value, cntkey_len);

	*tmp_slot = AES_WV_SECURE_KEY_PTR;
	
	return true;
}
#endif



#if 0
///kernel interface for usr

bool DecryptCTR(SessionContext* session,
                const uint8_t* iv,
                size_t block_offset,
                const uint8_t* cipher_data,
                size_t cipher_data_length,
                bool is_encrypted,
                void* clear_data,
                BufferType buffer_type,uint32_t curtime)
{
	DMSG(" enter is_encrypted=%d \n",is_encrypted);
	
    // If the data is clear, we do not need a current key selected.
    if (!is_encrypted)
    {
        memcpy((uint8_t*)(clear_data),
               cipher_data, cipher_data_length);
        return true;
    }
	
 	//if the decryptCTR be placed in the kernel, the following code will be used. 
	unsigned short tmp_slot = 0;
	if (false == CheckDecCTRContentKey(session,curtime, buffer_type,&tmp_slot))
	{
		DMSG("CheckDecCTRContentKey  error\n");
		return false;	
	}
	
	/*if (gcpu_invalidslot == tmp_slot)
	{
		return false;	
	}*/
	
	symmetric_CTR ctr;
	int idx = 0;
	int n = block_offset;
	int len = cipher_data_length;
	uint8_t * tmpclear_data = (unsigned char *)clear_data;

	const uint8_t * content_key = session->current_content_key_->keyvalue_;
    const size_t cntkey_len = session->current_content_key_->keydatalen_;
	
  	if ((idx = find_cipher("aes")) == -1) 
	{
		DMSG("[DecryptCTR(): find_cipher fail]\n");
		return false;
	}
	 
	if (buffer_type == kBufferTypeDirect)
    {
        // For reference implementation, we quietly drop direct video.
        DMSG("[DecryptCTR(): kBufferTypeDirect]\n");
        return true;
    }

    if (buffer_type == kBufferTypeSecure)
    {
        // For reference implementation, we also quietly drop secure data.
        DMSG("[DecryptCTR(): kBufferTypeSecure]\n");
        return true;
    }

    // Local copy (will be modified).
    uint8_t aes_iv[AES_BLOCK_SIZE];
    memcpy(aes_iv, &iv[0], AES_BLOCK_SIZE);

    // Encrypt the IV.
    uint8_t ecount_buf[AES_BLOCK_SIZE];
    
    // Decryption.  |LTC_CTR_RFC3686  CTR_COUNTER_LITTLE_ENDIAN
    //unsigned int block_offset_cur = block_offset;
     if ((ctr_start(idx, aes_iv, content_key, cntkey_len, 0, CTR_COUNTER_BIG_ENDIAN, &ctr)) != CRYPT_OK) 
	 {
	 	DMSG("ctr_start  error\n");
        return false;
     }
	 
	 if (block_offset != 0)
     {
        // The context is needed only when not starting a new block.
       // AES_encrypt(aes_iv, ecount_buf, &aes_key);

		cipher_descriptor[ctr.cipher].ecb_encrypt(aes_iv, ecount_buf, &ctr.key); 
       	while (n && len)
		{
			*(tmpclear_data++) = *(cipher_data++) ^ ecount_buf[n];
			--len;
			n = (n+1) % 16;
		}
		if (len == 0)
		{
			return true;
		}
        ctr128_inc(aes_iv);
     }
	 
	 ctr_setiv(aes_iv,AES_BLOCK_SIZE,&ctr);
     if ((ctr_decrypt(cipher_data, tmpclear_data, len, &ctr)) != CRYPT_OK) 
	 {
	 	DMSG("ctr_decrypt  error\n");
        return false;
     }
     ctr_done(&ctr);

	dprintf("DecryptCTR(): success length = %d, clear data = \n",cipher_data_length);
	/*for(int i = 0; i < cipher_data_length; i++)
	{
		dprintf("%x ", tmpclear_data[i]);
	}
	dprintf("\n");*/

    return true;
}

bool Generic_Encrypt(SessionContext *sesscnt, const uint8_t* in_buffer,
                     size_t buffer_length,
                     const uint8_t* iv,
                     OEMCrypto_Algorithm algorithm,
                     uint8_t* out_buffer, uint32_t curtime)
{
	if ( buffer_length % AES_BLOCK_SIZE != 0 )
    {
        DMSG("[Generic_Encrypt(): buffers size bad.\n");
        return false;
    }
	
	if ( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING )
    {
        DMSG("[Generic_Encrypt(): algorithm bad.\n");
        return false;
    }
	
	//if the Generic_Encrypt is in kernel will call the following code
	unsigned short tmp_slot = 0;
	if (false == CheckEncDecCBCContentKey(sesscnt,curtime,0,&tmp_slot))
	{
		DMSG("CheckEncDecCBCContentKey  error\n");
		return false;	
	}
	/*if (gcpu_invalidslot == tmp_slot)
	{
		return false;	
	}*/

    uint8_t iv_buffer[ KEY_IV_SIZE];
    memcpy(iv_buffer, iv, KEY_IV_SIZE);
	const WvKey * key = sesscnt->current_content_key_;
	const uint8_t* key_u8 = key->keyvalue_;
	
	symmetric_CBC tmpcbc;
	int idx = find_cipher("aes");
	if (cbc_start(idx,
					iv_buffer,key_u8,16,
					0,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_start error \n");
		return false;
	}
	
	if (cbc_encrypt(in_buffer,out_buffer,buffer_length,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_decrypt error \n");
		return false;
	}
	if (cbc_done(&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_done error \n");
		return false;
	}

    return true;
}

bool Generic_Decrypt(SessionContext *sesscnt, const uint8_t* in_buffer,
                     size_t buffer_length,
                     const uint8_t* iv,
                     OEMCrypto_Algorithm algorithm,
                     uint8_t* out_buffer, uint32_t curtime)
{
	if ( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING )
    {
        DMSG("[Generic_Decrypt(): bad algorithm.\n");
        return false;
    }
	
    if ( buffer_length % AES_BLOCK_SIZE != 0 )
    {
        DMSG("[Generic_Decrypt(): bad buffer size.\n");
        return false;
    }
	
  	//when Generic_Decrypt is in kernel, use the else 
	unsigned short tmp_slot = 0;
	if (false == CheckEncDecCBCContentKey(sesscnt,curtime,1,&tmp_slot))
	{
		DMSG("CheckEncDecCBCContentKey  error\n");
		return false;	
	}
	/*if (gcpu_invalidslot == tmp_slot)
	{
		return false;	
	}*/

    uint8_t iv_buffer[ KEY_IV_SIZE];
    memcpy(iv_buffer, iv, KEY_IV_SIZE);
	const WvKey * key = sesscnt->current_content_key_;
	const uint8_t* key_u8 = key->keyvalue_;
	
	symmetric_CBC tmpcbc;
	int idx = find_cipher("aes");
	if (cbc_start(idx,
					iv_buffer,key_u8,16,
					0,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_start error \n");
		return false;
	}
	
	if (cbc_decrypt(in_buffer,out_buffer,buffer_length,&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_decrypt error \n");
		return false;
	}
	if (cbc_done(&tmpcbc) != CRYPT_OK)
	{
		DMSG("[DecryptRSAKey(): cbc_done error \n");
		return false;
	}

    return true;
}
#endif

bool GetRandomNum(uint8_t * randomData, size_t dataLength)
{
 	//when in kernel, use the gcpu randnum
	#define SYSENTROPY_LEN 10
	prng_state prng;
	unsigned char sysentropy[SYSENTROPY_LEN]={};
	
	sprng_read(sysentropy, SYSENTROPY_LEN, NULL);
	if (fortuna_start(&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNum(): fortuna_start error \n");
	}
	if(fortuna_add_entropy(sysentropy, SYSENTROPY_LEN,&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNum(): for_add_entropy error \n");
	}
	if(fortuna_ready(&prng) != CRYPT_OK)
	{
		DMSG("[GetRandomNum(): fortuna_ready error \n");
	}
	fortuna_read(randomData,dataLength,&prng);
	fortuna_done(&prng);

	return true;
}


