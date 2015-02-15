/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     ccsac.c                                                    */
/*                                                                           */
/* Description:   CI+ Content Control Resource: Secure Authenticated Channel */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 16/01/2009                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* compiler library header files */
#if 0
#include <string.h>
#include <stdio.h>
#endif

/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "ccactx.h"
#include "ccfunc.h"
#include "ccmsg.h"
#include "ccsac.h"
#include "cccert.h"
#include "sectk.h"

/* --- constant definitions for this file --- */
enum
{
   SAC_MESSAGE_COUNTER_LEN = 4,
   SAC_MESSAGE_HEADER_LEN  = 4,
   SAC_AUTHENTICATION_LEN  = 16,
   SAC_OVERHEAD_LEN        = SAC_MESSAGE_COUNTER_LEN + SAC_MESSAGE_HEADER_LEN
};

enum
{
   SAC_PROTOCOL_VERSION           = 0x0,
   SAC_AUTHENTICATION_CIPHER_FLAG = 0x0,
   SAC_PAYLOAD_ENCRYPTION_FLAG    = 0x1,
   SAC_ENCRYPTION_CIPHER_FLAG     = 0x0
};

enum
{
   KEY_SIZE   = 16,
   BLOCK_SIZE = 16
};
#define APDU_TAG_LEN       3     /* 3 bytes in apdu tag */
#define APDU_HDR_MAX_LEN   8     /* max header length is 3 for tag plus 5 for length field */

#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)

#define CC_DEBUG_PRINT_FORCE(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER_FORCE(x,y) CC_PRINT_BUFFER(x,y)

#ifdef FUNCTION_START
#undef FUNCTION_START
/* #define FUNCTION_START(x) do { printf(">> %s\n",#x); } while(0) */
#endif
#define FUNCTION_START(x) do { CC_PRINT((">> %s\n",#x)); } while(0)

#ifdef FUNCTION_FINISH
#undef FUNCTION_FINISH
/* #define FUNCTION_FINISH(x) do { printf("<< %s\n",#x); } while(0) */
#endif
#define FUNCTION_FINISH(x) do { CC_PRINT((">> %s\n",#x)); } while(0)

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static U32BIT GetPaddedLength(U32BIT unpadded_len);
static U32BIT GetMaxApduLength(U16BIT payload_len);
static U32BIT GetApduDataLength(U16BIT payload_len);
static U32BIT GetMinApduDataLength(void);
static U32BIT GetEncryptedPayloadLength(U32BIT apdu_data_len);
static U8BIT *ReadMessageCounter(U8BIT *ptr, U32BIT *message_counter);
static U8BIT *WriteMessageCounter(U8BIT *ptr, U32BIT message_counter);
static U8BIT *ReadMessageHeader(U8BIT *ptr, U16BIT *payload_len,
                                BOOL *valid_header);
static BOOL IsValidHeader(U8BIT *header);
static U8BIT *WriteMessageHeader(U8BIT *ptr, U16BIT payload_len);
static U8BIT *WriteMessagePayload(U8BIT *ptr, U8BIT *payload,
                                  U16BIT payload_len);
static void CalculateMac(U8BIT *apdu_ptr, U16BIT payload_len, U8BIT *sak,
                         U8BIT *mac);
static void RecalculateMac(U8BIT *payload_ptr, U16BIT payload_len,
                           U8BIT *sac_overhead, U8BIT *sak, U8BIT *mac);
static void ReadMac(U8BIT *payload_ptr, U32BIT payload_len, U8BIT *mac);
static U8BIT *WriteMac(U8BIT *ptr, U8BIT *mac);
static BOOL IsValidMac(U8BIT *received_mac, U8BIT *calculated_mac);
static U8BIT *DecryptPayloadAndMac(U8BIT *payload, U32BIT payload_len,
                                   U8BIT *sek, U8BIT *siv,
                                   void *(*mem_alloc)(U32BIT));
static void EncryptPayloadAndMac(U8BIT *payload, U16BIT payload_len,
                                 U8BIT *sek, U8BIT *siv);
static BOOL IsValidPayload(U8BIT *payload, U16BIT payload_len,
                              U8BIT *sac_overhead, U8BIT *sak);
