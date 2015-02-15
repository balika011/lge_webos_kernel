/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLS_PRINT_H
#define XMLS_PRINT_H

#define xml_l_print_string(in_str)                        npi_xml_l_print_string(in_str)
#define xml_l_print_string_nr(in_str)                     npi_xml_l_print_string_nr(in_str)
#define xml_l_print_prologue(in_prologue)                 npi_xml_l_print_prologue(in_prologue)
#define xml_l_print_attribute(in_attr, isns, indent)      npi_xml_l_print_attribute(in_attr, isns, indent)
#define xml_l_print_attribute_vec(in_attrs, isns, indent) npi_xml_l_print_attribute_vec(in_attrs, isns, indent)
#define xml_l_print_content(in_content, indent)           npi_xml_l_print_content(in_content, indent)
#define xml_l_print_content_vec(in_conts, indent)         npi_xml_l_print_content_vec(in_conts, indent)
#define xml_l_print_element(in_element, indent)           npi_xml_l_print_element(in_element, indent)
#define xml_l_print_document(in_xmldoc)                   npi_xml_l_print_document(in_xmldoc)
#define xml_l_print_node(in_node)                         npi_xml_l_print_node(in_node)
#define xml_l_print_node_vec(in_nodevec)                  npi_xml_l_print_node_vec(in_nodevec)

#endif /* XMLS_PRINT_H */
