/** @addtogroup xmls xmls
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef XMLS_API_H
#define XMLS_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief XML processing library
 *
 * dependency base,b64
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   29 Nov, 2004 last modified by n-goto
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * macro definitions
 *---------------------------------------------------------------------------*/

/**
 * @brief Error code of XML library
 */
#define ERR_XMLS_ILLEGALSIGNATURE      (ERR_XMLS_BASE|1)
#define ERR_XMLS_UNSUPPORTEDSIGNATURE  (ERR_XMLS_BASE|2)
#define ERR_XMLS_NOREFSINSIGNATURE     (ERR_XMLS_BASE|3)
#define ERR_XMLS_MISMATCHNODE          (ERR_XMLS_BASE|4)
#define ERR_XMLS_PARSEFAIL             (ERR_XMLS_BASE|5)
#define ERR_XMLS_ILLEGALHASHLEN        (ERR_XMLS_BASE|6)
#define ERR_XMLS_REVOKEDCERT           (ERR_XMLS_BASE|7)

/**
 * @brief xml_sigkey-type definitions
 */
#define XML_SIGKEY_TYPE_KEYNAME           0
#define XML_SIGKEY_TYPE_CERTS             1
#define XML_SIGKEY_TYPE_X509SKI           2
#define XML_SIGKEY_TYPE_STOKREF_KEYID     3
#define XML_SIGKEY_TYPE_STOKREF_REFERENCE 4

/**
 * @brief xml_dsig-algo definitions
 */
#define XML_DSIG_ALGO_RSASHA1     0
#define XML_DSIG_ALGO_RSASHA256   1
#define XML_DSIG_ALGO_HMACSHA1    2
#define XML_DSIG_ALGO_HMACSHA256  3
#define XML_DSIG_ALGO_UNSUPPORTED -1

/**
 * @brief xml_dsig-type definitions
 */
#define XML_DSIG_TYPE_DETACHED    0
#define XML_DSIG_TYPE_ENVELOPED   1
#define XML_DSIG_TYPE_ENVELOPING  2

/*
 */
enum {
    XML_TRANSFORM_NONE = 0,
    /* general transform algorithms */
    XML_TRANSFORM_EXCLC14N,
    XML_TRANSFORM_ENVSIG,
    XML_TRANSFORM_ENVSIG_AND_EXCLC14N,
    /* transform algorithms defined in marlin spec. */
    XML_TRANSFORM_OCTOPUS_CBS1_0,
    /* end */
    XML_TRANSFORM_MAX
};

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

/**
 * @brief XML parsed node type
 */
typedef struct {
    binstr_t *uri;
    int32_t  ntransforms;
    binstr_t **transforms;
    binstr_t *algo;
    binstr_t *digest;
} xml_sigref_t;

typedef struct {
    binstr_t     *algo_c14n;
    binstr_t     *algo_sig;
    int32_t      n_refs;
    xml_sigref_t *refs;
    binstr_t     *c14ned;
} xml_siginfo_t;

typedef struct {
    int32_t count;
    binstr_t **data;
} xml_certchain_t;

typedef struct {
    int32_t type;
    union {
        binstr_t        *keyname;
        binstr_t        *ski;
        char            *ref;
        xml_certchain_t  certs;
    } key;
} xml_sigkey_t;

typedef struct {
    binstr_t      *id;
    xml_siginfo_t siginfo;
    binstr_t      *sigvalue;
    xml_sigkey_t  keyinfo;
} xml_dsig_t;

