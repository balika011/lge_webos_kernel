/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef XMLS_INDEF_H
#define XMLS_INDEF_H

/**
 * xml_types.h
 */
#define xml_l_prologue_version_info(x)        npi_xml_l_prologue_version_info(x)
#define xml_l_prologue_encoding_decl(x)       npi_xml_l_prologue_encoding_decl(x)
#define xml_l_prologue_set_version_info(x,v)  npi_xml_l_prologue_set_version_info(x,v)
#define xml_l_prologue_set_encoding_decl(x,v) npi_xml_l_prologue_set_encoding_decl(x,v)
#define xml_l_prologue_create(out_prologue) \
    npi_xml_l_prologue_create(NPI_CMN_CONTEXT_SECURE, out_prologue)
#define xml_l_prologue_free(io_prologue) \
    npi_xml_l_prologue_free(NPI_CMN_CONTEXT_SECURE, io_prologue)

#define xml_l_attribute_copy(x)        npi_xml_l_attribute_copy(x)
#define xml_l_attribute_name(x)        npi_xml_l_attribute_name(x)
#define xml_l_attribute_value(x)       npi_xml_l_attribute_value(x)
#define xml_l_attribute_set_name(x,v)  npi_xml_l_attribute_set_name(x,v)
#define xml_l_attribute_set_value(x,v) npi_xml_l_attribute_set_value(x,v)
#define xml_l_attribute_create(out_attr) \
    npi_xml_l_attribute_create(NPI_CMN_CONTEXT_SECURE, out_attr)
#define xml_l_attribute_free(io_attr) \
    npi_xml_l_attribute_free(NPI_CMN_CONTEXT_SECURE, io_attr)

#define xml_l_attribute_vec_attr(x,i)       npi_xml_l_attribute_vec_attr(x,i)
#define xml_l_attribute_vec_size(x)         npi_xml_l_attribute_vec_size(x)
#define xml_l_attribute_vec_set_attr(x,i,v) npi_xml_l_attribute_vec_set_attr(x,i,v)
#define xml_l_attribute_vec_clear_attr(x,i) npi_xml_l_attribute_vec_clear_attr(x,i)
#define xml_l_attribute_vec_create(out_attrvec, in_size) \
    npi_xml_l_attribute_vec_create(NPI_CMN_CONTEXT_SECURE, out_attrvec, in_size)
#define xml_l_attribute_vec_free(io_attrvec) \
    npi_xml_l_attribute_vec_free(NPI_CMN_CONTEXT_SECURE, io_attrvec)

#define xml_l_content_copy(x)          npi_xml_l_content_copy(x)
#define xml_l_content_type(x)          npi_xml_l_content_type(x)
#define xml_l_content_element(x)       npi_xml_l_content_element(x)
#define xml_l_content_text(x)          npi_xml_l_content_text(x)
#define xml_l_content_set_type(x,v)    npi_xml_l_content_set_type(x,v)
#define xml_l_content_set_element(x,v) npi_xml_l_content_set_element(x,v)
#define xml_l_content_set_text(x,v)    npi_xml_l_content_set_text(x,v)
#define xml_l_content_create(out_cont) \
    npi_xml_l_content_create(NPI_CMN_CONTEXT_SECURE, out_cont)
#define xml_l_content_free(io_cont) \
    npi_xml_l_content_free(NPI_CMN_CONTEXT_SECURE, io_cont)

#define xml_l_content_vec_content(x,i)       npi_xml_l_content_vec_content(x,i)
#define xml_l_content_vec_size(x)            npi_xml_l_content_vec_size(x)
#define xml_l_content_vec_set_content(x,i,v) npi_xml_l_content_vec_set_content(x,i,v)
#define xml_l_content_vec_create(out_contvec, in_size) \
    npi_xml_l_content_vec_create(NPI_CMN_CONTEXT_SECURE, out_contvec, in_size)
#define xml_l_content_vec_free(io_contvec) \
    npi_xml_l_content_vec_free(NPI_CMN_CONTEXT_SECURE, io_contvec)

#define xml_l_element_copy(x)             npi_xml_l_element_copy(x)
#define xml_l_element_name(x)             npi_xml_l_element_name(x)
#define xml_l_element_namespaces(x)       npi_xml_l_element_namespaces(x)
#define xml_l_element_attributes(x)       npi_xml_l_element_attributes(x)
#define xml_l_element_contents(x)         npi_xml_l_element_contents(x)
#define xml_l_element_set_name(x,v)       npi_xml_l_element_set_name(x,v)
#define xml_l_element_set_namespaces(x,v) npi_xml_l_element_set_namespaces(x,v)
#define xml_l_element_set_attributes(x,v) npi_xml_l_element_set_attributes(x,v)
#define xml_l_element_set_contents(x,v)   npi_xml_l_element_set_contents(x,v)
#define xml_l_element_create(out_element) \
    npi_xml_l_element_create(NPI_CMN_CONTEXT_SECURE, out_element)
