/** @addtogroup bb bb
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef BB_TYPES_H
#define BB_TYPES_H

/** @file */

/* Public */

/* Marlin BB */

/* Security Metadata names (Marlin BB/Sony Connect) */
/* TrustedTime */
#define L_DS_TTNAME             "trusted-time"
#define L_DS_TTNS               "urn:marlin:broadband:security-metadata:attributes"
/* RL */
#define L_DS_RLNS               "urn:marlin:broadband:security-metadata:attributes:license-suspension-serial"   /* Sony Connect */

/* DataUpdate */
#define L_DUS_DATASETNAME       "attributes"
#define L_DUS_DATASETNS         "urn:marlin:broadband:security-metadata"

/* actions */
#define L_ACTION_NODEACQ "urn:marlin:broadband:1-1:registration-service:nodeAcquisition"
#define L_ACTION_LINKACQ "urn:marlin:broadband:1-1:registration-service:linkAcquisition"
#define L_ACTION_DEREG "urn:marlin:broadband:1-1:registration-service:deregistration"
#define L_ACTION_CONFIRMATION "urn:marlin:broadband:1-1:registration-service:confirmDRMObjects"
#define L_ACTION_LA "urn:marlin:broadband:1-1:license-service:requestLicense"
#define L_ACTION_DCS "urn:marlin:broadband:1-1:data-certification-service:requestDataCertification"
#define L_ACTION_DUS "urn:marlin:broadband:1-1:data-update-service:dataUpdateRequest"
#define L_ACTION_LTP "urn:marlin:core:1-0:service:license-transfer:move"

/* namespaces */
#define L_NS_NSU_DCSASSERTION \
    "urn:marlin:broadband:1.0:nemo:services:datacertification-service:assertion"

/* elements digest algorithm and signature digest algorithm for NEMO messages
   i.e., RSA_DIGESTALGO_SHA1 or RSA_DIGESTALGO_SHA256    */
#define L_ELEM_DIGESTALGO    RSA_DIGESTALGO_SHA256
#define L_SIGN_DIGESTALGO    RSA_DIGESTALGO_SHA256

typedef struct {
    int32_t type;
    nemo_wssedata_t data;
} bb_l_context_t;

/**
 * Converts internal bb errors to generic bb errors
 *
 * @param[in] in_err internal bb error
 * @return error code
 */
extern retcode_t
bb_l_ConvErr(retcode_t in_err);


/**
 * Frees memory allocated by a Node\Link\License Request
 * @param[io] io_context bb context
 */
extern void
bb_l_FreeRequest(bb_l_context_t *io_context);

/**
 * Frees a suspension list update
 * @param[in] in_item suspension list item to free
 * @return ::ERR_OK
 */
extern retcode_t bb_l_FreeSL_update(octobj_sl_update_t *in_item);

/**
 * Frees License Suspension Additions/Subtractions ID list
 * @param[in] in_list list of IDs
 * @return ::ERR_OK
 */
extern retcode_t bb_l_FreeSL_idlist(octobj_sl_idlist_t *in_list);

#endif /* BB_TYPES_H */

