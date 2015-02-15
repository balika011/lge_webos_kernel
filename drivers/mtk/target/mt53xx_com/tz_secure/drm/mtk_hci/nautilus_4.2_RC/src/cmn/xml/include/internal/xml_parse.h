/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLP_PARSE_H
#define XMLP_PARSE_H

/*===========================================================================*
 *
 *===========================================================================*/
extern retcode_t
npi_xml_l_ParsePrologue(npi_cmncontext_t in_context,
                        xml_l_internal_state_t *io_state,
                        xml_l_prologue_t **out_prologue,
                        binstr_t *io_str);

extern retcode_t
npi_xml_l_ParseContent(npi_cmncontext_t in_context,
                       xml_l_internal_state_t *io_state,
                       xml_l_content_t **out_cont,
                       binstr_t *io_str);

extern int32_t
npi_xml_l_CmpNamespace(npi_cmncontext_t in_context,
                       xml_l_attribute_vec_t *in_nsvec,
                       binstr_t *in_ename,
                       binstr_t *in_nsa,
                       binstr_t *in_nsb);

#endif /* XMLP_PARSE_H */