static U32BIT PadMessage(U8BIT *buffer, U32BIT unpadded_len);
static void UpdateMessageCounter(U32BIT *message_counter);
static U32BIT NextMessageCounter(U32BIT message_counter);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Initialise SAC
 * @param    sac - pointer to SAC structure
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    siv - SAC initialisation vector (128 bits = 16 bytes)
 * @param    mem_alloc - dynamic memory allocation function
 * @param    mem_free - dynamic memory freeing function
 ****************************************************************************/

void CC_InitialiseSac(S_CC_SAC *sac, U8BIT *ks, U8BIT *siv,
                      void *(*mem_alloc)(U32BIT), void (*mem_free)(void *))
{
   FUNCTION_START(CC_InitialiseSac);

   ASSERT(sac != NULL);
   ASSERT(ks != NULL);
   ASSERT(siv != NULL);
   ASSERT(mem_alloc != NULL);
   ASSERT(mem_free != NULL);

   memset(sac, 0, sizeof *sac);

   memcpy_s2s_chk(sac->ks, ks, BITS_TO_BYTES(DT_SIZE_KP),NULL);
   memcpy_s2s_chk(sac->siv, siv, BITS_TO_BYTES(DT_SIZE_SIV),NULL);
   
   sac->sent_message_counter = 1;
   sac->received_message_counter = 0;
   sac->mem_alloc = mem_alloc;
   sac->mem_free = mem_free;

   CC_CalculateSacKeys(sac->ks, sac->sak, sac->sek);

   FUNCTION_FINISH(CC_InitialiseSac);
}
/*!**************************************************************************
 * @brief    Embed SAC message in an APDU
 * @param    sac - pointer to SAC structure
 * @param    tag - APDU tag
 * @param    payload - payload of message to embed
 * @param    payload_len - length of payload
 * @param    apdu - output APDU
 * @param    apdu_len - length of APDU
 * @note     If the output APDU is not NULL, it must be freed
 ****************************************************************************/
void CC_EmbedSacMessage(S_CC_SAC *sac,U8BIT *payload,
                        U16BIT payload_len, U8BIT **apdu, U32BIT *apdu_len)
{
   U8BIT *apdu_ptr;
   U8BIT *apdu_data;
   U8BIT *payload_ptr;
   U32BIT max_apdu_len;
   U32BIT apdu_data_len;
   U16BIT padded_len;
   U8BIT mac[SAC_AUTHENTICATION_LEN];

   FUNCTION_START(CC_EmbedSacMessage);

   *apdu = NULL;
   *apdu_len = 0;

 //  CC_DEBUG_PRINT(("Embedding for SAC (tag=%06lx):\n", tag));
   CC_DEBUG_BUFFER(payload, payload_len);

   /* Allocate buffer for APDU */
   max_apdu_len = GetMaxApduLength(payload_len);
   apdu_ptr = sac->mem_alloc(max_apdu_len);
   if (apdu_ptr != NULL)   {

      apdu_data_len = GetApduDataLength(payload_len);
      /* Write SAC overhead */
      apdu_data = apdu_ptr+1;
      apdu_data = WriteMessageCounter(apdu_data, sac->sent_message_counter);
      padded_len = GetPaddedLength(payload_len);
      apdu_data = WriteMessageHeader(apdu_data, padded_len);

      /* Write padded payload into APDU */
      payload_ptr = apdu_data;
      apdu_data = WriteMessagePayload(payload_ptr, payload, payload_len);

      /* Calculate MAC */
      CalculateMac(apdu_ptr, padded_len, sac->sak, mac);

      /* Write MAC into APDU */
      apdu_data = WriteMac(apdu_data, mac);

      CC_DEBUG_PRINT(("SAC: Before encryption:\n"));
      CC_DEBUG_BUFFER(apdu_ptr+1, apdu_data - apdu_ptr-1);

      /* Encrypt payload + authentication */
      EncryptPayloadAndMac(payload_ptr, padded_len, sac->sek, sac->siv);

      /* Update message counter */
      UpdateMessageCounter(&sac->sent_message_counter);

      /* Return APDU */
      *apdu = apdu_ptr;
      *apdu_len =apdu_data_len;
   }

   FUNCTION_FINISH(CC_EmbedSacMessage);
}

