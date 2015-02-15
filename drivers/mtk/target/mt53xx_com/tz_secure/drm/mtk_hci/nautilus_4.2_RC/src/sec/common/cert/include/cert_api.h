/** @addtogroup cert cert
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 *
 */

#ifndef CERT_API_H
#define CERT_API_H

NP_BEGIN_EXTERN_C

/** @file
 * x509 Cert library
 *
 * @require common/asn
 * @require common/crypto/rsa
 * @require common/crypto/sha
 * @require platform/base/binstr
 * @require platform/base/time
 * @require platform/base/mem
 * @author  SunHui Hong <suni@sm.sony.co.jp>
 * @code
 *  // verification the cert chain
 *{
 *  cert_chain_t chain = { 0, NULL };
 *  int32_t kp = CERT_MARLIN_KP_SIGN_PERSONALITY;
 *
 *  chain.num = n; // number of certs
 *  chain.cert = (binstr_t**)mem_Alloc(sizeof(binstr_t*)*n);
 *  if ( !chain.cert ) RET( ::ERR_NOMEM );
 *  mem_Set(chain.cert, 0, sizeof(binstr_t*)*n);
 *
 *  for ( i = 0; i < chain.num; i++ ) {
 *    RET(set_cert(chain.cert+i));
 *  }
 *
 *  RET(cert_marln_VerifyChain(kp, &chain));
 *
 *  EXIT:
 *    if ( chain.cert ) {
 *       for ( i = 0; i < chain.num; i++ ) {
 *           free chain.cert[i];
 *       }
 *       mem_Free(chain.cert);
 *}
 * // parse the cert
 *{
 *    cert_handle_t hcert = CERT_HANDLE_NULL;
 *    binstr_t cert = { the length of cert, the buffer of cert };
 *    binstr_t *ski = NULL, *n = NULL, *e = NULL;
 *
 *    RET( cert_ParseCert(cert, &hcert) );
 *
 *    RET( cert_GetSubjectKeyIdentifier(hcert, &ski) );
 *
 *    RET( cert_GetRSAPublicKey(hcert, &n, &e) );
 *
 *   EXIT:
 *     cert_FreeCert(hcert);
 *
 *}
 */
/*---------------------------------------------------------------------------*
 * Define
 *---------------------------------------------------------------------------*/
/* ERROR */
#define ERR_CERT_ILLEGALCERT        (ERR_CERT_BASE|1)
#define ERR_CERT_ILLEGALMSG         (ERR_CERT_BASE|2)
#define ERR_CERT_ILLEGALCRL         (ERR_CERT_BASE|3)
#define ERR_CERT_OLDCRL             (ERR_CERT_BASE|4)
#define ERR_CERT_REVOKED            (ERR_CERT_BASE|5)


/**
 * KEY PURPOSE
 */
/*
 * Sign Personality nodes
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-octopus-marlin-signing-personality
 */
#define CERT_MARLIN_KP_SIGN_PRIV_PERSONALITY 0
#define CERT_MARLIN_KP_SIGN_PUB_PERSONALITY 19

/*
 * Keys for signing the client role
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-signing-client-role
 */
#define CERT_MARLIN_KP_SIGN_CLIENTROLE 1

/*
 * Keys for signing the drm service role
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-signing-service-role
 */
#define CERT_MARLIN_KP_SIGN_DRMSERVICEROLE 2

/*
 * Sign Non-personality Nodes
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-octopus-marlin-signing-node
 */
#define CERT_MARLIN_KP_SIGN_NODE 3

/*
 * Sign Links
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-octopus-marlin-signing-link
 */
#define CERT_MARLIN_KP_SIGN_LINK 4

/*
 * Sign Controls
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-octopus-marlin-signing-control
 */
#define CERT_MARLIN_KP_SIGN_CONTROL 5

/*
 * Sign Controller
 *  TrustAnchor : marlin-drm-root
 *  KeyUsage : digitalSignature
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-octopus-marlin-signing-controller
 */
#define CERT_MARLIN_KP_SIGN_CONTROLLER 6

/*
 * Keys for signing the security metadata service role
 *  TrustAnchor : marlin-dcs-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-signing-service-role
 */
#define CERT_MARLIN_KP_SIGN_DCSSERVICEROLE 7

/*
 * Keys for signing DCS Assertion
 *  TrustAnchor : marlin-dcs-root
 *  KeyUsage : digitalSignture
 *  CertPolicy : id-cp-nemo-marlin-signing-data-certification
 */
#define CERT_MARLIN_KP_SIGN_DCSASSERTION 8

/*
 * Sign Protocol Data by Service
 *  TrustAnchor : marlin-peer-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-service-key
 */
#define CERT_MARLIN_KP_SIGN_NEMOPROTOCOLSERVICEDATA 9