#define xml_l_element_free(io_element) \
    npi_xml_l_element_free(NPI_CMN_CONTEXT_SECURE, io_element)

#define xml_l_document_prologue(x)       npi_xml_l_document_prologue(x)
#define xml_l_document_element(x)        npi_xml_l_document_element(x)
#define xml_l_document_set_prologue(x,v) npi_xml_l_document_set_prologue(x,v)
#define xml_l_document_set_element(x,v)  npi_xml_l_document_set_element(x,v)
#define xml_l_document_create(out_xmldoc) \
    npi_xml_l_document_create(NPI_CMN_CONTEXT_SECURE, out_xmldoc)
#define xml_l_document_free(io_xmldoc) \
    npi_xml_l_document_free(NPI_CMN_CONTEXT_SECURE, io_xmldoc)

#define xml_l_node_copy(x)             npi_xml_l_node_copy(x)
#define xml_l_node_type(x)             npi_xml_l_node_type(x)
#define xml_l_node_root(x)             npi_xml_l_node_root(x)
#define xml_l_node_element(x)          npi_xml_l_node_element(x)
#define xml_l_node_namespace(x)        npi_xml_l_node_namespace(x)
#define xml_l_node_attribute(x)        npi_xml_l_node_attribute(x)
#define xml_l_node_text(x)             npi_xml_l_node_text(x)
#define xml_l_node_set_type(x,v)       npi_xml_l_node_set_type(x,v)
#define xml_l_node_set_root(x,v)       npi_xml_l_node_set_root(x,v)
#define xml_l_node_set_element(x,v)    npi_xml_l_node_set_element(x,v)
#define xml_l_node_set_namespace(x,v)  npi_xml_l_node_set_namespace(x,v)
#define xml_l_node_set_attribute(x,v)  npi_xml_l_node_set_attribute(x,v)
#define xml_l_node_set_text(x,v)       npi_xml_l_node_set_text(x,v)
#define xml_l_node_create(out_xmlnode) \
    npi_xml_l_node_create(NPI_CMN_CONTEXT_SECURE, out_xmlnode)
#define xml_l_node_create_root(out_xmlnode, in_document) \
    npi_xml_l_node_create_root(NPI_CMN_CONTEXT_SECURE, out_xmlnode, in_document)
#define xml_l_node_create_element(out_xmlnode, in_element) \
    npi_xml_l_node_create_element(NPI_CMN_CONTEXT_SECURE, out_xmlnode, in_element)
#define xml_l_node_create_namespace(out_xmlnode, in_ns) \
    npi_xml_l_node_create_namespace(NPI_CMN_CONTEXT_SECURE, out_xmlnode, in_ns)
#define xml_l_node_create_attribute(out_xmlnode, in_attr) \
    npi_xml_l_node_create_attribute(NPI_CMN_CONTEXT_SECURE, out_xmlnode, in_attr)
#define xml_l_node_create_text(out_xmlnode, in_text) \
    npi_xml_l_node_create_text(NPI_CMN_CONTEXT_SECURE, out_xmlnode, in_text)
#define xml_l_node_free(io_xmlnode) \
    npi_xml_l_node_free(NPI_CMN_CONTEXT_SECURE, io_xmlnode)

#define xml_l_node_vec_node(x,i)        npi_xml_l_node_vec_node(x,i)
#define xml_l_node_vec_size(x)         npi_xml_l_node_vec_size(x)
#define xml_l_node_vec_set_node(x,i,v) npi_xml_l_node_vec_set_node(x,i,v)
#define xml_l_node_vec_create(out_nodevec, in_size) \
    npi_xml_l_node_vec_create(NPI_CMN_CONTEXT_SECURE, out_nodevec, in_size)
#define xml_l_node_vec_free(io_nodevec) \
    npi_xml_l_node_vec_free(NPI_CMN_CONTEXT_SECURE, io_nodevec)

#define xml_l_is_init(out_state) \
    npi_xml_l_is_init(NPI_CMN_CONTEXT_SECURE, out_state)
#define xml_l_conv_itos(in_i, out_str) \
    npi_xml_l_conv_itos(NPI_CMN_CONTEXT_SECURE, in_i, out_str)
#define xml_l_conv_ttos(in_t, out_str) \
    npi_xml_l_conv_ttos(NPI_CMN_CONTEXT_SECURE, in_t, out_str)


extern retcode_t
xml_l_FreeSigRef(xml_sigref_t *io_ref);

#endif /* XMLS_INDEF_H */
