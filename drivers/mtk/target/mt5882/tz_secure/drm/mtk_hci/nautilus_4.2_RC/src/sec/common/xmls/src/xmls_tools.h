/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLS_TOOLS_H
#define XMLS_TOOLS_H

/**
 * xml_tools.h
 */
#define xml_l_string_set_string(io_str, in_newstr, in_newlen) \
    npi_xml_l_string_set_length(NPI_CMN_CONTEXT_SECURE, io_str, in_newlen)

#define xml_l_list_create(out_list) \
    npi_xml_l_list_create(NPI_CMN_CONTEXT_SECURE, out_list)

#define xml_l_list_free(io_list, type) \
    npi_xml_l_list_free(NPI_CMN_CONTEXT_SECURE, io_list, type)

#define xml_l_list_inattr_free(l) \
    npi_xml_l_list_inattr_free(NPI_CMN_CONTEXT_SECURE, l)
#define xml_l_list_incont_free(l) \
    npi_xml_l_list_incont_free(NPI_CMN_CONTEXT_SECURE, l)
#define xml_l_list_innode_free(l) \
    npi_xml_l_list_innode_free(NPI_CMN_CONTEXT_SECURE, l)

#define xml_l_list_push(io_list, item, type) \
    npi_xml_l_list_push(NPI_CMN_CONTEXT_SECURE, io_list, item, type)

#define xml_l_list_addlast(io_list, item, type) \
    npi_xml_l_list_addlast(NPI_CMN_CONTEXT_SECURE, io_list, item, type)

#define xml_l_list_inattr_push_sort(io_list, item) \
    npi_xml_l_list_inattr_push_sort(NPI_CMN_CONTEXT_SECURE, io_list, item)

#define xml_l_list_pop(io_list, type) \
    npi_xml_l_list_pop(NPI_CMN_CONTEXT_SECURE, io_list, type)

#define xml_l_list_create_attribute_vec(in_list, out_attrvec) \
    npi_xml_l_list_create_attribute_vec(NPI_CMN_CONTEXT_SECURE, in_list, out_attrvec)

#define xml_l_list_create_attribute_vec_sort(in_list, out_attrvec) \
    npi_xml_l_list_create_attribute_vec_sort(NPI_CMN_CONTEXT_SECURE, in_list, out_attrvec)

#define xml_l_list_create_content_vec(in_list, out_contvec) \
    npi_xml_l_list_create_content_vec(NPI_CMN_CONTEXT_SECURE, in_list, out_contvec)

#define xml_l_list_create_node_vec(in_list, out_nodevec) \
    npi_xml_l_list_create_node_vec(NPI_CMN_CONTEXT_SECURE, in_list, out_nodevec)

#define xml_l_is_fin(io_state) \
    npi_xml_l_is_fin(NPI_CMN_CONTEXT_SECURE, io_state)

#define xml_l_is_push_namespaces(io_state, in_ns) \
    npi_xml_l_is_push_namespaces(NPI_CMN_CONTEXT_SECURE, io_state, in_ns)

#define xml_l_is_pop_namespaces(io_state) \
    npi_xml_l_is_pop_namespaces(NPI_CMN_CONTEXT_SECURE, io_state)

#define xml_l_is_copy_namespaces_sort(io_state, out_nsl) \
    npi_xml_l_is_copy_namespaces_sort(NPI_CMN_CONTEXT_SECURE, io_state, out_nsl)

#endif /* XMLS_TOOLS_H */
