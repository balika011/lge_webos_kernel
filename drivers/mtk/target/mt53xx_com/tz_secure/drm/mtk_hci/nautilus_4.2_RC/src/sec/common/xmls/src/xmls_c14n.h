/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLS_C14N_H
#define XMLS_C14N_H

#define xml_l_ExclC14N(io_state, in_node, in_omit, out_str) \
    npi_xml_l_ExclC14N(NPI_CMN_CONTEXT_SECURE, io_state, in_node, in_omit, out_str)

#endif /* XMLS_C14N_H */