/*!**************************************************************************
 * @brief    Extract Sac message from APDU
 * @param    sac - pointer to SAC structure
 * @param    data - input APDU data (not including tag and length)
 * @param    data_len - input APDU data length
 * @param    payload - embedded payload
 * @param    payload_len - length of embedded payload
 * @note     If the output payload is not NULL, it must be freed
 ****************************************************************************/
void CC_ExtractSacMessage(S_CC_SAC *sac, U8BIT *data, U32BIT data_len,
                          U8BIT **payload, U16BIT *payload_len)
{
   U32BIT min_data_len;
   U32BIT encrypted_payload_len;
   U32BIT message_counter;
   U16BIT length_payload;
   U8BIT *sac_overhead;
   U8BIT *payload_ptr;
   U8BIT *encrypted_payload;
   BOOL valid_header;
   BOOL valid_payload;

   FUNCTION_START(CC_ExtractSacMessage);

   *payload = NULL;
   *payload_len = 0;

   CC_DEBUG_PRINT(("Extracting SAC message\n"));
   min_data_len = GetMinApduDataLength();
   if (data_len >= min_data_len)
   {
      CC_DEBUG_PRINT(("Checking message counter\n"));
      /* Check message counter */
      sac_overhead = data;
      data = ReadMessageCounter(data, &message_counter);
      if(sac->received_message_counter == 0)
      {
        sac->received_message_counter = message_counter-1;
        sac->sent_message_counter = message_counter;
      }
      if (message_counter == NextMessageCounter(sac->received_message_counter))
      {
         CC_DEBUG_PRINT(("Message mcounter ok, checking header\n"));
         /* Check message header */
         data = ReadMessageHeader(data, &length_payload, &valid_header);
         if (valid_header)
         {
            CC_DEBUG_PRINT(("Header ok, decrypting\n"));
            /* Decrypt payload and authentication */
            encrypted_payload = data;
            encrypted_payload_len = GetEncryptedPayloadLength(data_len);
            CC_DEBUG_PRINT(("encrypted_payload_len = %d\n", encrypted_payload_len));
            CC_DEBUG_PRINT(("encrypted_payload:\n"));
            CC_DEBUG_BUFFER(encrypted_payload,encrypted_payload_len);
            CC_DEBUG_PRINT(("sek:\n"));
            CC_DEBUG_BUFFER(sac->sek, BITS_TO_BYTES(DT_SIZE_SEK));
            CC_DEBUG_PRINT(("siv:\n"));
            CC_DEBUG_BUFFER(sac->siv, BITS_TO_BYTES(DT_SIZE_SIV));
            payload_ptr = DecryptPayloadAndMac(encrypted_payload,
                                               encrypted_payload_len,
                                               sac->sek, sac->siv,
                                               sac->mem_alloc);
            if (payload_ptr != NULL)
            {
                CC_DEBUG_PRINT(("length_payload = %d\n", length_payload));
                CC_DEBUG_PRINT(("payload:\n"));
                CC_DEBUG_BUFFER(payload_ptr,length_payload);
               CC_DEBUG_PRINT(("Decrypted, checking if payload is valid\n"));
               /* Check if payload is valid */
               valid_payload = IsValidPayload(payload_ptr, length_payload,
                                              sac_overhead, sac->sak);
               if (valid_payload)
               {
                  CC_DEBUG_PRINT(("SAC: Extracted payload:\n"));
                  *payload = payload_ptr;
                  *payload_len = length_payload;

                  CC_DEBUG_BUFFER(payload_ptr, length_payload);

                  CC_DEBUG_PRINT(("Updating message counter\n"));
                  UpdateMessageCounter(&sac->received_message_counter);
               }
               else
               {
               
                   /* Invalid payload - reset SAC */
                   memset(sac->ks, 0, BITS_TO_BYTES(DT_SIZE_KP));
                   memset(sac->siv, 0, BITS_TO_BYTES(DT_SIZE_SIV));
                   
                   sac->sent_message_counter = 0;
                   sac->received_message_counter = 0;
                  sac->mem_free(payload_ptr);
               }
            }
         }
      }
   }

   FUNCTION_FINISH(CC_ExtractSacMessage);
}