/*---------------------------------------------------------------------------*/
/**
 * @brief Serialize in_xmlroot XML node except that the Signature
 * element using a <dsig:Transform> as given in in_xmlnode is omitted
 * from the output.
 *
 * @param[in]  in_xmlroot   XML node to be serialized
 * @param[in]  in_xmlnode   XML <dsig:Transform> node to be omitted
 * @param[out] out_xmltext  serialized document
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_XMLS_MISMATCHNODE
 * @return ::xml_ExclC14N
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_EnvelopedSig(xml_node_t *in_xmlroot, xml_node_t *in_xmlnode, binstr_t **out_xmltext);

/*---------------------------------------------------------------------------*/
/**
 * @brief Given a (B64 + ASN.1) encoded data, extract and return an
 * array corresponding to the decoded data.
 *
 * @require xml_l_ExtractEncodedLength
 *
 * @param[in] in_encodeddata           encoded data
 * @param[out] out_decodeddata         decoded data
 * @param[out] out_decodeddata_count   size of decoded data
 *
 * @return retcode_t
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_ExtractEncodedData(binstr_t *in_encodeddata, binstr_t ***out_decodeddata,
                       int32_t *out_decodeddata_count);

/*---------------------------------------------------------------------------*/
/**
 * @brief Parse XML Signature node in_node.
 *
 * @param[in]  in_dsignode   node to process
 * @param[out] out_dsignode  signature details
 *
 * @return  ::ERR_OK
 * @return  ::ERR_NOMEM
 * @return  ::ERR_XMLS_PARSEFAIL
 * @return  ::xml_Find
 * @return  ::xml_GetAttr
 * @return  ::xml_GetContent
 * @return  ::xml_ExclC14N
 * @return  ::b64_Decode
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_ParseSignature(xml_node_t *in_dsignode, xml_dsig_t **out_dsig);

/*---------------------------------------------------------------------------*/
/**
 * @brief Parse signature for in_node.
 * Extends xml_ParseSignature with processing
 * SecurityTokenReference/Reference element.
 *
 * @see xml_ParseSignature
 *
 * @todo For refactoring reasons this function should be merged with
 * xml_ParseSignature.
 *
 * @param[in] in_dsignode   node to process
 * @param[in] in_hnode      a node defining the BinarySecurityToken reference
 *                          indicated in SecurityTokenReference/Reference
 * @param[out] out_dsignode  signature details
 *
 * @return  ::ERR_OK
 * @return  ::ERR_NOMEM
 * @return  ::ERR_XMLS_PARSEFAIL
 * @return  ::xml_Find
 * @return  ::xml_GetAttr
 * @return  ::xml_GetContent
 * @return  ::xml_ExclC14N
 * @return  ::b64_Decode
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_ParseSignature_ext(xml_node_t *in_dsignode,
                       xml_node_t *in_hnode,
                       xml_dsig_t **out_dsig);

/*---------------------------------------------------------------------------*/
/**
 * @brief Check whether the given URI matches any ID within a given array.
 *
 * @param[in]  in_uri     URI to match
 * @param[in]  in_nids    size of in_ids array
 * @param[in]  in_ids     array of IDs to match against
 *
 * @return  the index within in_ids that matches in_uri
 * @return  -1  if no match was found
 */
/*---------------------------------------------------------------------------*/
extern int32_t
xml_CheckSignatureURI(binstr_t *in_uri, int32_t in_nids, binstr_t *in_ids[]);

/*---------------------------------------------------------------------------*/
/**
 * @brief Given a Signature structure, return its type.
 *
 * @param[in]  in_dsig   Signature structure
 *
 * @return  XML_DSIG_ALGO_RSASHA1       RSA with SHA1
 * @return  XML_DSIG_ALGO_RSASHA256     RSA with SHA256
 * @return  XML_DSIG_ALGO_HMACSHA1      HMAC with SHA1
 * @return  XML_DSIG_ALGO_UNSUPPORTED   unknown
 */
/*---------------------------------------------------------------------------*/
extern int32_t
xml_GetSignatureType(xml_dsig_t *in_dsig);

/*---------------------------------------------------------------------------*/
/**
 * @brief Verify the given xml_sigref_t structure against the given
 * transform method, and digest.
 *
 * @param[in]  in_ref         reference structure to check
 * @param[in]  in_transform   transform method to check against
 *                            (e.g., XML_TRANSFORM_EXCLC14N)
 * @param[in]  in_digestlen   size of in_digestbuf
 * @param[in]  in_digestbuf   digest to check against
 *
 * @return  ::ERR_OK
 * @return  ::ERR_ILLEGALARGS
 * @return  ::ERR_XMLS_ILLEGALSIGNATURE
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_CheckSignatureReference(xml_sigref_t *in_ref,
                            int32_t in_transform,
                            /*np_size_t in_talgolen, u_int8_t *in_talgobuf,*/
                            np_size_t in_digestlen, u_int8_t *in_digestbuf);


