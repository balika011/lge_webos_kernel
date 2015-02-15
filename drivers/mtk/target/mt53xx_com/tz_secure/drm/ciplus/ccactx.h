/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    ccactx.h                                                      */
/*   DATE:    02/02/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Authentication Context          */
/*                                                                          */
/****************************************************************************/
#ifndef __CCACTX_H_
#define __CCACTX_H_

#include "ccdef.h"

typedef struct CI_CC_INFO CI_CC_INFO;

struct CI_CC_INFO{
   /* DHX (local) */
   U8BIT dhx[BITS_TO_BYTES(DT_SIZE_DHX)];
   BOOL dhx_generated;

   /* DHPH (local) */
   U8BIT dhph[BITS_TO_BYTES(DT_SIZE_DHPH)]; //output
   BOOL dhph_generated;

   /* DHSK (local) */
   U8BIT dhsk[BITS_TO_BYTES(DT_SIZE_DHSK)];//need store
   BOOL dhsk_generated;

   /* CICAM_ID (local) */
   U8BIT local_cicam_id[BITS_TO_BYTES(DT_SIZE_CICAM_ID)]; //output and need store
   BOOL local_cicam_id_extracted;

   /* Ks (local) */
   U8BIT ks[BITS_TO_BYTES(DT_SIZE_KS)];
   BOOL ks_generated;
   /* Key register (local) */
   U8BIT key_register;
   BOOL key_register_received;

   /* CIV (local) */
   U8BIT civ[BITS_TO_BYTES(DT_SIZE_CIV)];
   BOOL civ_generated;

   /* CCK (local) */
   U8BIT cck[BITS_TO_BYTES(DT_SIZE_CCK)];
   BOOL cck_generated;
   /* Kp (12) */
   U8BIT kp[BITS_TO_BYTES(DT_SIZE_KP)];
   BOOL kp_received;

   /* Host device certificate (local) */
   void *host_dev_cert;
   BOOL host_dev_cert_parsed;

   /* Module scrambler capabilities (local) */
   S32BIT module_caps_capability;
   S32BIT module_caps_version;
   BOOL module_caps_extracted;

   /* CICAM brand ID (local) */
   U16BIT cicam_brand_id;                          //need store
   BOOL cicam_brand_id_extracted;

   /* Host scrambler capabilities (local) */
   S32BIT host_caps_capability;
   S32BIT host_caps_version;
   BOOL host_caps_extracted;

   /* Cipher (local) */
   U8BIT cipher;                                     //need store
   BOOL cipher_negotiated;

   /* HOST_ID (5) */
   U8BIT host_id[BITS_TO_BYTES(DT_SIZE_HOST_ID)];   //need output
   BOOL host_id_extracted;
   
   /* CICAM_ID (6) */
   U8BIT cicam_id[BITS_TO_BYTES(DT_SIZE_CICAM_ID)];
   BOOL cicam_id_received;

   /* CICAM_BrandCert (8) */
   void *cicam_brand_cert;
   BOOL cicam_brand_cert_received;


   /* DHPM (14) */
   U8BIT dhpm[BITS_TO_BYTES(DT_SIZE_DHPM)];  //input
   BOOL dhpm_received;

   /* CICAM_DevCert (16) */
   void *cicam_dev_cert;
   BOOL cicam_dev_cert_received;

   /* Signature_B (18) */
   U8BIT signature_b[BITS_TO_BYTES(DT_SIZE_SIGNATURE_B)]; //need output
   BOOL signature_b_recevied;

   /* auth_nonce (19) */
   U8BIT auth_nonce[BITS_TO_BYTES(DT_SIZE_AUTH_NONCE)]; //input
   BOOL auth_nonce_received;

   /* Ns_Host (20) */
   U8BIT ns_host[BITS_TO_BYTES(DT_SIZE_NS_HOST)]; //need output
   BOOL ns_host_generated;

   /* AKH (22) */
   U8BIT akh[BITS_TO_BYTES(DT_SIZE_AKH)];        //need output                    //need store
   BOOL akh_generated;

   /* SAC information */
   S_CC_SAC sac;
   BOOL sac_initialised;
   BOOL sac_used;
   /*current TS UTC time*/
   U32BIT epoch_time;
   /* status_field (30) */
   U8BIT status_field;

   U8BIT auth_context;
   BOOL auth_context_initialised;

   /*cck key id*/
   U32BIT cck_keyid;
};

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

#define CC_NUM_AUTH_CONTEXTS 5

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Read authentication contexts
 ****************************************************************************/
void CC_ReadAuthContexts(void);

/*!**************************************************************************
 * @brief    Reset authentication context iterator
 * @param session_info - content control session information
 ****************************************************************************/
void CC_ResetAuthContextIter(CI_CC_INFO* cc);

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
void CC_CopyNextAuthContext(CI_CC_INFO* cc);

/*!**************************************************************************
 * @brief    Update authentication context
 * @param session_info - content control session information
 ****************************************************************************/
void CC_UpdateAuthContext(CI_CC_INFO* cc);
#endif   /* !__CCACTX_H */
