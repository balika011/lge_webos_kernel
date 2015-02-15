/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised 
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and 
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2907 $
 * ----------------------------------------------------------------------------
 */

/* ============================================================================
    Includes
============================================================================ */
#include "tzst.h"
#include "svc_registry.h"

#include "x_hal_5381.h"
#include "tee_securestorage.h"
#include "tomcrypt.h"
#include "x_chipreg.h"
#include "ci_trust_zone.h"

#define TZ_CIPLUS_SHA1_DIGEST_LENGTH         20
#define TZ_CIPLUS_SHA512_DIGEST_LENGTH       64

#define TZ_CIPLUS_AES_BLOCK_SIZE          16
#define TZ_CIPLUS_KEY_IV_SIZE             16
#define TZ_CIPLUS_SS_EXT_SIZE             128
#define TZ_CIPLUS_AES_128_CBC_KEY_SIZE    16
#define TZ_CIPLUS_RAW_CRED_HDR_OFFSET      ((UINT16)4)
#define TZ_CIPLUS_RAW_CRED_TAG_SIZE        ((UINT16)1)
#define TZ_CIPLUS_RAW_CRED_ITEM_LEN_SIZE   ((UINT16)2)
#define TZ_CIPLUS_MAX_KEY_PACK             (0x8000)
#define TZ_CIPLUS_DEVICE_ID_SIZE           ((UINT16)16)


typedef UINT8 CI_KEY_TYPE_T; /**< indicates ci plus key type       */

#define TZ_CIPLUS_KEY_ROOT_CERT          ((CI_KEY_TYPE_T) 0x00)/**< indicates ci plus root certificate key type       */
#define TZ_CIPLUS_KEY_BRAND_CERT         ((CI_KEY_TYPE_T) 0x01)/**< indicates ci plus brand certificate key type     */
#define TZ_CIPLUS_KEY_DEVICE_CERT        ((CI_KEY_TYPE_T) 0x02)/**<indicates ci plus device certificate key type    */
#define TZ_CIPLUS_KEY_PRNG_KEY           ((CI_KEY_TYPE_T) 0x03)/**< indicates ci plus prng seed  key type    */
#define TZ_CIPLUS_KEY_DH_P               ((CI_KEY_TYPE_T) 0x04)/**< indicates ci plus DH_P key type       */
#define TZ_CIPLUS_KEY_DH_G               ((CI_KEY_TYPE_T) 0x05)/**< indicates ci plus DH_G key type       */
#define TZ_CIPLUS_KEY_DH_Q               ((CI_KEY_TYPE_T) 0x06)/**< indicates ci plus DH_Qkey type       */
#define TZ_CIPLUS_KEY_HDQ                ((CI_KEY_TYPE_T) 0x07)/**< indicates ci plus HDQ key type       */
#define TZ_CIPLUS_KEY_SIV                ((CI_KEY_TYPE_T) 0x08)/**< indicates ci plus SIV key type       */
#define TZ_CIPLUS_KEY_CLK                ((CI_KEY_TYPE_T) 0x09)/**< indicates ci plus CLK key type       */
#define TZ_CIPLUS_KEY_SLK                ((CI_KEY_TYPE_T) 0x0A)/**< indicates ci plus SLK key type       */

#define TZ_SIZE_CIPLUS_KEY_PRNG_SEED     ((UINT16) 16)/**< define ci plus PRNG SEED key size      */
#define TZ_SIZE_CIPLUS_KEY_PRNG_KEY      ((UINT16) 16)/**< define ci plus PRNG key size      */
#define TZ_SIZE_CIPLUS_KEY_DH_P          ((UINT16) 256)/**< define ci plus DH_P key size       */
#define TZ_SIZE_CIPLUS_KEY_DH_G          ((UINT16) 256)/**< define ci plus DH_G key size       */
#define TZ_SIZE_CIPLUS_KEY_DH_Q          ((UINT16) 32)/**< define ci plus DH_QP key size       */
#define TZ_SIZE_CIPLUS_KEY_SIV           ((UINT16) 16)/**< define ci plus SIV key size       */
#define TZ_SIZE_CIPLUS_KEY_CLK           ((UINT16) 16)/**< define ci plus CLK key size       */
#define TZ_SIZE_CIPLUS_KEY_SLK           ((UINT16) 16)/**< define ci plus SLK key size       */



#define TZ_CIR_OK                      ((INT32)    0)    /**< The routine is successful     */
#define TZ_CIR_FAILED                  ((INT32)    -1)    /**<The routine is failed         */
#define TZ_CIR_INV_ARG                 ((INT32)    -2)    /**< Indicates at least one argument is invalid. */
#define TZ_CIR_INV_GET_TYPE            ((INT32)    -3)    /**<  */
#define TZ_CIR_INV_GET_INFO            ((INT32)    -4)    /**< pv_get_info value is invalid in c_ci_get()      */
#define TZ_CIR_INV_GET_DATA            ((INT32)    -5)    /**<  pv_data value is invalid in c_ci_get()      */
#define TZ_CIR_INV_SET_TYPE            ((INT32)    -6)    /**< e_set_type value is invalid in c_ci_set()       */
#define TZ_CIR_INV_SET_INFO            ((INT32)    -7)    /**<  pv_set_info value is invalid in c_ci_set()      */
#define TZ_CIR_INV_SET_DATA            ((INT32)    -8)    /**<   pv_data value is invalid in c_ci_set()          */
#define TZ_CIR_NOT_ENOUGH_SPACE        ((INT32)    -9)    /**<There is not enough memory to complete the operation.        */

#define TZ_CIPLUS_AKH_CONTEXT_SIZE      ((UINT16)1537)


#define TZ_CIPLUS_KEY_SUPPORT_SERIAL_NUMBER

enum
{
   DT_SIZE_BRAND_ID             = 400,
   DT_SIZE_HOST_ID              = 64,
   DT_SIZE_CICAM_ID             = 64,
   DT_SIZE_KP                   = 256,
   DT_SIZE_DHPH                 = 2048,
   DT_SIZE_DHPM                 = 2048,
   DT_SIZE_SIGNATURE_A          = 2048,
   DT_SIZE_SIGNATURE_B          = 2048,
   DT_SIZE_AUTH_NONCE           = 256,
   DT_SIZE_NS_HOST              = 64,
   DT_SIZE_NS_MODULE            = 64,
   DT_SIZE_AKH                  = 256,
   DT_SIZE_AKM                  = 256,
   DT_SIZE_URI_MESSAGE          = 64,
   DT_SIZE_PROGRAM_NUMBER       = 16,
   DT_SIZE_URI_CONFIRM          = 256,
   DT_SIZE_KEY_REGISTER         = 8,
   DT_SIZE_URI_VERSIONS         = 256,
   DT_SIZE_STATUS_FIELD         = 8,
   DT_SIZE_SRM_CONFIRM          = 256,
   DT_SIZE_LICENSE_STATUS       = 8,
   DT_SIZE_LICENSE_RCVD_STATUS  = 8,
   DT_SIZE_PLAY_COUNT           = 8,
   DT_SIZE_OPERATING_MODE       = 8,
   DT_SIZE_RECORD_START_STATUS  = 8,
   DT_SIZE_MODE_CHANGE_STATUS   = 8,
   DT_SIZE_RECORD_STOP_STATUS   = 8,

   DT_SIZE_DH_P           = 2048,
   DT_SIZE_DH_G           = 2048,
   DT_SIZE_DH_Q           = 256,
   DT_SIZE_DHX            = 2048,
   DT_SIZE_DHSK           = 2048,
   DT_SIZE_DHSK_TRUNCATED = 128,
   DT_SIZE_KS             = 256,
   DT_SIZE_SAK            = 128,
   DT_SIZE_SEK            = 128,
   DT_SIZE_SIV            = 128,
   DT_SIZE_CIV            = 128,
   DT_SIZE_CCK            = 128
};