/*!**************************************************************************
 * @brief    Return SAK (SAC authentication key) from SAC structure
 * @param    sac - pointer to SAC structure
 * @return   Pointer to SAK
 ****************************************************************************/
U8BIT *CC_GetSacAuthenticationKey(S_CC_SAC *sac)
{
   FUNCTION_START(CC_GetSacAuthenticationKey);

   FUNCTION_FINISH(CC_GetSacAuthenticationKey);
   CC_DEBUG_PRINT(("CC_GetSacAuthenticationKey:\n"));
   CC_DEBUG_BUFFER(sac->sak,BITS_TO_BYTES(DT_SIZE_SAK));

   return sac->sak;
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Return padded length of buffer
 *
 * This function returns the length of a buffer when padded according to the
 * CI+ specification, by adding a 1 (one) bit followed by 0 (zero) bits, up to
 * a multiple of 128 bits.
 *
 * @param    unpadded_len - length of unpadded data in buffer
 * @return   length of padded data in bytes
 ****************************************************************************/
static U32BIT GetPaddedLength(U32BIT unpadded_len)
{
   U32BIT padded_len;

   FUNCTION_START(GetPaddedLength);

   padded_len = (unpadded_len + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);

   FUNCTION_FINISH(GetPaddedLength);

   return padded_len;
}

/*!**************************************************************************
 * @brief    Return maximum APDU length for payload
 * @param    payload_len - length of payload in bytes
 * @return   Maximum APDU length for payload
 * @note     Return value includes APDU tag and length_field()
 ****************************************************************************/
static U32BIT GetMaxApduLength(U16BIT payload_len)
{
   U32BIT max_apdu_len;

   FUNCTION_START(GetMaxApduLength);

   max_apdu_len = APDU_TAG_LEN;
   max_apdu_len += APDU_HDR_MAX_LEN;
   max_apdu_len += SAC_OVERHEAD_LEN;
   max_apdu_len += GetPaddedLength(payload_len + SAC_AUTHENTICATION_LEN);

   FUNCTION_FINISH(GetMaxApduLength);

   return max_apdu_len;
}

/*!**************************************************************************
 * @brief    Return APDU data length for payload
 * @param    payload_len - length of payload in bytes
 * @return   APDU data length for payload
 ****************************************************************************/
static U32BIT GetApduDataLength(U16BIT payload_len)
{
   U32BIT apdu_data_len;

   FUNCTION_START(GetApduDataLength);

   apdu_data_len = SAC_OVERHEAD_LEN;
   apdu_data_len += GetPaddedLength(payload_len + SAC_AUTHENTICATION_LEN);

   FUNCTION_FINISH(GetPaddedLength);

   return apdu_data_len;
}

/*!**************************************************************************
 * @brief    Return minimum APDU data length
 * @return   minimum APDU data length
 ****************************************************************************/
static U32BIT GetMinApduDataLength(void)
{
   U32BIT min_apdu_data_len;

   FUNCTION_START(GetMinApduDataLength);

   min_apdu_data_len = (SAC_MESSAGE_COUNTER_LEN +
                        SAC_MESSAGE_HEADER_LEN +
                        SAC_AUTHENTICATION_LEN);

   FUNCTION_FINISH(GetMinApduDataLength);

   return min_apdu_data_len;
}

/*!**************************************************************************
 * @brief    Return encrypted payload length given APDU data length
 * @param    apdu_data_len - APDU data length in bytes
 * @return   encrypted payload length
 ****************************************************************************/
static U32BIT GetEncryptedPayloadLength(U32BIT apdu_data_len)
{
   U32BIT encrypted_payload_len;

   FUNCTION_START(GetEncryptedPayloadLength);

   encrypted_payload_len = apdu_data_len - SAC_OVERHEAD_LEN;

   FUNCTION_FINISH(GetEncryptedPayloadLength);

   return encrypted_payload_len;
}

/*!**************************************************************************
 * @brief    Read message counter from APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    message_counter - message counter
 * @return   pointer to byte after the message counter
 ****************************************************************************/
static U8BIT *ReadMessageCounter(U8BIT *ptr, U32BIT *message_counter)
{
   FUNCTION_START(ReadMessageCounter);

   *message_counter = (ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3]);

   FUNCTION_FINISH(ReadMessageCounter);

   return ptr + SAC_MESSAGE_COUNTER_LEN;
}

