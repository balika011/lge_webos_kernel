/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLP_TOOLS_H
#define XMLP_TOOLS_H

#define XML_L_LIST_TYPE_ATTRIBUTE    0
#define XML_L_LIST_TYPE_CONTENT      1
#define XML_L_LIST_TYPE_NODE         2

/*===========================================================================*
 *
 *===========================================================================*/
extern retcode_t
npi_xml_l_string_set_string(npi_cmncontext_t in_context,
                            binstr_t **io_str, u_int8_t *in_newstr, np_size_t in_newlen);

extern retcode_t
npi_xml_l_string_set_length(npi_cmncontext_t in_context,
                            binstr_t **io_str, np_size_t in_newlen);

extern retcode_t
npi_xml_l_list_create(npi_cmncontext_t in_context,
                      xml_l_list_t **out_list);

extern retcode_t
npi_xml_l_list_free(npi_cmncontext_t in_context,
                    xml_l_list_t **io_list, int32_t type);

#define npi_xml_l_list_inattr_free(c,l) npi_xml_l_list_free(c,l,XML_L_LIST_TYPE_ATTRIBUTE)
#define npi_xml_l_list_incont_free(c,l) npi_xml_l_list_free(c,l,XML_L_LIST_TYPE_CONTENT)
#define npi_xml_l_list_innode_free(c,l) npi_xml_l_list_free(c,l,XML_L_LIST_TYPE_NODE)

extern retcode_t
npi_xml_l_list_push(npi_cmncontext_t in_context,
                    xml_l_list_t **io_list, void *item, int32_t type);

extern retcode_t
npi_xml_l_list_addlast(npi_cmncontext_t in_context,
                       xml_l_list_t **io_list, void *item, int32_t type);

extern retcode_t
npi_xml_l_list_inattr_push_sort(npi_cmncontext_t in_context,
                                xml_l_list_t **io_list, xml_l_attribute_t *item);

extern retcode_t
npi_xml_l_list_pop(npi_cmncontext_t in_context,
                   xml_l_list_t **io_list, int32_t type);

extern retcode_t
npi_xml_l_list_create_attribute_vec(npi_cmncontext_t in_context,
                                    xml_l_list_t *in_list,
                                    xml_l_attribute_vec_t **out_attrvec);

extern retcode_t
npi_xml_l_list_create_attribute_vec_sort(npi_cmncontext_t in_context,
                                         xml_l_list_t *in_list,
                                         xml_l_attribute_vec_t **out_attrvec);

extern retcode_t
npi_xml_l_list_create_content_vec(npi_cmncontext_t in_context,
                                  xml_l_list_t *in_list,
                                  xml_l_content_vec_t **out_contvec);

extern retcode_t
npi_xml_l_list_create_node_vec(npi_cmncontext_t in_context,
                               xml_l_list_t *in_list,
                               xml_node_vec_t **out_nodevec);

extern retcode_t
npi_xml_l_is_fin(npi_cmncontext_t in_context,
                 xml_l_internal_state_t *io_state);

extern retcode_t
npi_xml_l_is_push_namespaces(npi_cmncontext_t in_context,
                             xml_l_internal_state_t *io_state,
                             xml_l_attribute_t *in_ns);

extern retcode_t
npi_xml_l_is_pop_namespaces(npi_cmncontext_t in_context,
                            xml_l_internal_state_t *io_state);

extern retcode_t
npi_xml_l_is_copy_namespaces_sort(npi_cmncontext_t in_context,
                                  xml_l_internal_state_t *io_state,
                                  xml_l_attribute_vec_t **out_nsl);

#endif /* XMLP_TOOLS_H */
