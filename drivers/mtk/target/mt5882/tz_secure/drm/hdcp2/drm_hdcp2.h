#ifndef _DRV_HDCP2
#define _DRV_HDCP2


/*test vector*/
#define KEYLEN_ENCDCP (862)
#define KEYLEN_DCP (858)
#define KEYLEN_LC128 (16)
#define KEYLEN_CERTRX (522)
#define KEYLEN_P (64)
#define KEYLEN_Q (64)
#define KEYLEN_DP (64)
#define KEYLEN_DQ (64)
#define KEYLEN_QINV (64)

#define KEYLEN_RXID (5)                  // Receiver ID 40 bit Low Secure Storage
#define KEYLEN_PUBLIC (128 + 3)     // Device Public Modulus Key 1024bit Low Secure Storage
#define KEYLEN_PUBMN (128)     // Device Public Modulus Key 1024bit Low Secure Storage
#define KEYLEN_PUBEE (3)     // Device Public Exponent E Key 24 bit Low Secure Storage
#define KEYLEN_RESERVED (2)          // RESERVED
#define KEYLEN_ROOTSIGN (384)      // Root Signature 3072 bit Low Secure Storage
#define OK 0
#define NOT_OK -1
#define KEYSET_ERROR_INVALID_LEN	0x2
#define KEYSET_ERROR_SHA1_CHECK		0x3
#define KEYSET_ERROR_INVALID		0x4

#define ENCRYPTION_DATA_LENGTH 900      // Key length efore AES encryption is 862, and become 864 after AES enryption
#define ENCRYPTION_BLOCKS (ENCRYPTION_DATA_LENGTH / 16)
#define ENCRYPTION_SIZE (ENCRYPTION_BLOCKS * 16)
#define PRIV_KEY_SIZE (16)
#define DECRYPTION_DATA_LENGTH 862

#define PEKPUB_KM_SIZE 128
#define PRTX_SIZE 8
#define KM_SIZE 16
#define DKEY_SIZE 16
#define KD_SIZE 32
#define PRN_SIZE 8
#define PRRX_SIZE 8
#define KH_SIZE 16
#define KPRIVRX_SIZE 320
#define KEY_P_SIZE 64
#define KEY_Q_SIZE 64
#define KEY_DP_SIZE 64
#define KEY_DQ_SIZE 64
#define KEY_QINV_SIZE 64
#define SHA256_OUTPUT_SIZE 32
#define PH_PRIME_SIZE 32
#define PL_PRIME_SIZE 32
#define IV_SIZE 16
#define KS_SIZE 16
#define KS_XOR_LC128_SIZE 16
#define LC128_SIZE 16
#define ENCKMKH_SIZE 16
#define DECKMKH_SIZE 16
#define ENCKM_SIZE (KM_SIZE+64)
#define SECUREKM_SIZE (KM_SIZE+52)
#define MASKED_DB_SIZE 99
#define DB_SIZE 95
#define DB_MASK_SIZE 95



#define HMAC_SHA256_LEN 32



/* for HDCP TX */
#define MAX_PAIRING_SIZE 31
#define M_SIZE 16
#define seq_num_M_LEN	(3)
#define KEYLEN_M (16)   
#define PEKH_KM_SIZE (16)
#define PM_PRIME_SIZE 32
#define PV_PRIME_SIZE 32
#define KEYLEN_RX_CERT_CONTENT (138)

#define PROTECTED_KEY_MAX (32)

#define HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE 		(880)

// key layout: 	(912 + 36)	// 18 + 18 + 862 + PADDING +36bytes secure storage padding


#define HDCP2X_RX_PHILIPS_2K14_ENC_KEY_SET_SIZE		(912 + 36)

#define DTV_STATUS_T int

