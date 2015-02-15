/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010 Sony Corporation.
 */

#ifndef STOKEN_H
#define STOKEN_H

#define L_NS_WSDLSOAP \
    "http://schemas.xmlsoap.org/wsdl/soap/"

np_ret_t
stoken_l_PrintConfigToken(stoken_config_token_t *in_cnf);

np_ret_t
stoken_l_PrintActionToken(stoken_action_token_t *in_act);

#endif /* STOKEN_H */
