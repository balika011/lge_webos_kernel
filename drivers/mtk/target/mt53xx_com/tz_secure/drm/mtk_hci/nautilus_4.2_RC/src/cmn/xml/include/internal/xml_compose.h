/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XML_COMPOSE_H
#define XML_COMPOSE_H

extern retcode_t
npi_xml_l_AddContents(npi_cmncontext_t  in_context,
                      xml_l_element_t  *io_parent,
                      int32_t           in_nchildren,
                      xml_l_content_t **io_children);

extern retcode_t
npi_xml_l_RemoveElement(npi_cmncontext_t in_context,
                         xml_l_element_t *io_parent,
                        xml_l_element_t *io_child);

extern retcode_t
npi_xml_l_ReplaceContent(npi_cmncontext_t in_context,
                         xml_l_element_t *io_parent,
                         xml_l_element_t *io_oldchild,
                         xml_l_content_t *io_newchild);

#endif /* XML_COMPOSE_H */
