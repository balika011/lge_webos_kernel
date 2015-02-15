
/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "ccactx.h"
#include "ccfunc.h"
#include "ccmsg.h"
#include "ccsac.h"
#include "cccert.h"
#include "sectk.h"
#include "ci_trust_zone.h"

#ifdef CC_CI_PLUS_TZ_DUAL_INSTANCE_SUPPORT
#define DRM_HANDLE_MAX_NUM 2
#else
#define DRM_HANDLE_MAX_NUM 1
#endif
CI_CC_INFO ci_cc_info[DRM_HANDLE_MAX_NUM];
#define CCK_ID_INVALID_VALUE 0
static U32BIT g_key_id = CCK_ID_INVALID_VALUE;
static BOOL g_drm_init = FALSE;
/*!**************************************************************************
 * @brief    Create buffer to sign. This can be either for Signature_A
 *           where the data will be signed and sent to the module, or for
 *           Signature_B where the data will be verified.
 * @param    cc - content control information for session
 * @return   Buffer to sign, must be freed using free
 ****************************************************************************/
 
#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)

#ifdef FUNCTION_START
#undef FUNCTION_START
/* #define FUNCTION_START(x) do { printf(">> %s\n",#x); } while(0) */
#endif
#define FUNCTION_START(x) do { CC_PRINT((">>Trust Zone API: %s\n",#x)); } while(0)

#ifdef FUNCTION_FINISH
#undef FUNCTION_FINISH
/* #define FUNCTION_FINISH(x) do { printf("<< %s\n",#x); } while(0) */
#endif
#define FUNCTION_FINISH(x) do { CC_PRINT((">>Trust Zone API: %s\n",#x)); } while(0)

/* signature related constants */
#define SIGNATURE_A_DATA_LENGTH     302
#define SIGNATURE_B_DATA_LENGTH     561

#define TAG_SIGNATURE_VERSION       0
#define TAG_SIGNATURE_MSG_LABEL     1
#define TAG_SIGNATURE_AUTH_NONCE    2
#define TAG_SIGNATURE_DHPM          3
#define TAG_SIGNATURE_DHPH          4
   
#define SIGNATURE_VERSION           1
#define SIGNATURE_VERSION_LENGTH    8
#define SIGNATURE_A_MSG_LABEL       2
#define SIGNATURE_B_MSG_LABEL       3
#define SIGNATURE_MSG_LABEL_LENGTH  8
#define FAKE_CERT_SIZE  1122

typedef struct update_info
{
   void *userdata;
   BOOL success;
} CI_CC_UPDATE_INFO;

typedef struct send_info
{ 
   CI_CC_INFO *cc_info;
   U8BIT *buffer;
   U32BIT buffer_len;
   U32BIT bytes_written;
   BOOL success;
} CI_CC_SEND_INFO;

extern void SECTK_key_certificate_valid_period_print(U8BIT* cert_data );
extern VOID TZ_ciplus_read_cred (E_TZ_STB_CIPLUS_KEY_TYPE type, UINT8 **key, UINT16 *length);
static CI_CC_RESULT_T encryptMessageSac_Inner(U32BIT drm_handle,U8BIT* Message, U16BIT msglen, U8BIT *enc_msg, U16BIT *enc_msg_len);
CC_CI_PLUS_KEY_TYPE gui1_key_type;
/* SAC key seed */
#define SAC_KEY_SEED_BUFFER_SIZE    64

static U8BIT *CreateBufferToSign(CI_CC_INFO *cc)
{
   U8BIT *buffer;
   U16BIT length;
   U16BIT offset;
   U8BIT msg_label;

   FUNCTION_START(CreateBufferToSign);

   if (cc->dhph_generated)
   {
      if (cc->signature_b_recevied)
      {
         msg_label = SIGNATURE_B_MSG_LABEL;
         length = SIGNATURE_B_DATA_LENGTH;
      }
      else
      {
         msg_label = SIGNATURE_A_MSG_LABEL;
         length = SIGNATURE_A_DATA_LENGTH;
      }

      buffer = malloc(length);
      if (buffer != NULL)
      {
         /* Populate buffer */
         buffer[0] = TAG_SIGNATURE_VERSION;
         buffer[1] = (U8BIT)(SIGNATURE_VERSION_LENGTH >> 8);
         buffer[2] = (U8BIT)SIGNATURE_VERSION_LENGTH;
         buffer[3] = SIGNATURE_VERSION;
         buffer[4] = TAG_SIGNATURE_MSG_LABEL;
         buffer[5] = (U8BIT)(SIGNATURE_MSG_LABEL_LENGTH >> 8);
         buffer[6] = (U8BIT)SIGNATURE_MSG_LABEL_LENGTH;
         buffer[7] = msg_label;
         buffer[8] = TAG_SIGNATURE_AUTH_NONCE;
         buffer[9] = (U8BIT)(DT_SIZE_AUTH_NONCE >> 8);
         buffer[10] = (U8BIT)DT_SIZE_AUTH_NONCE;
         memcpy_s2s_chk(buffer+11, cc->auth_nonce, BITS_TO_BYTES(DT_SIZE_AUTH_NONCE),NULL);
         offset = 11 + BITS_TO_BYTES(DT_SIZE_AUTH_NONCE);
         buffer[offset] = TAG_SIGNATURE_DHPH;
         buffer[offset+1] = (U8BIT)(DT_SIZE_DHPH >> 8);
         buffer[offset+2] = (U8BIT)DT_SIZE_DHPH;
         memcpy_s2s_chk(buffer+offset+3, cc->dhph, BITS_TO_BYTES(DT_SIZE_DHPH),NULL);
         offset += 3 + BITS_TO_BYTES(DT_SIZE_DHPH);
         if (cc->signature_b_recevied)
         {
            buffer[offset] = TAG_SIGNATURE_DHPM;
            buffer[offset+1] = (U8BIT)(DT_SIZE_DHPM >> 8);
            buffer[offset+2] = (U8BIT)DT_SIZE_DHPM;
            memcpy_s2s_chk(buffer+offset+3, cc->dhpm, BITS_TO_BYTES(DT_SIZE_DHPM),NULL);
            offset += 3 + BITS_TO_BYTES(DT_SIZE_DHPM);
         }
      }
   }
   else
   {
      buffer = NULL;
   }

   FUNCTION_FINISH(CreateBufferToSign);

   return buffer;
}
/*!**************************************************************************
 * @brief    Generate SAC key seed (Ks)
 * @param    cc - content control information for session
 ****************************************************************************/
static void GenerateSacKeySeed(CI_CC_INFO *cc,U8BIT *Ns_Module)
{
   U8BIT buffer[SAC_KEY_SEED_BUFFER_SIZE];
   U16BIT offset;

   FUNCTION_START(GenerateSacKeySeed);

   if (cc->dhsk_generated &&
       cc->akh_generated &&
       cc->ns_host_generated )
   {
       CC_DEBUG_PRINT(("GenerateSacKeySeed() dhsk,akh,ns_host gerenated! \n"));
      offset = 0;
      memcpy_s2s_chk(buffer+offset, cc->dhsk + BITS_TO_BYTES(DT_SIZE_DHSK) - BITS_TO_BYTES(DT_SIZE_DHSK_TRUNCATED), BITS_TO_BYTES(DT_SIZE_DHSK_TRUNCATED),NULL);
      offset += BITS_TO_BYTES(DT_SIZE_DHSK_TRUNCATED);
      memcpy_s2s_chk(buffer+offset, cc->akh, BITS_TO_BYTES(DT_SIZE_AKH),NULL);
      offset += BITS_TO_BYTES(DT_SIZE_AKH);
      memcpy_s2s_chk(buffer+offset, cc->ns_host, BITS_TO_BYTES(DT_SIZE_NS_HOST),NULL);
      offset += BITS_TO_BYTES(DT_SIZE_NS_HOST);
      memcpy_n2s_chk(buffer+offset, Ns_Module, BITS_TO_BYTES(DT_SIZE_NS_MODULE),NULL);
      offset += BITS_TO_BYTES(DT_SIZE_NS_MODULE);

      ASSERT(offset == sizeof(buffer));

      SECTK_Sha256(buffer, offset, cc->ks);
      cc->ks_generated = TRUE;
   }

   FUNCTION_FINISH(GenerateSacKeySeed);
}
/*!**************************************************************************
 * @brief    Dynamic memory allocation function for SAC
 * @param    size - size of buffer to allocate
 * @return   Pointer to newly allocated buffer or NULL if failed
 ****************************************************************************/
static void *SacDynMemAlloc(U32BIT size)
{
   void *ptr;

   FUNCTION_START(SacDynMemAlloc);

   ptr = malloc(size);

   FUNCTION_FINISH(SacDynMemAlloc);

   return ptr;
}

/*!**************************************************************************
 * @brief    Free dynamically allocated buffer (using SacDynMemFree);
 * @param    ptr - pointer to allocated buffer
 ****************************************************************************/
static void SacDynMemFree(void *ptr)
{
   FUNCTION_START(SacDynMemFree);

   free(ptr);

   FUNCTION_FINISH(SacDynMemFree);
}

/****************************************************************************
 *
 * Function Name: ClearCCInfo
 *
 * Description:   Clear content-control information
 *
 * Parameters:    cc - content control information for session
 *
 * Returns:       Nothing
 *
 *****************************************************************************/