static char tz_ci_cust_key[TZ_CIPLUS_AES_128_CBC_KEY_SIZE+1] = "9f966becfdd211c1";
static char tz_ci_cust_iv[TZ_CIPLUS_AES_128_CBC_KEY_SIZE+1] = "c6e577b26a8500c7";

/* host ci plus credentials */
static UINT8 *pui1_tz_ciplus_root_cert = NULL;
static UINT16 ui2_tz_ciplus_root_cert_size = 0;

static UINT8 *pui1_tz_ciplus_brand_cert = NULL;
static UINT16 ui2_tz_ciplus_brand_cert_size = 0;

static UINT8 *pui1_tz_ciplus_dev_cert = NULL;
static UINT16 ui2_tz_ciplus_dev_cert_size = 0;

static UINT8 *pui1_tz_ciplus_hdq = NULL;
static UINT16 ui2_tz_ciplus_hdq_size = 0;

static UINT8 *pui1_tz_ciplus_dh_g = NULL;
static UINT16 ui2_tz_ciplus_dh_g_size = 0;

static UINT8 *pui1_tz_ciplus_dh_p = NULL;
static UINT16 ui2_tz_ciplus_dh_p_size = 0;

static UINT8 *pui1_tz_ciplus_dh_q = NULL;
static UINT16 ui2_tz_ciplus_dh_q_size = 0;

static UINT8 *pui1_tz_ciplus_prng_seed = NULL;
static UINT16 ui2_tz_ciplus_prng_seed_size = 0;

static UINT8 *pui1_tz_ciplus_prng_key = NULL;
static UINT16 ui2_tz_ciplus_prng_key_size = 0;

static UINT8 *pui1_tz_ciplus_siv = NULL;
static UINT16 ui2_tz_ciplus_siv_size = 0;

static UINT8 *pui1_tz_ciplus_clk = NULL;
static UINT16 ui2_tz_ciplus_clk_size = 0;

static UINT8 *pui1_tz_ciplus_slk = NULL;
static UINT16 ui2_tz_ciplus_slk_size = 0;


UINT8*   pui1_tz_serial_buffer = NULL;
UINT16   ui2_tz_serial_length = 0;


static UINT8 ui1_clear_akh[TZ_CIPLUS_AKH_CONTEXT_SIZE];

/*use for calculate Prng Seed*/
static hash_state t_ciplus_hash_state;
static UINT8 ui1_ciplus_seed[128]={
0xA1, 0xC2, 0x86, 0x84, 0x75, 0x66, 0xF9, 0x01, 0xC8, 0x31, 0xA4, 0x4E, 0xE8, 0xC4, 0xD3, 0x7C, 
0xD5, 0xE8, 0x6D, 0x1A, 0x79, 0xF7, 0xCF, 0xFC, 0xE1, 0x05, 0x86, 0xA8, 0x32, 0x1E, 0xAD, 0x2D, 
0x31, 0x13, 0xE4, 0xE3, 0x20, 0xEF, 0xB6, 0x89, 0x2A, 0xFB, 0x32, 0x41, 0xDB, 0x4D, 0x26, 0x03, 
0x51, 0x9A, 0xBB, 0xF7, 0x2B, 0x44, 0x89, 0x77, 0x24, 0x2E, 0x17, 0x04, 0x4F, 0x0F, 0xC5, 0xD2, /*replace this line with secure id*/ 
0x8E, 0xD0, 0x35, 0x54, 0x60, 0xB1, 0x86, 0xD4, 0x0A, 0xA1, 0x69, 0x32, 0xA3, 0x0C, 0x59, 0xDC, 
0x5A, 0xEE, 0xFD, 0xEC, 0x3A, 0xD7, 0x9B, 0x2B, 0x7E, 0x69, 0xE0, 0x8B, 0xF2, 0xAF, 0x8C, 0x0C, 
0xFE, 0xC3, 0x53, 0xB2, 0xE6, 0x42, 0xF6, 0x4E, 0x63, 0xE0, 0x48, 0xDF, 0xE5, 0xB7, 0xE5, 0x6E, 
0xED, 0x24, 0x94, 0x7E, 0x34, 0x9B, 0xBB, 0xB3, 0x4B, 0xA2, 0xC6, 0x71, 0x20, 0x91, 0xC1, 0xAF};
static UINT8 ui1_ciplus_md[TZ_CIPLUS_SHA512_DIGEST_LENGTH];


#if 1
/*! Calculates digest value of SHA-1
 *
 * @param data      [input]  The pointer of the calculation domain of the digest value is specified with data.
 * @param data_size [input]  The size of data is specified with data_size.
 * @param digest    [output] The area where the calculation result of the digest value is stored is specified with digest.
 *
 * @return none.
 */