/*!**************************************************************************
 * @brief    Write message counter into APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    message_counter - message counter
 * @return   pointer to byte after the message counter
 ****************************************************************************/
static U8BIT *WriteMessageCounter(U8BIT *ptr, U32BIT message_counter)
{
   FUNCTION_START(WriteMessageCounter);

   /* message counter */
   ptr[0] = (U8BIT)(message_counter >> 24);
   ptr[1] = (U8BIT)(message_counter >> 16);
   ptr[2] = (U8BIT)(message_counter >> 8);
   ptr[3] = (U8BIT)message_counter;

   FUNCTION_FINISH(WriteMessageCounter);

   return ptr + SAC_MESSAGE_COUNTER_LEN;
}

/*!**************************************************************************
 * @brief    Read message header from APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    payload_len - length of payload in bytes
 * @param    valid_header - whether the header is valid
 * @return   pointer to byte after the message header
 ****************************************************************************/
static U8BIT *ReadMessageHeader(U8BIT *ptr, U16BIT *payload_len,
                                BOOL *valid_header)
{
   FUNCTION_START(ReadMessageHeader);

   *valid_header = IsValidHeader(ptr);
   if (*valid_header)
   {
      *payload_len = ptr[2] << 8 | ptr[3];
   }

   FUNCTION_FINISH(ReadMessageHeader);

   return ptr + 4;
}

/*!**************************************************************************
 * @brief    Tell whether message header is valid
 * @param    header - pointer to message header
 * @return   TRUE if header is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidHeader(U8BIT *header)
{
   U8BIT protocol_version;
   U8BIT authentication_cipher_flag;
   U8BIT payload_encryption_flag;
   U8BIT encryption_cipher_flag;
   BOOL valid;

   FUNCTION_START(IsValidHeader);

   valid = FALSE;

   protocol_version = header[0] >> 4;
   authentication_cipher_flag = (header[0] >> 1) & 0x7;
   payload_encryption_flag = header[0] & 0x1;
   encryption_cipher_flag = header[1] >> 5;

   if (protocol_version == SAC_PROTOCOL_VERSION &&
       authentication_cipher_flag == SAC_AUTHENTICATION_CIPHER_FLAG &&
       payload_encryption_flag == SAC_PAYLOAD_ENCRYPTION_FLAG &&
       encryption_cipher_flag == SAC_ENCRYPTION_CIPHER_FLAG)
   {
      valid = TRUE;
   }

   FUNCTION_FINISH(ReadMessageHeader);

   return valid;
}

/*!**************************************************************************
 * @brief    Write message header into APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    payload_len - length of payload in bytes
 * @return   pointer to byte after the message header
 ****************************************************************************/
static U8BIT *WriteMessageHeader(U8BIT *ptr, U16BIT payload_len)
{
   FUNCTION_START(WriteMessageHeader);

   ptr[0] = ((SAC_PROTOCOL_VERSION << 4) |
             (SAC_AUTHENTICATION_CIPHER_FLAG << 1) |
             (SAC_PAYLOAD_ENCRYPTION_FLAG));
   ptr[1] = SAC_ENCRYPTION_CIPHER_FLAG << 5;
   ptr[2] = (U8BIT)(payload_len >> 8);
   ptr[3] = (U8BIT)payload_len;

   FUNCTION_FINISH(WriteMessageHeader);

   return ptr + SAC_MESSAGE_HEADER_LEN;
}

/*!**************************************************************************
 * @brief    Write message payload into APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    payload - message payload
 * @param    payload_len - length of payload in bytes
 * @return   pointer to byte after the message payload
 ****************************************************************************/
static U8BIT *WriteMessagePayload(U8BIT *ptr, U8BIT *payload,
                                  U16BIT payload_len)
{
   U16BIT padded_len;

   FUNCTION_START(WriteMessagePayload);

   if (payload != NULL)
   {
      memcpy_s2s_chk(ptr, payload, payload_len,NULL);

      /* Pad payload in APDU */
      padded_len = PadMessage(ptr, payload_len);
   }
   else
   {
      padded_len = 0;
   }

   FUNCTION_FINISH(WriteMessagePayload);

   return ptr + padded_len;
}

