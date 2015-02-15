/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tee_securestorage.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef TEE_PLAYREADY_H
#define TEE_PLAYREADY_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_PLAYREADY_OMAC1_KEY_SIZE    (16)
#define TEE_PLAYREADY_CONTENT_KEY_SIZE  (16)
#define TEE_PLAYREADY_AESECB_KEY_SIZE   (16)
#define TEE_PLAYREADY_AESCBC_KEY_SIZE   (16)
#define TEE_PLAYREADY_AES_BLOCKLEN      (16)
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct 
{
    UINT64  qwInitializationVector; 
    UINT64  qwBlockOffset;
    BYTE    bByteOffset;  /* Byte offset within the current block */
} DRM_AES_COUNTER_MODE_CONTEXT;

typedef struct 
{
    UINT64  qwInitializationVector; 
	UINT64  qwCount;
    UINT64  qwBlockOffset;
    BYTE    bByteOffset;  /* Byte offset within the current block */
} DRM_DASHAES_COUNTER_MODE_CONTEXT;


typedef enum _ENUM_CONTENTKEY_TYPE_T
{
    PLAYREADY_CONTENTKEY_VIDEO = 0,
	PLAYREADY_CONTENTKEY_AUDIO = 1,
    NAUTILUS_CONTENTKEY_VIDEO = 2,
    NAUTILUS_CONTENTKEY_AUDIO = 3,
}
ENUM_CONTENTKEY_TYPE_T;


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
UINT32 _teePlayReadyBaseUT(UINT8 *pu1Buf, UINT32 u4Len);
UINT32 _teePlayReadyGetKFKEY(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, BOOL bProtect);
UINT32 _teePlayReadyGetProvisionData(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, BOOL bProtect);
UINT32 _teePlayReadyECCP256GenKeyPair(UINT8 *pu1PubKeyBuf, UINT8 * pub1PrivKeyBuf, BOOL bEncKeyPair);
UINT32 _teePlayReadyECCP256SetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect);
UINT32 _teePlayReadyECDSAP256Sign(UINT8 *pu1Data, UINT32 u4DataLen, UINT8 * pu1Signature);
UINT32 _teePlayReadyECCP256Decrypt(UINT8 * pu1CipherText, UINT8 * pu1PlainText, BOOL bProtect);
UINT32 _teePlayReadyCopyBytes(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count);
UINT32 _teePlayReadyOMAC1SetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4BufLen, BOOL bProtect);
UINT32 _teePlayReadyOMAC1Verify(UINT8 *pu1DataBuf, UINT32 u4BufIdx, UINT32 u4DataLen, UINT8 *pu1Signature, UINT32 u4SigOffset, BOOL *pbResult);
UINT32 _teePlayReadyOMAC1Sign(UINT8 *pu1DataBuf, UINT32 u4BufIdx, UINT32 u4DataLen, UINT8 *pu1Signature, BOOL bProtect);
UINT32 _teePlayReadyContentSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4BufLen, BOOL bProtect, ENUM_CONTENTKEY_TYPE_T keytype);
UINT32 _teePlayReadyContentDecrypt(DRM_AES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, BOOL bProtect, UINT64 u8ByteOffset);
UINT32 _teePlayReadyMultiContentDecrypt(DRM_AES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, UINT32 *au4Seg, UINT32 u4SegLen, UINT64 u8ByteOffset, BOOL bProtect);
UINT32 _teePlayReadyDashSampleDecrypt(DRM_DASHAES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, UINT32 *au4Seg, UINT32 u4SegLen, UINT64 u8ByteOffset, BOOL bProtect, BOOL isAudio);
UINT32 _teePlayReadyAESECBSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect);    	
UINT32 _teePlayReadyAESECBEncrypt(UINT8 * pu1Data, UINT32 u4DataLen, BOOL bProtect);
UINT32 _teePlayReadyAESECBDecrypt(UINT8 * pu1Data, UINT32 u4DataLen, BOOL bProtect);  
UINT32 _teePlayReadyAESCBCSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect);    	
UINT32 _teePlayReadyAESCBCEncrypt(UINT8 * pu1Data, UINT32 u4DataLen, UINT8* IV,UINT32 IVLen, BOOL bProtect);
UINT32 _teePlayReadyAESCBCDecrypt(UINT8 * pu1Data, UINT32 u4DataLen, UINT8* IV,UINT32 IVLen, BOOL bProtect);  
UINT32 _teePlayReadyDMXSetKey(UINT8 *pu1Buf, UINT32 u4Len);
UINT32 _teePlayReadyUnshuffleContentKey(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count);
UINT32 _teePlayReadyPrepareContentKey_Prime(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count);
UINT32 _teePlayReadyLinkBytes(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromSize, UINT8 *pu1From2, UINT32 u4FromSize2);

#if 1 //securestopintee

#define DER_INTEGER  0x02
#define DER_SEQUENCE 0x30
#define DER_OCTETS 0x04

#define TARGET_LITTLE_ENDIAN   1

#if TARGET_LITTLE_ENDIAN
#define INT32(X) ((((X)>>24)&0x000000ff)|(((X)<<8)&0x00ff0000)|(((X)>>8)&0x0000ff00)|(((X)<<24)&0xff000000))
#else
#define INT32(X) (X)
#endif