static void ClearCCInfo(CI_CC_INFO *cc)
{
   FUNCTION_START(ClearCCInfo);

   /* DHX */
   memset(cc->dhx, 0, BITS_TO_BYTES(DT_SIZE_DHX));
   cc->dhx_generated = FALSE;

   /* DHPH */
   memset(cc->dhph, 0, BITS_TO_BYTES(DT_SIZE_DHPH));
   cc->dhph_generated = FALSE;

   /* DHSK */
   memset(cc->dhsk, 0, BITS_TO_BYTES(DT_SIZE_DHSK));
   cc->dhsk_generated = FALSE;

   /* CICAM_ID */
   memset(cc->local_cicam_id, 0x00, BITS_TO_BYTES(DT_SIZE_CICAM_ID));
   cc->local_cicam_id_extracted = FALSE;

   /* Ks */
   memset(cc->ks, 0, BITS_TO_BYTES(DT_SIZE_KS));
   cc->ks_generated = FALSE;
   /* CIV */
   memset(cc->civ, 0, BITS_TO_BYTES(DT_SIZE_CIV));
   cc->civ_generated = FALSE;
   /* Key register */
   cc->key_register = 0xff;
   cc->key_register_received = FALSE;

   /* CCK */
   memset(cc->cck, 0, BITS_TO_BYTES(DT_SIZE_CCK));
   cc->cck_generated = FALSE;
   
   /* Kp */
   memset(cc->kp, 0, BITS_TO_BYTES(DT_SIZE_KP));
   cc->kp_received = FALSE;

   /* Host device certificate (local) */
   if (cc->host_dev_cert != NULL)
   {
      SECTK_FreeCertificate(cc->host_dev_cert);
   }
   cc->host_dev_cert = NULL;
   cc->host_dev_cert_parsed = FALSE;

   /* Module scrambler capabilities */
   cc->module_caps_capability = 0;
   cc->module_caps_version = 0;
   cc->module_caps_extracted = FALSE;

   /* Module brand ID */
   cc->cicam_brand_id = 0;
   cc->cicam_brand_id_extracted = FALSE;

   /* Host scrambler capabilities */
   cc->host_caps_capability = 0;
   cc->host_caps_version = 0;
   cc->host_caps_extracted = FALSE;
   /* Cipher */
   cc->cipher = 0xff;
   cc->cipher_negotiated = FALSE;

   /* HOST_ID */
   memset(cc->host_id, 0x00, BITS_TO_BYTES(DT_SIZE_HOST_ID));
   cc->host_id_extracted = FALSE;

   /* CICAM_BrandCert */
   if (cc->cicam_brand_cert != NULL)
   {
      SECTK_FreeCertificate(cc->cicam_brand_cert);
   }
   cc->cicam_brand_cert = NULL;
   cc->cicam_brand_cert_received = FALSE;

   /* DHPM */
   memset(cc->dhpm, 0, BITS_TO_BYTES(DT_SIZE_DHPM));
   cc->dhpm_received = FALSE;

   /* CICAM_DevCert */
   if (cc->cicam_dev_cert != NULL)
   {
      SECTK_FreeCertificate(cc->cicam_dev_cert);
   }
   cc->cicam_dev_cert = NULL;
   cc->cicam_dev_cert_received = FALSE;

   /* Signature_B */
   memset(cc->signature_b, 0, BITS_TO_BYTES(DT_SIZE_SIGNATURE_B));
   cc->signature_b_recevied = FALSE;

   /* auth_nonce */
   memset(cc->auth_nonce, 0, BITS_TO_BYTES(DT_SIZE_AUTH_NONCE));
   cc->auth_nonce_received = FALSE;

   /* Ns_Host */
   memset(cc->ns_host, 0, BITS_TO_BYTES(DT_SIZE_NS_HOST));
   cc->ns_host_generated = FALSE;

   /* AKH */
   memset(cc->akh, 0, BITS_TO_BYTES(DT_SIZE_AKH));
   cc->akh_generated = FALSE;
   /* SAC information */
   memset(&cc->sac, 0, sizeof(cc->sac));
   cc->sac_initialised = FALSE;
   cc->auth_context_initialised = FALSE;

   cc->cck_keyid = CCK_ID_INVALID_VALUE;
   cc->epoch_time = 0;

   FUNCTION_FINISH(ClearCCInfo);
}


/*!**************************************************************************
 * @brief    Initialise the SAC (secure authenticated channel)
 * @param    cc - content control information for session
 ****************************************************************************/
static void ci_cc_drm_InitialiseSac(CI_CC_INFO *cc)
{
   U8BIT *siv;
   U16BIT siv_len;

   FUNCTION_START(ci_cc_drm_InitialiseSac);

   TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_SIV, &siv, &siv_len);
   CC_InitialiseSac(&cc->sac, cc->ks, siv, SacDynMemAlloc, SacDynMemFree);
   cc->sac_initialised = TRUE;

   FUNCTION_FINISH(ci_cc_drm_InitialiseSac);
}
/*!**************************************************************************
 * @brief    Extract host scrambler capabilities
 * @param    cc - content control information for session
 ****************************************************************************/
static void ExtractHostScramblerCaps(CI_CC_INFO *cc)
{
   BOOL success;

   FUNCTION_START(ExtractHostScramblerCaps);

   if (cc->host_dev_cert_parsed)
   {
      success = SECTK_ExtractScramblerCaps(cc->host_dev_cert,
                                           &cc->host_caps_capability,
                                           &cc->host_caps_version);
      if (success)
      {
         cc->host_caps_extracted = TRUE;
      }
   }

   FUNCTION_FINISH(ExtractHostScramblerCaps);
}

/*!**************************************************************************
 * @brief    Extract CICAM scrambler capabilities
 * @param    cc - content control information for session
 ****************************************************************************/
static void ExtractCicamScramblerCaps(CI_CC_INFO *cc)
{
   BOOL success;

   FUNCTION_START(ExtractCicamScramblerCaps);

   if (cc->cicam_dev_cert_received)
   {
      success = SECTK_ExtractScramblerCaps(cc->cicam_dev_cert,
                                           &cc->module_caps_capability,
                                           &cc->module_caps_version);
      if (success)
      {
         cc->module_caps_extracted = TRUE;
      }
   }

   FUNCTION_FINISH(ExtractCicamScramblerCaps);
}

/*!**************************************************************************
 * @brief    Extract CICAM brand ID
 * @param    cc - content control information for session
 ****************************************************************************/
static void ExtractCicamBrandId(CI_CC_INFO *cc)
{
   BOOL success;

   FUNCTION_START(ExtractCicamBrandId);

   if (cc->cicam_dev_cert_received)
   {
      success = SECTK_ExtractBrandId(cc->cicam_dev_cert,
                                     &cc->cicam_brand_id);
      CC_DEBUG_PRINT((" ExtractCicamBrandId() success:%d,brand id:0x%x\n",success,cc->cicam_brand_id));
      if (!success)
      {
         cc->cicam_brand_id = 0;
      }
      cc->cicam_brand_id_extracted = TRUE;
   }

   FUNCTION_FINISH(ExtractCicamBrandId);
}

/*!**************************************************************************
 * @brief    Determine which cipher to use, given host capabilities and
 *           module capabilities
 * @param    cc - content control information for session
 ****************************************************************************/
static void DetermineCipher(CI_CC_INFO *cc)
{
   FUNCTION_START(DetermineCipher);

   if (cc->host_caps_capability == 0x00)
   {
      /* Host supports DES only */
      cc->cipher = STB_CI_CIPHER_DES;
      cc->cipher_negotiated = TRUE;
   }
   else if (cc->host_caps_capability == 0x01)
   {
      /* Host supports DES and AES, check module capability */
      if (cc->module_caps_capability == 0x00)
      {
         cc->cipher = STB_CI_CIPHER_DES;
         cc->cipher_negotiated = TRUE;
      }
      else if (cc->module_caps_capability == 0x01)
      {
         cc->cipher = STB_CI_CIPHER_AES;
         cc->cipher_negotiated = TRUE;
      }
   }

   FUNCTION_FINISH(DetermineCipher);
}