void TZ_ciplus_sha1_digest(const unsigned char* data, const size_t data_size, unsigned char* digest)
{
  hash_state context;
  INT32 i4_ret = 0;
  //! The SHA-1 function is initialized.
  sha1_init(&context);

  //! The digest is calculated.
  i4_ret = sha1_process(&context, data, data_size);
  if(i4_ret!=CRYPT_OK)
  {
     dprintf("sha1_process failed");
  }
  //! The calculation result of the digest is stored.
  i4_ret = sha1_done(&context, digest);
  if(i4_ret!=CRYPT_OK)
  {
     dprintf("sha1_done failed");
  }

  return;
}
BOOL TZ_ciplus_aes_cbc_decrypt( const unsigned char* key, 
                                      size_t keylen,
                                      const unsigned char* iv,
                                      const unsigned char* in, 
                                      unsigned char*out, 
                                      const size_t len)
{
    symmetric_CBC cbc;
    INT32 cipher_idx = -1;
    UINT8 ctsbypes = len % TZ_CIPLUS_AES_BLOCK_SIZE; // Cipher Text Stealing


    if(len < TZ_CIPLUS_AES_BLOCK_SIZE)
    {
        return FALSE;
    }

    if ((cipher_idx = find_cipher("aes")) == -1) 
    {
       if ((cipher_idx = find_cipher("rijndael")) == -1) 
       {
           return FALSE;
       }
    }

    if(ctsbypes!=0)
    {
       return FALSE;
    }        
    if (cbc_start(cipher_idx, iv , key, keylen, 0 , &cbc) != CRYPT_OK)
    {
        return FALSE;
    }
    if (cbc_decrypt(in  , out,  len, &cbc)  != CRYPT_OK)
    {
        return FALSE;
    }

    if (cbc_done(&cbc) != CRYPT_OK)
    {
        return FALSE;
    }
    /*reasonable , need cut padding here, else ,cut in TZ_ciplus_extract_cred,according real data length*/

    return TRUE;
  
}
BOOL TZ_ciplus_GetSecureID(UINT8* pu1Buf, UINT32 u4Size)
{
    UINT8 i=0;
    UINT32 *pu4SID = (UINT32 *)pu1Buf;

    BIM_WRITE32(REG_EFUSE_KEY, 0x883);
    BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

    pu4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
    pu4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
    pu4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
    pu4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

    /* Compute SHA-512 */
    for (i=0; i<4; i++)
    {
        ui1_ciplus_seed[32*i] = (BYTE)(pu4SID[i]&0xFF);
        ui1_ciplus_seed[32*i+8] = (BYTE)((pu4SID[i]&0xFF00)>>8);
        ui1_ciplus_seed[32*i+16] = (BYTE)((pu4SID[i]&0xFF0000)>>16);
        ui1_ciplus_seed[32*i+24] = (BYTE)((pu4SID[i]&0xFF000000)>>24);
    }
    sha512_init(&t_ciplus_hash_state);
    sha512_process(&t_ciplus_hash_state, ui1_ciplus_seed, 128);
    sha512_done(&t_ciplus_hash_state, ui1_ciplus_md);

    /* Select 16 bytes as SecureID*/
    for (i=0; i<4; i++)
    {
    pu4SID[i] = (ui1_ciplus_md[i*16+12]<<24)|(ui1_ciplus_md[i*16+8]<<16)
        |(ui1_ciplus_md[i*16+4]<<8)|(ui1_ciplus_md[i*16]);
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Name: TZ_ciplus_set_cred
 *
 * Description: This function is called by TZ_ciplus_extract_cred fill the CI plus credentials.
 *
 * Inputs:  
 *          type        Contains the request key type.
 *          pui1_data   Contains pointer of input data.
 *          ui2_data_len    Contains length of input data in byte.
 * Returns: 
 *         
 ----------------------------------------------------------------------------*/

INT32 TZ_ciplus_set_cred (E_TZ_STB_CIPLUS_KEY_TYPE e_type, UINT8 *pui1_data, UINT16 ui2_data_len)
{
   INT32 i4_ret = TZ_CIR_FAILED;
   UINT32 i = 0;
    switch (e_type)
    {
        case TZ_STB_CIPLUS_KEY_ROOT_CERT:
        {
            pui1_tz_ciplus_root_cert = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_root_cert != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_root_cert, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_root_cert_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_root_cert = NULL;
                ui2_tz_ciplus_root_cert_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;
        
        case TZ_STB_CIPLUS_KEY_BRAND_CERT:
        {
            pui1_tz_ciplus_brand_cert = (UINT8*)malloc(ui2_data_len); 
            if (pui1_tz_ciplus_brand_cert != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_brand_cert, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_brand_cert_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_brand_cert = NULL;
                ui2_tz_ciplus_brand_cert_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE; 
            }             
        }
        break;
        
        case TZ_STB_CIPLUS_KEY_DEVICE_CERT:
        {
            pui1_tz_ciplus_dev_cert = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_dev_cert != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_dev_cert, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_dev_cert_size = ui2_data_len;       
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_dev_cert = NULL;
                ui2_tz_ciplus_dev_cert_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;

        case TZ_STB_CIPLUS_KEY_PRNG_SEED:
        { 
            UINT32 ui4_data_len = TZ_SIZE_CIPLUS_KEY_PRNG_SEED;

            i4_ret = TZ_CIR_FAILED;
            pui1_tz_ciplus_prng_seed = (UINT8*)malloc(TZ_SIZE_CIPLUS_KEY_PRNG_SEED);
            if (pui1_tz_ciplus_prng_seed != NULL)
            {
                /* set unique device constant as the random seed */
                if(TZ_ciplus_GetSecureID(pui1_tz_ciplus_prng_seed,ui4_data_len))
                {
                    ui2_tz_ciplus_prng_seed_size = TZ_SIZE_CIPLUS_KEY_PRNG_SEED;                  
                    i4_ret = TZ_CIR_OK;
                }
                else
                {
                    free(pui1_tz_ciplus_prng_seed);
                    i4_ret = TZ_CIR_FAILED;
                }
            }
            if (i4_ret != TZ_CIR_OK)            
            {
                pui1_tz_ciplus_prng_seed = NULL;
                ui2_tz_ciplus_prng_seed_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;

        case TZ_STB_CIPLUS_KEY_PRNG_KEY_K:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_PRNG_KEY)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }
            
            pui1_tz_ciplus_prng_key = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_prng_key != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_prng_key, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_prng_key_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_prng_key = NULL;
                ui2_tz_ciplus_prng_key_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;
       
        case TZ_STB_CIPLUS_KEY_DH_P:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_DH_P)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }
            pui1_tz_ciplus_dh_p = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_dh_p != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_dh_p, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_dh_p_size = ui2_data_len;      
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_dh_p = NULL;
                ui2_tz_ciplus_dh_p_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
          	}
        }
        break;        
        
        case TZ_STB_CIPLUS_KEY_DH_G:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_DH_G)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }
            pui1_tz_ciplus_dh_g = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_dh_g != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_dh_g, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_dh_g_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_dh_g = NULL;
                ui2_tz_ciplus_dh_g_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;
        
        case TZ_STB_CIPLUS_KEY_DH_Q:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_DH_Q)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }
            pui1_tz_ciplus_dh_q = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_dh_q != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_dh_q, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_dh_q_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_dh_q = NULL;
                ui2_tz_ciplus_dh_q_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;
        
        case TZ_STB_CIPLUS_KEY_HDQ:
        {
            pui1_tz_ciplus_hdq = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_hdq != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_hdq, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_hdq_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_hdq = NULL;
                ui2_tz_ciplus_hdq_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;        
               
        case TZ_STB_CIPLUS_KEY_SIV:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_SIV)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }            
            pui1_tz_ciplus_siv = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_siv != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_siv, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_siv_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_siv = NULL;
                ui2_tz_ciplus_siv_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;

        case TZ_STB_CIPLUS_KEY_CLK:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_CLK)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }            
            pui1_tz_ciplus_clk = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_clk != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_clk, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_clk_size = ui2_data_len;

                dprintf( "tz*******CLK\n" );
                for (i = 0; i < ui2_tz_ciplus_clk_size; i++)
               {
                   if (0 == i%10)
                   {
                       dprintf( "\n" );
                   }
   
                   if (pui1_tz_ciplus_clk[i] < 0x10)
                   {
                       dprintf( " 0%x", pui1_tz_ciplus_clk[i] );
                   }
                   else
                   {
                       dprintf( " %x", pui1_tz_ciplus_clk[i] );
                   }
               }
   
               dprintf( "\n" );
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_clk = NULL;
                ui2_tz_ciplus_clk_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;

        case TZ_STB_CIPLUS_KEY_SLK:
        {
            if(ui2_data_len != TZ_SIZE_CIPLUS_KEY_SLK)
            {
                i4_ret = TZ_CIR_INV_ARG;
                return i4_ret;
            }            
            pui1_tz_ciplus_slk = (UINT8*)malloc(ui2_data_len);
            if (pui1_tz_ciplus_slk != NULL)
            {
                memcpy_s2s_chk(pui1_tz_ciplus_slk, pui1_data, ui2_data_len,NULL);
                ui2_tz_ciplus_slk_size = ui2_data_len;
                i4_ret = TZ_CIR_OK;
            }
            else
            {
                pui1_tz_ciplus_slk = NULL;
                ui2_tz_ciplus_slk_size = 0;
                i4_ret = TZ_CIR_NOT_ENOUGH_SPACE;
            }
        }
        break;        
    
        default:
            /* It should not reach here. */
            dprintf("Invalid set type of credential\n");
            i4_ret = TZ_CIR_INV_ARG;
        break;
    }
    return i4_ret;
}

