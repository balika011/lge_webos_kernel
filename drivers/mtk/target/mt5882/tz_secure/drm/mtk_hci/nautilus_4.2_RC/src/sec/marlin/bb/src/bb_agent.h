/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef BB_AGENT_H
#define BB_AGENT_H

NP_BEGIN_EXTERN_C

/**
 * @todo       To be removed
 * @deprecated This function (with 2 arguments) should be removed,
 *             it is only called from sec/marlin/nemo/test/agent_test.c
 */
retcode_t
bb_AgentEval(
    xml_node_t            *in_xagent,
    xml_node_t           **out_xresult);

NP_END_EXTERN_C

#endif
