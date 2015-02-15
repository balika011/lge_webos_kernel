/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLP_PRINT_H
#define XMLP_PRINT_H

#if defined(NS_DEBUG)
#define XML_PRINT_ON

extern retcode_t
npi_xml_l_print_string(binstr_t *in_str);

extern retcode_t
npi_xml_l_print_string_nr(binstr_t *in_str);

extern retcode_t
npi_xml_l_print_prologue(xml_l_prologue_t *in_prologue);

extern retcode_t
npi_xml_l_print_attribute(xml_l_attribute_t *in_attr, int32_t isns, int32_t indent);

extern retcode_t
npi_xml_l_print_attribute_vec(xml_l_attribute_vec_t *in_attrs, int32_t isns, int32_t indent);

extern retcode_t
npi_xml_l_print_content(xml_l_content_t *in_content, int32_t indent);

extern retcode_t
npi_xml_l_print_content_vec(xml_l_content_vec_t *in_conts, int32_t indent);

extern retcode_t
npi_xml_l_print_element(xml_l_element_t *in_element, int32_t indent);

extern retcode_t
npi_xml_l_print_document(xml_l_document_t *in_xmldoc);

extern retcode_t
npi_xml_l_print_node(xml_l_node_t *in_node);

extern retcode_t
npi_xml_l_print_node_vec(xml_node_vec_t *in_nodevec);

#else
#if defined(XML_PRINT_ON)
#  undef XML_PRINT_ON
#endif

#define npi_xml_l_print_string(x) do{}while(0)
#define npi_xml_l_print_string_nr(x) do{}while(0)
#define npi_xml_l_print_prologue(x) do{}while(0)
#define npi_xml_l_print_attribute(x,a,b) do{}while(0)
#define npi_xml_l_print_attribute_vec(x,a,b) do{}while(0)
#define npi_xml_l_print_content(x,a) do{}while(0)
#define npi_xml_l_print_content_vec(x,a) do{}while(0)
#define npi_xml_l_print_element(x,a) do{}while(0)
#define npi_xml_l_print_document(x) do{}while(0)
#define npi_xml_l_print_node(x) do{}while(0)
#define npi_xml_l_print_node_vec(x) do{}while(0)

#endif

#endif /* XMLP_PRINT_H */