BOOL TZ_ciplus_extract_cred(UINT8 *pui1_raw_cred, UINT32 ui2_raw_cred_len,UINT8 *pu1CustCode, UINT32 u4CustCodeLen)
{
    INT32    i4_ret = 0;
    BOOL     b_ret = TRUE;
    UINT16   ui2_offset = 0;
    UINT8    ui1_tag = 0;
    UINT16   ui2_item_len = 0;
    UINT8    *ui1_data_from = NULL;
    UINT8    *ui1_temp_raw_cred = NULL;
    UINT16   ui2_cur_pos = 0;
    BOOL     b_valid_header = FALSE;

    UINT16   ui2_real_data_len = 0;
 
    dprintf("TZ_ciplus_extract_cred---start\n");
    
    /* the minimum data length should larger the ( header offset + hash length )  */
    if ((pui1_raw_cred == NULL) ||
        (ui2_raw_cred_len < (TZ_CIPLUS_RAW_CRED_HDR_OFFSET+TZ_CIPLUS_SHA1_DIGEST_LENGTH)))
    {
        dprintf("TZ_ciplus_extract_cred---invalid parameters\n");
        return FALSE;
    }
    
    dprintf("TZ_ciplus_extract_cred---ui2_raw_cred_len %d\n",ui2_raw_cred_len);
    /*cut the padding, according to the ciplus packing tools*/
    ui2_real_data_len = pui1_raw_cred[2]<<8;
    ui2_real_data_len = (ui2_real_data_len&0xFF00)|(pui1_raw_cred[3]&0x00FF)+TZ_CIPLUS_RAW_CRED_HDR_OFFSET;
   
    dprintf("TZ_ciplus_extract_cred---ui2_real_data_len %d\n",ui2_real_data_len);
    ui1_temp_raw_cred = pui1_raw_cred;
    ui2_raw_cred_len = ui2_real_data_len;

    /* check header and digest validity */
    if (ui1_temp_raw_cred[0] == 0x00)
    {
        if (ui1_temp_raw_cred[1] == 0x00)
        {
            UINT8  *pui1_hash_data_from = NULL;
            UINT16  ui2_hash_data_len;
            UINT8   aui1_digest[TZ_CIPLUS_SHA1_DIGEST_LENGTH];
        
            UINT32 i = 0;
            /* SHA1 */      
            pui1_hash_data_from = ui1_temp_raw_cred + TZ_CIPLUS_RAW_CRED_HDR_OFFSET;
            ui2_hash_data_len = ui2_raw_cred_len - TZ_CIPLUS_SHA1_DIGEST_LENGTH - TZ_CIPLUS_RAW_CRED_HDR_OFFSET;

            
            for (i = 0; i < TZ_CIPLUS_SHA1_DIGEST_LENGTH; i++)
            {
                if (0 == i%10)
                {
                    dprintf( "\n" );
                }
     
                if ( ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] < 0x10)
                {
                    dprintf( " 0%x", ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] );
                }
                else
                {
                    dprintf( " %x",  ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] );
                }
            }
            
            TZ_ciplus_sha1_digest(pui1_hash_data_from, ui2_hash_data_len, aui1_digest);
            if (memcmp(ui1_temp_raw_cred+(ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH), aui1_digest, TZ_CIPLUS_SHA1_DIGEST_LENGTH)!= 0)
            {
               dprintf( "TZ_ciplus_extract_cred prwint aui1_digest \n" );
               for (i = 0; i < TZ_CIPLUS_SHA1_DIGEST_LENGTH; i++)
               {
                   if (0 == i%10)
                   {
                       dprintf( "\n" );
                   }
   
                   if (aui1_digest[i] < 0x10)
                   {
                       dprintf( " 0%x", aui1_digest[i] );
                   }
                   else
                   {
                       dprintf( " %x", aui1_digest[i] );
                   }
               }
                
                dprintf("TZ_ciplus_extract_cred---TZ_ciplus_sha1_digest failed\n");
                return FALSE;
            }
            else
            {
                dprintf("TZ_ciplus_extract_cred valid headers\n");
                b_valid_header = TRUE;
                #if 0
                b_valid_header = FALSE;
                if (t_ciplus_config.b_custom_code == TRUE)//may be can passed from normal word
                {
                    if(t_ciplus_config.pc_custom_code != NULL)
                    {
                        /* check the custome code inside raw data */
                        if (x_memcmp(pui1_hash_data_from, t_ciplus_config.pc_custom_code, t_ciplus_config.ui1_len_code) == 0)
                        {
                            b_valid_header = TRUE;
                            i4_ret = CIR_OK;
                        }
                    }
                }
                else
                {
                    if (t_ciplus_config.ui1_len_code == 0)
                    {
                        b_valid_header = TRUE;
                        i4_ret = CIR_OK;
                    }
                }
                #endif
            }        
        }
        else
        {
            dprintf("TZ_ciplus_extract_cred ui1_temp_raw_cred[1] != 0x00\n");
        }
    }
    else
    {
        dprintf("TZ_ciplus_extract_cred ui1_temp_raw_cred[0] != 0x00\n");
    }
    
    if (b_valid_header!= TRUE)
    {
        return FALSE;
    }

    /* judge tag data validity. */
    //ui2_offset = TZ_CIPLUS_RAW_CRED_HDR_OFFSET;+t_ciplus_config.ui1_len_code; //fix len code, or can be passed from normal word 
    ui2_offset = TZ_CIPLUS_RAW_CRED_HDR_OFFSET+u4CustCodeLen; //fix len code, or can be passed from normal word 

    ui1_temp_raw_cred = ui1_temp_raw_cred + ui2_offset;
    ui2_cur_pos = ui2_offset;
        
    while (ui2_cur_pos < (ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH))
    {
        ui1_tag = ui1_temp_raw_cred[0];
        
        dprintf("TZ_ciplus_extract_cred ui1_temp_raw_cred tag %d\n",ui1_tag);
        ui2_item_len = *(ui1_temp_raw_cred + TZ_CIPLUS_RAW_CRED_TAG_SIZE);
        ui2_item_len = ((ui2_item_len<<8)&0xFF00)|((*(ui1_temp_raw_cred + TZ_CIPLUS_RAW_CRED_TAG_SIZE +1))&0x00FF);
        ui2_cur_pos = ui2_cur_pos + TZ_CIPLUS_RAW_CRED_TAG_SIZE + TZ_CIPLUS_RAW_CRED_ITEM_LEN_SIZE;

        
        dprintf("TZ_ciplus_extract_cred ui1_temp_raw_cred ui2_item_len %d\n",ui2_item_len);
        dprintf("TZ_ciplus_extract_cred ui1_temp_raw_cred ui2_cur_pos %d\n",ui2_cur_pos);
        
        /* check item validity */
        if (ui2_item_len > (ui2_raw_cred_len - ui2_cur_pos + 1))
        {
           dprintf("Invalid item length...\n");
           b_ret = FALSE;
           break;
        }
        
        ui1_data_from = ui1_temp_raw_cred + TZ_CIPLUS_RAW_CRED_TAG_SIZE + TZ_CIPLUS_RAW_CRED_ITEM_LEN_SIZE;        
        /* fill items */
        if (TZ_CIPLUS_KEY_ROOT_CERT == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_ROOT_CERT, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
               dprintf("Invalid ROOT_CERT\n");
               b_ret = FALSE;
               break;
            }
        }
        else if (TZ_CIPLUS_KEY_BRAND_CERT == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_BRAND_CERT, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid BRAND_CERT\n");
                b_ret = FALSE;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_DEVICE_CERT == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_DEVICE_CERT, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid DEVICE_CERT\n");
                i4_ret = TZ_CIR_INV_ARG;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_PRNG_KEY == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_PRNG_KEY_K, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid PRNG_KEY\n");
                b_ret = FALSE;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_DH_P == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_DH_P, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid DH_P\n");
                b_ret = FALSE;
                break;
            }    	       
        }    	   
        else if (TZ_CIPLUS_KEY_DH_G == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_DH_G, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid HDQ\n");
                b_ret = FALSE;
                break;
            }    	       
        }        
        else if (TZ_CIPLUS_KEY_DH_Q == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_DH_Q, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid DH_Q\n");
                b_ret = FALSE;
                break;
            }    	       
        }
        else if (TZ_CIPLUS_KEY_HDQ == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_HDQ, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid HDQ\n");
                b_ret = FALSE;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_SIV == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_SIV, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid SIV\n");
                b_ret = FALSE;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_CLK == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_CLK, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid CLK\n");
                b_ret = FALSE;
                break;
            }
        }
        else if (TZ_CIPLUS_KEY_SLK == ui1_tag)
        {
            i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_SLK, ui1_data_from, ui2_item_len);
            if (i4_ret != TZ_CIR_OK)
            {
                dprintf("Invalid SLK\n");
                b_ret = FALSE;
                break;
            }
        }
        else
        {
            dprintf("Invalid key\n");
            b_ret = FALSE;
            break;
        }
        ui2_cur_pos = ui2_cur_pos + ui2_item_len;
        ui2_offset = TZ_CIPLUS_RAW_CRED_TAG_SIZE + TZ_CIPLUS_RAW_CRED_ITEM_LEN_SIZE + ui2_item_len;
        ui1_temp_raw_cred = ui1_temp_raw_cred + ui2_offset;
    }
    
    i4_ret = TZ_ciplus_set_cred (TZ_STB_CIPLUS_KEY_PRNG_SEED, NULL, 0);
    if (i4_ret != TZ_CIR_OK)
    {
        dprintf("Invalid PRNG_SEED\n");
        b_ret = FALSE;
    }

    if (i4_ret != TZ_CIR_OK)
    {  
        /* free all key buffer */
        if (pui1_tz_ciplus_root_cert != NULL)
        {
            free(pui1_tz_ciplus_root_cert);
            pui1_tz_ciplus_root_cert = NULL;
            ui2_tz_ciplus_root_cert_size = 0;
        }
        if (pui1_tz_ciplus_brand_cert != NULL)
        {
            free(pui1_tz_ciplus_brand_cert);
            pui1_tz_ciplus_brand_cert = NULL;
            ui2_tz_ciplus_brand_cert_size = 0;
        }
        if (pui1_tz_ciplus_dev_cert != NULL)
        {
            free(pui1_tz_ciplus_dev_cert);
            pui1_tz_ciplus_dev_cert = NULL;
            ui2_tz_ciplus_dev_cert_size = 0;
        }
        if (pui1_tz_ciplus_prng_seed != NULL)
        {
            free(pui1_tz_ciplus_prng_seed);
            pui1_tz_ciplus_prng_seed = NULL;
            ui2_tz_ciplus_prng_seed_size = 0;
        }
        if (pui1_tz_ciplus_prng_key != NULL)
        {
            free(pui1_tz_ciplus_prng_key);
            pui1_tz_ciplus_prng_key = NULL;
            ui2_tz_ciplus_prng_key_size = 0;
        }
        if (pui1_tz_ciplus_dh_g != NULL)
        {
            free(pui1_tz_ciplus_dh_g);
            pui1_tz_ciplus_dh_g = NULL;
            ui2_tz_ciplus_dh_g_size = 0;
        }
        if (pui1_tz_ciplus_dh_p != NULL)
        {
            x_mem_free(pui1_tz_ciplus_dh_p);
            pui1_tz_ciplus_dh_p = NULL;
            ui2_tz_ciplus_dh_p_size = 0;
        }
        if (pui1_tz_ciplus_dh_q != NULL)
        {
            free(pui1_tz_ciplus_dh_q);
            pui1_tz_ciplus_dh_q = NULL;
            ui2_tz_ciplus_dh_q_size = 0;
        }
        if (pui1_tz_ciplus_hdq != NULL)
        {
            free(pui1_tz_ciplus_hdq);
            pui1_tz_ciplus_hdq = NULL;
            ui2_tz_ciplus_hdq_size = 0;
        }
        if (pui1_tz_ciplus_siv != NULL)
        {
            free(pui1_tz_ciplus_siv);
            pui1_tz_ciplus_siv = NULL;
            ui2_tz_ciplus_siv_size = 0;
        }
        if (pui1_tz_ciplus_clk != NULL)
        {
            free(pui1_tz_ciplus_clk);
            pui1_tz_ciplus_clk = NULL;
            ui2_tz_ciplus_clk_size = 0;
        }
        if (pui1_tz_ciplus_slk != NULL)
        {
            free(pui1_tz_ciplus_slk);
            pui1_tz_ciplus_slk = NULL;
            ui2_tz_ciplus_slk_size = 0;
        }
        return b_ret;
    }
    
    return b_ret;
}
BOOL TZ_ciplus_check_cred(UINT8 *pui1_raw_cred, UINT32 ui2_raw_cred_len,UINT8 *pu1CustCode, UINT32 u4CustCodeLen,BOOL *pbvalid)
{
    INT32    i4_ret = 0;
    BOOL     b_ret = TRUE;
    UINT16   ui2_offset = 0;
    UINT8    *ui1_temp_raw_cred = NULL;
    BOOL     b_valid_header = FALSE;

    UINT16   ui2_real_data_len = 0;
    if((pui1_raw_cred == NULL)||
       (pu1CustCode == NULL)||
       (pbvalid == NULL))
    {
        dprintf("TZ_ciplus_check_cred invalid parameters\n");
        return FALSE;
    }
    *pbvalid = FALSE;
    dprintf("TZ_ciplus_check_cred---start\n");
    
    /* the minimum data length should larger the ( header offset + hash length )  */
    if ((pui1_raw_cred == NULL) ||
        (ui2_raw_cred_len < (TZ_CIPLUS_RAW_CRED_HDR_OFFSET+TZ_CIPLUS_SHA1_DIGEST_LENGTH)))
    {
        dprintf("TZ_ciplus_check_cred---invalid parameters\n");
        return FALSE;
    }
    
    dprintf("TZ_ciplus_check_cred---ui2_raw_cred_len %d\n",ui2_raw_cred_len);
    /*cut the padding, according to the ciplus packing tools*/
    ui2_real_data_len = pui1_raw_cred[2]<<8;
    ui2_real_data_len = (ui2_real_data_len&0xFF00)|(pui1_raw_cred[3]&0x00FF)+TZ_CIPLUS_RAW_CRED_HDR_OFFSET;
   
    dprintf("TZ_ciplus_check_cred---ui2_real_data_len %d\n",ui2_real_data_len);
    ui1_temp_raw_cred = pui1_raw_cred;
    ui2_raw_cred_len = ui2_real_data_len;

    /* check header and digest validity */
    if (ui1_temp_raw_cred[0] == 0x00)
    {
        if (ui1_temp_raw_cred[1] == 0x00)
        {
            UINT8  *pui1_hash_data_from = NULL;
            UINT16  ui2_hash_data_len;
	        UINT8   aui1_digest[TZ_CIPLUS_SHA1_DIGEST_LENGTH];
	        
            UINT32 i = 0;
            /* SHA1 */      
            pui1_hash_data_from = ui1_temp_raw_cred + TZ_CIPLUS_RAW_CRED_HDR_OFFSET;
            ui2_hash_data_len = ui2_raw_cred_len - TZ_CIPLUS_SHA1_DIGEST_LENGTH - TZ_CIPLUS_RAW_CRED_HDR_OFFSET;

            
            for (i = 0; i < TZ_CIPLUS_SHA1_DIGEST_LENGTH; i++)
                   {
                       if (0 == i%10)
                       {
                           dprintf( "\n" );
                       }
            
                       if ( ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] < 0x10)
                       {
                           dprintf( " 0%x", ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] );
                       }
                       else
                       {
                           dprintf( " %x",  ui1_temp_raw_cred[ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH+i] );
                       }
                   }
            
            TZ_ciplus_sha1_digest(pui1_hash_data_from, ui2_hash_data_len, aui1_digest);
            if (memcmp(ui1_temp_raw_cred+(ui2_raw_cred_len-TZ_CIPLUS_SHA1_DIGEST_LENGTH), aui1_digest, TZ_CIPLUS_SHA1_DIGEST_LENGTH)!= 0)
            {
               dprintf( "TZ_ciplus_check_cred prwint aui1_digest \n" );
               for (i = 0; i < TZ_CIPLUS_SHA1_DIGEST_LENGTH; i++)
               {
                   if (0 == i%10)
                   {
                       dprintf( "\n" );
                   }
   
                   if (aui1_digest[i] < 0x10)
                   {
                       dprintf( " 0%x", aui1_digest[i] );
                   }
                   else
                   {
                       dprintf( " %x", aui1_digest[i] );
                   }
               }
                
                dprintf("TZ_ciplus_check_cred---TZ_ciplus_sha1_digest failed\n");
                return FALSE;
            }
            else
            {
                dprintf("TZ_ciplus_check_cred valid headers\n");
                b_valid_header = TRUE;
            }        
        }
        else
        {
            dprintf("TZ_ciplus_check_cred ui1_temp_raw_cred[1] != 0x00\n");
        }
    }
    else
    {
        dprintf("TZ_ciplus_check_cred ui1_temp_raw_cred[0] != 0x00\n");
    }
    
    if (b_valid_header!= TRUE)
    {
        return FALSE;
    }

    /* judge tag data validity. */
    //ui2_offset = TZ_CIPLUS_RAW_CRED_HDR_OFFSET;+t_ciplus_config.ui1_len_code; //fix len code, or can be passed from normal word 
    ui2_offset = TZ_CIPLUS_RAW_CRED_HDR_OFFSET; //fix len code, or can be passed from normal word 

    ui1_temp_raw_cred = ui1_temp_raw_cred + ui2_offset;
    if(memcmp(ui1_temp_raw_cred,pu1CustCode,u4CustCodeLen)==0)
    {
        *pbvalid = FALSE;
    }

       
    return b_ret;
}