#define FREE(X) if ((X) != NULL) { _tee_free(X); (X) = NULL; }
#define FREE_ON_ERROR(X) if ( (status != TEE_SUCCESS) && ((X) != NULL)) { _tee_free(X); (X) = NULL; }
#define CHECK_NULL(P) if (P == NULL) { _dbg("NULL %s passed\n", #P); RETURN(TEE_ERR_FAIL); }


#define UNINIT_TYPE_ID (-1)
#define SECURE_STOP_VERSION 1

// error codes - to be expanded
#define TEE_SUCCESS 0
#define TEE_ERR_FAIL 1
#define TEE_ERR_BUFFERTOOSMALL 2

// session ID
#define TEE_SESSION_ID_LEN                        16 // session ID length

// key ID
#define TEE_KEY_ID_LEN                            16 // content key ID length
#define TEE_MAX_STATE_SIZE                      2048 // maximum state size
#define TEE_MAX_NUM_SECURE_STOPS                  20 // maximum number of persisted secure stops
#define SECURE_STOP_UPDATE_TIME_SEC 60
#define TEE_MAX_SECRET_KEY_SIZE  16 // support up to AES-128 key size// signature
#define AES_128_KEY_SIZE 16
#define TEE_MAX_SIGNATURE_SIZE                   256 // ECDSA-SHA256 signature
typedef enum { UNINIT_TYPE = 0, SECRET_KEY_TYPE = 1, PRIVATE_KEY_TYPE = 2, CIPHER_CTX_TYPE = 3 } TEE_OBJ_TYPE;

typedef struct {
    int id;
} TEE_SECRET_KEY;

typedef struct {
    unsigned char keyBytes[TEE_MAX_SECRET_KEY_SIZE];
    int keySize;
} TEE_SECRET_KEY_IMPL;
// Secure Stop Data
typedef struct {
    int version;
    unsigned char session_id[TEE_SESSION_ID_LEN]; // nonce in license challenge becomes session id
    unsigned char kid[TEE_KEY_ID_LEN]; // content key id
    long start_time; // license arrival time - set once
    long update_time; // last playback-in-progress time - updated miltiple times
    long auth_time; // secure stop authentication (signing) time - set once
    int stopped; // 1 if was normally stopped, 0 - if abnormally shut down
} SECURE_STOP;

typedef struct {
    int id; // sequential session id, for internal reference
    int key_id; // id of the corresponding content decryption key
    SECURE_STOP ss;
} TEE_SESSION;

typedef struct {
    unsigned char data[TEE_MAX_SIGNATURE_SIZE];
    int length;
} TEE_SIGNATURE;
int _tee_antirollback_gettime(unsigned long *time);
int _tee_antirollback_loadinclock(char* updatedclock , int updateclock_len);
int _tee_antirollback_updateclock(unsigned long time , char* updatedclock , int updateclock_len);


int _tee_add_session(const unsigned char * const session_id);
int _tee_cancel_session(const unsigned char * const session_id);
int _tee_bind_license(const unsigned char  * const session_id,
                      const unsigned char  * const kid);
int _tee_bind_session(const TEE_SECRET_KEY * const secKey ,
                      const unsigned char  * const kid, long update_time, const unsigned char  * const SessionID);
int _tee_remove_session(const int key_id);
int _tee_reset_sessions();
void __tee_write_secure_stop(SECURE_STOP   * const ss,
                             unsigned char * const outbuf,
                             int             const inlen);
int _tee_init_secure_stops(const unsigned char * const inbuf,
                                          int    const inlen);
int _tee_get_secure_stops(unsigned char * const outbuf, int * const outlen);
int _tee_get_secure_stop_ids(unsigned char          session_id[][TEE_SESSION_ID_LEN],
                                       int * const outnum);
int _tee_get_secure_stop(const unsigned char * const session_id,
                               unsigned char * const outSecureStop,
                                         int * const outSecureStopLen, long update_time, TEE_SIGNATURE * sigout);
int _tee_update_secure_stop(const int key_id, int * const updated, long update_time);
int _tee_commit_secure_stop(const unsigned char * const session_id);
int _tee_reset_secure_stops(int * const outNumRemoved);
int _tee_UnloadSecretKey(TEE_SECRET_KEY * const inSecretKey);




#if 1
#define RETURN(X) { status = X; _dbg("TEE Failure %d\n", X); goto Exit; }
#define _print_bin(x, y, z) _print_bin_func(__func__, x, y, z)
#define _print_hex(x, y, z) _print_hex_func(__func__, x, y, z)
#define _print_txt(x, y, z) _print_txt_func(__func__, x, y, z)
#define _dbg(fmt, ...) printf("****** %s: " fmt, __func__, ##__VA_ARGS__)
#else
#define RETURN(X) { status = X; goto Exit; }
#define _print_bin(fmt, args...) ((void)0)
#define _print_hex(fmt, args...) ((void)0)
#define _print_txt(fmt, args...) ((void)0)
#define _dbg(fmt, args...)       ((void)0)
#endif
#define CHECK(X) if ((X) != TEE_SUCCESS) { printf("%s:%d: TEE Failure\n", __FILE__, __LINE__);  RETURN(TEE_ERR_FAIL); } 
#define DBG_ENTER _dbg("Enter\n");
#define DBG_EXIT _dbg("Exit %d\n", status);

#define _info(fmt, ...) printf("****** %s: " fmt, __func__, ##__VA_ARGS__)

#endif
     	 
#endif