static void UpdateCcCallback(U8BIT id, U8BIT *data, U16BIT len, void *userdata)
{
   CI_CC_UPDATE_INFO *update_info;
   CI_CC_INFO* cc_info;
   BOOL success = FALSE;

   FUNCTION_START(UpdateCcCallback);

   update_info = userdata;
   cc_info = (CI_CC_INFO*)update_info->userdata;
   if (update_info->success)
   {
      CC_DEBUG_PRINT(("The data_id = [%d]\n", id));
      if(id == 12)
      {
          if (len == BITS_TO_BYTES(DT_SIZE_KP))
          {
             CC_DEBUG_PRINT(("[cipls_cc]: Updating Kp\n"));
             memcpy_s2s_chk(cc_info->kp, data, len,NULL);
             CC_DEBUG_BUFFER(data, len);
             cc_info->kp_received = TRUE;
             success = TRUE;
          }
      }
      else if(id == 6)
      {
          if (len == BITS_TO_BYTES(DT_SIZE_CICAM_ID))
          {
             CC_DEBUG_PRINT(("[cipls_cc]: Updating CICAM_ID\n"));
             memcpy_s2s_chk(cc_info->cicam_id, data, len,NULL);
             cc_info->cicam_id_received = TRUE;
             success = TRUE;
          }
      }
      else if(id == 28)
      {
          if (len == BITS_TO_BYTES(DT_SIZE_KEY_REGISTER))
          {
             CC_DEBUG_PRINT(("[cipls_cc]: Updating Key register:%d\n",data[0]));
             cc_info->key_register = data[0];
             cc_info->key_register_received = TRUE;
             success = TRUE;
          }
      }
      if (success == FALSE)
      {
         update_info->success = FALSE;
      }
   }

   FUNCTION_FINISH(UpdateCcCallback);
}
static BOOL CI_CC_FetchItemData(CI_CC_INFO *cc_info, U8BIT id,
                         U8BIT *data, U16BIT maxlen, U16BIT *len)
{
   BOOL success;

   FUNCTION_START(CC_FetchItemData);

   success = FALSE;

   if(id == 5)
    {
        if (cc_info->host_id_extracted)
        {
           *len = BITS_TO_BYTES(DT_SIZE_HOST_ID);
           if (*len <= maxlen)
           {
              memcpy_s2s_chk(data, cc_info->host_id, *len,NULL);
              success = TRUE;
           }
        }
    }
   else if(id == 30)
   {
        *len = BITS_TO_BYTES(DT_SIZE_STATUS_FIELD);
        if (*len <= maxlen)
        {
           memcpy_s2s_chk(data, &cc_info->status_field, *len,NULL);
           CC_DEBUG_PRINT(("GetStatusField  status:%d\n",*data));
           success = TRUE;
        }
    }
    
   ASSERT(success);

   FUNCTION_FINISH(CC_FetchItemData);

   return success;
}

static void SendCallback(U8BIT id, void *userdata)
{
   U8BIT *write_ptr;
   CI_CC_SEND_INFO *send_info;
   U16BIT maxlen;
   U16BIT bytes_written = 0;
   BOOL success;

   FUNCTION_START(SendCallback);

   send_info = userdata;
   if (send_info->success)
   {
      send_info->success = FALSE;
      write_ptr = send_info->buffer + send_info->bytes_written;
      maxlen = (U16BIT)(send_info->buffer_len - send_info->bytes_written);
      if (maxlen >= 3)
      {
         success = CI_CC_FetchItemData(send_info->cc_info, id, write_ptr + 3,
                                    maxlen - 3, &bytes_written);
         if (success)
         {
            CC_DEBUG_PRINT(("The data_id = [%d]\n", id));
            write_ptr[0] = id;
            write_ptr[1] = (U8BIT)(bytes_written >> 8);
            write_ptr[2] = (U8BIT)bytes_written;
            send_info->bytes_written += bytes_written + 3;
            send_info->success = TRUE;
         }
      }
   }

   FUNCTION_FINISH(SendCallback);
}

static U32BIT WriteDataResponse(CI_CC_INFO *cc_info, U8BIT *data,
                                U32BIT len, U8BIT *buffer, U32BIT buffer_len)
{
   CI_CC_SEND_INFO send_info;

   FUNCTION_START(WriteDataResponse);

   /* fix klocwork */
   memset(&send_info, 0x0, sizeof(CI_CC_SEND_INFO));

   if (buffer_len > 2)
   {
      buffer[0] = CC_SYSTEM_ID_BITMASK;
      buffer[1] = CC_GetSendItemCount(data, len);

      send_info.cc_info = cc_info;
      send_info.buffer = buffer;
      send_info.buffer_len = buffer_len;
      send_info.bytes_written = 2;
      send_info.success = TRUE;
      CC_LoopRequestedItems(data, len, SendCallback, &send_info);
      ASSERT(send_info.success == TRUE);
   }

   FUNCTION_FINISH(WriteDataResponse);

   return send_info.bytes_written;
}
CI_CC_RESULT_T HandleCCKDataMsg(U32BIT drm_handle,U8BIT *data, U16BIT len,U8BIT *enc_resp_msg, U16BIT *enc_resp_msg_len)
{
    
    CI_CC_UPDATE_INFO update_info;
    CI_CC_INFO* cc_info = &ci_cc_info[drm_handle];
    BOOL valid = FALSE;
    CI_CC_RESULT_T result = CI_CC_OK;
    
    update_info.userdata = &ci_cc_info[drm_handle];
    update_info.success = TRUE;
    
    FUNCTION_START(HandleCCKDataMsg);
    CC_LoopSentItems(data, len, UpdateCcCallback, &update_info);
    if (update_info.success)
    {
       /* Note: If CCK data is not valid, the host does not report an error.
        *       See Annex F, error code 04.
        */
       cc_info->status_field = 0x01;
      
       if (cc_info->kp_received && cc_info->key_register_received)
       {
          if (cc_info->cicam_id_received && cc_info->local_cicam_id_extracted)
          {
             if (memcmp(cc_info->cicam_id, cc_info->local_cicam_id,
                        BITS_TO_BYTES(DT_SIZE_CICAM_ID)) == 0)
             {
                valid = TRUE;
             }
          }
       }
       if (valid)
       {
          cc_info->status_field = 0x00;
       }
       CC_DEBUG_PRINT(("CckDataTransition valid:%d\n",valid));
       U32BIT resp_len = 0;
       U8BIT* resp_ptr = NULL;
       U32BIT bytes_written = 0;
       if (cc_info->sac_initialised)
       {
          resp_len = CC_GetResponseLength(CC_SAC_DATA_CNF_TAG, data, len);
          resp_ptr = malloc(resp_len);
          if (resp_ptr != NULL)
          {
             bytes_written = WriteDataResponse(cc_info, data, len, resp_ptr,
                                               resp_len);
             if(bytes_written != resp_len)
             {
                 ASSERT(0);
             }
    
            result = encryptMessageSac_Inner(drm_handle,resp_ptr, resp_len, enc_resp_msg, enc_resp_msg_len);
            free(resp_ptr);
          }
       }
    
    }
    FUNCTION_FINISH(HandleCCKDataMsg);
    return result;
}
static U32BIT get_next_drm_index()
{
  U32BIT index = 0;
  for(;index<DRM_HANDLE_MAX_NUM;index++)
  {
      if(!ci_cc_info[index].auth_context_initialised)
        {
          break;
        }
  }  
  return index;
}
static U32BIT get_drm_index(UINT32 key_id)
{
  U32BIT index = 0;
  for(;index<DRM_HANDLE_MAX_NUM;index++)
  {
      if(ci_cc_info[index].cck_keyid == key_id)
        {
            break;
        }
  }  
  return index;
}
/***************************************************************************
    Description:
        To Initialize the CI PLus DRM Context.

    Input Parameters:
        AKH: AKH Key

    Output Parameters:
        drm_handle: DRM Context Handle

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_init(U32BIT *drm_handle)
{
    U8BIT *prng_seed;
    U8BIT *prng_key_k;
    U16BIT seed_len;
    U16BIT key_k_len;    
    U32BIT handle;
    /* length of pseudo-random number generator seeds (in bytes) */
   #define PRNG_SEED_LEN 16
   #define PRNG_KEY_K_LEN 16
   FUNCTION_START(ci_cc_drm_init);
   if(g_drm_init == FALSE)
    {
        U32BIT index = 0;
        CC_DEBUG_PRINT(("First init all cci info here!\n"));
        for(;index<DRM_HANDLE_MAX_NUM;index++)
        {
            ClearCCInfo(&ci_cc_info[index]);    
        }
        g_drm_init = TRUE;
    }
   if(drm_handle == NULL)
    {
       CC_DEBUG_PRINT(("ERR: ci_cc_drm_init() invalid agrument\n"));
       return CI_CC_INV_ARG;
    }
   handle = get_next_drm_index();
   if(handle >=DRM_HANDLE_MAX_NUM)
    {
       CC_DEBUG_PRINT(("ERR: ci_cc_drm_init() no valid handle\n"));
       return CI_CC_FAILED;
    }
   if(SECTK_RecognizeCertType())
   {
       gui1_key_type = CC_CI_PLUS_KEY_TYPE_PRODUCT;
   }
   else
   {
       gui1_key_type = CC_CI_PLUS_KEY_TYPE_TEST;
   }
   CC_DEBUG_PRINT(("ci_cc_drm_init() handle:%d\n",handle));
    ClearCCInfo(&ci_cc_info[handle]);    
    memcpy_s2n_chk(drm_handle,&handle,sizeof(U32BIT),NULL);
    if(!ci_cc_info[handle].auth_context_initialised)
    {
        /* Initialise security module */
        SECTK_Initialise();
        
        /* Set random seed (using host constants) */
        TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_PRNG_SEED, &prng_seed, &seed_len);
        TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_PRNG_KEY_K, &prng_key_k, &key_k_len);
        if ((prng_seed != NULL) && (seed_len == PRNG_SEED_LEN) &&
            (prng_key_k != NULL) && (key_k_len == PRNG_KEY_K_LEN))
        {
           SECTK_SetRandomSeed(prng_seed, prng_key_k);
        }
        ci_cc_info[handle].auth_context_initialised = TRUE;
    }
    CC_ReadAuthContexts();
    FUNCTION_FINISH(ci_cc_drm_init);
    return CI_CC_OK;
}