/*---------------------------------------------------------------------------*/
/**
 * @brief Verify the PKI Signature of a given XML node against some
 * given entry params. That is, key purpose of the public key to be
 * used, reference IDs, array of digests, array of transform methods
 * used, the public key for signature check.
 *
 * @param[in] in_node      node to process
 * @param[in] in_kp        key purpose of the public key to be used
 *                         (e.g., CERT_MARLIN_KP_SIGN_NEMOPROTOCOLDRMCLIENTDATA)
 * @param[in] in_nids      size of the arrays in_ids, in_sha1, in_transforms,
 *                         out_results
 * @param[in] in_ids       array of reference IDs of size in_nids
 * @param[in] in_sha1      array of SHA1 digests of size in_nids
 * @param[in] in_transforms    array of transforms methods of size in_nids
 * @param[in] in_cert      certificate whose key is to be used to signature check
 * @param[in] out_results  array of retcodes of size in_nids, corresponding to
 *                         each individual digest check
 *
 * @return ::xml_ParseSignature
 * @return ::xml_CheckSignatureReference
 * @return ::xml_ExclC14
 * @return ::xml_Find
 * @return ::ERR_XMLS_UNSUPPORTEDSIGNATURE
 * @return ::ERR_XMLS_ILLEGALSIGNATURE
 * @return ::ERR_XMLS_NOREFSINSIGNATURE
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_VerifyPKISignature(xml_node_t *in_node, u_int32_t in_kp,
                       int32_t in_nids,
                       binstr_t *in_ids[], u_int8_t *in_sha1[], int32_t *in_transforms,
                       binstr_t *in_cert,
                       retcode_t out_results[]);

/*---------------------------------------------------------------------------*/
/**
 * @brief Verify PKI signature for in_node. This is a simple extention
 * of the previous xml_VerifyPKISignature with the functionality of
 * processing SecurityTokenReference/Reference elements.
 *
 * @note This function is using xml_ParseSignature_ext
 *
 * @todo For refactoring reasons this function should be merged with
 * xml_VerifyPKISignature.
 *
 * @see xml_VerifyPKISignature
 *
 * @param[in] in_node      node to process
 * @param[in] in_hnode     a node defining the BinarySecurityToken reference
 *                         indicated in SecurityTokenReference/Reference
 * @param ...
 *
 * @return ::xml_VerifyPKISignature
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_VerifyPKISignature_ext(xml_node_t *in_node, xml_node_t *in_hnode,
                           u_int32_t in_kp, int32_t in_nids,
                           binstr_t *in_ids[], u_int8_t *in_sha1[], int32_t *in_transforms,
                           binstr_t *in_cert,
                           retcode_t out_results[]);

/*---------------------------------------------------------------------------*/
/**
 * @brief Verify PKI signature for in_node. This is a simple extention
 * of the previous xml_VerifyPKISignature with the functionality of
 * acquiring subject element of certificate.
 *
 * @param[in]    in_node         node to process
 * @param ...
 * @param[out]    out_subject     subject in certificate
 * @param ...
 *
 * @return ::xml_VerifyPKISignature_ext
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_VerifyPKISignature_ext2(xml_node_t *in_node,
                            u_int32_t in_kp, int32_t in_nids,
                            binstr_t *in_ids[], u_int8_t *in_sha1[], int32_t *in_transforms,
                            binstr_t *in_cert,
                            binstr_t **out_subject,
                            retcode_t out_results[]);

/*---------------------------------------------------------------------------*/
/**
 * @brief Create a signature info node.
 *
 * @param[in] in_nrefids      Number of signatures.
 * @param[in] in_dalgo        Signature digest algorithm.
 * @param[in] in_dtypes       Array of signature digest types.
 * @param[in] in_refids       Array of signature digest reference ids.
 * @param[in] in_transforms   Array of serialization transforms
 * @param[in] in_sha1digests  Array of signature digests.
 * @param[out] out_node       The generated xml node.
 *
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_XMLS_UNSUPPORTEDSIGNATURE
 * @return ::xml_ComposeElement
 * @return ::xml_AddElement
 * @return ::xml_ExclC14N
 * @return ::b64_Encode

 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_CreateSignedInfoNode(int32_t in_nrefids,
                         char  *in_dalgo,
                         int32_t in_dtypes[],
                         char *in_refids[],
                         char *in_transforms[],
                         u_int8_t *in_sha1digests[],
                         xml_node_t **out_node);

/*---------------------------------------------------------------------------*/
/**
 * @brief Create a signature node.
 *
 * @param[in] in_siginfo_node  Signature info node.
 * @param[in] in_sig           Signature data.
 * @param[in] in_sigkey        Signature key info. Either certificates, key name, SKI or security token reference
 * @param[in] in_id            ID of the signature node. Can be NULL.
 * @param[out] out_node        The generated xml node.
 *
 * @return ::ERR_ILLEGALARGS
 * @return ::xml_ComposeElement
 * @return ::xml_Find
 * @return ::b64_Encode
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_CreateSignatureNode(xml_node_t *in_siginfo_node,
                        binstr_t *in_sig,
                        xml_sigkey_t *in_sigkey,
                        char *in_id,
                        xml_node_t **out_node);

/*---------------------------------------------------------------------------*/
/**
 * @brief Free the given xml_dsig_t structure.
 *
 * @param[in,out]  io_dsig   structure to free
 *
 * @return   ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xml_FreeDsig(xml_dsig_t *io_dsig);

/*---------------------------------------------------------------------------*/
/**
 * @brief Given a ds:Signature node, parse it and determine the length
 * of the sha algorithm used for hashing the elements.
 *
 * @note Only XML_ALGO_RSASHA1 and XML_ALGO_RSASHA256 supported.
 * @note Only XML_ALGO_SHA256 and XML_ALGO_SHA1 supported.
 * @note As inforced by the profiles in MCS, the digests in
 * Signature/SignedInfo should use the same digest algorithm.
 * @todo Add support for XML_ALGO_HMACSHA1 and XML_ALGO_HMACSHA256.
 *
 * @param[in] in_dsignode   ds:Signature node
 * @param[out] out_len      hash length
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_XML_*
 * @return ::ERR_XMLS_ILLEGALHASHLEN
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
xmls_GetSignatureHashLen(xml_node_t *in_dsignode, u_int32_t *out_len);

NP_END_EXTERN_C

#endif /* XMLS_API_H */
/** @} */
