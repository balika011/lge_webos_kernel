/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */
#ifndef NSFF_COMMON_H
#define NSFF_COMMON_H

NP_BEGIN_EXTERN_C

extern np_ret_t
nsff_l_AllowOptionalXmlNode( retcode_t in_ret,
                             np_ret_t  in_default_ret );

extern np_ret_t
nsff_l_GetXmlIntegerAttribute( xml_node_t    *in_node,
                               char          *in_namespace,
                               char          *in_attribute,
                               np_ret_t       in_default_ret,
                               u_int32_t     *out_value );

NP_END_EXTERN_C

#endif /* NSFF_COMMON_H */
