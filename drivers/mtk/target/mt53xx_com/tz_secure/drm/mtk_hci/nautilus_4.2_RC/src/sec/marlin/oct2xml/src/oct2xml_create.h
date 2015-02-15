/** @addtogroup oct2xml oct2xml
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef OCT2XML_CREATE_H
#define OCT2XML_CREATE_H

NP_BEGIN_EXTERN_C

/**
 * @brief Create XML nodes for a license from an octopus object
 *
 * @param[in]    in_license                      License as a collection of octopus objects
 * @param[in]    in_num_certs                    Number of certs in the chain
 * @param[in]    in_certs                        The cert chain as a list
 * @param[out]   out_contentkeys                 Array of XML nodes containing the content keys
 * @param[out]   out_protectors                  Array of XML nodes containing the CK protectors
 * @param[out]   out_control                     Code Module control node
 * @param[out]   out_controller                  Controller node
 * @param[out]   out_pki_signature               Public Key signature node of the controller
 * @param[out]   out_contentkey_sigs             Array of XML nodes containing the signatures of the content keys
 * @param[out]   out_extension                   LTP extension
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::convert_ToAscii
 * @return ::b64_Encode
 * @return ::oct2xml_l_ValidateLicenseObject
 * @return ::octobj_CBSSerializeController
 * @return ::xml_ExclC14N
 * @return ::xml_ComposeElement
 * @return ::xml_AddElement
 * @return ::sha256_MakeDigest
 * @return ::hci_Lock
 * @return ::hci_CreateKeyRef
 * @return ::hci_SignData
 * @return ::hci_GenerateHMAC
 */
extern retcode_t
oct2xml_l_CreateLicenseNodes( octobj_license_t  *in_license,
                              np_size_t          in_num_certs,
                              binstr_t         **in_certs,
                              xml_node_vec_t   **out_contentkeys,
                              xml_node_vec_t   **out_protectors,
                              xml_node_t       **out_control,
                              xml_node_t       **out_controller,
                              xml_node_t       **out_pki_signature,
                              xml_node_vec_t   **out_contentkey_sigs,
                              xml_node_t       **out_extension );

/**
 * @brief Create a ContentKey XML Node
 *
 * e.g.
 * <ContentKey uid="urn:marlin:organization:service-provider-v:license-service:content-key-2018855958">
 *   <SecretKey uid="urn:marlin:organization:service-provider-v:license-service:secret-key-2018855958">
 *      .
 *      .
 *      .
 *   </SecretKey>
 *  </ContentKey>
 *
 * @param[in]  in_contentkey  Octopus Object representation of a content key
 * @param[out] out_node       XML representation of a content key
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::npi_binstr_ToStr
 * @return ::npi_xml_ComposeElement
 * @return ::hci_Lock
 * @return ::hci_CreateKeyRef
 * @return ::hci_ImportKey
 * @return ::rand_Init
 * @return ::rand_GetBytes
 * @return ::convert_ToAscii
 */
extern retcode_t
oct2xml_l_CreateContentKey( octobj_contentkey_t  *in_contentkey,
                            xml_node_t          **out_node );

NP_END_EXTERN_C


#endif /* OCT2XML_CREATE_H */

/** @} */
