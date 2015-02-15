/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XML_C14N_H
#define XML_C14N_H

extern retcode_t
npi_xml_l_ExclC14N(npi_cmncontext_t in_context,
                   xml_l_internal_state_t *io_state,
                   xml_l_node_t *in_node,
                   xml_l_element_t *in_omit,
                   binstr_t **out_str);

#endif /* XMLC_C14N_H */
