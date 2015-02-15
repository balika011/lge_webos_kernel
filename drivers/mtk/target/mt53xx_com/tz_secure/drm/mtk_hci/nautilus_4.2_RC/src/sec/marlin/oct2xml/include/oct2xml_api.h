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

#ifndef OCT2XML_API_H
#define OCT2XML_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Composes the license's XML string from the internal representation
 *
 * @note  Ideally, the BKB if any, should be in the input octobj_license_t.
 *        However, the caller typically gets the BKB as an XML string.
 *        Since octobj should not contain XML strings, such BKB should not be
 *        entered into octobj_license_t without parsing it.
 *        But eventually only the XML string will be used, not the parsed BKB.
 *        Therefore, it was decided to pass the XML string directly to oct2xml, as a parameter,
 *        to avoid parsing and regenerating the BKB unnecessarily
 *
 * @param[in]  in_license              Internal representation of the license (without a BKB)
 * @param[in]  in_bkb                  NULL, or a BKB (matching the BK used for encrypting the CKs)
 * @param[in]  in_num_certs            Number of license signing certs in chain
 * @param[in]  in_certs                The license signing certificate chain as a list
 * @param[out] out_license             XML string representation of the License
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::oct2xml_l_CreateLicenseNodes
 * @return ::xml_ComposeElement
 * @return ::xml_AddElement
 * @return ::xml_ToString
 */
extern retcode_t
oct2xml_ComposeLicense(octobj_license_t   *in_license,
                       binstr_t           *in_bkb,
                       np_size_t           in_num_certs,
                       binstr_t          **in_certs,
                       binstr_t          **out_license);

/**
 * @brief Create a Content Key XML node
 *
 * @param[in]  in_contentkey  Octopus Object representation of a content key
 * @param[out] out_node       XML representation of a content key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::oct2xml_l_CreateContentKey
 */
extern retcode_t
oct2xml_CreateContentKey(octobj_contentkey_t  *in_contentkey,
                         xml_node_t          **out_node);


NP_END_EXTERN_C

#endif /* OCT2XML_API_H */

/** @} */