extern DTV_STATUS_T DRV_HDCP2_SetEncLc128 (unsigned char *pEncKey, UINT32 u4Length);
extern DTV_STATUS_T DRV_HDCP2_SetEncKey(unsigned char *pEncKey, UINT32 u4length);
extern DTV_STATUS_T DRV_HDCP2_GetCertInfo(unsigned char *pData, UINT32 u4length);
extern DTV_STATUS_T DRV_HDCP2_DecryptRSAESOAEP (unsigned char *pEkpub_km);
extern DTV_STATUS_T DRV_HDCP2_KdKeyDerivation (unsigned char *pRtx, UINT32 u4length);
extern DTV_STATUS_T DRV_HDCP2_ComputeHprime (unsigned char *pH_prime, unsigned char Repeater, unsigned char *pRtx);
extern DTV_STATUS_T DRV_HDCP2_ComputeLprime (unsigned char *pL_prime, unsigned char *pRn, unsigned char *pRrx);
extern DTV_STATUS_T DRV_HDCP2_ComputeKh (void);
extern DTV_STATUS_T DRV_HDCP2_EncryptKmUsingKh (unsigned char *pEkh_km, unsigned char *pM);
extern DTV_STATUS_T DRV_HDCP2_DecryptKmUsingKh (unsigned char *pM, unsigned char *ekh_km);
extern DTV_STATUS_T DRV_HDCP2_DecryptEKs (unsigned char *pEks, unsigned char *pRtx, unsigned char *pRrx, unsigned char *pRn);
extern DTV_STATUS_T DRV_HDCP2_KsXorLC128 (void);
extern DTV_STATUS_T DRV_HDCP2_DataDecrypt(
				unsigned char *ucDstFrame,
				unsigned char *ucSrcFrame,
				unsigned long ulCount,
				unsigned char *rx_p				/* iv */
                );
extern DTV_STATUS_T DRV_HDCP2_SetRiv_Pid(unsigned char *pRiv, UINT32 pid);
extern DTV_STATUS_T DRV_HDCP2_PresetKsLC128(unsigned char *ks, unsigned char *lc128);
extern DTV_STATUS_T DRV_HDCP2_Generate_Km (unsigned char *pMaskedDB, unsigned char *pDB, unsigned char *pDBmask);
extern DTV_STATUS_T DRV_HDCP2_Generate_Ks (unsigned char *pEks, unsigned char *pRrx, unsigned char *pRtx, unsigned char *pRn);
extern DTV_STATUS_T DRV_HDCP2_SetEncKm (unsigned char *pEncKm);
extern DTV_STATUS_T DRV_HDCP2_GetEncKm (unsigned char *pEncKm);


/*
	for HDCP 2.2 
*/
extern DTV_STATUS_T DRV_HDCP2_2_KdKeyDerivation(unsigned char * pRtx,unsigned char * pRrx,UINT32 u4length);
extern DTV_STATUS_T DRV_HDCP2_2_ComputeHprime(unsigned char * pH_prime,unsigned char Repeater,unsigned char * pRtx,unsigned char rx_version,unsigned short rx_cap_mask,unsigned char tx_version,unsigned short tx_cap_mask);
extern DTV_STATUS_T DRV_HDCP2_2_ComputeLprime(unsigned char * pL_prime,unsigned char * pRn,unsigned char * pRrx);



extern BOOL SvcEx_MTDRM_HDCP2_SetEncLc128(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_SetEncKey(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_GetCertInfo(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_DecryptRSAESOAEP(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_KdKeyDerivation(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_ComputeHprime(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_ComputeLprime(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_ComputeKh(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_EncryptKmUsingKh(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_DecryptKmUsingKh(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_DecryptEKs(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_KsXorLC128(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_SetRiv_Pid(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_PresetKsLC128(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_Generate_Km(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_Generate_Ks(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_SetEncKm(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_GetEncKm(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_2_KdKeyDerivation(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_2_ComputeHprime(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_MTDRM_HDCP2_2_ComputeLprime(UINT32 u4Addr, UINT32 u4Size);



#endif