/***************************************************************************
    Description:
        Removes the CI PLus DRM Context.

    Input Parameters:
        drm_handle: DRM Context Handle

    Output Parameters:
        Nil

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_remove(U32BIT drm_handle)
{
    FUNCTION_START(ci_cc_drm_remove);
    if(drm_handle >=DRM_HANDLE_MAX_NUM)
     {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_remove() invalid input argument\n"));
        return CI_CC_INV_ARG;
     }
    if(ci_cc_info[drm_handle].auth_context_initialised)
    {
        ClearCCInfo(&ci_cc_info[drm_handle]);    
    }
    FUNCTION_FINISH(ci_cc_drm_remove);
    return CI_CC_OK;
}
/***************************************************************************
    Description:
        Generates 64 Bits Random Numbers.

    Input Parameters:
        drm_handle: DRM Handle Context
        Ns_Host_len:Ns_Host buffer length

    Output Parameters:
        Ns_Host: random Number Generated
       Ns_Host_len:actual Ns_Host data length

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getNs(U32BIT drm_handle, U8BIT *Ns_Host,U16BIT *Ns_Host_len)
{
    
    U16BIT len = BITS_TO_BYTES(DT_SIZE_NS_HOST);
    FUNCTION_START(ci_cc_drm_getNs);
    if(drm_handle >=DRM_HANDLE_MAX_NUM || Ns_Host == NULL ||Ns_Host_len == NULL )
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getNs() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*Ns_Host_len <  BITS_TO_BYTES(DT_SIZE_NS_HOST))
    {
        memcpy_s2n_chk(Ns_Host_len,&len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getNs() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    /* Create Ns_Module */
    SECTK_GenerateRandomSequence(ci_cc_info[drm_handle].ns_host, BITS_TO_BYTES(DT_SIZE_NS_HOST));
    memcpy_s2n_chk(Ns_Host_len,&len,sizeof(U16BIT),NULL);
    memcpy_s2n_chk(Ns_Host,ci_cc_info[drm_handle].ns_host,BITS_TO_BYTES(DT_SIZE_NS_HOST),NULL);
    ci_cc_info[drm_handle].ns_host_generated = TRUE;
    FUNCTION_FINISH(ci_cc_drm_getNs);
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Gets the content of Host Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_cert_len:dev_certificate buffer length

    Output Parameters:
        dev_certificate: Memory Buffer Pointer for Device Certificate
        dev_cert_len: Device Certificate Length

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostDevCertificates(U32BIT drm_handle, U8BIT *dev_certificate, U16BIT *dev_certificate_len)
{
    U8BIT *local_dev_certificate = NULL;
    U16BIT local_dev_cert_len = 0;

    FUNCTION_START(ci_cc_drm_getHostDevCertificates);
    if(drm_handle >=DRM_HANDLE_MAX_NUM || dev_certificate_len == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostDevCertificates() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DEVICE_CERT, &local_dev_certificate, &local_dev_cert_len);
    CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostDevCertificates() input length:%d,dev cert len:%d \n",(*dev_certificate_len),local_dev_cert_len));
    if(local_dev_cert_len == 0)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostDevCertificates() failed local_dev_cert_len 0\n"));
        *dev_certificate_len = FAKE_CERT_SIZE;
        return CI_CC_OK;
    }
    if(dev_certificate == NULL || *dev_certificate_len < local_dev_cert_len)
    {
        memcpy_s2n_chk(dev_certificate_len,&local_dev_cert_len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostDevCertificates() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    memcpy_s2n_chk(dev_certificate_len,&local_dev_cert_len,sizeof(U16BIT),NULL);
    memcpy_s2n_chk(dev_certificate,local_dev_certificate,local_dev_cert_len*sizeof(U8BIT),NULL);
    CC_DEBUG_PRINT(("{MW CI} host dev certificate check: \n"));
    SECTK_key_certificate_valid_period_print(dev_certificate);
    if(!ci_cc_info[drm_handle].host_dev_cert_parsed)
    {
       ci_cc_info[drm_handle].host_dev_cert = SECTK_ParseCertificate(local_dev_certificate,
                                                  local_dev_cert_len);
       if (ci_cc_info[drm_handle].host_dev_cert != NULL)
       {
          ci_cc_info[drm_handle].host_dev_cert_parsed = TRUE;
       }
       ExtractHostScramblerCaps(&ci_cc_info[drm_handle]);
    }
    FUNCTION_FINISH(ci_cc_drm_getHostDevCertificates);
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Gets the content of Host Brand Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_brand_cert_len:dev_brand_certificate buffer length

    Output Parameters:
        dev_brand_certificate: Memory Buffer Pointer for Brand Certificate
        dev_brand_cert_len: Brand Certificate Length

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostBrandCertificates(U32BIT drm_handle, U8BIT *dev_brand_certificate, U16BIT *dev_brand_cert_len)
{
    U8BIT *local_dev_certificate;
    U16BIT local_dev_cert_len;
    FUNCTION_START(ci_cc_drm_getHostBrandCertificates);
    if(drm_handle >=DRM_HANDLE_MAX_NUM || dev_brand_cert_len == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostBrandCertificates() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_BRAND_CERT, &local_dev_certificate, &local_dev_cert_len);
        if(local_dev_cert_len == 0)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostBrandCertificates() failed local_dev_cert_len 0\n"));
        *dev_brand_cert_len = FAKE_CERT_SIZE;
        return CI_CC_OK;
    }
    if(dev_brand_certificate == NULL || *dev_brand_cert_len < local_dev_cert_len)
    {
        memcpy_s2n_chk(dev_brand_cert_len,&local_dev_cert_len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostBrandCertificates() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    memcpy_s2n_chk(dev_brand_cert_len,&local_dev_cert_len,sizeof(U16BIT),NULL);
    memcpy_s2n_chk(dev_brand_certificate,local_dev_certificate,local_dev_cert_len*sizeof(U8BIT),NULL);
     CC_DEBUG_PRINT(("{MW CI} host brand certificate check: \n"));
    SECTK_key_certificate_valid_period_print(dev_brand_certificate);
    FUNCTION_FINISH(ci_cc_drm_getHostBrandCertificates);
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Get the Host Id from Host Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        host_id_len:host_id buffer length
    
      Output Parameters:
          host_id: Host id from Host Certificate
          host_id_len:actual host_id data len

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostId(U32BIT drm_handle, U8BIT *host_id,UINT16* host_id_len)
{
    BOOL success;    
    U8BIT *local_dev_certificate;
    U16BIT local_dev_cert_len;
    UINT16 len = BITS_TO_BYTES(DT_SIZE_HOST_ID);
    FUNCTION_START(ci_cc_drm_getHostId);
    if(drm_handle >=DRM_HANDLE_MAX_NUM ||host_id == NULL || host_id_len == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostId() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*host_id_len<BITS_TO_BYTES(DT_SIZE_HOST_ID))
    {
        memcpy_s2n_chk(host_id_len,&len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getHostId() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    if (!ci_cc_info[drm_handle].host_dev_cert_parsed)
    {
       TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DEVICE_CERT, &local_dev_certificate, &local_dev_cert_len);
       ci_cc_info[drm_handle].host_dev_cert = SECTK_ParseCertificate(local_dev_certificate,
                                                  local_dev_cert_len);
       if (ci_cc_info[drm_handle].host_dev_cert != NULL)
       {
          ci_cc_info[drm_handle].host_dev_cert_parsed = TRUE;
       }
        ExtractHostScramblerCaps(&ci_cc_info[drm_handle]);
        CC_ResetAuthContextIter(&ci_cc_info[drm_handle]);
    }

    if (ci_cc_info[drm_handle].host_dev_cert_parsed)
    {
       success = SECTK_ExtractDeviceId(ci_cc_info[drm_handle].host_dev_cert, ci_cc_info[drm_handle].host_id);
       if (success)
       {
          ci_cc_info[drm_handle].host_id_extracted = TRUE;          
          memcpy_s2n_chk(host_id_len,&len,sizeof(U16BIT),NULL);
          memcpy_s2n_chk(host_id, ci_cc_info[drm_handle].host_id, BITS_TO_BYTES(DT_SIZE_HOST_ID),NULL);
          CC_DEBUG_PRINT(("ci_cc_drm_getHostId() host id success \n"));
       }
    }
    FUNCTION_FINISH(ci_cc_drm_getHostId);    
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Get the CAM Id from Host Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        cam_id_len:cam_id buffer length
    
      Output Parameters:
          cam_id: CAM id from CAM Certificate
          cam_id_len:actual cam_id data len

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getCamId(U32BIT drm_handle, U8BIT *cam_id,UINT16* cam_id_len)
{
    BOOL success;
    UINT16 len = BITS_TO_BYTES(DT_SIZE_CICAM_ID);
    FUNCTION_START(ci_cc_drm_getCamId);
    if(drm_handle >=DRM_HANDLE_MAX_NUM ||cam_id == NULL || cam_id_len == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getCamId() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    
    if(*cam_id_len<BITS_TO_BYTES(DT_SIZE_CICAM_ID))
    {
        memcpy_s2n_chk(cam_id_len,&len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getCamId() input length not enough \n"));
         return CI_CC_NOT_ENOUGH_SPACE;
    }
    memcpy_s2n_chk(cam_id_len,&len,sizeof(U16BIT),NULL);
    if (ci_cc_info[drm_handle].cicam_dev_cert_received)
    {
       success = SECTK_ExtractDeviceId(ci_cc_info[drm_handle].cicam_dev_cert, ci_cc_info[drm_handle].local_cicam_id);
       if (success)
       {
          ci_cc_info[drm_handle].local_cicam_id_extracted = TRUE;
          memcpy_s2n_chk(cam_id,ci_cc_info[drm_handle].local_cicam_id,BITS_TO_BYTES(DT_SIZE_CICAM_ID),NULL);
       }
    }
    
    FUNCTION_FINISH(ci_cc_drm_getCamId);    
    return CI_CC_OK;
}
CI_CC_RESULT_T ci_cc_drm_setUTCTime(U32BIT drm_handle,UINT32 epoch_time)
{
    FUNCTION_START(ci_cc_drm_setUTCTime);
    
    if(drm_handle >=DRM_HANDLE_MAX_NUM)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_setUTCTime() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    ci_cc_info[drm_handle].epoch_time = epoch_time;
    CC_DEBUG_PRINT(("ci_cc_drm_setUTCTime with time:%d\n",epoch_time));
    FUNCTION_FINISH(ci_cc_drm_setUTCTime);        
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Verifies the CICAM Module and Brand Certificate with Root Certificate.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_certificate: Buffer having Module Certificate
        dev_cert_len: Module Certificate Length
        dev_brand_certificate: Buffer having Brand Certificate
        dev_brand_cert_len: Brand Certificate Length

    Output Parameters:
        Nil

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_verifyModuleCertificates(U32BIT drm_handle, U8BIT *dev_certificate, U32BIT dev_cert_len, U8BIT *dev_brand_certificate, U32BIT dev_brand_cert_len)
{
    BOOL success;   
    
    E_CC_CERT_CHK chk;
    CI_CC_RESULT_T result;
    
    void *root_cert;
    
    U8BIT *cert_data;
    U16BIT cert_len;
    
    FUNCTION_START(ci_cc_drm_verifyModuleCertificates);
    
    if(drm_handle >=DRM_HANDLE_MAX_NUM ||dev_certificate == NULL || dev_brand_certificate == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_verifyModuleCertificates() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    
    result = CI_CC_CERT_ERROR;
    
    success = FALSE;
    
    ci_cc_info[drm_handle].cicam_dev_cert = SECTK_ParseCertificate(dev_certificate, dev_cert_len);
    if (ci_cc_info[drm_handle].cicam_dev_cert != NULL)
    {
       CC_DEBUG_PRINT(("[cipls_cc]: Updating CICAM_DevCert\n"));
       CC_DEBUG_BUFFER(dev_certificate, dev_cert_len);
       ci_cc_info[drm_handle].cicam_dev_cert_received = TRUE;
    }
    ci_cc_info[drm_handle].cicam_brand_cert = SECTK_ParseCertificate(dev_brand_certificate, dev_brand_cert_len);
    if (ci_cc_info[drm_handle].cicam_brand_cert != NULL)
    {
       CC_DEBUG_PRINT(("[cipls_cc]: Updating CICAM_BrandCert\n"));
       CC_DEBUG_BUFFER(dev_brand_certificate, dev_brand_cert_len);
       ci_cc_info[drm_handle].cicam_brand_cert_received = TRUE;
    }
    
    if (ci_cc_info[drm_handle].cicam_brand_cert_received && ci_cc_info[drm_handle].cicam_dev_cert_received)
    {
       TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_ROOT_CERT, &cert_data, &cert_len);
       root_cert = SECTK_ParseCertificate(cert_data, cert_len);
       if (root_cert != NULL)
       {
          chk = CC_CheckCertificates(root_cert, ci_cc_info[drm_handle].cicam_brand_cert,
                                     ci_cc_info[drm_handle].cicam_dev_cert,ci_cc_info[drm_handle].epoch_time);
          if (chk == CC_CERT_CHK_OK)
          {
             result = CI_CC_OK;
          }
          else if (chk == CC_CERT_CHK_EXPIRED)
          {
             result = CI_CC_CERT_EXPIRED;
          }
       }
       if (root_cert != NULL)
       {
          SECTK_FreeCertificate(root_cert);
       }
    }
    
    FUNCTION_FINISH(ci_cc_drm_verifyModuleCertificates);        
    return result;

}

/***************************************************************************
    Description:
        To Get the DHPH.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPH_len:DHPH buffer length
        
    Output Parameters:
        DHPH: DHPH of Host
        DHPH_len:actual DHPH data length
**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getDhph(U32BIT drm_handle, U8BIT* DHPH,U16BIT *DHPH_len)
{
    U8BIT *dh_g;
    U8BIT *dh_p;
    U8BIT *dh_q;
    U16BIT dh_g_len;
    U16BIT dh_p_len;
    U16BIT dh_q_len;
    U16BIT len = BITS_TO_BYTES(DT_SIZE_DHPH);
    
    FUNCTION_START(ci_cc_drm_getDhph);
    if(drm_handle >=DRM_HANDLE_MAX_NUM || DHPH == NULL || DHPH_len == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getDhph() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*DHPH_len <BITS_TO_BYTES(DT_SIZE_DHPH))
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getDhph() input length not enough \n"));
        memcpy_s2n_chk(DHPH_len,&len,sizeof(U16BIT),NULL);
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    /* Get DH parameters */
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DH_P, &dh_p, &dh_p_len);
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DH_G, &dh_g, &dh_g_len);
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DH_Q, &dh_q, &dh_q_len);
    
    /* Create DH keys */
    if (dh_p != NULL && dh_p_len == BITS_TO_BYTES(DT_SIZE_DH_P) &&
        dh_g != NULL && dh_g_len == BITS_TO_BYTES(DT_SIZE_DH_G) &&
        dh_q != NULL && dh_q_len == BITS_TO_BYTES(DT_SIZE_DH_Q))
    {
        SECTK_GenerateDhKeys(dh_g, dh_p, dh_q,
                             ci_cc_info[drm_handle].dhph, ci_cc_info[drm_handle].dhx);       
        memcpy_s2n_chk(DHPH, ci_cc_info[drm_handle].dhph, BITS_TO_BYTES(DT_SIZE_DHPH),NULL);
        memcpy_s2n_chk(DHPH_len,&len,sizeof(U16BIT),NULL);
        ci_cc_info[drm_handle].dhph_generated = TRUE;
        ci_cc_info[drm_handle].dhx_generated = TRUE;
        CC_DEBUG_PRINT(("ci_cc_drm_getDhph() dhph,dhx generated\n"));
    }
    
    FUNCTION_FINISH(ci_cc_drm_getDhph);        
    return CI_CC_OK;

}

/***************************************************************************
    Description:
        To sign a Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        auth_nonce: Random Number
       auth_nonce_len:auth_nonce buffer length       
       Signlen: input SingMsg buffer length

    Output Parameters:
        SignMsg: Signed Message
        Signlen: Signed Message Length

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_signMessage(U32BIT drm_handle, U8BIT *auth_nonce, UINT16 auth_nonce_len,U8BIT *SignMsg, U16BIT *Signlen)
{
    U8BIT *buffer;
    BOOL success;
    U8BIT *key;
    U16BIT key_len;
    CI_CC_RESULT_T result = CI_CC_FAILED;
    U16BIT len = BITS_TO_BYTES(DT_SIZE_SIGNATURE_A); 
    FUNCTION_START(ci_cc_drm_signMessage);
    if(drm_handle >=DRM_HANDLE_MAX_NUM || 
       auth_nonce == NULL || 
       auth_nonce_len <BITS_TO_BYTES(DT_SIZE_AUTH_NONCE) ||
       SignMsg == NULL || 
       Signlen == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_signMessage() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*Signlen < BITS_TO_BYTES(DT_SIZE_SIGNATURE_A))
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_signMessage() input length not enough \n"));
        memcpy_s2n_chk(Signlen,&len,sizeof(U16BIT),NULL);
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    CC_DEBUG_PRINT(("[cipls_cc]: Updating auth_nonce\n"));
    memcpy_n2s_chk(ci_cc_info[drm_handle].auth_nonce, auth_nonce, BITS_TO_BYTES(DT_SIZE_AUTH_NONCE),NULL);
    ci_cc_info[drm_handle].auth_nonce_received = TRUE;
    /* Create buffer to be digitally signed */
    buffer = CreateBufferToSign(&ci_cc_info[drm_handle]);
    if (buffer != NULL)
    {
       /* Get private key for RSA signature */
       TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_HDQ, &key, &key_len);
    
       if (key != NULL)
       {
          /* Sign buffer */
          success = SECTK_Sign(buffer, SIGNATURE_A_DATA_LENGTH, key, key_len,
                               SignMsg);
          if (success)
          {
              memcpy_s2n_chk(Signlen,&len,sizeof(U16BIT),NULL);
              result = CI_CC_OK;
          }
          else
          {
              CC_DEBUG_PRINT(("ERR: ci_cc_drm_signMessage() failed\n"));
          }
       }
    
       free(buffer);
    }
    
    FUNCTION_FINISH(ci_cc_drm_signMessage);        
    return result;
}

/***************************************************************************
    Description:
        To Verify the Signature Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPM: CICAM DHPM
        SignMsg: Signed Message
        Signlen: Signed Message Length

    Output Parameters:
        Nil

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_verifySignatureB(U32BIT drm_handle, U8BIT *DHPM,U16BIT DHPMlen, U8BIT* SignMsg, U16BIT Signlen)
{
    U8BIT *signed_buffer;
    BOOL verified;

    FUNCTION_START(ci_cc_drm_verifySignatureB);
    
    if(drm_handle >=DRM_HANDLE_MAX_NUM || 
       DHPM == NULL || 
       DHPMlen <BITS_TO_BYTES(DT_SIZE_DHPM) ||
       SignMsg == NULL ||  
       Signlen< BITS_TO_BYTES(DT_SIZE_SIGNATURE_B))
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_verifySignatureB() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    
    if (Signlen == BITS_TO_BYTES(DT_SIZE_SIGNATURE_B))
    {
       /* Update Signature_B */
       CC_DEBUG_PRINT(("[cipls_cc]: Updating Signature_B\n"));
       memcpy_n2s_chk(ci_cc_info[drm_handle].signature_b, SignMsg, Signlen,NULL);
       ci_cc_info[drm_handle].signature_b_recevied = TRUE;
    }
    
    if (DHPMlen == BITS_TO_BYTES(DT_SIZE_DHPM))
    {
       /* Update DHPM */
       CC_DEBUG_PRINT(("[ci trust zone]: Updating DHPM\n"));
       memcpy_n2s_chk(ci_cc_info[drm_handle].dhpm, DHPM, DHPMlen,NULL);
       ci_cc_info[drm_handle].dhpm_received = TRUE;
    }
    verified = FALSE;
    if (ci_cc_info[drm_handle].signature_b_recevied &&
        ci_cc_info[drm_handle].cicam_dev_cert_received &&
        ci_cc_info[drm_handle].dhph_generated &&
        ci_cc_info[drm_handle].dhpm_received)
    {
       signed_buffer = CreateBufferToSign(&ci_cc_info[drm_handle]);
       if (signed_buffer != NULL)
       {
          verified = SECTK_Verify(signed_buffer, SIGNATURE_B_DATA_LENGTH,
                                  ci_cc_info[drm_handle].cicam_dev_cert, ci_cc_info[drm_handle].signature_b);
          free(signed_buffer);
       }
    }
    if(verified)
    {
        CC_DEBUG_PRINT(("[ci trust zone]: ci_cc_drm_verifySignatureB() sucessful\n"));
        FUNCTION_FINISH(ci_cc_drm_verifySignatureB);        
        return CI_CC_OK;
    }
    return CI_CC_FAILED;
}

CI_CC_RESULT_T ci_cc_drm_generateDHSK(U32BIT drm_handle)
{
    U8BIT *dh_p;
    U8BIT *dh_q;
    U16BIT dh_p_len;
    U16BIT dh_q_len;
    FUNCTION_START(ci_cc_drm_generateDHSK);
    if(drm_handle >=DRM_HANDLE_MAX_NUM)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateDHSK() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    
    if (ci_cc_info[drm_handle].dhpm_received && ci_cc_info[drm_handle].dhx_generated)
    {
       TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DH_P, &dh_p, &dh_p_len);
       TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DH_Q, &dh_q, &dh_q_len);
    
       if (dh_p_len == BITS_TO_BYTES(DT_SIZE_DH_P) &&
           dh_q_len == BITS_TO_BYTES(DT_SIZE_DH_Q))
       {
           SECTK_CalculateDhShared(ci_cc_info[drm_handle].dhpm, ci_cc_info[drm_handle].dhx,
                                   dh_p, ci_cc_info[drm_handle].dhsk);
           ci_cc_info[drm_handle].dhsk_generated = TRUE;
           CC_DEBUG_PRINT(("ci_cc_drm_generateDHSK() dhsk generated\n"));
       }
    }
    else
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateDHSK() dhpm or dhx not generated\n"));
        return CI_CC_FAILED;
    }
    FUNCTION_FINISH(ci_cc_drm_generateDHSK);        
    return CI_CC_OK;
}
/***************************************************************************
    Description:
        Generates DHSK and then finally AKH.

    Input Parameters:
        drm_handle: DRM Handle Context
       AKH_len: akh buffer length

    Output Parameters:
        AKH: AKH Generated
        AKH_len:actual AKH data length
**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_generateAkh(U32BIT drm_handle, U8BIT *AKH,U16BIT *AKH_len)
{
    U16BIT offset;
    U8BIT buffer[BITS_TO_BYTES(DT_SIZE_CICAM_ID) +
                 BITS_TO_BYTES(DT_SIZE_HOST_ID) +
                 BITS_TO_BYTES(DT_SIZE_DHSK)];
    U16BIT len = BITS_TO_BYTES(DT_SIZE_AKH);
    FUNCTION_START(ci_cc_drm_generateAkh);
    if(drm_handle >=DRM_HANDLE_MAX_NUM|| AKH == NULL || AKH_len== NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateAkh() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*AKH_len  <BITS_TO_BYTES(DT_SIZE_AKH))
    {
        memcpy_s2n_chk(AKH_len,&len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateAkh() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    if (ci_cc_info[drm_handle].cicam_dev_cert_received &&
        ci_cc_info[drm_handle].dhsk_generated &&
        ci_cc_info[drm_handle].local_cicam_id_extracted &&
        ci_cc_info[drm_handle].host_id_extracted)
    {
       offset = 0;
       memcpy_s2s_chk(buffer + offset, ci_cc_info[drm_handle].local_cicam_id, BITS_TO_BYTES(DT_SIZE_CICAM_ID),NULL);
       offset += BITS_TO_BYTES(DT_SIZE_CICAM_ID);
       memcpy_s2s_chk(buffer + offset, ci_cc_info[drm_handle].host_id, BITS_TO_BYTES(DT_SIZE_HOST_ID),NULL);
       offset += BITS_TO_BYTES(DT_SIZE_HOST_ID);
       memcpy_s2s_chk(buffer + offset, ci_cc_info[drm_handle].dhsk, BITS_TO_BYTES(DT_SIZE_DHSK),NULL);
       offset += BITS_TO_BYTES(DT_SIZE_DHSK);
       ASSERT(offset == sizeof(buffer));
       SECTK_Sha256(buffer, sizeof(buffer), ci_cc_info[drm_handle].akh);
       ci_cc_info[drm_handle].akh_generated = TRUE;
       memcpy_s2n_chk(AKH,ci_cc_info[drm_handle].akh,BITS_TO_BYTES(DT_SIZE_AKH),NULL);
       memcpy_s2n_chk(AKH_len,&len,sizeof(U16BIT),NULL);
    }
    /* Extract scrambler capabilities */
    ExtractHostScramblerCaps(&ci_cc_info[drm_handle]);
    ExtractCicamScramblerCaps(&ci_cc_info[drm_handle]);
    
   // /* Extract CICAM brand ID */
    ExtractCicamBrandId(&ci_cc_info[drm_handle]);
    
    /* Determine cipher to use */
    DetermineCipher(&ci_cc_info[drm_handle]);
    CC_UpdateAuthContext(&ci_cc_info[drm_handle]);
    FUNCTION_FINISH(ci_cc_drm_generateAkh);        
    return CI_CC_OK;
}