/*!**************************************************************************
 * @brief    Calculate message authentication code for APDU
 * @param    apdu_ptr - pointer to APDU
 * @param    payload_len - length of payload in bytes
 * @param    sak - SAC authentication key
 * @param    mac - message authentication code
 ****************************************************************************/
static void CalculateMac(U8BIT *apdu_ptr, U16BIT payload_len, U8BIT *sak,
                         U8BIT *mac)
{
   U8BIT *auth_ptr;
   U32BIT auth_len;
   U8BIT hdr_len;
   U8BIT byte;

   FUNCTION_START(CalculateMac);


   /* We apply a little trick here. This relies on apdu_ptr being a writable
    * pointer, which in always true in this context.
    *
    * Authentication is calculated over len(h)||i||h||p where h is the message
    * header and p is the message payload. We have i, h and p already in place,
    * so in order to avoid an extra allocation, we store len(h) in the APDU,
    * calculate the MAC and then restore the data that was in the APDU.
    */

   auth_ptr = apdu_ptr;
   auth_ptr[0] = SAC_MESSAGE_HEADER_LEN;
   auth_len = 1 + SAC_OVERHEAD_LEN + payload_len;

   CC_DEBUG_PRINT(("Authenticating buffer:\n"));
   CC_DEBUG_BUFFER(auth_ptr, auth_len);
   SECTK_Authenticate(auth_ptr, auth_len, sak, mac);

   FUNCTION_FINISH(CalculateMac);
}

/*!**************************************************************************
 * @brief    Recalculate message authentication code
 * @param    payload_ptr - pointer to payload in APDU
 * @param    payload_len - length of payload in bytes
 * @param    sac_overhead - SAC message counter and header
 * @param    sak - SAC authentication key
 * @param    mac - message authentication code
 ****************************************************************************/
static void RecalculateMac(U8BIT *payload_ptr, U16BIT payload_len,
                           U8BIT *sac_overhead, U8BIT *sak, U8BIT *mac)
{
   U8BIT *sac_overhead_ptr;
   U8BIT *auth_payload_ptr;
   U32BIT auth_len;
   U8BIT orig_mac[SAC_AUTHENTICATION_LEN];

   FUNCTION_START(RecalculateMac);

   /* We apply a little trick here. This relies on payload_ptr being a
    * writable pointer, pointer to a buffer that has at least 16 more bytes
    * in it (MAC).
    *
    * We use the extra space to create a buffer for authentication. The
    * extra data is:
    *   header_length (1 byte)
    *   message_counter (4 bytes)
    *   message_header (4 bytes)
    *
    * So this should fit in the space that was taken by the original MAC.
    * We store and restore the MAC so the caller should not see any difference.
    */

   sac_overhead_ptr = payload_ptr + 1;
   auth_payload_ptr = payload_ptr + 1 + SAC_OVERHEAD_LEN;

   /* Store original MAC */
   memcpy_s2s_chk(orig_mac, payload_ptr + payload_len, SAC_AUTHENTICATION_LEN,NULL);

   /* Prepare buffer for authentication */
   memmove(auth_payload_ptr, payload_ptr, payload_len);
   payload_ptr[0] = SAC_MESSAGE_HEADER_LEN;
   memcpy_s2s_chk(sac_overhead_ptr, sac_overhead, SAC_OVERHEAD_LEN,NULL);

   /* Perform SAC authentication */
   auth_len = 1 + SAC_OVERHEAD_LEN + payload_len;
   SECTK_Authenticate(payload_ptr, auth_len, sak, mac);

   /* Restore original payload */
   memmove(payload_ptr, auth_payload_ptr, payload_len);
   memcpy_s2s_chk(payload_ptr + payload_len, orig_mac, SAC_AUTHENTICATION_LEN,NULL);

   FUNCTION_FINISH(RecalculateMac);
}

/*!**************************************************************************
 * @brief    Read message authentication code from APDU buffer
 * @param    payload_ptr - pointer to payload in APDU buffer
 * @param    payload_len - length of payload in bytes
 * @param    mac - message authentication code
 ****************************************************************************/
