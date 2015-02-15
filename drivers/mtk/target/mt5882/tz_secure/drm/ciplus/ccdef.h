/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    ccdef.h                                                       */
/*   DATE:    19/01/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Common Definitions              */
/*                                                                          */
/****************************************************************************/
#ifndef __CCDEF_H_
#define __CCDEF_H_

#include "mtk_util.h"
#include "x_os.h"
#include "x_assert.h"
#include "tzst.h"

#include "techtype.h"

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define CC_PRINT(x) \
{ \
    dprintf x; \
}
//#define CC_PRINT(x) printf x
#ifndef CC_PRINT_BUFFER
#define CC_PRINT_BUFFER(d,l) do {                \
      int jj;                                    \
      for (jj = 0; jj < (l); ++jj)               \
      {                                          \
         CC_PRINT(("%02x ", (d)[jj]));           \
         if ((jj & 0xf) == 0xf)                  \
         {                                       \
            CC_PRINT(("\n"));                    \
         }                                       \
      }                                          \
      if (((l) & 0xf) != 0)                      \
      {                                          \
         CC_PRINT(("\n"));                       \
      }                                          \
   } while (0)
#endif
   
/* Bits to Bytes */
#define BITS_TO_BYTES(x) ((x) >> 3)

/* content control system ID bitmask */
#define CC_SYSTEM_ID_BITMASK    0x01

/* apdu message tag ids */
enum
{
   CC_OPEN_REQ_TAG                = 0x9f9001,
   CC_OPEN_CNF_TAG                = 0x9f9002,
   CC_DATA_REQ_TAG                = 0x9f9003,
   CC_DATA_CNF_TAG                = 0x9f9004,
   CC_SYNC_REQ_TAG                = 0x9f9005,
   CC_SYNC_CNF_TAG                = 0x9f9006,
   CC_SAC_DATA_REQ_TAG            = 0x9f9007,
   CC_SAC_DATA_CNF_TAG            = 0x9f9008,
   CC_SAC_SYNC_REQ_TAG            = 0x9f9009,
   CC_SAC_SYNC_CNF_TAG            = 0x9f9010,
   CC_PIN_CAPABILITIES_REQ_TAG    = 0x9f9011,
   CC_PIN_CAPABILITIES_REPLY_TAG  = 0x9f9012,
   CC_PIN_CMD_TAG                 = 0x9f9013,
   CC_PIN_REPLY_TAG               = 0x9f9014,
   CC_PIN_EVENT_TAG               = 0x9f9015,
   CC_PIN_PLAYBACK_TAG            = 0x9f9016,
   CC_PIN_MMI_REQ_TAG             = 0x9f9017,
   CC_TASK_TAG                    = 0xff0101
};

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/* datatype IDs and sizes (in bits) */
enum
{
   DT_ID_BRAND_ID              = 1,
   DT_ID_HOST_ID               = 5,
   DT_ID_CICAM_ID              = 6,
   DT_ID_HOST_BRAND_CERT       = 7,
   DT_ID_CICAM_BRAND_CERT      = 8,
   DT_ID_KP                    = 12,
   DT_ID_DHPH                  = 13,
   DT_ID_DHPM                  = 14,
   DT_ID_HOST_DEV_CERT         = 15,
   DT_ID_CICAM_DEV_CERT        = 16,
   DT_ID_SIGNATURE_A           = 17,
   DT_ID_SIGNATURE_B           = 18,
   DT_ID_AUTH_NONCE            = 19,
   DT_ID_NS_HOST               = 20,
   DT_ID_NS_MODULE             = 21,
   DT_ID_AKH                   = 22,
   DT_ID_AKM                   = 23,
   DT_ID_URI_MESSAGE           = 25,
   DT_ID_PROGRAM_NUMBER        = 26,
   DT_ID_URI_CONFIRM           = 27,
   DT_ID_KEY_REGISTER          = 28,
   DT_ID_URI_VERSIONS          = 29,
   DT_ID_STATUS_FIELD          = 30,
   DT_ID_SRM_DATA              = 31,
   DT_ID_SRM_CONFIRM           = 32,
   DT_ID_CICAM_LICENSE         = 33,
   DT_ID_LICENSE_STATUS        = 34,
   DT_ID_LICENSE_RCVD_STATUS   = 35,
   DT_ID_HOST_LICENSE          = 36,
   DT_ID_PLAY_COUNT            = 37,
   DT_ID_OPERATING_MODE        = 38,
   DT_ID_PINCODE_DATA          = 39,
   DT_ID_RECORD_START_STATUS   = 40,
   DT_ID_MODE_CHANGE_STATUS    = 41,
   DT_ID_RECORD_STOP_STATUS    = 42
};

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
typedef struct S_CC_SAC S_CC_SAC;
struct S_CC_SAC{
   U8BIT ks[BITS_TO_BYTES(DT_SIZE_KS)];
   U8BIT sak[BITS_TO_BYTES(DT_SIZE_SAK)];
   U8BIT sek[BITS_TO_BYTES(DT_SIZE_SEK)];
   U8BIT siv[BITS_TO_BYTES(DT_SIZE_SIV)];
   U32BIT sent_message_counter;
   U32BIT received_message_counter;
   void *(*mem_alloc)(U32BIT);
   void (*mem_free)(void *);
};
/* Cipher type */
#define STB_CI_CIPHER_DES           0x00
#define STB_CI_CIPHER_AES           0x01
/* key register type */
#define STB_CI_KEY_EVEN          0x00
#define STB_CI_KEY_ODD           0x01


/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

#endif   /* !__CCDEF_H */