/***************************************************************************
    Description:
        Generates SAC Keys.

    Input Parameters:
        drm_handle: DRM handle Context
        Ns_Module: Random Number from Module
        CICAM_ID: CI Cam Id

    Output Parameters:
        Nil

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_generateSacKeys(U32BIT drm_handle, U8BIT *Ns_Module, U16BIT ns_module_len,U8BIT *CICAM_ID,U16BIT cicam_id_len)
{
    BOOL valid;
    /* Note: If SAC data is not valid, the host does not report an error.
     *       See Annex F, error code 03.
     */
    FUNCTION_START(ci_cc_drm_generateSacKeys);
    if(drm_handle >=DRM_HANDLE_MAX_NUM|| 
       Ns_Module == NULL || 
       ns_module_len< BITS_TO_BYTES(DT_SIZE_NS_MODULE)||
       CICAM_ID == NULL ||
       cicam_id_len<BITS_TO_BYTES(DT_SIZE_CICAM_ID))
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateSacKeys() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if (ci_cc_info[drm_handle].local_cicam_id_extracted)
    {
       if (memcmp(CICAM_ID, ci_cc_info[drm_handle].local_cicam_id,
                  BITS_TO_BYTES(DT_SIZE_CICAM_ID)) == 0)
       {
          valid = TRUE;
       }
    }
    if (valid)
    {
       CC_DEBUG_PRINT(("ci_cc_drm_generateSacKeys cicam id matched!\n"));
       GenerateSacKeySeed(&ci_cc_info[drm_handle],Ns_Module);
       ci_cc_drm_InitialiseSac(&ci_cc_info[drm_handle]);
       FUNCTION_FINISH(ci_cc_drm_generateSacKeys);        
       return CI_CC_OK;
    }
    else
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_generateSacKeys() cam id dismatch\n"));
    }
    FUNCTION_FINISH(ci_cc_drm_generateSacKeys);        
    return CI_CC_FAILED;
}