/*
 * Enc Protocol Data by Service
 *  TrustAnchor : marlin-peer-root
 *  KeyUsage : keyEncipherment
 *  CertPolicy : id-cp-nemo-marlin-service-key
 */
#define CERT_MARLIN_KP_ENC_NEMOPROTOCOLSERVICEDATA 10

/*
 * Keys for signing DRM data
 *  TrustAnchor : marlin-peer-root
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-client-drm-key
 */
#define CERT_MARLIN_KP_SIGN_NEMOPROTOCOLDRMCLIENTDATA 11

/*
 * Keys for encryption DRM data
 *  TrustAnchor : marlin-peer-root
 *  KeyUsage : keyEncipherment
 *  CertPolicy : id-cp-nemo-marlin-client-drm-key
 */
#define CERT_MARLIN_KP_ENC_NEMOPROTOCOLDRMCLIENTDATA 12

/* Watch out! number 19 is already used by
 * CERT_MARLIN_KP_SIGN_PUB_PERSONALITY */

/*
 * Keys for encryption of Nemo Personalization messages
 *  TrustAnchor : urn:marlin:organization:testpdc:device-maker-x:bootstrap
 *  KeyUsage : keyEncipherment
 *  CertPolicy : id-cp-nemo-marlin-service-key
 */
#define CERT_MARLIN_KP_ENC_NEMOPERS  13

/*
 * Keys for signing Nemo Personalization messages
 *  TrustAnchor : urn:marlin:organization:testpdc:device-maker-x:bootstrap
 *  KeyUsage : digitalSignature
 *  CertPolicy : id-cp-nemo-marlin-service-key
 */
#define CERT_MARLIN_KP_SIGN_NEMOPERS  14

/*
 * Keys for signing BKB
 *  TrustAnchor : marlin-dcs-root
 *  KeyUsage : digitalSignture
 *  CertPolicy : any-policy
 */
#define CERT_MARLIN_KP_SIGN_BKB 15

/*---------------------------------------------------------------------------*
 * Structure
 *---------------------------------------------------------------------------*/
/*
 * cert[0] = the cert signed by the trust anchor
 * cert[1] = the cert signed by cert[0]
 * ...
 * cert[num-1] = the cert to be verified
 */
typedef struct {
    int32_t     num;     /* the number of certs */
    binstr_t    **cert;
} cert_chain_t;


/**
 * Handle
 */
typedef void *cert_handle_t;
#define CERT_HANDLE_NULL ((cert_handle_t)0) /* the initial value of cert handle */


/** The certification crl number structure that can handle 20 octets */
#define MAX_NR_OCTET_CRL 20
typedef struct {
    u_int8_t valid;                     /* 1 indicates set, 0 indicates net yet set (invalid)*/
    u_int8_t crlnumber[MAX_NR_OCTET_CRL];    /* Unsigned integer in Big Endian Format */
} crlnumber_t;
/* This macro can be used to initialize an unset crl number */
#define INVALID_CRL_NUMBER {0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
/* This macro can be used to initialize a crl number from a positive integer */
/* Note: the argument of this macro will be called 4 times if it is a function */
#define CRL_NUMBER_FROM_INT(crlnumber_int) {1, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (crlnumber_int)/256/256/256%256, (crlnumber_int)/256/256%256, (crlnumber_int)/256%256, (crlnumber_int)%256}}


/*---------------------------------------------------------------------------*
 * APIs
 *---------------------------------------------------------------------------*/
/**
 * Parse the cert and create the cert handle.
 *
 * @param [in]  in_cert         cert
 * @param [out] out_handle      handle
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_ParseCert(binstr_t         *in_cert,
               cert_handle_t    *out_handle);

/**
 * Parse the public key part of the cert and return the modulus and the public
 * exponent.
 *
 * @param [in]  in_cert_pubkey  public key part of the cert
 * @param [out] out_n           the modulus value
 * @param [out] out_e           the public exponent value
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_ParseCertPublicKey(binstr_t  *in_cert_pubkey,
                        binstr_t **out_n,
                        binstr_t **out_e);

/**
 * Get the value of the subject key identifier field.
 *
 * @param [in]  in_handle       handle which is created by cert_ParseCert().
 * @param [out] out_ski         the value of the subject key identifier field.
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_GetSubjectKeyIdentifier(cert_handle_t      in_handle,
                             binstr_t           **out_ski);

/**
 * Get the modulus and the public exponent in the subjectPublicKeyInfo field.
 * If the algorithm in the subjectPublicKeyInfo field is not RSAPublickey,
 * it returns ::ERR_CERT_ILLEGALCERT.
 *
 * @param [in]  in_handle       handle which is created by cert_ParseCert().
 * @param [out] out_n           the modulus value
 * @param [out] out_e           the public exponent value
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_GetRSAPublicKey(cert_handle_t      in_handle,
                     binstr_t           **out_n,
                     binstr_t           **out_e);

/**
 * Get the value of the subject field.
 *
 * @param [in]  in_handle       handle which is created by cert_ParseCert().
 * @param [out] out_subject     the subject value
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_GetSubject(cert_handle_t      in_handle,
                binstr_t           **out_subject);

/**
 * Free handle.
 *
 * @param [in,out] io_handle   handle which is created by cert_ParseCert().
 *                             If it is NULL, this function returns ::ERR_OK.
 * @return ::ERR_OK
 */