/*-----------------------------------------------------------------------------
 * Name: TZ_ciplus_read_cred
 *
 * Description: This function is called in TZ, to get CI+ credential item data
 *
 * Inputs:  
 *           type:	Contains the request key type.
 *
 * Outputs: 
 *          key:         Contains pointer of data.
 *          length:      length of key In byte
 *
 * Returns: NONE
 ----------------------------------------------------------------------------*/
VOID TZ_ciplus_read_cred (E_TZ_STB_CIPLUS_KEY_TYPE type, UINT8 **key, UINT16 *length)
{
    switch(type)
    {
        case TZ_STB_CIPLUS_KEY_ROOT_CERT:
        {
            *key = pui1_tz_ciplus_root_cert;
            *length = ui2_tz_ciplus_root_cert_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_BRAND_CERT:
        {
            *key = pui1_tz_ciplus_brand_cert;
            *length = ui2_tz_ciplus_brand_cert_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_DEVICE_CERT:
        {
            *key = pui1_tz_ciplus_dev_cert;
            *length = ui2_tz_ciplus_dev_cert_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_PRNG_SEED:
        {
            *key = pui1_tz_ciplus_prng_seed;
            *length = ui2_tz_ciplus_prng_seed_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_PRNG_KEY_K:
        {
            *key = pui1_tz_ciplus_prng_key;
            *length = ui2_tz_ciplus_prng_key_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_DH_G:
        {
            *key = pui1_tz_ciplus_dh_g;
            *length = ui2_tz_ciplus_dh_g_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_DH_P:
        {
            *key = pui1_tz_ciplus_dh_p;
            *length = ui2_tz_ciplus_dh_p_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_DH_Q:
        {
            *key = pui1_tz_ciplus_dh_q;
            *length = ui2_tz_ciplus_dh_q_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_HDQ:
        {
            *key = pui1_tz_ciplus_hdq;
            *length = ui2_tz_ciplus_hdq_size;
        }
        break;
        case TZ_STB_CIPLUS_KEY_SIV:
        {
            *key = pui1_tz_ciplus_siv;
            *length = ui2_tz_ciplus_siv_size;
        }
        break;

        case TZ_STB_CIPLUS_KEY_CLK:
        {
            *key = pui1_tz_ciplus_clk;
            *length = ui2_tz_ciplus_clk_size;
        }
        break;

        case TZ_STB_CIPLUS_KEY_SLK:
        {
            *key = pui1_tz_ciplus_slk;
            *length = ui2_tz_ciplus_slk_size;
        }
        break;        
        
        default:
            /* It should not reach here. */
            dprintf("Invalid read type of credential\n");
        break;        
    }

    dprintf(*key, *length);
}



#endif

BOOL _teeCIPLUSExtractCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1CustCode, UINT32 u4CustCodeLen)
{
    INT32 i4_ret = 0;
    UINT8* pui1clear = NULL;    
    UINT8* pui1dst = NULL;
    
    UINT16    ui2_buffer_data_len;
    UINT8     *pui1_enc_src_data_from = NULL;   
    
    UINT8   aui1_digest[TZ_CIPLUS_SHA1_DIGEST_LENGTH];
    
    long i4ClearLen = u4SrcLen + TZ_CIPLUS_SS_EXT_SIZE;
    if((pu1SrcBuf == NULL)||
       (u4SrcLen  == 0)||
       (pu1CustCode == NULL)||
       (u4CustCodeLen == 0))
    {
        dprintf("_teeCIPLUSExtractCred invalid parameters\n");
        return FALSE;
    }
    pui1clear = (UINT8*)malloc(i4ClearLen);
    if(!pui1clear)
    {
        dprintf("_teeCIPLUSExtractCred---not enough memory\n");
        return FALSE;
    }
    dprintf("_teeCIPLUSExtractCred---start\n");
     
    /*get the ci +key(packing by mtk tools) afer securestorage descryption*/
    i4_ret = _teeSecureStorageSecretDec(pu1SrcBuf, 
                        (long)u4SrcLen,
                        pui1clear,
                        &i4ClearLen);
    if(i4_ret)
    {   
        free(pui1clear);
        return FALSE;
    }
    dprintf("_teeCIPLUSExtractCred after ss ,len is %d \n",i4ClearLen);
    /*get serial number */
    #ifdef TZ_CIPLUS_KEY_SUPPORT_SERIAL_NUMBER
    ui2_tz_serial_length =(pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1];
    i4ClearLen = i4ClearLen - ((pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1])-2; 
    if(ui2_tz_serial_length>0)
    {
        if(!pui1_tz_serial_buffer)
        {
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        else
        {
            free(pui1_tz_serial_buffer);
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        memcpy_s2s_chk(pui1_tz_serial_buffer,&(pui1clear[i4ClearLen]),ui2_tz_serial_length,NULL);
    }
    #endif
    
    ui2_buffer_data_len = (i4ClearLen-2) - TZ_CIPLUS_SHA1_DIGEST_LENGTH;
    pui1_enc_src_data_from = pui1clear+2;


    dprintf("_teeCIPLUSExtractCred ui2_buffer_data_len %d\n",ui2_buffer_data_len);

    TZ_ciplus_sha1_digest(pui1_enc_src_data_from, ui2_buffer_data_len, aui1_digest);
    if (memcmp(pui1clear+(i4ClearLen-TZ_CIPLUS_SHA1_DIGEST_LENGTH), aui1_digest, TZ_CIPLUS_SHA1_DIGEST_LENGTH)!= 0)
    {
        dprintf("_teeCIPLUSExtractCred TZ_ciplus_sha1_digest  failed \n");
        free(pui1clear);
        return FALSE;
    }  

    
    dprintf("_teeCIPLUSExtractCred after sha1 check \n");
    pui1dst = (UINT8*)malloc(ui2_buffer_data_len);
    
    dprintf("_teeCIPLUSExtractCred before aescbc length %d\n",ui2_buffer_data_len);
    if (!TZ_ciplus_aes_cbc_decrypt(tz_ci_cust_key,
                            TZ_CIPLUS_AES_128_CBC_KEY_SIZE,
                            tz_ci_cust_iv,
                            pui1_enc_src_data_from,
                            pui1dst,
                            ui2_buffer_data_len))
    {
        free(pui1clear);
        free(pui1dst);
        return FALSE;
    }
    
    free(pui1clear);
    
    /*input len must be real length, after cbc_descrypt, keep source len*/
    if (!TZ_ciplus_extract_cred(pui1dst,ui2_buffer_data_len,pu1CustCode,u4CustCodeLen))
    {
        free(pui1dst);
        
        dprintf("_teeCIPLUSExtractCred TZ_ciplus_extract_cred failed\n");
        return FALSE;
    }    
    free(pui1dst);
    
    dprintf("_teeCIPLUSExtractCred after TZ_ciplus_extract_cred\n");
    dprintf("_teeCIPLUSExtractCred---end\n");
    
    return TRUE;
}

BOOL _teeCILPLUSVerifyCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1Ref, UINT32 u4RefLen,BOOL *pbValid)
{
    INT32 i4_ret = 0;
    UINT8* pui1clear = NULL;    
    
    UINT8   aui1_digest[TZ_CIPLUS_SHA1_DIGEST_LENGTH];
    
    long i4ClearLen = u4SrcLen + TZ_CIPLUS_SS_EXT_SIZE;
    if((pu1SrcBuf == NULL)||
       (u4SrcLen  == 0)||
       (pu1Ref == NULL)||
       (u4RefLen == 0))
    {
        dprintf("_teeCILPLUSSetEncryAKH invalid parameters\n");
        return FALSE;
    }
       
    pui1clear = (UINT8*)malloc(i4ClearLen);
    if(!pui1clear)
    {
        dprintf("_teeCILPLUSVerifyCred---not enough memory\n");
        return FALSE;
    }
    dprintf("_teeCILPLUSVerifyCred---start\n");
   
    /*get the ci +key(packing by mtk tools) afer securestorage descryption*/
    i4_ret = _teeSecureStorageSecretDec(pu1SrcBuf, 
                        (long)u4SrcLen,
                        pui1clear,
                        &i4ClearLen);
    if(i4_ret)
    {   
        free(pui1clear);
        return FALSE;
    }
    dprintf("_teeCILPLUSVerifyCred after ss ,len is %d \n",i4ClearLen);
    /*get serial number */
    #ifdef TZ_CIPLUS_KEY_SUPPORT_SERIAL_NUMBER
    ui2_tz_serial_length =(pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1];
    i4ClearLen = i4ClearLen - ((pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1])-2; 
    if(ui2_tz_serial_length>0)
    {
        if(!pui1_tz_serial_buffer)
        {
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        else
        {
            free(pui1_tz_serial_buffer);
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        memcpy_s2s_chk(pui1_tz_serial_buffer,&(pui1clear[i4ClearLen]),ui2_tz_serial_length,NULL);
    }
    #endif
    
    dprintf("_teeCILPLUSVerifyCred after sha1 check \n");
    
    dprintf("_teeCILPLUSVerifyCred compare \n");
    
    if((u4RefLen == i4ClearLen)&&( 
        memcmp(pui1clear,pu1Ref,u4RefLen)==0
        ))
    {
        *pbValid = TRUE;
    }
    else
    {
        *pbValid = FALSE;
    }
    free(pui1clear);

    dprintf("_teeCILPLUSVerifyCred---end\n");
    
    return TRUE;
    
}
BOOL _teeCIPLUSCheckCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1CustCode, UINT32 u4CustCodeLen,BOOL *pbValid)
{
    INT32 i4_ret = 0;
    UINT8* pui1clear = NULL;    
    UINT8* pui1dst = NULL;
    
    UINT16    ui2_buffer_data_len;
    UINT8     *pui1_enc_src_data_from = NULL;   
    
    UINT8   aui1_digest[TZ_CIPLUS_SHA1_DIGEST_LENGTH];
    
    long i4ClearLen = u4SrcLen + TZ_CIPLUS_SS_EXT_SIZE;
    if((pu1SrcBuf == NULL)||
       (u4SrcLen  == 0)||
       (pu1CustCode == NULL)||
       (u4CustCodeLen == 0)||
       (pbValid == NULL))
    {
        dprintf("_teeCIPLUSCheckCred invalid parameters\n");
        return FALSE;
    }
    pui1clear = (UINT8*)malloc(i4ClearLen);
    if(!pui1clear)
    {
        dprintf("_teeCIPLUSCheckCred---not enough memory\n");
        return FALSE;
    }
    dprintf("_teeCIPLUSCheckCred---start\n");
     
    /*get the ci +key(packing by mtk tools) afer securestorage descryption*/
    i4_ret = _teeSecureStorageSecretDec(pu1SrcBuf, 
                        (long)u4SrcLen,
                        pui1clear,
                        &i4ClearLen);
    if(i4_ret)
    {   
        free(pui1clear);
        return FALSE;
    }
    dprintf("_teeCIPLUSExtractCred after ss ,len is %d \n",i4ClearLen);
    /*get serial number */
    #ifdef TZ_CIPLUS_KEY_SUPPORT_SERIAL_NUMBER
    ui2_tz_serial_length =(pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1];
    i4ClearLen = i4ClearLen - ((pui1clear[i4ClearLen-2]<<8)|pui1clear[i4ClearLen-1])-2; 
    if(ui2_tz_serial_length>0)
    {
        if(!pui1_tz_serial_buffer)
        {
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        else
        {
            free(pui1_tz_serial_buffer);
            pui1_tz_serial_buffer = (UINT8*)malloc(ui2_tz_serial_length);
        }
        memcpy_s2s_chk(pui1_tz_serial_buffer,&(pui1clear[i4ClearLen]),ui2_tz_serial_length,NULL);
    }
    #endif
    
    ui2_buffer_data_len = (i4ClearLen-2) - TZ_CIPLUS_SHA1_DIGEST_LENGTH;
    pui1_enc_src_data_from = pui1clear+2;

    TZ_ciplus_sha1_digest(pui1_enc_src_data_from, ui2_buffer_data_len, aui1_digest);
    if (memcmp(pui1clear+(i4ClearLen-TZ_CIPLUS_SHA1_DIGEST_LENGTH), aui1_digest, TZ_CIPLUS_SHA1_DIGEST_LENGTH)!= 0)
    {
        free(pui1clear);
        return FALSE;
    }  

    
    dprintf("_teeCIPLUSCheckCred after sha1 check \n");
    pui1dst = (UINT8*)malloc(ui2_buffer_data_len);
    
    dprintf("_teeCIPLUSCheckCred before aescbc length %d\n",ui2_buffer_data_len);
    if (!TZ_ciplus_aes_cbc_decrypt(tz_ci_cust_key,
                            TZ_CIPLUS_AES_128_CBC_KEY_SIZE,
                            tz_ci_cust_iv,
                            pui1_enc_src_data_from,
                            pui1dst,
                            ui2_buffer_data_len))
    {
        free(pui1clear);
        free(pui1dst);
        return FALSE;
    }
    
    free(pui1clear);
    
    /*input len must be real length, after cbc_descrypt, keep source len*/
    if (!TZ_ciplus_check_cred(pui1dst,ui2_buffer_data_len,pu1CustCode,u4CustCodeLen,pbValid))
    {
        free(pui1dst);
        
        dprintf("_teeCIPLUSCheckCred TZ_ciplus_check_cred failed\n");
        return FALSE;
    }    
    free(pui1dst);
    
    dprintf("_teeCIPLUSCheckCred---end\n");
    
    return TRUE;
}

BOOL _teeCIPLUSSetEncryAKH(UINT8 *pu1SrcBuf, UINT32 u4SrcLen)
{
    /*descry the AKH and cached in TZ*/
    INT32 i4_ret = 0;
    long l_clear_akh_len = TZ_CIPLUS_AKH_CONTEXT_SIZE;
    if((pu1SrcBuf == NULL)||
      (u4SrcLen  == 0))
    {
        dprintf("_teeCILPLUSSetEncryAKH invalid parameters\n");
        return FALSE;
    }

    dprintf("_teeCIPLUSSetEncryAKH---start\n");
    dprintf("_teeCIPLUSSetEncryAKH---start u4SrcLen %d\n",u4SrcLen);

    i4_ret = _teeSecureStorageSecretDec(pu1SrcBuf, 
                        (long)u4SrcLen,
                        ui1_clear_akh,
                        &l_clear_akh_len);
    if(i4_ret)
    {   
        dprintf("_teeCILPLUSSetEncryAKH---failed\n");
        return FALSE;
    }
    
    CC_Set_AuthContexts(ui1_clear_akh,l_clear_akh_len);
    dprintf("_teeCIPLUSSetEncryAKH---end\n");
    
    return TRUE;
}
BOOL _teeCIPLUSGetEncryAKH(UINT8 *pu1DstBuf, UINT32* pu4SrcLen)
{
    /*Encrypt cached AKH in TZ,then return encry data to normal word and storage in perm*/
    INT32 i4_ret = 0;
    UINT8* pui1_enc_data = NULL;    
    long l_enc_data_len = TZ_CIPLUS_AKH_CONTEXT_SIZE+TZ_CIPLUS_SS_EXT_SIZE;
    long l_src_data_len = TZ_CIPLUS_AKH_CONTEXT_SIZE;

    if(pu1DstBuf == NULL)
    {
        dprintf("_teeCILPLUSSetEncryAKH invalid parameters\n");
        return FALSE;
    }
    dprintf("_teeCLPLUSGetEncryAKH---start\n");
 
    pui1_enc_data = (UINT8*)malloc(l_enc_data_len);
    if(!pui1_enc_data)
    {
        dprintf("_teeCLPLUSGetEncryAKH---not enough memory\n");
        return FALSE;
    }
    CC_Get_AuthContexts(ui1_clear_akh,l_src_data_len);
    i4_ret = _teeSecureStorageSecretEnc(ui1_clear_akh, 
                        (long)l_src_data_len,
                        pui1_enc_data,
                        &l_enc_data_len);
    if(i4_ret)
    {   
        dprintf("_teeCLPLUSGetEncryAKH---_teeSecureStorageEnc failed\n");
        return FALSE;
    }
    if(l_enc_data_len>*pu4SrcLen) 
    {
        *pu4SrcLen = l_enc_data_len;
        free(pui1_enc_data);
        dprintf("_teeCLPLUSGetEncryAKH---input buffer not enough\n");
        return FALSE;
    }
    else
    {
       *pu4SrcLen = l_enc_data_len;
        memcpy_s2n_chk(pu1DstBuf,pui1_enc_data,l_enc_data_len,NULL);
        
        dprintf("_teeCLPLUSGetEncryAKH---l_enc_data_len %d\n",l_enc_data_len);
    }
    
    dprintf("_teeCLPLUSGetEncryAKH---end\n");
    return TRUE;
}

BOOL _teeCIPLUS_DRM_Init(UINT32 *drm_handle,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_init(drm_handle);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Remove(UINT32 drm_handle,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_remove(drm_handle);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Get_HostDevCert(UINT32 drm_handle, UINT8 *dev_certificate,UINT16 *dev_cert_len,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
   *err_code = ci_cc_drm_getHostDevCertificates(drm_handle,dev_certificate,dev_cert_len);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_HostBrandCert(UINT32 drm_handle, UINT8 *dev_brand_certificate, UINT16 *dev_brand_cert_len,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getHostBrandCertificates(drm_handle,dev_brand_certificate,dev_brand_cert_len);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Set_UTC_Time(UINT32 drm_handle,UINT32 epoch_time,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_setUTCTime(drm_handle,epoch_time);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Verify_ModuleCert(UINT32 drm_handle, UINT8 *dev_certificate, UINT32 dev_cert_len, UINT8 *dev_brand_certificate, UINT32 dev_brand_cert_len,UINT32* err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
     *err_code = ci_cc_drm_verifyModuleCertificates(drm_handle,dev_certificate,dev_cert_len,dev_brand_certificate,dev_brand_cert_len);
     return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Sign_Message(UINT32 drm_handle, UINT8 *auth_nonce, UINT16 auth_nonce_len,UINT8 *SignMsg, UINT16 *Signlen,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_signMessage(drm_handle,auth_nonce,auth_nonce_len,SignMsg,Signlen);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Verify_SignatureB(UINT32 drm_handle, UINT8 *DHPM,UINT16 DHPM_len, UINT8* SignMsg, UINT16 Signlen,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_verifySignatureB(drm_handle,DHPM,DHPM_len,SignMsg,Signlen);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_Dhph(UINT32 drm_handle, UINT8* DHPH,UINT16 *DHPH_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getDhph(drm_handle,DHPH,DHPH_len);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_HostId(UINT32 drm_handle, UINT8 *host_id,UINT16 *host_id_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getHostId(drm_handle,host_id,host_id_len);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_CamId(UINT32 drm_handle, UINT8 *cam_id,UINT16* cam_id_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getCamId(drm_handle,cam_id,cam_id_len);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Get_CamBrandId(UINT32 drm_handle,UINT16 *brand_id,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
   *err_code = ci_cc_drm_getCamBrandId(drm_handle,brand_id);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Generate_Akh(UINT32 drm_handle, UINT8 *AKH,UINT16 *AKH_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_generateAkh(drm_handle,AKH,AKH_len);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Generate_DHSK(UINT32 drm_handle,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_generateDHSK(drm_handle);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Generate_SacKeys(UINT32 drm_handle,UINT8 *Ns_Module, UINT16 Ns_Module_len,UINT8 *CICAM_ID,UINT16 CICAM_ID_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
     *err_code = ci_cc_drm_generateSacKeys(drm_handle,Ns_Module,Ns_Module_len,CICAM_ID,CICAM_ID_len);
     return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_Ns(UINT32 drm_handle, UINT8 *Ns_Host,UINT16 *Ns_Host_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getNs(drm_handle,Ns_Host,Ns_Host_len);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Encrypt_MessageSac(UINT32 drm_handle,UINT8* clearmsg, UINT16 clear_msg_len, UINT8 *enc_msg, UINT16 *enc_msg_len,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_encryptMessageSac(drm_handle,clearmsg,clear_msg_len,enc_msg,enc_msg_len);
    return (*err_code)==CI_CC_OK;
}

BOOL _teeCIPLUS_DRM_Decrypt_MessageSac(UINT32 drm_handle,UINT8 *encmsg, UINT16 enc_msg_length, UINT8 *dec_msg, UINT16 *dec_msg_length, UINT8 *decrypted,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_decryptMessageSac(drm_handle,encmsg,enc_msg_length,dec_msg,dec_msg_length,decrypted);
    return (*err_code)==CI_CC_OK;
}


BOOL _teeCIPLUS_DRM_Get_KeyId(UINT32 drm_handle, UINT32 * puKeyID,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_getKeyId(drm_handle,puKeyID);
    return (*err_code)==CI_CC_OK;

}
#if 0
BOOL _teeCIPLUS_DRM_Get_CCKId(UINT32 drm_handle,UINT8 *cck, UINT16 cck_len,UINT8 *civ,UINT16 civ_len,UINT8 *cipher,UINT8* key_register,UINT32 *err_code)
{
    *err_code = ci_cc_drm_getCCKId(drm_handle, cck, civ, cipher,key_register);
    return (*err_code)==CI_CC_OK;
}
#endif
BOOL _teeCIPLUS_DRM_Data_Confirm(UINT32 drm_handle, UINT8* data, UINT16 data_length, UINT8* confirm, UINT16* confirm_length,UINT32 *err_code)
{
    if(err_code == NULL)
    {
      return FALSE;
    }
    *err_code = ci_cc_drm_dataConfirm(drm_handle,data,data_length,confirm,confirm_length);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUS_DRM_Get_NextAuthContext(UINT32 drm_handle,UINT8 *AKH,UINT16 *AKH_len,UINT8 *cam_id,UINT16 *cam_id_len,UINT16 *cam_brand_id,UINT32 *err_code)
{
    UINT16 cam_brand_id_len = 2;
    if(err_code == NULL)
    {
      return FALSE;
    }

    *err_code = ci_cc_drm_GetNextAuthContext(drm_handle,AKH,AKH_len,cam_id,cam_id_len,cam_brand_id,&cam_brand_id_len);
    return (*err_code)==CI_CC_OK;
}
BOOL _teeCIPLUSGetCCK(UINT32 key_id,UINT8 *cipher,UINT8* key_register,UINT8 *cck, UINT16* cck_len,UINT8 *civ,UINT16* civ_len)
{

    UINT32 err_code;
    err_code = ci_cc_drm_getCCKValue(key_id,cipher,key_register,cck,cck_len,civ,civ_len);
    return (err_code)==CI_CC_OK;
}




