/** @addtogroup starfish starfish
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef STARFISH_API_H
#define STARFISH_API_H

NP_BEGIN_EXTERN_C

/*-----------------------------------------------------------------------*/
/** @file
 * @brief Starfish Library
 *
 * dependency none
 * @depends common/xml
 * @depends common/binstr
 * @depends octopus/octobj
 * @author Olivier Demarto <olivierd@sonycom.com>
 * @date   13 Mar, 2006 last modified by olivierd
 */
/*-----------------------------------------------------------------------*/

#define ERR_SF_FAILED       (ERR_SF_BASE|1)
#define ERR_SF_REVOKED      (ERR_SF_BASE|2)
#define ERR_SF_WRONGID      (ERR_SF_BASE|3)
#define ERR_SF_WRONGVERSION (ERR_SF_BASE|4)
#define ERR_SF_WRONGVAL     (ERR_SF_BASE|5)
#define ERR_SF_WRONGBKB     (ERR_SF_BASE|6)

#define SF_KEY_LENGTH (16)

/*-----------------------------------------------------------------------*/
/**
 * @brief Decrypt the Starfish protected content key using the Broadcast key
 *
 * @param[in]  in_bkb    broadcast key block
 * @param[in]  in_key    starfish key
 * @param[out] out_ref   hci reference that holds the decrypted key
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_SF_WRONGVAL
 * @return ::ERR_SF_REVOKED
 * @return ::ERR_SF_WRONGID
 * @return ::ERR_SF_WRONGVERSION
 * @return ::ERR_SF_FAILED
 * @return ::hci_GetStarfishId
 * @return ::hci_SessionOpen
 * @return ::hci_CreateKeyRef
 * @return ::hci_ImportStarfishKey
 * @return ::hci_DestroyKeyRef
 * @return ::hci_SessionClose
 **/
/*-----------------------------------------------------------------------*/
extern retcode_t
sf_GetKey(octobj_bkb_t    *in_bkb,
          binstr_t        *in_key,
          hci_key_ref_t   *out_ref);

/*-----------------------------------------------------------------------*/
/**
 * @brief Decrypt the Starfish protected content key using the Broadcast key
 *
 * @param[in]  in_bkb   broadcast key block
 * @param[in]  in_key   starfish key
 * @param[out] out_ref  hci reference that holds the decrypted key
 *
 * @retval ERR_OK
 * @retval ERR_NOMEM
 * @retval ::l_GetKey
 * @retval ::l_BkbBinInfo
 */
/*-----------------------------------------------------------------------*/
extern retcode_t
sf_test_GetKey(binstr_t        *in_bkb,
               binstr_t        *in_key,
               hci_key_ref_t   *out_ref);

/*-----------------------------------------------------------------------*/
/**
 * @brief Compare the supplied BKB container with the current one
 *        to check the freshness and save it to the Nautilus registry
 * @param[in]  in_bkb_container    BKB container to update
 * @param[in]  in_from_ppproc      Flag indicates bkb container is from ppproc bundle
 * @param[in]  in_pv               Application data used to encrypt bkb container
 * @param[in]  in_cur_bkb          BKB data existing in the import context structure
 * @param[in]  in_store_timestamp  Flag to specify whether to store time stamp or not, when BKB continer is empty
 * @param[out] out_bkb             BKB parsed from the container if it is fresh one
 *                                 else it will be current_bkb.
 *                                 NULL if bkb is not present
 *
 * @retval ERR_OK
 * @retval ERR_NOMEM
 * @retval ERR_ILLEGALARGS
 * @retval ERR_INTERNALERROR
 * @retval ERR_SF_WRONGBKB
 */
/*-----------------------------------------------------------------------*/
extern retcode_t
sf_UpdateBKBContainer(binstr_t    *in_bkb_container,
                      bool_t       in_from_ppproc,
                      binstr_t    *in_pv,
                      binstr_t    *in_cur_bkb,
                      bool_t       in_store_timestamp,
                      binstr_t   **out_bkb);

/*-----------------------------------------------------------------------*/
/**
 * @brief Check whether BKB is up-to-date and gets the BKB
 *
 * @param[out] out_is_bkb_fresh   Value indicates whether stored bkb is up-to-date
 * @param[out] out_bkb            BKB data
 *
 * @retval ERR_OK
 * @retval ERR_NOMEM
 * @retval ERR_ILLEGALARGS
 *-----------------------------------------------------------------------*/
extern retcode_t
sf_GetBKBInfo(bool_t      *out_is_bkb_fresh,
              binstr_t   **out_bkb);


/*-----------------------------------------------------------------------*/
/**
 * @brief Protect the supplied content key with starfish (if BK is available)
 *
 * @param[in]  in_key            content key to encrypt
 * @param[out] out_protected_ck  NULL (if no BK is available), or
 *                               super-encrypted CK as per [Core] section 7.4 (end)
 *
 * @note it is recommended that the caller should decide to call the BK
 *       encryption based on the presence on a cached copy of a BKB.
 *
 * @retval ERR_OK
 * @retval ERR_NOMEM
 * @retval ERR_ILLEGALARGS
 * @retval ::hci_EncryptKeyData
 *
 **/
extern retcode_t
sf_EncryptContentKey(hci_key_ref_t    in_ck,
                     binstr_t       **out_protected_ck);

NP_END_EXTERN_C

#endif /* STARFISH_API_H */
/** @} */