/***************************************************************************
    Description:

This function is used to encrypt and sign a SAC message to send it to the CICAM over the SAC.
After use, the encrypted message memory should be freed by the caller using free().

Input Parameters:
drm_handle: DRM Handle Context
clearmsg Pointer to buffer holding the clear message
clear_msg_len pointer to length of clear message

Output Parameters:
encmsg Double pointer to the encrypted and signed message.
enc_msg_length Pointer to the encrypted and signed message length

**************************************************************************/

CI_CC_RESULT_T ci_cc_drm_encryptMessageSac(U32BIT drm_handle,U8BIT* Message, U16BIT msglen, U8BIT *enc_msg, U16BIT *enc_msg_len)
{
   U8BIT* Message_copy = NULL;
   CI_CC_RESULT_T result = CI_CC_OK;
   FUNCTION_START(ci_cc_drm_encryptMessageSac);
   if(drm_handle >=DRM_HANDLE_MAX_NUM||
      Message == NULL || 
      msglen==0 || 
      enc_msg_len == NULL || 
      enc_msg == NULL )
   {
       CC_DEBUG_PRINT(("ERR: ci_cc_drm_encryptMessageSac() invalid agrument\n"));
       return CI_CC_INV_ARG;
   }
   Message_copy = malloc(msglen*sizeof(U8BIT));
   if(Message_copy == NULL)
   {
       CC_DEBUG_PRINT(("ERR: ci_cc_drm_encryptMessageSac() alloc memory failed!\n"));
       return CI_CC_NOT_ENOUGH_SPACE;
   }
   memcpy_n2s_chk(Message_copy,Message,msglen,NULL);
   encryptMessageSac_Inner(drm_handle,Message_copy,msglen,enc_msg,enc_msg_len);
   free(Message_copy);
   FUNCTION_FINISH(ci_cc_drm_encryptMessageSac);        
    return result;
}