retcode_t
cert_FreeCert(cert_handle_t io_handle);

/**
 * Verify the cert chain.
 *
 * @param [in] in_kp           the key purpose
 * @param [in] in_certs        the certs on the path of cert chain
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_CERT_ILLEGALCERT
 */
retcode_t
cert_marlin_VerifyChain(int32_t in_kp,
                        cert_chain_t *in_certs);

/**
 * @brief Encode the given certificate chain into a PKIPath as
 * specified by ITU-T Recommendation X.690.
 *
 * @param[in] in_certchain       certificate chain
 * @param[out] out_pkipath       X509 PKI Path
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ASN_*
 */
retcode_t
cert_EncodePKIPath(cert_chain_t *in_certchain,
                   binstr_t     **out_pkipath);

/**
 * This functions goes through a list of CRL DPs of a given cert and
 * finds a first DP supporting HTTP porotocol.
 *
 * @param [in] in_handle   A handle of a certificate which DP we are interested
 *                         in.
 * @param [out] out_crl_dp A pointer to a buffer which will hold a pointer
 *                         to a string containing a URI of the found DP.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_CERT_ILLEGALCERT
 * @return ::ERR_INTERNALERROR
 */
retcode_t
cert_GetCRLDistributionPoint(cert_handle_t in_handle,
                             binstr_t    **out_crl_dp);


/*---------------------------------------------------------------------------*
 * CRL API
 *---------------------------------------------------------------------------*/


/**
 * This function checks if a cert with a  given serial number is revoked.
 *
 * @param [in] in_serialnumber A serial number of a cert to be checked.
 * @param [in] in_issuer       An issuer of a cert.
 *
 * @return TRUE if a cert with a given CRL number is revoked.
 * @return FALSE otherwise.
 *
 * @note if the current CRL is not up-to-date, FALSE is returned.
 * @note if an error occurred while executing the call, FALSE is returned.
 */
bool_t
cert_IsRevoked(binstr_t *in_serialnumber,
               binstr_t *in_issuer);

/**
 * This function checks if the current CRL is valid.
 *
 * @note if thete is no current CRL set, it is considered as invlalid.
 *
 * @param [in,out] io_is_valid A pointer to a buffer which will contain TRUE
 *                             if the CRL is valid and FALSE otherwise. This
 *                             value is trusted only when function returns
 *                             ::ERR_OK.
 *
 * @return ::ERR_OK
 * @return ::ERR_TIME_OUTOFRANGE If the time is out of range.
 * @return ::ERR_CERT_ILLEGALCERT If a given CERT doesn't follow the specs..
 * @return ::ERR_ILLEGALARGS The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR An unrecoverable error has occurred in Nautilus.
 */
retcode_t
cert_IsCRLValid(bool_t *io_is_valid);

/**
 * This function sets a new CRL if it is fresher than the current one.
 *
 * @return ::ERR_OK A given CRL has been successfully set.
 * @return ::ERR_NOMEM If it is not enough memory to complete the operation.
 * @return ::ERR_CERT_ILLEGALCRL If a given CERT doesn't follow the specs.
 * @return ::ERR_CERT_OLDCRL If the new cert is too not valid => new CRL
 *                         has to be fetched.
 * @return ::ERR_INTERNALERROR In all other cases.
 */
retcode_t
cert_SetCRL(binstr_t *in_crl_buf);

/**
 * This function returns current CRL number or a crlnumber_t struct with .valid==0 if there is no CRL set.
 *
 * @return ::ERR_OK, ERR_ILLEGALARGS
 */
retcode_t
cert_GetCRLNumber(crlnumber_t *crlnumber);

/*---------------------------------------------------------------------------*/
/**
 * @brief Compare two crl numbers, both in crlnumber_t representation.
 *
 * @param[in]  in_crlnumber_lhs       Left hand side
 * @param[in]  in_crlnumber_lhs       Right hand side
 * @param[out] out_cmp                Comparison (-1 == strictly less, 0 == equal, 1 == strictly greater)
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS         When NULL-pointer are passed, or one of the crlnumbers is not set (.valid==0)
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
cert_CompareCRLNumbers(const crlnumber_t       *in_crlnumber_lhs,
                       const crlnumber_t       *in_crlnumber_rhs,
                       int32_t                 *out_cmp);


NP_END_EXTERN_C

#endif /* CERT_API_H */
/** @} */