static void ReadMac(U8BIT *payload_ptr, U32BIT payload_len, U8BIT *mac)
{
   FUNCTION_START(ReadMac);

   memcpy_s2s_chk(mac, payload_ptr + payload_len, SAC_AUTHENTICATION_LEN,NULL);

   FUNCTION_FINISH(ReadMac);
}

/*!**************************************************************************
 * @brief    Write message authentication code into APDU buffer
 * @param    ptr - pointer into APDU buffer
 * @param    mac - message authentication code
 * @return   pointer to byte after the message authentication code
 ****************************************************************************/
static U8BIT *WriteMac(U8BIT *ptr, U8BIT *mac)
{
   FUNCTION_START(WriteMac);

   memcpy_s2s_chk(ptr, mac, SAC_AUTHENTICATION_LEN,NULL);

   FUNCTION_FINISH(WriteMac);

   return ptr + SAC_AUTHENTICATION_LEN;
}

/*!**************************************************************************
 * @brief    Tell whether the calculated MAC is valid
 * @param    received_mac - MAC received in APDU
 * @param    calculated_mac - MAC calculated over the actual data
 * @return   TRUE if the calculated MAC is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidMac(U8BIT *received_mac, U8BIT *calculated_mac)
{
   BOOL valid;

   FUNCTION_START(IsValidMac);

   valid = FALSE;
   CC_DEBUG_PRINT(("Comparing MAC\n"));
   if (memcmp(received_mac, calculated_mac, SAC_AUTHENTICATION_LEN) == 0)
   {
      CC_DEBUG_PRINT(("MAC is valid\n"));
      valid = TRUE;
   }

   FUNCTION_FINISH(IsValidMac);

   return valid;
}

/*!**************************************************************************
 * @brief    Decrypt authentiated payload and MAC
 * @param    payload - pointer to encrypted payload and MAC
 * @param    payload_len - length of encrypted payload
 * @param    sek - SAC encryption key
 * @param    siv - SAC initialisation vector
 * @param    mem_alloc - dynamic memory allocation function
 * @return   pointer to newly allocated buffer containing decrypted payload
 *           and MAC, or NULL if memory allocation failed
 ****************************************************************************/
static U8BIT *DecryptPayloadAndMac(U8BIT *payload, U32BIT payload_len,
                                   U8BIT *sek, U8BIT *siv,
                                   void *(*mem_alloc)(U32BIT))
{
   U8BIT *decrypted_payload;
   U8BIT iv[KEY_SIZE];

   FUNCTION_START(DecryptPayloadAndMac);

   decrypted_payload = mem_alloc(payload_len);
   if (decrypted_payload)
   {
      memcpy_s2s_chk(decrypted_payload, payload, payload_len,NULL);
      memcpy_s2s_chk(iv, siv, KEY_SIZE,NULL);
      SECTK_AesCbcDecrypt(decrypted_payload, payload_len, sek, iv);
   }

   FUNCTION_FINISH(DecryptPayloadAndMac);

   return decrypted_payload;
}

/*!**************************************************************************
 * @brief    Encrypt authenticated payload
 * @param    payload - pointer to payload (padded)
 * @param    padded_len - length of padded payload in bytes
 * @param    sek - SAC encryption key
 * @param    siv - SAC initialisation vector
 * @note     This function assumes that there is enough space after payload
 *           to store the MAC.
 ****************************************************************************/
static void EncryptPayloadAndMac(U8BIT *payload, U16BIT padded_len,
                                 U8BIT *sek, U8BIT *siv)
{
   U8BIT iv[KEY_SIZE];
   U32BIT encrypt_len;

   FUNCTION_START(EncryptPayloadAndMac);

   encrypt_len = padded_len + SAC_AUTHENTICATION_LEN;
   memcpy_s2s_chk(iv, siv, KEY_SIZE,NULL);
   CC_DEBUG_PRINT(("sek:\n"));
   CC_DEBUG_BUFFER(sek, BITS_TO_BYTES(DT_SIZE_SEK));
   CC_DEBUG_PRINT(("iv:\n"));
   CC_DEBUG_BUFFER(iv, BITS_TO_BYTES(DT_SIZE_SIV));
   SECTK_AesCbcEncrypt(payload, encrypt_len, sek, iv);

   FUNCTION_FINISH(EncryptPayloadAndMac);
}