static CI_CC_RESULT_T encryptMessageSac_Inner(U32BIT drm_handle,U8BIT* Message, U16BIT msglen, U8BIT *enc_msg, U16BIT *enc_msg_len)
{
   U8BIT *local_enc_msg = NULL;
   U32BIT local_enc_msg_len = 0;
   CI_CC_RESULT_T result = CI_CC_OK;
   FUNCTION_START(encryptMessageSac_Inner);
   CC_DEBUG_PRINT(("encryptMessageSac_Inner called \n"));
   if(Message == NULL || 
      msglen==0 || 
      enc_msg_len == NULL || 
      enc_msg == NULL )
   {
       CC_DEBUG_PRINT(("ERR: ci_cc_drm_encryptMessageSac() invalid agrument\n"));
       return CI_CC_INV_ARG;
   }
    CC_EmbedSacMessage(&ci_cc_info[drm_handle].sac,Message, msglen,&local_enc_msg, &local_enc_msg_len);
    if(local_enc_msg!= NULL)
    {
      if(local_enc_msg_len > 0)
      {
          if(*enc_msg_len < local_enc_msg_len)
          {
            CC_DEBUG_PRINT(("ERR: ci_cc_drm_encryptMessageSac() input length not enough \n"));
            result = CI_CC_NOT_ENOUGH_SPACE;
          }
          else
          {
              memcpy_s2n_chk(enc_msg,local_enc_msg+1,local_enc_msg_len,NULL);
          }
      }
      memcpy_s2n_chk(enc_msg_len,&local_enc_msg_len,sizeof(U16BIT),NULL);
      ci_cc_info[drm_handle].sac.mem_free(local_enc_msg);
    }
    FUNCTION_FINISH(encryptMessageSac_Inner);        
    return result;
}

/***************************************************************************
    Description:

This function is used to authenticate and decrypt a SAC message and return the decrypted message contents.
If the SAC message contains the Key precursor Kp used for CCK derivation, the decrypted message is not output. 
Instead the CCK operation will be processed. In this case clearmsg/clear_msg_length 
will carry the reply message to be sent to the CICAM, and the decrypted' output variable will be set to 0.
If the SAC message does not contain the Key precursor Kp, the message is decrypted and returned in clearmsg/clear_msg_length 
and the decrypted' output variable is set to 1

Input Parameters:
drm_handle Pointer to DRM handle
enc_msg Pointer to buffer holding the encrypted message 
enc_msg_len Pointer to length of encrypted message
dec_msg_length:dec_msg buffer length

Output Parameters:
clearmsg Double pointer to the clear message buffer / reply message buffer
clear_msg_length Pointer to the clear message length/reply message length
decrypted Pointer to variable indicating which operation was performed. 1=message decrypted 0=reply generated.


**************************************************************************/

CI_CC_RESULT_T ci_cc_drm_decryptMessageSac(U32BIT drm_handle,U8BIT *encmsg, U16BIT enc_msg_length, U8BIT *dec_msg, U16BIT *dec_msg_length, U8BIT *decrypted)
{
    U8BIT *local_enc_msg = NULL;
    U16BIT local_enc_msg_len = 0;
    E_CC_MSG_TYPE msg_type;
    U8BIT decrypted_val;
    U8BIT* encmsg_copy = NULL;
    CI_CC_RESULT_T result = CI_CC_OK;

    FUNCTION_START(ci_cc_drm_decryptMessageSac);
    CC_DEBUG_PRINT(("ci_cc_drm_decryptMessageSac call\n"));
    if(drm_handle >=DRM_HANDLE_MAX_NUM||
       encmsg == NULL || 
       enc_msg_length==0 || 
       dec_msg_length == NULL || 
       dec_msg == NULL )
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_decryptMessageSac() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    encmsg_copy = malloc(enc_msg_length*sizeof(U8BIT));
    if(encmsg_copy == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_decryptMessageSac() alloc memory failed!\n"));
        return CI_CC_FAILED;
    }
     memcpy_n2s_chk(encmsg_copy,encmsg,enc_msg_length,NULL);
     CC_ExtractSacMessage(&ci_cc_info[drm_handle].sac,encmsg_copy,enc_msg_length,&local_enc_msg, &local_enc_msg_len);
     CC_DEBUG_PRINT(("ci_cc_drm_decryptMessageSac() local_enc_msg:0x%x,decrypt msg len:%d\n",local_enc_msg,local_enc_msg_len));
     decrypted_val = 1;
     memcpy_s2n_chk(decrypted,&decrypted_val,sizeof(U8BIT),NULL);
     memcpy_s2n_chk(dec_msg_length,&local_enc_msg_len,sizeof(U16BIT),NULL);
     if(local_enc_msg!= NULL  )
     { 
        if(local_enc_msg_len > 0)
        {
            memcpy_s2n_chk(decrypted,&decrypted_val,sizeof(U8BIT),NULL);
            msg_type = CC_GetMessageType(local_enc_msg,local_enc_msg_len);
            if(msg_type == CC_MSG_TYPE_KEY_DATA)
            {
                decrypted_val = 0;
                memcpy_s2n_chk(decrypted,&decrypted_val,sizeof(U8BIT),NULL);
            }
            CC_DEBUG_PRINT(("ci_cc_drm_decryptMessageSac() decrypted:%d\n",(*decrypted)));
            if(decrypted_val == 1)
            {
               if(*dec_msg_length < local_enc_msg_len)
               {
                 CC_DEBUG_PRINT(("ERR: ci_cc_drm_decryptMessageSac() input length not enough \n"));
                 result = CI_CC_NOT_ENOUGH_SPACE;
               }
               else
               {
                    memcpy_s2n_chk(dec_msg,local_enc_msg,local_enc_msg_len,NULL);
               }
               memcpy_s2n_chk(dec_msg_length,&local_enc_msg_len,sizeof(U16BIT),NULL);
            }
            else
            {
               result = HandleCCKDataMsg(drm_handle,local_enc_msg,local_enc_msg_len,dec_msg,dec_msg_length);
                if(result== CI_CC_OK&& ci_cc_info[drm_handle].kp_received && ci_cc_info[drm_handle].cipher_negotiated)
                {
                    CC_CalculateCcKeys(ci_cc_info[drm_handle].kp, ci_cc_info[drm_handle].cipher, ci_cc_info[drm_handle].cck,ci_cc_info[drm_handle].civ);
                    ci_cc_info[drm_handle].cck_generated = TRUE;
                    ci_cc_info[drm_handle].civ_generated = TRUE;
                }
            }
        }
        free(local_enc_msg);
     }
     free(encmsg_copy);
     FUNCTION_FINISH(ci_cc_drm_decryptMessageSac);        
   return result;
}

/***************************************************************************
    Description:

   This API returns key ID which shall be associated with DMX for content decryption 
   This function only needs to be called after the initial CCK establishment. Any subsequent CCK refresh will use the same KeyID. 
   The KeyID actually represents a key pair (ODD, EVEN), and key rotation will be handled automatically 
   Then processing SAC messages containing Key precursor Kp

Input Parameters:
        drm_handle pointer to DRM handle

        Output Parameters:
        KeyID  Key ID for use with AMP DMX.


**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getKeyId(U32BIT drm_handle, U32BIT * puKeyID)
{
    if(drm_handle >=DRM_HANDLE_MAX_NUM || puKeyID == NULL )
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getCCKId() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(ci_cc_info[drm_handle].cck_keyid == CCK_ID_INVALID_VALUE)
    {
        g_key_id ++;
        if(g_key_id == 10)
        {
            g_key_id = 1;
        }
        ci_cc_info[drm_handle].cck_keyid = g_key_id;
    }
    CC_DEBUG_PRINT(("ci_cc_drm_getCCKId() handle:%d,keyid:%d\n",drm_handle,ci_cc_info[drm_handle].cck_keyid));
    memcpy_s2n_chk(puKeyID,&ci_cc_info[drm_handle].cck_keyid,sizeof(U32BIT),NULL);
    return CI_CC_OK;
}
/***************************************************************************
    Description:

This API generates a confirmation message for either the URI refresh protocol or the SRM transfer protocol. 
he output message will be the SHA-256 checksum of a message consisting of the input data and the UCK. The UCK is a SHA-256 checksum of the SAK

Input Parameters:
rm_handle Pointer to DRM handle
Data Pointer to data of message body
Data_length Length of message body

Output Parameters:
Confirm double pointer to confirmation message
Cofirm_length output length of confirmation message

**************************************************************************/