/*!**************************************************************************
 * @brief    Validate authenticated payload
 * @param    payload - payload (followed by MAC)
 * @param    payload_len - length of payload in bytes
 * @param    sac_overhead - pointer to message counter and message header
 * @param    sak - SAC authentication key
 * @return   TRUE if payload is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidPayload(U8BIT *payload, U16BIT payload_len,
                              U8BIT *sac_overhead, U8BIT *sak)
{
   U8BIT received_mac[SAC_AUTHENTICATION_LEN];
   U8BIT calculated_mac[SAC_AUTHENTICATION_LEN];
   BOOL valid;

   FUNCTION_START(IsValidPayload);
                                               
   /* Extract MAC from APDU */
   CC_DEBUG_PRINT(("Extracting MAC from APDU\n"));
   ReadMac(payload, payload_len, received_mac);

   /* Calculate MAC for received payload */
   CC_DEBUG_PRINT(("Calculating MAC for received payload\n"));
   RecalculateMac(payload, payload_len, sac_overhead, sak, calculated_mac);

   {
      CC_DEBUG_PRINT(("payload:\n"));
      CC_DEBUG_BUFFER(payload,payload_len);

      CC_DEBUG_PRINT(("sac overhead:\n"));
      CC_DEBUG_BUFFER(sac_overhead, SAC_OVERHEAD_LEN);

      CC_DEBUG_PRINT(("sak:\n"));
      CC_DEBUG_BUFFER(sak, BITS_TO_BYTES(DT_SIZE_SAK));

      CC_DEBUG_PRINT(("received MAC:\n"));
      CC_DEBUG_BUFFER(received_mac, SAC_AUTHENTICATION_LEN);

      CC_DEBUG_PRINT(("calculated MAC:\n"));
      CC_DEBUG_BUFFER(calculated_mac, SAC_AUTHENTICATION_LEN);
   }
    
   /* Check if calculated MAC is valid */
   CC_DEBUG_PRINT(("Checking MAC\n"));
   valid = IsValidMac(received_mac, calculated_mac);

   FUNCTION_FINISH(IsValidPayload);

   return valid;
}

/*!**************************************************************************
 * @brief    Pad buffer
 *
 * This function pads the buffer according to the CI+ specification, by adding
 * a 1 (one) bit followed by 0 (zero) bits, up to a multiple of 128 bits.
 *
 * @param    buffer - buffer to pad
 * @param    unpadded_len - length of unpadded data in buffer
 * @return   length of padded data in bytes
 *
 * @note     The buffer must be large enough to accommodate the extra padding.
 ****************************************************************************/
static U32BIT PadMessage(U8BIT *buffer, U32BIT unpadded_len)
{
   U32BIT remainder;
   U32BIT padded_len;

   FUNCTION_START(PadMessage);

   padded_len = unpadded_len;
   remainder = unpadded_len & (BLOCK_SIZE-1);
   if (remainder > 0)
   {
      memset(buffer + unpadded_len, 0x00, BLOCK_SIZE - remainder);
      buffer[unpadded_len] = 0x80;
      padded_len += BLOCK_SIZE - remainder;
   }

   FUNCTION_FINISH(PadMessage);

   return padded_len;
}

/*!**************************************************************************
 * @brief    Update message counter
 * @param    message_counter - current / next message counter
 ****************************************************************************/
static void UpdateMessageCounter(U32BIT *message_counter)
{
   FUNCTION_START(UpdateMessageCounter);

   *message_counter = NextMessageCounter(*message_counter);

   FUNCTION_FINISH(UpdateMessageCounter);
}

/*!**************************************************************************
 * @brief    Return next message counter
 * @param    message_counter - current message counter
 * @return   next message counter
 ****************************************************************************/
static U32BIT NextMessageCounter(U32BIT message_counter)
{
   FUNCTION_START(NextMessageCounter);

   ++message_counter;
   if (message_counter == 0)
   {
      message_counter = 1;
   }

   FUNCTION_FINISH(NextMessageCounter);

   return message_counter;
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