CI_CC_RESULT_T ci_cc_drm_dataConfirm(U32BIT drm_handle, U8BIT* buffer, U16BIT buffer_len, U8BIT* confirm, U16BIT* confirm_length)
{
    U16BIT len = BITS_TO_BYTES(256);
    U8BIT* buffer_copy = NULL;
    FUNCTION_START(ci_cc_drm_dataConfirm);
    if(drm_handle >=DRM_HANDLE_MAX_NUM||buffer == NULL ||buffer_len==0 || confirm== NULL || confirm_length == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_dataConfirm() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(*confirm_length < BITS_TO_BYTES(256) )
    {
        memcpy_s2n_chk(confirm_length,&len,sizeof(U16BIT),NULL);
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_dataConfirm() input length not enough \n"));
        return CI_CC_NOT_ENOUGH_SPACE;
    }

    CC_DEBUG_PRINT(("ci_cc_drm_dataConfirm buffer len:%d, buffer data:\n",buffer_len));
    CC_DEBUG_BUFFER(buffer, buffer_len);
    if (ci_cc_info[drm_handle].sac_initialised)
    {
        buffer_copy = malloc((buffer_len+32)*sizeof(U8BIT));
        if(buffer_copy == NULL)
        {
            CC_DEBUG_PRINT(("ERR: ci_cc_drm_dataConfirm() alloc memory failed!\n"));
            return CI_CC_FAILED;
        }
        memcpy_n2s_chk(buffer_copy,buffer,buffer_len,NULL);
        memcpy_s2n_chk(confirm_length,&len,sizeof(U16BIT),NULL);
        SECTK_Sha256(CC_GetSacAuthenticationKey(&ci_cc_info[drm_handle].sac),
                    BITS_TO_BYTES(DT_SIZE_SAK),
                    (buffer_copy + buffer_len));
        SECTK_Sha256(buffer_copy, buffer_len+32, confirm);        
        free(buffer_copy);
    }
    FUNCTION_FINISH(ci_cc_drm_dataConfirm);        
    return CI_CC_OK;
}

/*!**************************************************************************
 * @brief    Return next authentication context
 *
 * This function should be called after authentication contexts have been
 * read and after a call to CC_ResetAuthContextIter.
 *
 * When there are no more valid contexts, this function returns invalid (i.e.
 * all-zeros) data. This is in accordance with the CI+ specification.
 *
 * @param session_info - content control session information
 *
 ****************************************************************************/
CI_CC_RESULT_T ci_cc_drm_GetNextAuthContext(U32BIT drm_handle,U8BIT *AKH,U16BIT *AKH_len,U8BIT *cam_id,U16BIT *cam_id_len,U16BIT *cam_brand_id,U16BIT *cam_brand_id_len)
{
    FUNCTION_START(ci_cc_drm_GetNextAuthContext);
    U16BIT len = BITS_TO_BYTES(DT_SIZE_AKH);
    if(drm_handle >=DRM_HANDLE_MAX_NUM||AKH == NULL || AKH_len == NULL )
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_GetNextAuthContext() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if( *AKH_len <BITS_TO_BYTES(DT_SIZE_AKH))
    {
        *AKH_len = BITS_TO_BYTES(DT_SIZE_AKH);
        return CI_CC_NOT_ENOUGH_SPACE;
    }
    CC_CopyNextAuthContext(&ci_cc_info[drm_handle]);
    memcpy_s2n_chk(AKH,ci_cc_info[drm_handle].akh,BITS_TO_BYTES(DT_SIZE_AKH),NULL);
    CC_DEBUG_PRINT(("ci_cc_drm_GetNextAuthContext AKH:\n"));
    CC_DEBUG_BUFFER(AKH, BITS_TO_BYTES(DT_SIZE_AKH));
    
    memcpy_s2n_chk(AKH_len,&len,sizeof(U16BIT),NULL);
    if(cam_id!= NULL && cam_id_len!=NULL && *cam_id_len>=BITS_TO_BYTES(DT_SIZE_CICAM_ID))
    {
       memcpy_s2n_chk(cam_id,ci_cc_info[drm_handle].local_cicam_id,BITS_TO_BYTES(DT_SIZE_CICAM_ID),NULL);
       len = BITS_TO_BYTES(DT_SIZE_CICAM_ID);
       memcpy_s2n_chk(cam_id_len,&len,sizeof(U16BIT),NULL);
    }
    if(cam_brand_id!= NULL && cam_brand_id_len!=NULL && *cam_brand_id_len>=2)
    {
        len = 2;
        memcpy_s2s_chk(cam_brand_id_len,&len,sizeof(U16BIT),NULL);
        memcpy_s2n_chk(cam_brand_id,&ci_cc_info[drm_handle].cicam_brand_id,sizeof(U16BIT),NULL);
    }
    FUNCTION_FINISH(ci_cc_drm_GetNextAuthContext);        
    return CI_CC_OK;
}
CI_CC_RESULT_T ci_cc_drm_getCamBrandId(U32BIT drm_handle,U16BIT *brand_id)
{
    CI_CC_RESULT_T result =CI_CC_FAILED;
    FUNCTION_START(ci_cc_drm_getBrandId);
    if(drm_handle >=DRM_HANDLE_MAX_NUM||brand_id == NULL)
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getBrandId() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    if(!ci_cc_info[drm_handle].cicam_brand_id_extracted)
    {
       ExtractCicamBrandId(&ci_cc_info[drm_handle]);
    }
    if(ci_cc_info[drm_handle].cicam_brand_id_extracted)
    {
        CC_DEBUG_PRINT(("ci_cc_drm_getBrandId() copy brand id\n"));
        memcpy_s2n_chk(brand_id,&ci_cc_info[drm_handle].cicam_brand_id,sizeof(U16BIT),NULL);
        result = CI_CC_OK;
    }
    FUNCTION_FINISH(ci_cc_drm_getBrandId);        
    return result;
}

CI_CC_RESULT_T ci_cc_drm_getCCKValue(UINT32 key_id,UINT8 *cipher,UINT8* key_register,UINT8 *cck, UINT16* cck_len,UINT8 *civ,UINT16* civ_len)
{
    CI_CC_RESULT_T result = CI_CC_OK;
    UINT32 drm_handle;
    FUNCTION_START(ci_cc_drm_getCCKValue);
    if(key_id == CCK_ID_INVALID_VALUE ||
        cipher == NULL ||
        key_register == NULL ||
        cck == NULL || 
        cck_len == NULL ||
        civ_len == NULL ||
        *cck_len < BITS_TO_BYTES(DT_SIZE_CCK)||
        *civ_len <BITS_TO_BYTES(DT_SIZE_CIV))
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getCCKValue() invalid agrument\n"));
        return CI_CC_INV_ARG;
    }
    drm_handle = get_drm_index(key_id);
    CC_DEBUG_PRINT(("ci_cc_drm_getCCKValue() handle:%d,keyid:%d\n",drm_handle,key_id));
    if(drm_handle >=DRM_HANDLE_MAX_NUM)
    {
        return CI_CC_INV_ARG;
    }
    if(ci_cc_info[drm_handle].kp_received && ci_cc_info[drm_handle].cipher_negotiated && ci_cc_info[drm_handle].cck_generated && ci_cc_info[drm_handle].civ_generated)
    {
        memcpy_s2s_chk(cipher,&ci_cc_info[drm_handle].cipher,sizeof(U8BIT),NULL);
        memcpy_s2s_chk(key_register,&ci_cc_info[drm_handle].key_register,sizeof(U8BIT),NULL);
        memcpy_s2s_chk(cck,&ci_cc_info[drm_handle].cck,BITS_TO_BYTES(DT_SIZE_CCK),NULL);
        memcpy_s2s_chk(civ,&ci_cc_info[drm_handle].civ,BITS_TO_BYTES(DT_SIZE_CIV),NULL);
        *cck_len = BITS_TO_BYTES(DT_SIZE_CCK);
        *civ_len = BITS_TO_BYTES(DT_SIZE_CIV);
        CC_DEBUG_PRINT(("key register:%d\n",(*key_register)));
        CC_DEBUG_PRINT(("cipher:%d\n",(*cipher)));
        CC_DEBUG_PRINT(("CCK:\n"));
        CC_DEBUG_BUFFER(cck, 16);     
        CC_DEBUG_PRINT(("CIV:\n"));
        CC_DEBUG_BUFFER(civ, 16);     
        CC_DEBUG_PRINT(("ci_cc_drm_getCCKValue() success!\n"));
        
        result = CI_CC_OK;
    }
    else
    {
        CC_DEBUG_PRINT(("ERR: ci_cc_drm_getCCKValue() cipher not negotiated\n"));
        result = CI_CC_FAILED;
    }
    FUNCTION_FINISH(ci_cc_drm_getCCKValue);        
    return result;
}
